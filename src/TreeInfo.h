/*
 * �p�X������c���[�\���𐶐����鏈���̒�`
 */

#ifndef TREEINFO_INCLUDED
#define TREEINFO_INCLUDED

#include <windows.h>
#include "OpeString.h"

typedef struct s_TreeMng {
	int nLayer;					// �K�w
	char *pName;				// ���ږ�
	struct s_TreeMng *pNext;	// ���̃f�[�^
	struct s_TreeMng *pChild;	// ����̊K�w�f�[�^
	struct s_TreeMng *pParent;	// ���̊K�w�f�[�^
} TREEMNG;

class TREEINFO {
	TREEMNG *m_pTop;	// �f�[�^�̐擪
	int m_nMaxDepth;	// �K�w�̍ő吔

public:
	TREEINFO();					// �R���X�g���N�^
	~TREEINFO();				// �f�X�g���N�^
	void FreeData(TREEMNG *);	// �Ǘ��f�[�^�̉��
	int RegFile(char *);		// �t�@�C�����̓o�^
	int MaxDepth();				// �ő�K�w��
	TREEMNG *RegData(TREEMNG *, TREEMNG *, char *, int);	// �Ǘ��f�[�^�̓o�^
	char *GetTree(int, int);								// �c���[������̎擾
	char *MakTree(TREEMNG *, char *, char *, int, int);		// �c���[�\���쐬
	char *TREEINFO::MakPath(TREEMNG *);						// ��ʂ̃p�X���쐬
	char *CatStr(char *, ...);// �����̕����񌋍�(p1�cp2�ȍ~�̈����̉���w��(0:����Ȃ�/1:�������)�A������=p2�ȍ~�̈����̐�)
	int SrchName(char *);					// �c���[�\������t�H���_/�t�@�C�����̈ʒu������
	char *Name2Path(STRLIST *, int);		// �t�H���_/�t�@�C��������p�X���𐶐�
	char *Tree2File(char *, int *, int *);	// �c���[�\�����t�@�C�����ϊ�
};

#endif
