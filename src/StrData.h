/*
 * StrData�N���X�̒�`
 */

#ifndef STRDATA_INCLUDED
#define STRDATA_INCLUDED

#include <stdio.h>
#include <windows.h>

class StrData {
	char *m_pStr;	// ������f�[�^
	char m_cType;	// �f�[�^�^�C�v(a:�S�����A1:1�����Ah:From-To�Ac:CSV)
	int m_nCurPt;	// ������f�[�^�̎Q�ƈʒu(����)
	int m_nPrevPt;	// ������f�[�^�̎Q�ƈʒu(�X�V�O)
	int m_nCurLen;	// �Q�ƈʒu�̃f�[�^��

public:
	StrData();	// �R���X�g���N�^
	~StrData();	// �f�X�g���N�^
	int SetData(char *pStr, char cType);	// �f�[�^�ݒ�
	char GetType();		// �f�[�^�^�C�v�擾
	char *GetCurPtr();	// ���݈ʒu�ɂ���l�|�C���^�擾
	int GetCurLen();	// ���݈ʒu�ɂ��镶���񒷎擾
	int GetCurIdx();	// �擪�ʒu���猻�݈ʒu�܂ł̃C���f�b�N�X�擾
	char *GetIdxPtr(int nIdx);		// �C���f�b�N�X�ʒu�ɂ���l�|�C���^�擾
	int GetCurStr(char *pOutput);	// ���݈ʒu�ɂ��镶����擾
	void MovNextPtr();	// ���݈ʒu�����ɐi�߂�
	void UpdCurPtr(int nOfset);		// ���݈ʒu���X�V����
	void MovTopPtr();	// ���݈ʒu��擪�ɖ߂�
	int SetCurLen();	// ���݈ʒu�ɂ���l��Byte���ݒ�
	int ChkRange(char *pStr);		// ���݈ʒuFrom-To�w��͈͓̔�����
	int GetRangeStr(int nIdx, int nOfset, char *pOutput);	// ���݈ʒuFrom-To�w��̕�����擾
};

class StrEdit {
	char *m_pStr;
	int m_nLen;
	int m_nMax;
	int m_bFreFlg;

public:
	StrEdit();	// �R���X�g���N�^
	StrEdit(int bFlg);	// �R���X�g���N�^
	~StrEdit();	// �f�X�g���N�^
	void AlocMem(int nLen);		// �������m��
	char *SetEdit(char *pStr);	// ������ݒ�
	char *AddEdit(char *pStr, int nSize);	// ������ǉ�
	char *Clear();	// ������N���A
	char *GetStr();	// ������擾
};

#endif
