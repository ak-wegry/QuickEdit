/*
 * StrDataクラスの定義
 */

#include <stdio.h>
#include <windows.h>
#include "StrData.h"

// 規定値
#define BLOCK_SIZE	1024	// 確保するメモリのブロックサイズ

// 文字判定
#define UCHAR		unsigned char
#define UWORD		unsigned short
#define IsKanji(c)	(((UCHAR)(c)>=0x80&&(UCHAR)(c)<0xA0)||(UCHAR)(c)>=0xE0)
#define CodeVal(c)	(*(UCHAR *)(c) * 256 + *(UCHAR *)(c+1))

//------------------------------------------------------------------------------
// コンストラクタ
StrData::StrData()
{
	m_pStr = NULL;
	m_cType = '\0';
	m_nCurPt = 0;
	m_nPrevPt = -1;
	m_nCurLen = 0;
}

// デストラクタ
StrData::~StrData()
{
}

// データ設定
int StrData::SetData(char *pStr, char cType)
{
	m_pStr = pStr;
	m_cType = cType;
	m_nCurPt = 0;
	m_nPrevPt = -1;
	m_nCurLen = strlen(m_pStr);

	return SetCurLen();
}

// データタイプ取得
char StrData::GetType()
{
	return m_cType;
}

// 現在位置にある値ポインタ取得
char *StrData::GetCurPtr()
{
	return m_pStr + m_nCurPt;
}

// 現在位置にある文字列長取得
int StrData::GetCurLen()
{
	return m_nCurLen;
}

// 先頭位置から現在位置までのインデックス取得
int StrData::GetCurIdx()
{
	int nIdx = 0;
	int nSavePt;

	nSavePt = m_nCurPt;
	MovTopPtr();
	while (m_nCurPt < nSavePt) {
		++nIdx;
		MovNextPtr();
	}

	return nIdx;
}

// インデックス位置にある値ポインタ取得
char *StrData::GetIdxPtr(int nIdx)
{
	int i;

	MovTopPtr();
	for (i = 0; i < nIdx; ++i) {
		MovNextPtr();
	}
	SetCurLen();

	return GetCurPtr();
}

// 現在位置にある文字列取得
int StrData::GetCurStr(char *pOutput)
{
	memcpy(pOutput, m_pStr + m_nCurPt, m_nCurLen);
	pOutput[m_nCurLen] = '\0';

	return m_nCurLen;
}

// 現在位置を次に進める
void StrData::MovNextPtr()
{
	m_nPrevPt = m_nCurPt;
	switch (m_cType) {
		case 'a':
		case '1':
		case 'h':
			m_nCurPt += m_nCurLen;
			break;
		case 'c':
			m_nCurPt += m_nCurLen;
			if (*(m_pStr + m_nCurPt) == ',') ++m_nCurPt;
			break;
	}
	SetCurLen();
}

// 現在位置を更新する
void StrData::UpdCurPtr(int nOfset)
{
	m_nPrevPt = m_nCurPt;
	m_nCurPt += nOfset;
	SetCurLen();
}

// 現在位置を先頭に戻す
void StrData::MovTopPtr()
{
	m_nCurPt = 0;
	m_nPrevPt = -1;
	SetCurLen();
}

// 現在位置にある値のByte長設定
int StrData::SetCurLen()
{
	int nRefPt;

	switch (m_cType) {
		case 'a':
			m_nCurLen = strlen(m_pStr + m_nCurPt);
			break;
		case '1':
			if (IsKanji(*(m_pStr + m_nCurPt))) {
				m_nCurLen = 2;
			} else {
				m_nCurLen = 1;
			}
			break;
		case 'h':
			if (IsKanji(*(m_pStr + m_nCurPt))) {
				m_nCurLen = 2;
			} else {
				m_nCurLen = 1;
			}
			if (*(m_pStr + m_nCurPt + m_nCurLen) == '-') {
				m_nCurLen = 2 * m_nCurLen + 1;
			}
			break;
		case 'c':
			nRefPt = m_nCurPt;
			while (m_pStr[nRefPt] != '\0' && m_pStr[nRefPt] != ',') {
				if (IsKanji(*(m_pStr + nRefPt))) {
					nRefPt += 2;
				} else {
					nRefPt += 1;
				}
			}
			m_nCurLen = nRefPt - m_nCurPt;
			break;
		default:
			return 1;
	}
	return 0;
}

// 現在位置From-To指定の範囲内判定
int StrData::ChkRange(char *pStr)
{
	int nVal, nStt, nEnd;
	int nStat = -1;

	if (m_cType == 'h') {
		switch (m_nCurLen) {
			case 1:
				nVal = *(UCHAR *)pStr;
				nStt = *(UCHAR *)(m_pStr + m_nCurPt);
				nEnd = *(UCHAR *)(m_pStr + m_nCurPt);
				break;
			case 2:
				nVal = CodeVal(pStr);
				nStt = CodeVal(m_pStr + m_nCurPt);
				nEnd = CodeVal(m_pStr + m_nCurPt);
				break;
			case 3:
				nVal = *(UCHAR *)pStr;
				nStt = *(UCHAR *)(m_pStr + m_nCurPt);
				nEnd = *(UCHAR *)(m_pStr + m_nCurPt + 2);
				break;
			case 5:
				nVal = CodeVal(pStr);
				nStt = CodeVal(m_pStr + m_nCurPt);
				nEnd = CodeVal(m_pStr + m_nCurPt + 3);
				break;
			default:
				nVal = 0;
				nStt = 1;
				nEnd = 1;
				break;
		}

		if (nStt <= nVal && nVal <= nEnd) {
			nStat = nVal - nStt;
		}
	}

	return nStat;
}

// 現在位置From-To指定の文字列取得
int StrData::GetRangeStr(int nIdx, int nOfset, char *pOutput)
{
	int nStat, nLen, nCode;
	char *pCnv;

	switch (m_cType) {
		case 'a':
			memcpy(pOutput, m_pStr + m_nCurPt, m_nCurLen);
			pOutput[m_nCurLen] = '\0';
			break;
		case '1':
		case 'c':
			pCnv = GetIdxPtr(nIdx);
			nLen = GetCurLen();
			memcpy(pOutput, pCnv, nLen);
			pOutput[nLen] = '\0';
			break;
		case 'h':
			pCnv = GetIdxPtr(nIdx);
			nLen = GetCurLen();
			if (nLen == 1 || nLen == 3) {
				pOutput[0] = *(UCHAR *)pCnv + nOfset;
				pOutput[1] = '\0';
			} else {
				nCode = CodeVal(pCnv) + nOfset;
				pOutput[0] = nCode >> 8;
				pOutput[1] = nCode & 0xFF;
				pOutput[2] = '\0';
			}
			break;
	}

	return 0;
}

// コンストラクタ
StrEdit::StrEdit()
{
	m_pStr = NULL;
	m_nLen = 0;
	m_nMax = 0;
	m_bFreFlg = FALSE;
}

StrEdit::StrEdit(int bFlg)
{
	m_pStr = NULL;
	m_nLen = 0;
	m_nMax = 0;
	m_bFreFlg = bFlg;
}

// デストラクタ
StrEdit::~StrEdit()
{
	if (m_pStr != NULL) {
		if (m_bFreFlg == TRUE)	free(m_pStr);
		m_pStr = NULL;
		m_nMax = 0;
	}
}

// メモリ確保
void StrEdit::AlocMem(int nLen)
{
	if (m_nMax < (nLen + 1)) {
		m_nMax = BLOCK_SIZE * (nLen / BLOCK_SIZE + 1);
		if (m_pStr == NULL) {
			m_pStr = (char *)malloc(m_nMax);
		} else {
			m_pStr = (char *)realloc(m_pStr, m_nMax);
		}
	}
}

// 文字列設定
char *StrEdit::SetEdit(char *pStr)
{
	m_nLen = strlen(pStr);
	AlocMem(m_nLen);
	strcpy(m_pStr, pStr);

	return m_pStr;
}

// 文字列追加
char *StrEdit::AddEdit(char *pStr, int nSize)
{
	int nNewLen, nAddLen;

	if (pStr != NULL) {
		if (nSize == 0) {
			nAddLen = strlen(pStr);
			nNewLen = m_nLen + nAddLen;
			AlocMem(nNewLen);
			strcpy(m_pStr + m_nLen, pStr);
		} else {
			nNewLen = m_nLen + nSize;
			AlocMem(nNewLen);
			memcpy(m_pStr + m_nLen, pStr, nSize);
			m_pStr[nNewLen] = '\0';
		}
		m_nLen = nNewLen;
	}

	return m_pStr;
}

// 文字列クリア
char *StrEdit::Clear()
{
	if (m_pStr != NULL) {
		*m_pStr = '\0';
		m_nLen = 0;
	}

	return m_pStr;
}

// 文字列取得
char *StrEdit::GetStr()
{
	return m_pStr;
}
