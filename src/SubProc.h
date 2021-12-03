/*
 * 各種の処理定義
 */

#ifndef SUBPROC_INCLUDED
#define SUBPROC_INCLUDED

#include <stdio.h>
#include <windows.h>

class CharData {
	char *m_pDat;	// 文字データ
	int m_nMax;		// データ最大長
	int m_nLen;		// 格納データ数

public:
	// コンストラクタ
	CharData(int nSize)
	{
		m_pDat = (char *)malloc(nSize);
		m_nMax = nSize;
		m_nLen = 0;
	}

	// デストラクタ
	~CharData()
	{
		if (m_pDat != NULL) free(m_pDat);
	}

	// 文字追加
	void Add(char bDat)
	{
		if (m_nLen < m_nMax) {
			m_pDat[m_nLen] = bDat;
			++m_nLen;
		}
	}

	// 文字取得
	char Get(int nIdx)
	{
		char bRet;

		if (nIdx < m_nLen) {
			bRet = m_pDat[nIdx];
		} else {
			bRet = 0;
		}

		return bRet;
	}

	// データ長取得
	int Len()
	{
		return m_nLen;
	}
	
	// データ消去
	void Clear()
	{
		*m_pDat = '\0';
		m_nLen = 0;
	}
};

#endif
