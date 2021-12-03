/*
 * �e��̏�����`
 */

#ifndef SUBPROC_INCLUDED
#define SUBPROC_INCLUDED

#include <stdio.h>
#include <windows.h>

class CharData {
	char *m_pDat;	// �����f�[�^
	int m_nMax;		// �f�[�^�ő咷
	int m_nLen;		// �i�[�f�[�^��

public:
	// �R���X�g���N�^
	CharData(int nSize)
	{
		m_pDat = (char *)malloc(nSize);
		m_nMax = nSize;
		m_nLen = 0;
	}

	// �f�X�g���N�^
	~CharData()
	{
		if (m_pDat != NULL) free(m_pDat);
	}

	// �����ǉ�
	void Add(char bDat)
	{
		if (m_nLen < m_nMax) {
			m_pDat[m_nLen] = bDat;
			++m_nLen;
		}
	}

	// �����擾
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

	// �f�[�^���擾
	int Len()
	{
		return m_nLen;
	}
	
	// �f�[�^����
	void Clear()
	{
		*m_pDat = '\0';
		m_nLen = 0;
	}
};

#endif
