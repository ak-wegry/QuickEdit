/*
 * 日付に関する処理
 */

#include <windows.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <pcreposi.h>
#include "DateCtrl.h"

// 最大値
#define MAX_BUF		1024

// 日付パターンID
#define PTN_MD		0x00000001	// mmdd
#define PTN_Y4MD	0x00000002	// yyyymmdd
#define PTN_Y2MD	0x00000004	// yymmdd
#define PTN_WEEK	0x00000010	// (week)
#define PTN_SLA		0x00000100	// yyyy/mm/dd
#define PTN_KNJ		0x00000200	// yyyy年mm月dd日
#define PTN_CSV		0x00000400	// Sunday|..., January|... dd, yyyy

// 日付パターンMASK
#define MASK_YMD	0x0000000F
#define MASK_WEEK 	0x000000F0
#define MASK_DLM	0x00000F00

// 日付の状態に関する情報
static DATESTAT DateStat;

// 日付を検出/表示する情報
typedef struct {
	int nId;			// 検出/表示する日付のパターンID
	char *pFmtYear;		// スキャン/表示する年のフォーマット
	char *pFmtMonth;	// スキャン/表示する月のフォーマット
	char *pFmtDay;		// スキャン/表示する日のフォーマット
	char *pFmtWeek;		// スキャン/表示する曜日のフォーマット
} DATEINFO;

static DATEINFO DateInfo[] = {
	{ PTN_Y4MD | PTN_KNJ | PTN_WEEK, "%d年"  , "%d月", "%d日", "(%s)" },
	{ PTN_Y2MD | PTN_KNJ | PTN_WEEK, "%02d年", "%d月", "%d日", "(%s)" },
	{ PTN_MD   | PTN_KNJ | PTN_WEEK, ""      , "%d月", "%d日", "(%s)" },
	{ PTN_Y4MD | PTN_SLA | PTN_WEEK, "%d/"   , "%d/" , "%d"  , "(%s)" },
	{ PTN_Y2MD | PTN_SLA | PTN_WEEK, "%02d/" , "%d/" , "%d"  , "(%s)" },
	{ PTN_MD   | PTN_SLA | PTN_WEEK, ""      , "%d/" , "%d"  , "(%s)" },
	{ PTN_Y4MD | PTN_KNJ           , "%d年"  , "%d月", "%d日", ""     },
	{ PTN_Y2MD | PTN_KNJ           , "%02d年", "%d月", "%d日", ""     },
	{ PTN_MD   | PTN_KNJ           , ""      , "%d月", "%d日", ""     },
	{ PTN_Y4MD | PTN_SLA           , "%d/"   , "%d/" , "%d"  , ""     },
	{ PTN_Y2MD | PTN_SLA           , "%02d/" , "%d/" , "%d"  , ""     },
	{ PTN_MD   | PTN_SLA           , ""      , "%d/" , "%d"  , ""     },
	{ PTN_Y4MD | PTN_CSV | PTN_WEEK, ", %d"  , "%s " , "%d"  , "%s, " },
	{ PTN_MD   | PTN_CSV | PTN_WEEK, ""      , "%s " , "%d"  , "%s, " },
	{ PTN_Y4MD | PTN_CSV           , ", %d"  , "%s " , "%d"  , ""     },
	{ PTN_MD   | PTN_CSV           , ""      , "%s " , "%d"  , ""     },
	{ PTN_Y4MD                     , "%04d"  , "%02d", "%02d", ""     },
	{ PTN_Y2MD                     , "%02d"  , "%02d", "%02d", ""     },
	{ PTN_MD                       , ""      , "%02d", "%02d", ""     },
	{ NULL                         , ""      , ""    , ""    , ""     }};

// 英語月,曜日の変換用データ
static char *pEgMon[] = {"","January","February","March","April","May","June","July","August","September","October","November","December", 0};
static char *pEgWk[] = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday",0};

#if 1
main(int argc, char **argv)
{
	char *pNext, *pPrev;

	if (argc != 2) {
		printf("Usage: test date-srting.\n");
		exit(1);
	}

	pNext = AdjDate(argv[1]);
	if (pNext == 0) {
		printf("No Match\n");
		exit(1);
	}
	printf("%s\n", pNext);
	for (int i = 0; i < 10; ++i) {
		pPrev = pNext;
		pNext = AdjDate(pPrev);
		if (pNext == 0) {
			printf("No Match\n");
			exit(1);
		}
		printf("%s\n", pNext);
		free(pPrev);

		if (!strcmp(pNext, argv[1])) {
			printf("\n");
			break;
		}
	}
	free(pNext);
}
#endif

// 日付変換
char *AdjDate(char *pText)
{
	int nPtnId, nMchStt, nMchEnd;
	char *pDate, *pConv, szMchStr[MAX_BUF+1];
	DATESTAT InpStat;

	// 日付パターンのチェック
	nPtnId = ChkDatePtn(pText, &nMchStt, &nMchEnd);
	if (nPtnId <= 0) {
		InitDateStat();	// 日付状態の初期化
		return 0;
	}

	// 一致した日付データ取得
	strncpy(szMchStr, pText + nMchStt, nMchEnd - nMchStt);
	szMchStr[nMchEnd - nMchStt] = '\0';

	// 日付データの分析
	AnlDate(nPtnId, szMchStr, &InpStat);
	
	if (CmpDateStat(&DateStat, &InpStat)) { // 日付状態の比較
		// 不一致
		// 日付状態の初期化
		InitDateStat();
		// 日付状態のコピー
		CpyDateStat(&DateStat, &InpStat);
		// 表示パターン順の設定
		SetDspPtn(nPtnId, &DateStat);
	} else {
		// 一致
		// 次の表示パターン設定
		NxtDateStat(&DateStat);
	}

	// 表示用日付文字列の生成
	pDate = MakDspDate(&DateStat);
	if (pDate == NULL) return 0;

	// 入力文字と同じ場合は次のパターンで再生成
	if (!strcmp(pDate, szMchStr)) {
		free(pDate);
		NxtDateStat(&DateStat);
		pDate = MakDspDate(&DateStat);
		if (pDate == NULL) return 0;
	}

	// 日付データ部分を置換え
	pConv = (char *)malloc(strlen(pText) + strlen(pDate) + 1);
	strncpy(pConv, pText, nMchStt);
	strcpy(pConv + nMchStt, pDate);
	strcat(pConv, pText + nMchEnd);
	free(pDate);
	
	return pConv;
}

// 日付状態の初期化
void InitDateStat()
{
	DateStat.year    = -1;
	DateStat.month   = -1;
	DateStat.day     = -1;
	DateStat.wk      = 0;
	DateStat.nCurIdx = 0;

	for (int i = 0; i < MAX_PTN; ++i) {
		DateStat.nId[i] = 0;
	}
}

// 日付状態の比較
int CmpDateStat(DATESTAT *pStat1, DATESTAT *pStat2)
{
#if 0
	if (pStat1->year  == pStat2->year &&
        pStat1->month == pStat2->month &&
        pStat1->day   == pStat2->day &&
        pStat1->wk    == pStat2->wk) {
		return 0;
#endif
	if (pStat1->month == pStat2->month &&
        pStat1->day   == pStat2->day) {
		return 0;
	} else {
		return 1;
	}
}

// 日付状態のコピー
void CpyDateStat(DATESTAT *pStat1, DATESTAT *pStat2)
{
    pStat1->year  = pStat2->year;
    pStat1->month = pStat2->month;
    pStat1->day   = pStat2->day;
    pStat1->wk    = pStat2->wk;
}

// 次の表示パターン設定
void NxtDateStat(DATESTAT *pStat)
{
	int nIdx, nNxtId;

	nIdx = pStat->nCurIdx + 1;
	if (nIdx < MAX_PTN) {
		nNxtId = pStat->nId[nIdx];
		if (nNxtId == 0) {
			pStat->nCurIdx = 0;
		} else {
			pStat->nCurIdx = nIdx;
		}
	} else {
		pStat->nCurIdx = 0;
	}
}

// 日付パターンのチェック
int ChkDatePtn(char *pDate, int *pnMchStt, int *pnMchEnd)
{
	int i, nSize;
	char szChkPtn[MAX_BUF+1];
	regex_t regexBuf;
	regmatch_t ptnMatch[5]; // 結果を格納する

	nSize = sizeof(ptnMatch) / sizeof(regmatch_t);
	for (i = 0; DateInfo[i].nId; ++i) {
		// 正規表現のパターン作成
		MakChkPtn(DateInfo[i].nId, szChkPtn);
		// 正規表現のコンパイル
		if (regcomp(&regexBuf, szChkPtn, REG_NEWLINE) != 0) {
//			printf("regex compile failed.\n");
			return -1;
		}

		// 正規表現との一致
		if (regexec(&regexBuf, pDate, nSize, ptnMatch, 0) == 0) {
//printf("*** Match Pattern = %s\n", szChkPtn);
//printf("*** start = %d, end = %d\n", ptnMatch[0].rm_so, ptnMatch[0].rm_eo);
			// 一致した開始/終了位置の設定
			*pnMchStt = ptnMatch[0].rm_so;
			*pnMchEnd = ptnMatch[0].rm_eo;
			regfree(&regexBuf);
			return DateInfo[i].nId;	// 一致したパターンID
		}

		regfree(&regexBuf);
	}

	return 0;	// 不一致
}

// 正規表現のパターン作成
void MakChkPtn(int nId, char *pPtn)
{
#define REGEX_MON	"(1[0-2]|0?[1-9])"
#define REGEX_MON2	"(1[0-2]|0[1-9])"
#define REGEX_DAY	"(3[0-1]|[1-2]\\d|0?[1-9])"
#define REGEX_DAY2	"(3[0-1]|[1-2]\\d|0[1-9])"
#define REGEX_WEEK	"\\\((日|月|火|水|木|金|土)\\\)"
#define REGEX_EGMON	"(January|February|March|April|May|June|July|August|September|October|November|December)"
#define REGEX_EGWK	"(Sunday|Monday|Tuesday|Wednesday|Thursday|Friday|Saturday|)"
	char szBuf[MAX_BUF+1];

	*pPtn = '\0';

	// 年設定
	if (nId & PTN_Y4MD) {
		strcat(pPtn, "\\d{4}");
	} else 	if (nId & PTN_Y2MD) {
		strcat(pPtn, "\\d{2}");
	}

	if (nId & PTN_Y4MD || nId & PTN_Y2MD) {
		if (nId & PTN_SLA) {
			strcat(pPtn, "/");
		} else if (nId & PTN_KNJ) {
			strcat(pPtn, "年");
		} else if (nId & PTN_CSV) {
			sprintf(szBuf, ", %s", pPtn);
			strcpy(pPtn, szBuf);
		}
	}

	// 月日設定
	if (nId & PTN_SLA) {
		sprintf(szBuf, "%s/%s", REGEX_MON, REGEX_DAY);
		strcat(pPtn, szBuf);
	} else if (nId & PTN_KNJ) {
		sprintf(szBuf, "%s月%s日", REGEX_MON, REGEX_DAY);
		strcat(pPtn, szBuf);
	} else if (nId & PTN_CSV) {
		sprintf(szBuf, "%s %s%s", REGEX_EGMON, REGEX_DAY, pPtn);
		strcpy(pPtn, szBuf);
	} else {
		strcat(pPtn, REGEX_MON2);
		strcat(pPtn, REGEX_DAY2);
	}

	// 曜日設定
	if (nId & PTN_WEEK) {
		if ((nId & PTN_CSV) == 0) {
			strcat(pPtn, REGEX_WEEK);
		} else {
			sprintf(szBuf, "%s, %s", REGEX_EGWK, pPtn);
			strcpy(pPtn, szBuf);
		}
	}

//	sprintf(szBuf, "^%s$", pPtn);
//	strcpy(pPtn, szBuf);
}

// 日付データの分析
void AnlDate(int nId, char *pDate, DATESTAT *pDateStat)
{
	int year = -1, month = -1, day = -1, wk;
	int nIdx;
	char szFmt[MAX_BUF+1], szComp[MAX_BUF+1];
	char szWeek[MAX_BUF+1], szMon[MAX_BUF+1];

	// 日付情報のパターンID検索
	nIdx = SrchPtnId(nId);
	if (nIdx == -1) return;

	// sscanf用フォーマット生成
	if ((nId & PTN_CSV) == 0) {	// 日本語フォーマット
		sprintf(szFmt, "%s%s%s%s",
		        DateInfo[nIdx].pFmtYear, DateInfo[nIdx].pFmtMonth,
		        DateInfo[nIdx].pFmtDay, DateInfo[nIdx].pFmtWeek);
	} else {
		sprintf(szFmt, "%s%s%s%s",
		        DateInfo[nIdx].pFmtWeek, DateInfo[nIdx].pFmtMonth,
		        DateInfo[nIdx].pFmtDay, DateInfo[nIdx].pFmtYear);
		DelChar(szFmt, szFmt, ',');		// 	カンマ削除
		DelChar(pDate, szComp, ',');	// 	カンマ削除
	}

	// 年月日、曜日の取得
	*szWeek = '\0';
	*szMon = '\0';
	if ((nId & MASK_YMD) == PTN_MD) {
		if ((nId & PTN_CSV) == 0) {	// 日本語フォーマット
			if (nId & MASK_WEEK) {
				sscanf(pDate, szFmt, &month, &day, szWeek);
			} else {
				sscanf(pDate, szFmt, &month, &day);
			}
		} else {
			if (nId & MASK_WEEK) {
				sscanf(szComp, szFmt, szWeek, szMon, &day);
			} else {
				sscanf(szComp, szFmt, szMon, &day);
			}
		}
	} else {
		if ((nId & PTN_CSV) == 0) {	// 日本語フォーマット
			if (nId & MASK_WEEK) {
				sscanf(pDate, szFmt, &year, &month, &day, szWeek);
			} else {
				sscanf(pDate, szFmt, &year, &month, &day);
			}
		} else {
			if (nId & MASK_WEEK) {
				sscanf(szComp, szFmt, szWeek, szMon, &day, &year);
			} else {
				sscanf(szComp, szFmt, szMon, &day, &year);
			}
		}
	}

	if (nId & PTN_CSV) {	// 英語フォーマット
		month = SrchCnvDat(pEgMon, szMon);
	}

	// 不足する年/曜日の補完
	AdjYearWeek(&year, &month, &day, &wk);

	// 年月日、曜日の設定
	pDateStat->year  = year;
	pDateStat->month = month;
	pDateStat->day   = day;
	pDateStat->wk    = wk;
}

// 特定文字の削除
int DelChar(char *pOrg, char *pDst, char cDel)
{
	int nOidx = 0, nDidx = 0;

	while (pOrg[nOidx]) {
		if (pOrg[nOidx] == cDel) {
			if (pOrg[nOidx] & 0x80) {
				nOidx += 2;
			} else {
				++nOidx;
			}
		} else {
			if (pOrg[nOidx] & 0x80) {
				pDst[nDidx++] = pOrg[nOidx++];
				pDst[nDidx++] = pOrg[nOidx++];
			} else {
				pDst[nDidx++] = pOrg[nOidx++];
			}
		}
	}
	pDst[nDidx++] = '\0';

	return 0;
}
 
// 変換用データの位置検索
int SrchCnvDat(char **pCnv, char *pStr)
{
	int i, nIdx = -1;

	for (i = 0; pCnv[i]; ++i) {
		if (!strcmp(pCnv[i], pStr)) {
			nIdx = i;
			break;
		}
	}

	return nIdx;
}

// 日付情報のパターンID検索
int SrchPtnId(int nId)
{
	int nIdx = -1;

	for (int i = 0; DateInfo[i].nId; ++i) {
		if (DateInfo[i].nId == nId) {
			nIdx = i;
			break;
		}
	}

	return nIdx;
}

// 不足する年/曜日の補完
int AdjYearWeek(int *year, int *month, int *day, int *wk)
{
	time_t timer;
	struct tm *local, time_in;
	int nDif, nHund, nCent;

	// 現在時刻を取得
	timer = time(NULL);

	// 地方時に変換
	local = localtime(&timer);

	// 年補完
	if (*year == -1) {
		// -5ヶ月～+6ヶ月の年を設定
		nDif = *month - (local->tm_mon + 1);
		if (-6 < nDif && nDif <= 6) {
			*year = local->tm_year + 1900;
		} else if(*month < (local->tm_mon + 1)) {
			*year = local->tm_year + 1900 + 1;
		} else {
			*year = local->tm_year + 1900 - 1;
		}
	} else if (*year < 100) { // 2桁指定
		// -70年～+30年の年を設定
		nCent = local->tm_year % 100;
		nHund = local->tm_year - nCent;
		nDif = *year - nCent;
		if (-70 < nDif && nDif <= 30) {
			*year += (nHund + 1900);
		} else if (*year < nCent) {
			*year +=  (nHund + 1900 + 100);
		} else {
			*year +=  (nHund + 1900 - 100);
		}
	}

	// 曜日算出
	*wk = GetWeekDay(*year, *month, *day);

	return 0;
}

// 曜日算出
int GetWeekDay(int y, int m, int d)
{
	static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
	y -= m < 3;	// 1月,2月を前年として扱う
	return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}

// 表示パターン順の設定
void SetDspPtn(int nId, DATESTAT *pDateStat)
{
	int nIdx = 0;

	pDateStat->nCurIdx= nIdx;

	switch (nId) {
		case PTN_MD:
			pDateStat->nId[nIdx++] = PTN_MD;
		case PTN_MD   | PTN_SLA:
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_SLA;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_SLA | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_KNJ;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_KNJ | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_SLA;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_SLA | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_KNJ;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_KNJ | PTN_WEEK;
			break;
		case PTN_MD   | PTN_KNJ:
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_KNJ;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_KNJ | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_KNJ;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_KNJ | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_SLA;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_SLA | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_SLA;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_SLA | PTN_WEEK;
			break;
		case PTN_Y2MD:
			pDateStat->nId[nIdx++] = PTN_Y2MD;
		case PTN_Y2MD | PTN_SLA:
			pDateStat->nId[nIdx++] = PTN_Y2MD | PTN_SLA;
			pDateStat->nId[nIdx++] = PTN_Y2MD | PTN_SLA | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_Y2MD | PTN_KNJ;
			pDateStat->nId[nIdx++] = PTN_Y2MD | PTN_KNJ | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_SLA;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_SLA | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_KNJ;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_KNJ | PTN_WEEK;
			break;
		case PTN_Y2MD | PTN_KNJ:
			pDateStat->nId[nIdx++] = PTN_Y2MD | PTN_KNJ;
			pDateStat->nId[nIdx++] = PTN_Y2MD | PTN_KNJ | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_Y2MD | PTN_SLA;
			pDateStat->nId[nIdx++] = PTN_Y2MD | PTN_SLA | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_KNJ;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_KNJ | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_SLA;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_SLA | PTN_WEEK;
			break;
		case PTN_Y4MD:
			pDateStat->nId[nIdx++] = PTN_Y4MD;
		case PTN_Y4MD | PTN_SLA:
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_SLA;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_SLA | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_KNJ;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_KNJ | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_SLA;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_SLA | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_KNJ;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_KNJ | PTN_WEEK;
			break;
		case PTN_Y4MD | PTN_KNJ:
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_KNJ;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_KNJ | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_KNJ;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_KNJ | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_SLA;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_SLA | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_SLA;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_SLA | PTN_WEEK;
			break;
		case PTN_MD   | PTN_SLA | PTN_WEEK:
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_SLA | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_KNJ | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_SLA | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_KNJ | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_SLA;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_KNJ;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_SLA;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_KNJ;
			break;
		case PTN_MD   | PTN_KNJ | PTN_WEEK:
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_KNJ | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_KNJ | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_SLA | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_SLA | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_KNJ;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_KNJ;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_SLA;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_SLA;
			break;
		case PTN_Y4MD | PTN_SLA | PTN_WEEK:
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_SLA | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_KNJ | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_SLA | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_KNJ | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_SLA;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_KNJ;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_SLA;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_KNJ;
			break;
		case PTN_Y4MD | PTN_KNJ | PTN_WEEK:
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_KNJ | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_SLA | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_KNJ | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_SLA | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_KNJ;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_SLA;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_KNJ;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_SLA;
			break;
		case PTN_Y4MD | PTN_CSV | PTN_WEEK:
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_CSV | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_SLA | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_KNJ | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_SLA | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_KNJ | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_SLA;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_KNJ;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_SLA;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_KNJ;
			break;
		case PTN_MD | PTN_CSV | PTN_WEEK:
			pDateStat->nId[nIdx++] = PTN_MD | PTN_CSV | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_SLA | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_KNJ | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_SLA | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_KNJ | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_SLA;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_KNJ;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_SLA;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_KNJ;
			break;
		case PTN_Y4MD | PTN_CSV:
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_CSV;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_SLA;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_SLA | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_KNJ;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_KNJ | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_SLA;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_SLA | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_KNJ;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_KNJ | PTN_WEEK;
			break;
		case PTN_MD   | PTN_CSV:
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_CSV;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_SLA;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_SLA | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_KNJ;
			pDateStat->nId[nIdx++] = PTN_MD   | PTN_KNJ | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_SLA;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_SLA | PTN_WEEK;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_KNJ;
			pDateStat->nId[nIdx++] = PTN_Y4MD | PTN_KNJ | PTN_WEEK;
			break;
	}
	pDateStat->nId[nIdx++] = 0;
}

// 表示用日付文字列の生成
char *MakDspDate(DATESTAT *pDateStat)
{
	int nId, nIdx;
	char szOutput[MAX_BUF+1], szPart[MAX_BUF+1],
		 szBuf[MAX_BUF+1], szFmt[MAX_BUF+1];
	char *pConv;
	char *pWkStr[] = {"日","月","火","水","木","金","土"};

	*szOutput = '\0';
	*szPart = '\0';
	nId = pDateStat->nId[pDateStat->nCurIdx];

	// 日付情報のパターンID検索
	nIdx = SrchPtnId(nId);
	if (nIdx == -1) return 0;

	// 年設定
	if (nId & PTN_Y4MD) {
		sprintf(szPart, DateInfo[nIdx].pFmtYear, pDateStat->year);
	} else if (nId & PTN_Y2MD) {
		sprintf(szPart, DateInfo[nIdx].pFmtYear, pDateStat->year % 100);
	}
	strcat(szOutput, szPart);

	// 月日設定
	sprintf(szFmt, "%s%s", DateInfo[nIdx].pFmtMonth, DateInfo[nIdx].pFmtDay);
	if ((nId & PTN_CSV) == 0) {	// 日本語フォーマット
		sprintf(szPart, szFmt, pDateStat->month, pDateStat->day);
		strcat(szOutput, szPart);
	} else {
		sprintf(szPart, szFmt, pEgMon[pDateStat->month], pDateStat->day);
		sprintf(szBuf, "%s%s", szPart, szOutput);
		strcpy(szOutput, szBuf);
	}

	// 曜日設定
	if (nId & PTN_WEEK) {
		if ((nId & PTN_CSV) == 0) {	// 日本語フォーマット
			sprintf(szPart, DateInfo[nIdx].pFmtWeek, pWkStr[pDateStat->wk]);
			strcat(szOutput, szPart);
		} else {
			sprintf(szPart, DateInfo[nIdx].pFmtWeek, pEgWk[pDateStat->wk]);
			sprintf(szBuf, "%s%s", szPart, szOutput);
			strcpy(szOutput, szBuf);
		}
	}

	return strdup(szOutput);
}
