/*
 * �����̐��`�Ɋւ��鏈���̒�`
 */

#ifndef FORMATING_INCLUDED
#define FORMATING_INCLUDED

// �f�[�^�^��`
typedef struct {
	int nMax;		// �J�����ő咷
	char *pStrFmt;	// ������p�J�����\���t�H�[�}�b�g
	char *pNumFmt;	// ���l�p�J�����\���t�H�[�}�b�g
} MngColFmt;

typedef struct {
	char *pStr;	// �J����������
	int nLin;	// �s�ԍ�
	int nCol;	// �J�����ԍ�
} MngColDat;

// �֐��̒�`
char *AdjHtml(char *pStr);
char *MakeTable(char *pText, char *pTokDlmt);
char *CnvColDat(char *pOrg, char *pFmt, char *pDlmt);
char *GrepLine(char *pOrg, char *pSrch, int bInc);

#endif
