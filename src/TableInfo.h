/*
 * TABLEINFO�N���X�̒�`
 */

#ifndef TABLEINFO_INCLUDED
#define TABLEINFO_INCLUDED

/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
*/

/* �\�[�g�L�[ */
enum SORT_ID {
	SORT_ASC_KEYPTR1,
	SORT_DSC_KEYPTR1,
	SORT_ASC_KEYVAL2,
	SORT_DSC_KEYVAL2
};

class TABLEINFO {
#define TRUE	1
#define FALSE	0
#define NULL	0
	int m_nNextPos;	// ���g�p�f�[�^�̈�̊J�n�ʒu
	int m_nMaxCnt;	// �m�ۃf�[�^�̈�̐�
	int m_nSize;	// 1�f�[�^�̃T�C�Y
	char *m_pData;	// �f�[�^�̈�̐擪�|�C���^
	int m_nSortId;	// �\�[�g�L�[

	int QuickSort(char *pData, int cbSz, int nTop, int nBottom);
	int CompData(char *pData1, char *pData2);

public:
	TABLEINFO();			// �R���X�g���N�^
	TABLEINFO(int cbSz);	// �R���X�g���N�^
	~TABLEINFO();			// �f�X�g���N�^
	void *GetDataPtr(int nPos, int bExpand);	//�e�[�u���̃|�C���^�擾
	int Count();			// �f�[�^���̎擾
	void ClearCount(); //�f�[�^���̃N���A
	void Sort(enum SORT_ID Id);	// �e�[�u���̃\�[�g
	void DelLastData();		// �ŏI�f�[�^�̍폜
	int MoveData(int nOrgIdx, int nDstIdx);		// �f�[�^�̈ړ�
	void debug();
};

class STRLIST {
	int nCurIdx;
	int nMaxCnt;
	TABLEINFO *pStrMng;

public:
	STRLIST();
	~STRLIST();
	void Clear();
	void AddString(char *pStr);
	void AddString(char *pString, int nLen);
	int InsString(int nIdx, char *pString);
	int DelString(int nIdx);
	char *GetString(int nIdx);
	int SrchString(char *pSrch);
	int STRLIST::MoveString(int nOrgIdx, int nDstIdx);
	char *MergeAllStr(char *pSep);
	char *GetCurrentString();
	int GetCurrentIndex();
	int GetMaxCount();
	int MoveNextIndex();
	int MovePrevIndex();
	void DelLastString();
	void Sort(enum SORT_ID Id);
	char *debug();
};

#endif
