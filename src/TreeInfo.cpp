/*
 * TREEINFOクラスの定義
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TreeInfo.h"

// -----------------------------------------------------------------------------
// TREEINFOクラスの定義

// コンストラクタ
TREEINFO::TREEINFO()
{
	m_pTop = NULL;
	m_nMaxDepth = 0;
}

// デストラクタ
TREEINFO::~TREEINFO()
{
	FreeData(m_pTop);
}

// 管理データの解放
void TREEINFO::FreeData(TREEMNG *pData)
{
	TREEMNG *pNext, *pChild;

	if (pData != NULL) {
		if (pData->pName != NULL) free(pData->pName);

		pNext = pData->pNext;
		pChild = pData->pChild;
		free(pData);

		if (pChild != NULL) FreeData(pChild);
		if (pNext != NULL) FreeData(pNext);
	}
}

// 最大階層数
int TREEINFO::MaxDepth()
{
	return m_nMaxDepth;
}

// ファイル情報の登録
int TREEINFO::RegFile(char *pFile)
{
	int nLinMax, nNamMax, nLayer, i, j;
	char *pLine, *pName, szBuf[MAX_PATH+3];
	STRLIST ClpLine, PthName;
	TREEMNG *pCurDat, *pPreDat, *pRegDat;

	// 行毎に分解して処理
	StrSplit(pFile, "\r\n", SPLIT_NONE | SPLIT_MULTI, &ClpLine);
	nLinMax = ClpLine.GetMaxCount();
	for (i = 0; i < nLinMax; ++i) {
		pLine = ClpLine.GetString(i);
		if (*pLine != '\0') {
			pCurDat = m_pTop;
			pPreDat = NULL;
			nLayer = 0;

			// パス名毎に分解して処理
			StrSplit(pLine, "\\", SPLIT_NONE | SPLIT_MULTI, &PthName);
			nNamMax = PthName.GetMaxCount();
			for (j = 0; j < nNamMax; ++j) {
				pName = PthName.GetString(j);
				if (j == 0 && *pName == '\0') {
					// 先頭が'\\'の場合の処理
					++j;
					pName = PthName.GetString(j);
					sprintf(szBuf, "\\\\%s", pName);
					pName = szBuf;
				}

				// 管理データ登録
				++nLayer;
//printf("*** RegData(%08X, %08X, %s, %d)\n", pCurDat, pPreDat, pName, nLayer);
				pRegDat = RegData(pCurDat, pPreDat, pName, nLayer);
				if (m_pTop == NULL) m_pTop = pRegDat;
				pPreDat = pRegDat;
				pCurDat = pPreDat->pChild;
			}

			// 階層数の更新
			if (m_nMaxDepth < nLayer) m_nMaxDepth = nLayer;
		}
		PthName.Clear();
	}

	return 0;
}

// 管理データの登録
TREEMNG *TREEINFO::RegData(TREEMNG *pData, TREEMNG *pParent, char *pName, int nLayer)
{
	int fEnd = FALSE;
	TREEMNG *pPre = NULL, *pNew;

	while (fEnd == FALSE) {
		if (pData != NULL) {
			if (!strcmp(pData->pName, pName)) {
				fEnd = TRUE;
			} else {
				pPre= pData;
				pData = pPre->pNext;
			}
		} else {
			pNew = (TREEMNG *)malloc(sizeof(TREEMNG));
			pNew->nLayer = nLayer;
			pNew->pName = strdup(pName);
			pNew->pNext = NULL;
			pNew->pChild = NULL;
			pNew->pParent = pParent;

			if (pPre != NULL) pPre->pNext = pNew;
			if (pParent != NULL) {
				if (pParent->pChild == NULL) pParent->pChild = pNew;
			}
			pData = pNew;
			fEnd = TRUE;
		}
	}

	return pData;
}

// ツリー文字列の取得
char *TREEINFO::GetTree(int nDspDepth, int nMargin)
{
	int i;
	char *pMgn, *pTree;

	pMgn = (char *)malloc(nMargin + 1);
	for (i = 0; i < nMargin; ++i) pMgn[i] = ' ';
	pMgn[i] = '\0';

	pTree = MakTree(m_pTop, pMgn, "", nDspDepth, FALSE);
	free(pMgn);

	return pTree;
}

// ツリー表示作成
char *TREEINFO::MakTree(TREEMNG *pData, char *pMargin, char *pIndent, int nDspDepth, int fNext)
{
	char *pLine, *pAdd, *pArg, *pName;
	char *pTre, *pRet, *pCat;
	int nCnt;

	pLine = "";
	pAdd  = "";
	if (pData != NULL) {
		pName = pData->pName;
		// 表示する階層数の設定
		if (nDspDepth < 0) {
			nCnt = m_nMaxDepth + nDspDepth;
			if (nCnt <= 0) {
				nCnt = m_nMaxDepth;
			}
		} else {
			nCnt = nDspDepth;
		}

		// 罫線＋フォルダ/ファイル名の生成
		if (pData->nLayer <= nCnt) {
			if (fNext == TRUE && pData->nLayer != 1) {
				pCat = MakPath(pData);
				pCat = CatStr("01", pMargin, pCat);
			} else {
				if (pData->pParent) {
					pCat = strdup("");
				} else {
					pCat = strdup(pMargin);
				}
			}

			if (pData->pChild == NULL || pData->nLayer == nCnt) {
				pCat = CatStr("100", pCat, pName, "\r\n");
			} else {
				pCat = CatStr("100", pCat, pName, "\\");
			}
		} else {
			if (pData->nLayer != 1) {
				if (pData->pNext == NULL) {
					pLine = "└ ";
					pAdd  = "   ";
				} else {
					pLine = "├ ";
					pAdd  = "│ ";
				}
			}
			pCat = CatStr("00000", pMargin, pIndent, pLine, pName, "\r\n");
		}

		// 一つしたの階層のフォルダ/ファイル名生成
		if (pData->pChild != NULL) {
			pArg = CatStr("00", pIndent, pAdd);
			pTre = MakTree(pData->pChild, pMargin, pArg, nDspDepth, FALSE);
			free(pArg);
			pCat = CatStr("10", pCat, pTre);
		}

		// 同じ階層の次のフォルダ/ファイル名生成
		if (pData->pNext != NULL) {
			pTre = MakTree(pData->pNext, pMargin, pIndent, nDspDepth, TRUE);
			pCat = CatStr("10", pCat, pTre);
		}
	} else {
		pCat = strdup("");
	}

	return pCat;
}

// 上位のパス名作成
char *TREEINFO::MakPath(TREEMNG *pData)
{
	char *pPath;

	pPath = strdup("");
	while (pData->pParent) {
		pData = pData->pParent;
		pPath = CatStr("001", pData->pName, "\\", pPath);
	}

	return pPath;
}

// 引数の文字列結合(p1…p2以降の引数の解放指定(0:解放なし/1:解放あり)、文字列長=p2以降の引数の数)
char *TREEINFO::CatStr(char *pOpe, ...)
{
	va_list List;
	char *pArg, *pCat;
	int nLen;

	pCat = strdup("");
	va_start(List, pOpe);
	pArg = va_arg(List, char *);
	while (*pOpe) {
		nLen = strlen(pCat) + strlen(pArg) + 1;
		pCat = (char *)realloc(pCat, nLen);
		strcat(pCat, pArg);

		if (*pOpe == '1') free(pArg);
		++pOpe;

		pArg = va_arg(List, char *);
	}
	va_end(List);

	return pCat;
}

// ツリー表示からフォルダ/ファイル名の位置を検索
int TREEINFO::SrchName(char *pLine)
{
	int i, nIdx = 0;

	for (i = 0; pLine[i]; ++i) {
		if (!strncmp(pLine + i, "│", 2)
		 || !strncmp(pLine + i, "├", 2)
		 || !strncmp(pLine + i, "└", 2)) {
			++i;
			nIdx += 2;
		} else if (pLine[i] == ' ') {
			++nIdx;
		} else {
			break;
		}
	}

	return nIdx;
}

// フォルダ/ファイル名からパス名を生成
char *TREEINFO::Name2Path(STRLIST *pName, int nLayer)
{
	int i;
	char *pPath, *pStr;

	pPath = strdup("");
	for (i = 0; i < nLayer; ++i) {
		if (i > 0) {
			pPath = CatStr("10", pPath, "\\");
		}
		pStr = pName->GetString(i);
		pPath = CatStr("10", pPath, pStr);
	}
	pPath = CatStr("10", pPath, "\r\n");

	return pPath;
}

// ツリー表示→ファイル名変換
char *TREEINFO::Tree2File(char *pTree, int *pnLayer, int *pnMargin)
{
	int nLinMax, i, nIdx;
	int nBgnMgn, nLayer, nPreLyr, nMaxLyr, nNamMax;
	int fFst;
	char *pLine, *pFile, *pPath;
	STRLIST ClpLine, Name;
	char *p;

	// 初期化
	pFile = strdup("");
	nPreLyr = 0;
	nMaxLyr = 0;
	fFst = FALSE;

	// 行毎に分解して処理
	StrSplit(pTree, "\r\n", SPLIT_NONE | SPLIT_MULTI, &ClpLine);
	nLinMax = ClpLine.GetMaxCount();
	for (i = 0; i < nLinMax; ++i) {
		pLine = ClpLine.GetString(i);
		if (*pLine != '\0') {
			// フォルダ/ファイル名の位置を検索
			nIdx = SrchName(pLine);

			// 先頭行における行頭の空白数保存
			if (fFst == FALSE) {
				nBgnMgn = nIdx;
				fFst = TRUE;
			}

			// 階層数を算出し、フォルダ/ファイル名の保存とパス名の生成
			nLayer = (nIdx - nBgnMgn) / 3 + 1;
			if (nLayer <= nPreLyr) {
				pPath = Name2Path(&Name, nPreLyr);
				pFile = CatStr("11", pFile, pPath);
			}
			if (nMaxLyr < nLayer) nMaxLyr = nLayer; 

			nNamMax = Name.GetMaxCount();
			if ((nLayer - nNamMax) == 1) {
				Name.AddString(pLine + nIdx);
			} else if (nLayer <= nNamMax) {
				Name.InsString(nLayer - 1, pLine + nIdx);
				Name.DelString(nLayer);
			}
			nPreLyr = nLayer;
		}
	}
	pPath = Name2Path(&Name, nPreLyr);
	pFile = CatStr("11", pFile, pPath);

	*pnLayer = nMaxLyr;
	*pnMargin = nBgnMgn;
	return pFile;
}
