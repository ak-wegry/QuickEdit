/*
 * ���t�Ɋւ��鏈��
 */

#include <windows.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <pcreposi.h>
#include "DateCtrl.h"

// �ő�l
#define MAX_BUF		1024

// ���t�p�^�[��ID
#define PTN_MD		0x00000001	// mmdd
#define PTN_Y4MD	0x00000002	// yyyymmdd
#define PTN_Y2MD	0x00000004	// yymmdd
#define PTN_WEEK	0x00000010	// (week)
#define PTN_SLA		0x00000100	// yyyy/mm/dd
#define PTN_KNJ		0x00000200	// yyyy�Nmm��dd��
#define PTN_CSV		0x00000400	// Sunday|..., January|... dd, yyyy

// ���t�p�^�[��MASK
#define MASK_YMD	0x0000000F
#define MASK_WEEK 	0x000000F0
#define MASK_DLM	0x00000F00

// ���t�̏�ԂɊւ�����
static DATESTAT DateStat;

// ���t�����o/�\��������
typedef struct {
	int nId;			// ���o/�\��������t�̃p�^�[��ID
	char *pFmtYear;		// �X�L����/�\������N�̃t�H�[�}�b�g
	char *pFmtMonth;	// �X�L����/�\�����錎�̃t�H�[�}�b�g
	char *pFmtDay;		// �X�L����/�\��������̃t�H�[�}�b�g
	char *pFmtWeek;		// �X�L����/�\������j���̃t�H�[�}�b�g
} DATEINFO;

static DATEINFO DateInfo[] = {
	{ PTN_Y4MD | PTN_KNJ | PTN_WEEK, "%d�N"  , "%d��", "%d��", "(%s)" },
	{ PTN_Y2MD | PTN_KNJ | PTN_WEEK, "%02d�N", "%d��", "%d��", "(%s)" },
	{ PTN_MD   | PTN_KNJ | PTN_WEEK, ""      , "%d��", "%d��", "(%s)" },
	{ PTN_Y4MD | PTN_SLA | PTN_WEEK, "%d/"   , "%d/" , "%d"  , "(%s)" },
	{ PTN_Y2MD | PTN_SLA | PTN_WEEK, "%02d/" , "%d/" , "%d"  , "(%s)" },
	{ PTN_MD   | PTN_SLA | PTN_WEEK, ""      , "%d/" , "%d"  , "(%s)" },
	{ PTN_Y4MD | PTN_KNJ           , "%d�N"  , "%d��", "%d��", ""     },
	{ PTN_Y2MD | PTN_KNJ           , "%02d�N", "%d��", "%d��", ""     },
	{ PTN_MD   | PTN_KNJ           , ""      , "%d��", "%d��", ""     },
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

// �p�ꌎ,�j���̕ϊ��p�f�[�^
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

// ���t�ϊ�
char *AdjDate(char *pText)
{
	int nPtnId, nMchStt, nMchEnd;
	char *pDate, *pConv, szMchStr[MAX_BUF+1];
	DATESTAT InpStat;

	// ���t�p�^�[���̃`�F�b�N
	nPtnId = ChkDatePtn(pText, &nMchStt, &nMchEnd);
	if (nPtnId <= 0) {
		InitDateStat();	// ���t��Ԃ̏�����
		return 0;
	}

	// ��v�������t�f�[�^�擾
	strncpy(szMchStr, pText + nMchStt, nMchEnd - nMchStt);
	szMchStr[nMchEnd - nMchStt] = '\0';

	// ���t�f�[�^�̕���
	AnlDate(nPtnId, szMchStr, &InpStat);
	
	if (CmpDateStat(&DateStat, &InpStat)) { // ���t��Ԃ̔�r
		// �s��v
		// ���t��Ԃ̏�����
		InitDateStat();
		// ���t��Ԃ̃R�s�[
		CpyDateStat(&DateStat, &InpStat);
		// �\���p�^�[�����̐ݒ�
		SetDspPtn(nPtnId, &DateStat);
	} else {
		// ��v
		// ���̕\���p�^�[���ݒ�
		NxtDateStat(&DateStat);
	}

	// �\���p���t������̐���
	pDate = MakDspDate(&DateStat);
	if (pDate == NULL) return 0;

	// ���͕����Ɠ����ꍇ�͎��̃p�^�[���ōĐ���
	if (!strcmp(pDate, szMchStr)) {
		free(pDate);
		NxtDateStat(&DateStat);
		pDate = MakDspDate(&DateStat);
		if (pDate == NULL) return 0;
	}

	// ���t�f�[�^������u����
	pConv = (char *)malloc(strlen(pText) + strlen(pDate) + 1);
	strncpy(pConv, pText, nMchStt);
	strcpy(pConv + nMchStt, pDate);
	strcat(pConv, pText + nMchEnd);
	free(pDate);
	
	return pConv;
}

// ���t��Ԃ̏�����
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

// ���t��Ԃ̔�r
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

// ���t��Ԃ̃R�s�[
void CpyDateStat(DATESTAT *pStat1, DATESTAT *pStat2)
{
    pStat1->year  = pStat2->year;
    pStat1->month = pStat2->month;
    pStat1->day   = pStat2->day;
    pStat1->wk    = pStat2->wk;
}

// ���̕\���p�^�[���ݒ�
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

// ���t�p�^�[���̃`�F�b�N
int ChkDatePtn(char *pDate, int *pnMchStt, int *pnMchEnd)
{
	int i, nSize;
	char szChkPtn[MAX_BUF+1];
	regex_t regexBuf;
	regmatch_t ptnMatch[5]; // ���ʂ��i�[����

	nSize = sizeof(ptnMatch) / sizeof(regmatch_t);
	for (i = 0; DateInfo[i].nId; ++i) {
		// ���K�\���̃p�^�[���쐬
		MakChkPtn(DateInfo[i].nId, szChkPtn);
		// ���K�\���̃R���p�C��
		if (regcomp(&regexBuf, szChkPtn, REG_NEWLINE) != 0) {
//			printf("regex compile failed.\n");
			return -1;
		}

		// ���K�\���Ƃ̈�v
		if (regexec(&regexBuf, pDate, nSize, ptnMatch, 0) == 0) {
//printf("*** Match Pattern = %s\n", szChkPtn);
//printf("*** start = %d, end = %d\n", ptnMatch[0].rm_so, ptnMatch[0].rm_eo);
			// ��v�����J�n/�I���ʒu�̐ݒ�
			*pnMchStt = ptnMatch[0].rm_so;
			*pnMchEnd = ptnMatch[0].rm_eo;
			regfree(&regexBuf);
			return DateInfo[i].nId;	// ��v�����p�^�[��ID
		}

		regfree(&regexBuf);
	}

	return 0;	// �s��v
}

// ���K�\���̃p�^�[���쐬
void MakChkPtn(int nId, char *pPtn)
{
#define REGEX_MON	"(1[0-2]|0?[1-9])"
#define REGEX_MON2	"(1[0-2]|0[1-9])"
#define REGEX_DAY	"(3[0-1]|[1-2]\\d|0?[1-9])"
#define REGEX_DAY2	"(3[0-1]|[1-2]\\d|0[1-9])"
#define REGEX_WEEK	"\\\((��|��|��|��|��|��|�y)\\\)"
#define REGEX_EGMON	"(January|February|March|April|May|June|July|August|September|October|November|December)"
#define REGEX_EGWK	"(Sunday|Monday|Tuesday|Wednesday|Thursday|Friday|Saturday|)"
	char szBuf[MAX_BUF+1];

	*pPtn = '\0';

	// �N�ݒ�
	if (nId & PTN_Y4MD) {
		strcat(pPtn, "\\d{4}");
	} else 	if (nId & PTN_Y2MD) {
		strcat(pPtn, "\\d{2}");
	}

	if (nId & PTN_Y4MD || nId & PTN_Y2MD) {
		if (nId & PTN_SLA) {
			strcat(pPtn, "/");
		} else if (nId & PTN_KNJ) {
			strcat(pPtn, "�N");
		} else if (nId & PTN_CSV) {
			sprintf(szBuf, ", %s", pPtn);
			strcpy(pPtn, szBuf);
		}
	}

	// �����ݒ�
	if (nId & PTN_SLA) {
		sprintf(szBuf, "%s/%s", REGEX_MON, REGEX_DAY);
		strcat(pPtn, szBuf);
	} else if (nId & PTN_KNJ) {
		sprintf(szBuf, "%s��%s��", REGEX_MON, REGEX_DAY);
		strcat(pPtn, szBuf);
	} else if (nId & PTN_CSV) {
		sprintf(szBuf, "%s %s%s", REGEX_EGMON, REGEX_DAY, pPtn);
		strcpy(pPtn, szBuf);
	} else {
		strcat(pPtn, REGEX_MON2);
		strcat(pPtn, REGEX_DAY2);
	}

	// �j���ݒ�
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

// ���t�f�[�^�̕���
void AnlDate(int nId, char *pDate, DATESTAT *pDateStat)
{
	int year = -1, month = -1, day = -1, wk;
	int nIdx;
	char szFmt[MAX_BUF+1], szComp[MAX_BUF+1];
	char szWeek[MAX_BUF+1], szMon[MAX_BUF+1];

	// ���t���̃p�^�[��ID����
	nIdx = SrchPtnId(nId);
	if (nIdx == -1) return;

	// sscanf�p�t�H�[�}�b�g����
	if ((nId & PTN_CSV) == 0) {	// ���{��t�H�[�}�b�g
		sprintf(szFmt, "%s%s%s%s",
		        DateInfo[nIdx].pFmtYear, DateInfo[nIdx].pFmtMonth,
		        DateInfo[nIdx].pFmtDay, DateInfo[nIdx].pFmtWeek);
	} else {
		sprintf(szFmt, "%s%s%s%s",
		        DateInfo[nIdx].pFmtWeek, DateInfo[nIdx].pFmtMonth,
		        DateInfo[nIdx].pFmtDay, DateInfo[nIdx].pFmtYear);
		DelChar(szFmt, szFmt, ',');		// 	�J���}�폜
		DelChar(pDate, szComp, ',');	// 	�J���}�폜
	}

	// �N�����A�j���̎擾
	*szWeek = '\0';
	*szMon = '\0';
	if ((nId & MASK_YMD) == PTN_MD) {
		if ((nId & PTN_CSV) == 0) {	// ���{��t�H�[�}�b�g
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
		if ((nId & PTN_CSV) == 0) {	// ���{��t�H�[�}�b�g
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

	if (nId & PTN_CSV) {	// �p��t�H�[�}�b�g
		month = SrchCnvDat(pEgMon, szMon);
	}

	// �s������N/�j���̕⊮
	AdjYearWeek(&year, &month, &day, &wk);

	// �N�����A�j���̐ݒ�
	pDateStat->year  = year;
	pDateStat->month = month;
	pDateStat->day   = day;
	pDateStat->wk    = wk;
}

// ���蕶���̍폜
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
 
// �ϊ��p�f�[�^�̈ʒu����
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

// ���t���̃p�^�[��ID����
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

// �s������N/�j���̕⊮
int AdjYearWeek(int *year, int *month, int *day, int *wk)
{
	time_t timer;
	struct tm *local, time_in;
	int nDif, nHund, nCent;

	// ���ݎ������擾
	timer = time(NULL);

	// �n�����ɕϊ�
	local = localtime(&timer);

	// �N�⊮
	if (*year == -1) {
		// -5�����`+6�����̔N��ݒ�
		nDif = *month - (local->tm_mon + 1);
		if (-6 < nDif && nDif <= 6) {
			*year = local->tm_year + 1900;
		} else if(*month < (local->tm_mon + 1)) {
			*year = local->tm_year + 1900 + 1;
		} else {
			*year = local->tm_year + 1900 - 1;
		}
	} else if (*year < 100) { // 2���w��
		// -70�N�`+30�N�̔N��ݒ�
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

	// �j���Z�o
	*wk = GetWeekDay(*year, *month, *day);

	return 0;
}

// �j���Z�o
int GetWeekDay(int y, int m, int d)
{
	static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
	y -= m < 3;	// 1��,2����O�N�Ƃ��Ĉ���
	return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}

// �\���p�^�[�����̐ݒ�
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

// �\���p���t������̐���
char *MakDspDate(DATESTAT *pDateStat)
{
	int nId, nIdx;
	char szOutput[MAX_BUF+1], szPart[MAX_BUF+1],
		 szBuf[MAX_BUF+1], szFmt[MAX_BUF+1];
	char *pConv;
	char *pWkStr[] = {"��","��","��","��","��","��","�y"};

	*szOutput = '\0';
	*szPart = '\0';
	nId = pDateStat->nId[pDateStat->nCurIdx];

	// ���t���̃p�^�[��ID����
	nIdx = SrchPtnId(nId);
	if (nIdx == -1) return 0;

	// �N�ݒ�
	if (nId & PTN_Y4MD) {
		sprintf(szPart, DateInfo[nIdx].pFmtYear, pDateStat->year);
	} else if (nId & PTN_Y2MD) {
		sprintf(szPart, DateInfo[nIdx].pFmtYear, pDateStat->year % 100);
	}
	strcat(szOutput, szPart);

	// �����ݒ�
	sprintf(szFmt, "%s%s", DateInfo[nIdx].pFmtMonth, DateInfo[nIdx].pFmtDay);
	if ((nId & PTN_CSV) == 0) {	// ���{��t�H�[�}�b�g
		sprintf(szPart, szFmt, pDateStat->month, pDateStat->day);
		strcat(szOutput, szPart);
	} else {
		sprintf(szPart, szFmt, pEgMon[pDateStat->month], pDateStat->day);
		sprintf(szBuf, "%s%s", szPart, szOutput);
		strcpy(szOutput, szBuf);
	}

	// �j���ݒ�
	if (nId & PTN_WEEK) {
		if ((nId & PTN_CSV) == 0) {	// ���{��t�H�[�}�b�g
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
