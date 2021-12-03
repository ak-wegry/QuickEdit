/*
 * TABLEINFO, STRLISTクラスの定義
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TableInfo.h"

// -----------------------------------------------------------------------------
// TABLEINFOクラスの定義
/* 
 * クイックソート
 */
int TABLEINFO::QuickSort(char *pData, int cbSz, int nTop, int nBottom)
{
	int Idx, i, Stat;
	char *pBuf;

	if(nTop >= nBottom) return TRUE;

	//pBuf = malloc(cbSz);
	pBuf = new char[cbSz];
	Idx = nTop;
	for(i = nTop + 1; i <= nBottom; ++i) {
		Stat = CompData(pData + (cbSz * i), pData + (cbSz * nTop));
		if(Stat < 0) {
			Idx = Idx + 1;

			memcpy(pBuf, pData + (cbSz * Idx), cbSz);
			memcpy(pData + (cbSz * Idx), pData + (cbSz * i), cbSz);
			memcpy(pData + (cbSz * i), pBuf, cbSz);
		}
	}

	memcpy(pBuf, pData + (cbSz * Idx), cbSz);
	memcpy(pData + (cbSz * Idx), pData + (cbSz * nTop), cbSz);
	memcpy(pData + (cbSz * nTop), pBuf, cbSz);

	QuickSort(pData, cbSz, nTop, Idx - 1);
	QuickSort(pData, cbSz, Idx + 1, nBottom);

	//free(pBuf);
	delete pBuf;
	return TRUE;
}

/*
 * ソートの比較処理
 */
int TABLEINFO::CompData(char *pData1, char *pData2)
{
	char *pStr1, *pStr2;
	int Val1, Val2, Ret = 0;

	switch (m_nSortId) {
	SORT_ASC_KEYPTR1:
		pStr1 = *(char **)pData1;
		pStr2 = *(char **)pData2;

		Ret = strcmp(pStr1, pStr2);
		break;
	SORT_DSC_KEYPTR1:
		pStr1 = *(char **)pData1;
		pStr2 = *(char **)pData2;

		Ret = strcmp(pStr2, pStr1);
		break;
	SORT_DSC_KEYVAL2:
		Val1 = *(int *)(pData1 + 4);
		Val2 = *(int *)(pData2 + 4);

		Ret = (Val2 - Val1);
		break;
	SORT_ASC_KEYVAL2:
		Val1 = *(int *)(pData1 + 4);
		Val2 = *(int *)(pData2 + 4);

		Ret = (Val1 - Val2);
		break;
	}
	return Ret;
}

/* 
 * コンストラクタ
 */
TABLEINFO::TABLEINFO()
{
	m_nNextPos = 0;
	m_nMaxCnt = 0;
	m_nSize = 0;
	m_pData = NULL;
}

TABLEINFO::TABLEINFO(int cbSz)
{
	m_nNextPos = 0;
	m_nMaxCnt = 0;
	m_nSize = cbSz;
	m_pData = NULL;
}

/*
 * デストラクタ
 */
TABLEINFO::~TABLEINFO()
{
	//if(m_pData != NULL) free(m_pData);
	if(m_pData != NULL) delete m_pData;

	m_nNextPos = 0;
	m_nMaxCnt = 0;
	m_pData = NULL;
}

/*
 * テーブルのポインタ取得
 */
void *TABLEINFO::GetDataPtr(int nPos, int bExpand)
{
	void *pRet;
	int nBlkCnt; // メモリを確保する1ブロック数

	nBlkCnt = 10;
	if(nPos < m_nNextPos) {
		/* 既存使用データ領域のポインタ設定 */
		pRet = m_pData + (nPos * m_nSize);
	} else {
		if(bExpand == FALSE) {
			/* 領域拡張なしの場合、NULLポインタ設定 */
			pRet = NULL;
		} else {
			if(m_nMaxCnt <= nPos) {
				/* 新規データ領域の確保 */
				if(m_nMaxCnt == 0) {
					//m_pData = malloc(nBlkCnt * m_nSize);
					m_pData = new char[nBlkCnt * m_nSize];
				} else {
					//m_pData = realloc(m_pData, (m_nMaxCnt + nBlkCnt) * m_nSize);
					char *pNew = new char[(m_nMaxCnt + nBlkCnt) * m_nSize];
					memcpy(pNew, m_pData, m_nMaxCnt * m_nSize);
					delete m_pData;
					m_pData = pNew;
				}

				/* 拡張データ領域のクリア */
				memset(m_pData + (m_nMaxCnt * m_nSize), 0, nBlkCnt * m_nSize);

				/* 確保データ領域数の更新 */
				m_nMaxCnt = m_nMaxCnt + nBlkCnt;
			}

			/* 未使用データ領域のポインタ設定 */
			pRet = m_pData + (nPos * m_nSize);

			/* 未使用データ領域の開始位置更新 */
			m_nNextPos = nPos + 1;
		}
	}

	return pRet;
}

/*
 * データ数の取得
 */
int TABLEINFO::Count()
{
	return m_nNextPos;
}

/*
 * データ数のクリア
 */
void TABLEINFO::ClearCount()
{
	m_nNextPos = 0;
}

/*
 * テーブルのソート
 */
void TABLEINFO::Sort(enum SORT_ID Id)
{
	m_nSortId = Id;
	QuickSort(m_pData, m_nSize, 0, m_nNextPos - 1);
}

/*
 * 最終データの削除
 */
void TABLEINFO::DelLastData()
{
	char *pData;

	if(m_nNextPos > 0) {
		pData = (char *)GetDataPtr(m_nNextPos - 1, FALSE);
		memset(pData, 0, m_nSize);
		--m_nNextPos;
	}
}

/*
 * データの移動
 */
int TABLEINFO::MoveData(int nOrgIdx, int nDstIdx)
{
	char *pData, *pSave, *pBuf;
	char *pOrg, *pDst;
	int	nSize;

	/* Idx値の正常性チェック */
	if(nOrgIdx < 0 || m_nNextPos <= nOrgIdx) {
		return FALSE;
	}

	if(nDstIdx < 0 || m_nNextPos <= nDstIdx) {
		return FALSE;
	}

	/* 移動元データ退避 */
	pData = (char *)GetDataPtr(nOrgIdx, FALSE);
	//pSave = malloc(m_nSize);
	pSave = new char[m_nSize];
	memcpy(pSave, pData, m_nSize);

	/* データの移動 */
	if(nDstIdx < nOrgIdx) {
		pOrg = (char *)GetDataPtr(nDstIdx, FALSE);
		pDst = (char *)GetDataPtr(nDstIdx + 1, FALSE);
		nSize = m_nSize * (nOrgIdx - nDstIdx);
		//pBuf = malloc(nSize);
		pBuf = new char[nSize];
		memcpy(pBuf, pOrg, nSize);
		memcpy(pDst, pBuf, nSize);
		//free(pBuf);
		delete pBuf;

		memcpy(pOrg, pSave, m_nSize);
	} else if (nOrgIdx < nDstIdx) {
		pOrg = (char *)GetDataPtr(nOrgIdx + 1, FALSE);
		pDst = (char *)GetDataPtr(nOrgIdx, FALSE);
		nSize = m_nSize * (nDstIdx - nOrgIdx);
		//pBuf = malloc(nSize);
		pBuf = new char[nSize];
		memcpy(pBuf, pOrg, nSize);
		memcpy(pDst, pBuf, nSize);
		//free(pBuf);
		delete pBuf;

		pData = (char *)GetDataPtr(nDstIdx, FALSE);
		memcpy(pData, pSave, m_nSize);
	}

	//free(pSave);
	delete pSave;
	return TRUE;
}

/* debug表示 */
void TABLEINFO::debug()
{
	int i, j;
	char *p, *v;
	unsigned char c;

	printf("<TABLEINFO>\n");
	printf(" m_nNextPos = %d\n", m_nNextPos);
	printf(" m_nMaxCnt  = %d\n", m_nMaxCnt);
	printf(" m_nSize    = %d\n", m_nSize);
	printf(" m_nSortId  = %d\n", m_nSortId);

	printf(" m_pData    = %08X\n", m_pData);
	if (m_pData != 0) {
		for (i = 0; i < m_nMaxCnt; ++i) {
			p = m_pData + (m_nSize * i);
			printf(" [%2d] %08X  ", i, p);
			v = *((char **)p);
			printf("%08X  ", v);
			if (v != 0) {
				printf("(%s)\n", v);
			} else {
				printf("\n");
			}
			
			for (j = 0; j < m_nSize; ++j) {
				//if (j != 0 && (j % 4) == 0) printf("      ");
				if ((j % 4) == 0) printf("                ");
				c = *(m_pData + (m_nSize * i) + j);
				printf("%02X", c);
				if ((j % 4) == 3) printf("\n");
			}
			
		}
	}
}

// -----------------------------------------------------------------------------
// STRLISTクラスの定義
/*
 * コンストラクタ
 */
STRLIST::STRLIST()
{
	nCurIdx = 0;
	nMaxCnt = 0;
	pStrMng = new TABLEINFO(sizeof(char *));
}

/*
 * デストラクタ
 */
STRLIST::~STRLIST()
{
	Clear();
	if(pStrMng != NULL) delete pStrMng;
}

/*
 * 文字列情報のクリア
 */
void STRLIST::Clear()
{
	char **pDataPtr;
	int i;

	if(pStrMng != NULL) {
		for(i = 0; i < nMaxCnt; ++i) {
			pDataPtr = (char **)pStrMng->GetDataPtr(i, FALSE);
			if(*pDataPtr != NULL) {
				delete *pDataPtr;
				*pDataPtr = NULL;
			}
		}
	}
	pStrMng->ClearCount();
	nCurIdx = 0;
	nMaxCnt = 0;
}

/*
 * 文字列の追加
 */
void STRLIST::AddString(char *pString)
{
	char *pAddStr, **pSetPtr;
	int len;

	pAddStr = strdup(pString);
	pSetPtr = (char **)(pStrMng->GetDataPtr(nMaxCnt, TRUE));

	*pSetPtr = pAddStr;
	++nMaxCnt;
}

/*
 * 文字列の追加(長さ指定)
 */
void STRLIST::AddString(char *pString, int nLen)
{
	char *pAddStr, **pSetPtr;
	int len;

	pAddStr = (char *)malloc(nLen + 1);
	memcpy(pAddStr, pString, nLen);
	pAddStr[nLen] = '\0';
	pSetPtr = (char **)(pStrMng->GetDataPtr(nMaxCnt, TRUE));

	*pSetPtr = pAddStr;
	++nMaxCnt;
}

/*
 * 文字列の挿入
 */
int STRLIST::InsString(int nIdx, char *pString)
{
	if(nIdx < 0 || (nMaxCnt > 0 && nIdx > (nMaxCnt - 1))) return FALSE;

	AddString(pString);
	pStrMng->MoveData(nMaxCnt - 1, nIdx);

	return TRUE;
}

/*
 * 文字列の削除
 */
int STRLIST::DelString(int nIdx)
{
	if(nIdx < 0 || nIdx > (nMaxCnt - 1)) return FALSE;

	pStrMng->MoveData(nIdx, nMaxCnt - 1);
	DelLastString();

	return TRUE;
}

/*
 * 文字列の取得
 */
char *STRLIST::GetString(int nIdx)
{
	char **pGetPtr;

	if(nIdx < 0 || nIdx > (nMaxCnt - 1)) {
		return NULL;
	} else {
		pGetPtr = (char **)pStrMng->GetDataPtr(nIdx, FALSE);
		return *pGetPtr;
	}
}

/*
 * 文字列の検索
 */
int STRLIST::SrchString(char *pSrch)
{
	char **pDataPtr;
	int i, nIdx = -1;

	if(pStrMng != NULL) {
		for(i = 0; i < nMaxCnt; ++i) {
			pDataPtr = (char **)pStrMng->GetDataPtr(i, FALSE);
			if(!strcmp(*pDataPtr, pSrch)) {
				nIdx = i;
				break;
			}
		}
	}

	return nIdx;
}
/*
 * 文字列の入替え
 */
int STRLIST::MoveString(int nOrgIdx, int nDstIdx)
{
	return pStrMng->MoveData(nOrgIdx, nDstIdx);
}

/*
 * マージした文字列の取得
 */
char *STRLIST::MergeAllStr(char *pSep)
{
	char *pTxtBuf, *pStr;
	int nIdx, nSize, nLen, nSepLen;

	nSepLen = strlen(pSep);
	nIdx = 0;
	nSize = 0;
	pStr = GetString(nIdx);
	while(pStr != NULL) {
		nLen = strlen(pStr);
		nSize += (nLen + nSepLen);

		++nIdx;
		pStr = GetString(nIdx);
	}

	pTxtBuf = new char[nSize + 1];
	pTxtBuf[0] = NULL;
	nIdx = 0;
	pStr = GetString(nIdx);
	while(pStr != NULL) {
		strcat(pTxtBuf, pStr);
		strcat(pTxtBuf, pSep);

		++nIdx;
		pStr = GetString(nIdx);
	}

	return pTxtBuf;
}

/*
 * 現在カーソル位置の文字列取得
 */
char *STRLIST::GetCurrentString()
{
	return GetString(nCurIdx);
}

/*
 * 現在カーソル位置の取得
 */
int STRLIST::GetCurrentIndex()
{
	return nCurIdx;
}

/*
 * 最大カーソル数の取得
 */
int STRLIST::GetMaxCount()
{
	return nMaxCnt;
}

/*
 * 次カーソル位置への移動
 */
int STRLIST::MoveNextIndex()
{
	int nIdx;

	nIdx = nCurIdx + 1;
	if(nIdx < nMaxCnt) {
		nCurIdx = nIdx;
	}

	return nCurIdx;
}

/*
 * 前カーソル位置への移動
 */
int STRLIST::MovePrevIndex()
{
	int nIdx;

	nIdx = nCurIdx - 1;
	if(nIdx >= 0) {
		nCurIdx = nIdx;
	}

	return nCurIdx;
}

/*
 * 最後の文字列削除
 */
void STRLIST::DelLastString()
{
	char *pStr;

	pStr = GetString(nMaxCnt - 1);
	if (pStr != NULL) free(pStr);

	pStrMng->DelLastData();
	--nMaxCnt;
}

/*
 * リストのソート
 */
void STRLIST::Sort(enum SORT_ID Id)
{
	pStrMng->Sort(Id);
}

/* debug表示 */
char *STRLIST::debug()
{
	STRLIST output;
	char *pStr, buf[1024];
	int i;

	sprintf(buf, "<STRLIST>");
	output.AddString(buf);
	sprintf(buf, " nCurIdx = %d", nCurIdx);
	output.AddString(buf);
	sprintf(buf, " nMaxCnt = %d", nMaxCnt);
	output.AddString(buf);

	for (i = 0; i < nMaxCnt; ++i) {
		pStr = GetString(i);
		sprintf(buf, " [%2d]%s", i, pStr);
		output.AddString(buf);
	}

	pStr = output.MergeAllStr("\n");
	output.Clear();

	return pStr;
}
