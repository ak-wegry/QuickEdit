/*
 * 文字列操作に関する処理
 */

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "OpeString.h"

/* SJISの漢字判定 */
int IsKanji(unsigned char c)
{
	if((c >= 0x80 && c < 0xA0) || c >= 0xE0) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/* 数値文字列の判定 */
int IsNumStr(char *p)
{
	while (*p) {
		if (*p < '0' || '9' < *p)	return FALSE;
		++p;
	}
	return TRUE;
}

/* 文字列から文字郡が見つかった位置を求める */
char *StrPbrk(char *s1, char *s2)
{
	char *ret;

	while (*s1) {
		ret = StrChr(s2, s1);
		if (ret != NULL) {
			ret = s1;
			break;
		}

		if (IsKanji(*s1)) {
			s1 += 2;
		} else {
			++s1;
		}
	}

	return ret;
}

/* 文字の検索 */
char *StrChr(char *s, char *c)
{
	char *ret = NULL;

	while (*s) {
		if (IsKanji(*s)) {
			if (!memcmp(s, c, 2)) {
				ret = s;
				break;
			} else { 
				s += 2;
			}
		} else {
			if (*s == *c) {
				ret = s;
				break;
			} else {
				++s;
			}
		}
	}

	return ret;
}

// 文字列から文字列の位置検索
// Args:
// 1	char *	検索対象文字列
// 2	char *	検索する文字列
// Return:		発見:ポインタ、未発見:NULL
char *StrStr(char *pOrg, char *pSrch)
{
	int nLen;	// 検索文字列の長さ
	char *pFnd;	// 発見した位置

	nLen = strlen(pSrch);
	pFnd = NULL;
	if (*pSrch) {
		while (*pOrg) {
			if (!memcmp(pOrg, pSrch, nLen)) {
				// 発見した場合は位置を戻り値へ設定
				pFnd = pOrg;
				break;
			} else {
				// 未発見の場合は参照位置更新
				if (IsKanji(*pOrg)) {
					pOrg += 2;
				} else {
					pOrg += 1;
				}
			}
		}
	} else {
		// 検索する文字がNULL文字列の場合は、
		// 検索対象文字列がNULL文字列のみ一致と判定
		if (*pOrg == '\0') pFnd = pOrg;
	}

	return pFnd;
}

/* CommandLineファイル名の分解 */
int GetElement(char *pOrg, int nIdx, char **pArg)
{
	static char *pSave = NULL;
	static int nPos[4096];
	static int nMax = 0;
	int i, stt_f;
	char cSrch;

	if (pOrg != NULL) {
		if (pSave != NULL) {
			delete pSave;
			nMax = 0;
		}
		pSave = strdup(pOrg);

		i = 0;
		stt_f = 0;
		while (pSave[i]) {
			if (stt_f == 0) {
				if(pSave[i] == '"') {
					++i;
					nPos[++nMax] = i;
					cSrch = '"';
					stt_f = 1;
				} else if(pSave[i] == ' ') {
					++i;
				} else {
					nPos[++nMax] = i;
					cSrch = ' ';
					stt_f = 1;
				}
			} else {
				if (pSave[i] == cSrch) {
					pSave[i] = '\0';
					++i;
					stt_f = 0;
				} else {
					if (IsKanji(pSave[i])) {
						i += 2;
					} else {
						++i;
					}
				}
			}
		}
		return nMax;
	} else {
		if (nIdx <= nMax) {
			*pArg = pSave + nPos[nIdx];
			return 1;
		} else {
			*pArg = NULL;
			return 0;
		}
	}
}

/* 特定文字のカウント */
int CountChar(char *p, char c)
{
	int n = 0;

	while (*p) {
		if (*p == c) ++n;
		if (IsKanji(*p)) {
			p += 2;
		} else {
			++p;
		}
	}

	return n;
}

/* n番目の特定文字位置の検索 */
int SrchCharPos(char *p, char c, int idx)
{
	int i = 0, n = 0, find_f = 0;

	while (p[i] && find_f == 0) {
		if (p[i] == c) {
			++n;
			if (idx <= n) {
				find_f = 1;
				break;
			}
		}

		if (IsKanji(p[i])) {
			i += 2;
		} else {
			++i;
		}
	}

	if (find_f) {
		return i;
	} else {
		return -1;
	}
}

/* 文字列の置き換え */
int SubStr(char *pStr, char *pSrch, char *pRep, int nCnt)
{
	int	nIdx = 0, nDone = 0;
	int nSrchLen, nRepLen;
	char buf[4096+1];

	nSrchLen = strlen(pSrch);
	nRepLen = strlen(pRep);
	while (pStr[nIdx]) {
		/* 置き換え文字の検索 */
		if(!strncmp(pStr + nIdx, pSrch, nSrchLen)) {
			/* 文字の置換え */
			strcpy(buf, pStr + nIdx + nSrchLen);
			strcpy(pStr + nIdx, pRep);
			strcpy(pStr + nIdx + nRepLen, buf);
			++nDone;
			/* 置換え回数のチェック */
			if (nDone == nCnt)	break;
			/* 検索位置の更新 */
			nIdx += (nRepLen + 1);
		} else {
			/* 検索位置の更新 */
			if (IsKanji(pStr[nIdx])) {
				nIdx += 2;
			} else {
				++nIdx;
			}
		}
	}
	return(nDone);
}

/* 後ろからN文字目のポインタ */
char *TailStr(char *pStr, int nCnt)
{
	int nLen;
	char *pTail;

	nLen = strlen(pStr);
	if (nCnt <= nLen) {
		pTail = pStr + nLen - nCnt;
	} else {
		pTail = pStr;
	}

	return pTail;
}

/* 文字列比較(大/小文字区別なし) */
int CmpStr(unsigned char *s1, unsigned char *s2)
{
	int nStat;
	unsigned char bLow1, bLow2;
	char bKanji = 0;

	while (*s1 && *s2) {
		if (isalpha(*s1) && isalpha(*s2) && bKanji == 0) {
			bLow1 = *s1 & 0x20;
			bLow2 = *s2 & 0x20;
			nStat = (int)(bLow1 - bLow2);
		} else {
			nStat = (int)(*s1 - *s2);
		}
		if (nStat != 0) break;

		if (IsKanji(*s1) && IsKanji(*s2)) {
			bKanji = 1;
		} else {
			bKanji = 0;
		}
		++s1;
		++s2;
	}

	return nStat;
}

/* ワイルドカード文字列判定関数 */
int WildSrch(char *pOrgStr, char *pWldStr, int *lpStart, int *lpLength)
{
#define INC_POS(x)	IsKanji(*x) ? x+=2 : x++

#define NOMATCH (-1)
#define SEARCH	0
#define BEGIN	1
#define MATCH	2
	int		nStat,			// 状態表示 (0:検査中 1:一致中 2:一致 -1:不一致)
			nMchPos = 0,	// 一致開始位置
			bMchFlg,		// 一致状態
			bSpcFlg,		// 特殊文字表示
			nSubTop,		// 再帰の時の開始位置
			nSubLen;		// 再帰の時の文字列の長さ
	char	*pOrgTop,		// 検査文字列の先頭位置
			*pWldTop;		// ワイルドカード先頭位置

	// 初期状態設定
	if (*pOrgStr) {
		nStat = SEARCH;
	} else {
		nStat = NOMATCH;
	}
	// 検査文字列，ワイルドカードの先頭位置保存
	pOrgTop = pOrgStr;
	pWldTop = pWldStr;

	while (nStat == SEARCH || nStat == BEGIN) { // 判定終了まで
		// 特殊文字表示クリア
		bSpcFlg = 0;
		switch (*pWldStr) { // ワイルドカード文字
			case '*':
				// ワイルドカード文字を１つ進める
				++pWldStr;
				if (*pWldStr == '\0') { // '*'は最終文字か？
					// 検査文字列の一致開始位置を設定
					*lpStart = nMchPos;
					*lpLength = strlen(pOrgTop + nMchPos);
					// 状態＝一致
					nStat = MATCH;
				} else {
					if (WildSrch(pOrgStr, pWldStr, &nSubTop, &nSubLen)==TRUE) {
						// 検査文字列の一致開始位置を設定
						*lpStart = nMchPos;
						*lpLength = (pOrgStr - pOrgTop) - nMchPos 
						             + nSubTop + nSubLen;
						// 状態＝一致
						nStat = MATCH;
					}
				}
				break;

			case '?':
				if (*pOrgStr == '\0') { // 検査文字列は終了か？
					// 状態表示＝不一致
					nStat = NOMATCH;
				} else {
					// 状態表示＝一致中
					nStat = BEGIN;
				}
				// 検査文字とワイルドカード文字を１つ進める
				INC_POS(pOrgStr);
				++pWldStr;
				break;

			case '\0': // Null
				if (nStat == SEARCH) { // 状態は検査中か？
					// 状態＝不一致
					nStat = NOMATCH;
				} else {
					// 検査文字列の一致開始位置を設定
					*lpStart = nMchPos;
					*lpLength = (pOrgStr - pOrgTop) - nMchPos;
					// 状態＝一致
					nStat = MATCH;
				}
				break;

			case '\\':
				// 特殊文字表示を設定する
				bSpcFlg = 1;
				; // No BREAK

			default: // 通常文字
				// 文字一致判定
				bMchFlg = FALSE;
				if (IsKanji(*pOrgStr)) { // 漢字か？
					if (*pOrgStr == *(pWldStr + bSpcFlg) &&
						*(pOrgStr + 1) == *(pWldStr + bSpcFlg + 1)) {
						bMchFlg = TRUE;
					}
				} else {
					if (*pOrgStr == *(pWldStr + bSpcFlg)) bMchFlg = TRUE;
				}

				if (bMchFlg) { // 文字一致か？
					// 状態が検査中であれば、状態＝一致中
					if (nStat == SEARCH) nStat = BEGIN;
					// 検査文字とワイルドカード文字を１つ進める
					INC_POS(pOrgStr);
					pWldStr += bSpcFlg;
					INC_POS(pWldStr);
				} else {
					if (nStat == BEGIN) { // 状態は一致中か？
						// 状態＝検査中
						nStat = SEARCH;
						// 検査文字を一致開始位置に戻す
						pOrgStr = pOrgTop + nMchPos;
						// ワイルドカード文字を先頭に戻す
						pWldStr = pWldTop;
					}
					// 検査文字を１つ進める
					INC_POS(pOrgStr);
				}
				break;
		}

		if (nStat == SEARCH) { // 状態は検査中か？
			// 一致開始位置を検査位置にする
			nMchPos = pOrgStr - pOrgTop;
			if (*pOrgStr == '\0') { // 検査文字終了か？
				// 状態＝不一致
				nStat = NOMATCH;
			}
		}
	}

	if (nStat == MATCH) { // 一致か？
		return TRUE; // 一致
	} else {
		return FALSE; // 不一致
	}
}

/* 文字列の分解 */
int StrSplit(char *pStr, char *pDlmt, int nFmt, STRLIST *pList)
{
#define PID_SRCH_DLMT	0		// デリミタ検索中
#define PID_SRCH_CHAR	1		// デリミタ以外検索中
#define PID_SRCH_QUOTE	2		// ダブルクォート検索中
	int nIdx = 0,				// 分解する文字列の参照位置
		nTop,					// 分解する各カラムの先頭位置
		nLen,					// カラム文字列の長さ
		nCnt = 0,				// 分解したデリミタ以外のデータ数
		nDLen;					// デリミタの長さ
	int nPid = PID_SRCH_DLMT,	// 処理種別
		nRetPid,				// ダブルクォート発見時の処理種別
		nAdjUpd = 0;			// 参照位置更新の補正バイト数
	int nAddCnt = 0,			// リストへの出力データ数
		bEndFlg = FALSE;		// 分解処理の終了表示
	char *pRet, *pBuf, *pOrg, *pFnd, szNull[2];

	pList->Clear();

	if ((nFmt & SPLIT_MULTI) == 0) { // 1固まりのデリミタで分解
		nDLen = strlen(pDlmt);
	} else {
		nDLen = 1;
	}

	nTop = nIdx;
	while (bEndFlg == FALSE) {
		if ((nFmt & SPLIT_MULTI) == 0) { // 1固まりのデリミタで分解
			pRet = NULL;
			if (pStr[nIdx] == '\0' ||
			    strncmp(pStr + nIdx, pDlmt, nDLen) == 0) pRet = pDlmt;
		} else {
			pRet = strchr(pDlmt, pStr[nIdx]);
		}

		if (nPid == PID_SRCH_DLMT) { // デリミタ検索中？
			if (pStr[nIdx] == '\"' && (nFmt & SPLIT_EXCEL)) { // EXCEL形式でダブルクォート？
				// 処理種別保存
				nRetPid = nPid;
				// ダブルクォートを検索する
				nPid = PID_SRCH_QUOTE;
			} else if (pRet) { // デリミタ？
				if (pStr[nIdx] == '\0') { // 文字列の最後？
					nAddCnt = 1;
					++nCnt;
				} else if ((nFmt & SPLIT_NONE) || (nFmt & SPLIT_SEP)) { // 文字列のみ or 文字列とデリミタに分解
					nAddCnt = 1;
					nAdjUpd = nDLen - 1;
				} else { // デリミタ付き文字列に分解
					nAdjUpd = nDLen - 1;
				}

				// デリミタ以外を検索する
				nPid = PID_SRCH_CHAR;
			}
		} else if (nPid == PID_SRCH_CHAR) { // デリミタ以外検索中
			if (pRet == NULL) { // デリミタ以外か？
				if (nFmt & SPLIT_NONE) { // 文字列のみに分解？
					nTop = nIdx;
				} else {
					nAddCnt = 1;
				}

				// デリミタを検索する
				nPid = PID_SRCH_DLMT;
				++nCnt;

				// EXCEL形式でダブルクォートの処理
				if (pStr[nIdx] == '\"' && (nFmt & SPLIT_EXCEL)) {
					// 処理種別保存
					nRetPid = nPid;
					// ダブルクォートを検索する
					nPid = PID_SRCH_QUOTE;
				}
			} else {
				if ((nFmt & SPLIT_MULTI) == 0) { // 1固まりのデリミタで分解
					if (nFmt & SPLIT_NONE) {
						nTop = nIdx;
						nAddCnt = 1;
					} else if (nFmt & SPLIT_SEP) { // 文字列とデリミタに分解
						nAddCnt = 2;
					} else {
						nAddCnt = 1;
					}
					nAdjUpd = nDLen - 1;
					++nCnt;
				}
			}
		} else if (nPid == PID_SRCH_QUOTE) { // ダブルクォート検索中
			if (pStr[nIdx] == '\"') { // ダブルクォート？
				if (pStr[nIdx+1] == '\"') { // 次の文字はダブルクォート？
					// 参照位置を1バイト余分に更新
					nAdjUpd = 1;
				} else {
					// 処理種別を元に戻す
					nPid = nRetPid;
				}
			}
		}

		// 行単位の文字列を登録
		if (nAddCnt) {
			nLen = nIdx - nTop;
			pBuf = (char *)malloc(nLen + 1);
			memcpy(pBuf, pStr + nTop, nLen);
			pBuf[nLen] = '\0';

			if (nFmt & SPLIT_EXCEL) { // EXCEL形式？
				// 前後のダブルクォート削除
				if (*pBuf == '\"' && pBuf[nLen-1] == '\"') {
					pBuf[nLen-1] = '\0';
					strcpy(pBuf, pBuf + 1);;

					// ダブルクォート2つ→1つへ変換
					pOrg = pBuf;
					pFnd = strstr(pOrg, "\"\"");
					while (pFnd) {
						strcpy(pFnd, pFnd + 1);
						pOrg = pFnd + 1;
						pFnd = strstr(pOrg, "\"\"");
					}
				}
			}

			pList->AddString(pBuf);
			free(pBuf);

			nTop = nIdx;
			if (nAddCnt == 2) {
				szNull[0] = '\0';
				pList->AddString(szNull);
			}
			nAddCnt = 0;
		}

		if (pStr[nIdx]) {
			// 参照位置更新
			if (IsKanji(pStr[nIdx])) {
				nIdx += 2;
			} else {
				nIdx = nIdx + 1 + nAdjUpd;
				nAdjUpd = 0;
			}
		} else {
			bEndFlg = TRUE;
		}
	}

	return nCnt;
}

/* ワイルドカード文字による文字列の置き換え */
char *WildReplace(char *pStr, char *pSrch, char *pRep)
{
	int	nIdx = 0, nStat;
	int nStart, nMchLen;
	STRLIST ConvList;
	char *pBuf;

	nStat = WildSrch(pStr + nIdx, pSrch, &nStart, &nMchLen);
	while (nStat) {
		ConvList.AddString(pStr + nIdx, nStart);
		ConvList.AddString(pRep);
//printf("nIdx = %d, nStart = %d, nMchLen = %d\n", nIdx, nStart, nMchLen);
		nIdx += (nStart + nMchLen);

		nStat = WildSrch(pStr + nIdx, pSrch, &nStart, &nMchLen);
	}
	ConvList.AddString(pStr + nIdx);
//printf("rest = /%s/\n", pStr + nIdx);

	return ConvList.MergeAllStr("");
}

/* 単独改行(直前が0x0dでない0x0a)の置き換え */
char *OnlyLfReplace(char *pStr, char *pRep)
{
	int	nIdx = 0, nStat;
	int nStart, nMchLen;
	BOOL bMchFlg;
	STRLIST ConvList;
	char *pBuf, *pSrch;

	pSrch = "\n";
	nStat = WildSrch(pStr + nIdx, pSrch, &nStart, &nMchLen);
	while (nStat) {
		// 単独改行判定
		bMchFlg = TRUE;
		if ((nIdx + nStart) > 0) {
			if (*(pStr + nIdx + nStart - 1) == '\r') {
				bMchFlg = FALSE;
			}
		}

		// 単独改行の置換え
		ConvList.AddString(pStr + nIdx, nStart);
		if (bMchFlg == TRUE) {
			ConvList.AddString(pRep);
		} else {
			ConvList.AddString(pSrch);
		}
		nIdx += (nStart + nMchLen);

		nStat = WildSrch(pStr + nIdx, pSrch, &nStart, &nMchLen);
	}
	ConvList.AddString(pStr + nIdx);

	return ConvList.MergeAllStr("");
}


#if DEBUG
main(int argc, char **argv)
{
	char *id, *p1, *p2, *p3, *p;
	int stt, len, ret;
	STRLIST List;

	id = (argc > 1) ? argv[1] : "";
	p1 = (argc > 2) ? argv[2] : "";
	p2 = (argc > 3) ? argv[3] : "";
	p3 = (argc > 4) ? argv[4] : "";

	if (!strcmp(id, "srch")) {
		ret = WildSrch(p1, p2, &stt, &len);
		printf("ret = %d, start = %d, len = %d\n", ret, stt, len);
	} else if (!strcmp(id, "replace")) {
		p = WildReplace(p1, p2, p3);
		printf("result = /%s/\n", p);
	} else if (!strcmp(id, "split")) {
		ret = StrSplit(p1, p2, atoi(p3), &List);
		printf("ret = %d\n", ret);
		for (int i = 0; i < List.GetMaxCount(); ++i) {
			printf("%d : /%s/\n", i, List.GetString(i));
		}
	}
}
#endif
