/*
 * �N���b�v�{�[�h�ҏW�c�[��
 */

#include "TableInfo.h"

// �Œ�l 
#define MAX_EXCHHIST	10	// �u�����������̍ő吔
#define MAX_FMTHIST		10	// ���������̍ő吔

// Trim�̎��s�^�C�v
#define TRIM_HEAD	1	// �s��
#define TRIM_TAIL	2	// �s��
#define TRIM_BOTH	3	// �s��/�s��

// ���[�h
#define MODE_CMD	0	// �R�}���h���[�h
#define MODE_EDIT	1	// �ҏW���[�h
#define MODE_HELP	2	// �w���v�\�����[�h

// ���萧��̏��
#define CTRL_NONE	0	// �����s���
#define CTRL_DEL	1	// �폜���

// �f�[�^�^��`
typedef struct {
	char *pSrch;
	char cStype;
	char *pConv;
	char cCtype;
} ConvParam;

typedef struct {
#define FMTID_ADJHTML	0	// HTML�R�[�h�̐��`
#define FMTID_COLFMT	1	// �J�����f�[�^ �� �����ϊ�
#define FMTID_MAKTBL	2	// �J�����f�[�^ �� �\�ϊ�
#define FMTID_GETINC	3	// ��������܂ލs�̒��o
#define FMTID_GETEXC	4	// ��������܂܂Ȃ��s�̒��o
	int nFmtId;			// �t�H�[�}�b�gID
	STRLIST FmtHist;	// �����ϊ��p�����[�^�̗���
#define DLMTID_TAB	0	// Tab
#define DLMTID_CSV	1	// CSV
	int nDlmtId;		// �f���~�^ID
} FormatParam;	// �����ϊ��p�����[�^

typedef struct {
	STRLIST Name;	// �}�N������
	STRLIST Macro;	// �}�N�����e
} MacroParam;	// �}�N�����s�p�����[�^

typedef struct {
	HWND hDate;			// ���t�_�C�A���O�̃n���h��
	HWND hStime;		// �J�n���ԃ_�C�A���O�̃n���h��
	HWND hEtime;		// �I�����ԃ_�C�A���O�̃n���h��
	HWND hCombo;		// �ݒ菑���_�C�A���O�̃n���h��
#define DTFMT_FROMTO	0	// �N����+�J�n�����`�I������
#define DTFMT_DATETIME	1	// �N����+����
#define DTFMT_DATE		2	// �N����
	int bMacro;			// �}�N�����s
	int nIdx;			// �ݒ菑���_�C�A���O�̃J�[�\���ʒu
	char *pDateTime;	// �o�͕�����
} DateWndParam;	// �����ݒ�p�����[�^

// �O���[�o���ϐ���`
extern HWND hMainWnd; 
extern STRLIST g_ExchHist;		// �u����������
extern WORD g_wHotKey;			// �z�b�g�L�[
extern FormatParam g_FmtInfo;	// �t�H�[�}�b�g���
extern MacroParam g_McrInfo;	// �}�N�����
extern DateWndParam g_DateInfo;	// �����ݒ���

// �֐���`
LRESULT CALLBACK EventCall_MainWnd(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT MainWnd_Create(HWND hWnd, LPCREATESTRUCT cs);
LRESULT MainWnd_Resize(HWND hWnd, LONG SizeType, WORD cx, WORD cy);
LRESULT MainWnd_Activate(HWND hWnd, WORD state, WORD minimized);
LRESULT MainWnd_DropFiles(HWND hWnd, HDROP hDrop);
LRESULT MainWnd_DrawClipBoard(HWND hWnd);
LRESULT MainWnd_Command(HWND hWnd, WORD wNotifyCode, WORD wID, HWND hWndCtl);
LRESULT MainWnd_MenuSelect(HWND hWnd);
LRESULT CALLBACK EventCall_EditBox(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT EditBox_KeyDown(HWND hWnd, DWORD KeyCode);

int ExcMacro(HWND hWnd);
int AddFncParam(char *pStr, STRLIST *pParam, char *pChkPrm);
int ChkMcrParam(char *pParam, char *pCheck);
void DspStatus(char *pStr);
char *GetEditBox();
void SetEditBox(char *pText);
char *GetClipText(HWND hWnd);
void SetClipText(char *pData);
char *Replace(char *pOrg, int nFreFlg, char *pSrch, char cStype, char *pConv, char cCtype);
char *TrimLine(char *pStr, char *pDel, int nType);
char *AdjIndent(char *pStr, int nType);
char *AdjQuote(char *pStr);
char *AddHeadTailChar(char *pStr, char *pHead, char *pTail);
void DelHeadTailChar(char *pStr);
char *GetLastChar(char *pStr);
char *CnvTree(char *pStr);
int ExecFile(char *pFile);
int GetCurLine(HWND hEdit);
char *WildReplaceLine(char *pStr, char *pParam, int *lpStat);
int GetParam(char *pInput, ConvParam *pParam);
int AnlyzeParam(char *pOrg, int nIdx, char **pArg);
void LoadParamFile(char *pExcFile);
int ChkParam(char *pLine, char *pParam);
void SaveParamFile();
int GetWinVer(DWORD *lpMajor, DWORD *lpMinor);
void GetVerInfo(char *pFmt, char *pOutStr);
void DspHelp(int nAct);
