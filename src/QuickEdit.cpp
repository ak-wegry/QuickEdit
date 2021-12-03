/*
 * �N���b�v�{�[�h�ҏW�c�[��
 *======================================================================
 *[�ύX����]
 * Ver0.00  2016/10/10 �쐬�J�n
 * Ver1.00  2016/12/24 �V�K�쐬
 * Ver1.01  2016/12/30 �_�C�A���O�̕\���ʒu�𒆉��㕔�֕ύX
 * Ver1.02  2017/04/02 �z�b�g�L�[�̒ǉ�
 * Ver1.03  2017/04/08 StrData�N���X�̃o�O�C��(CSV�`���̌��݈ʒu�f�[�^���Z�o)
 * Ver1.04  2017/04/09 �C���f���g�@�\�̒ǉ�
 * Ver1.05  2017/04/15 �z�b�g�L�[�ۑ��@�\�ADropFile�Ή��ǉ�
 * Ver1.06  2017/04/18 EditBox���ҏW�@�\�̒ǉ�
 * Ver1.07  2017/04/20 �P�Ƃ̑S�p���_/�����_�̔��p�ϊ��ǉ�
 * Ver1.08  2017/04/22 �ŏ�����ԂŏI�����̕ۑ�����Window�ʒu�C��
 * Ver1.09  2017/04/25 �S�p���p�J�i�ϊ��̕s��C��(�m�����A���ǉ�)
 * Ver1.10  2017/04/30 ���萧��@�\�̒ǉ�
 * Ver1.11  2017/05/04 ���t�ϊ��@�\�̒ǉ�
 * Ver1.12  2017/05/08 ���t�ϊ��@�\�̕s��C��(���t�̐��K�\��)
 * Ver1.13  2017/05/09 ���t�ϊ��@�\�̕s��C��(���K�\���~�X�A��Ԕ�r)
 * Ver1.14  2017/05/11 �^�X�N�o�[�̏ꏊ�ɂ��I�����̕ۑ��ʒu�s��C��
 * Ver1.15  2017/05/23 HTML�̐��`�@�\�ǉ�
 * Ver1.16  2017/05/28 ������u���̗����@�\�ǉ�
 * Ver1.17  2017/05/30 HTML���`�̕s��C��(LIK��LINK)
 *                     �h���b�v���̃t�@�C�����擾���P(1�t�@�C�����͉��s����)
 * Ver1.18  2017/06/04 ������u���Ƀ��C���h�J�[�h����/�A���u���@�\�ǉ�
 * Ver1.19  2017/06/16 �J�����f�[�^�̏����ϊ��@�\�ǉ�
 * Ver1.20  2017/06/18 �t�@�C���h���b�v����Activate�����ǉ�
 * Ver1.21  2017/06/21 �J�����f�[�^��EXCEL�`���Ή�
 * Ver1.22  2017/06/25 HTML���`�s��C��(�N���b�v�{�[�h����̏ꍇ)
 * Ver1.23  2017/07/01 �X�y�[�X���܂ރt�@�C�������s�̕s��C��
 * Ver1.24  2017/07/06 �J�[�\���ʒu�̍s�f�[�^�Ńt�@�C�������s�@�\�ǉ�
 * Ver1.25  2017/07/17 Trim�@�\�̃p�^�[���ǉ�
 * Ver1.26  2017/07/24 �t�@�C���h���b�v����260byte�𒴂���p�X���Ή�
 * Ver1.27  2017/08/29 �P��1�o�C�g���s(0x0a)�̒u���@�\�ǉ�
 * Ver1.28  2017/10/31 �}�N���@�\�ǉ�
 * Ver1.29  2018/02/05 �E�B���h�E�ŏ������̃N���b�v�{�[�h�Ǎ��ݗ}�~�@�\�ǉ�
 *                     ClipBoard���ω��ɔ�������Active�\���s��C��
 * Ver1.30  2018/02/15 DropFile���̃l�b�g���[�N�h���C�u���W�J�@�\�ǉ�
 * Ver1.31  2018/09/17 DropFile�̃p�X�����c���[�\���ϊ��@�\�ǉ�
 * Ver1.32  2019/04/01 DropFile���Ƀt�@�C���������R���\�[�g
 * Ver1.33  2019/04/30 �\�[�g�@�\/���s�R�[�h�ϊ�(CRLF��LF)�@�\�ǉ�
 *                     Windows10�Ή�(�\���ʒu�Y���␳) 
 * Ver1.34  2019/05/04 split,sort�����̕s��C��
 * Ver1.35  2019/05/23 Trim�����̕s��C��(�N���b�v�{�[�h�����ɉ��s�������ꍇ)
 * Ver1.36  2019/09/01 ���A�v���Ƃ̃N���b�v�{�[�h�������
 * Ver1.37  2021/02/08 ��������܂�/�܂܂Ȃ��s�̒��o�@�\�ǉ��A�����ݒ�@�\�ǉ�
 */
 
#include <windows.h>
#include <commctrl.h>
#include <winnetwk.h>
#include <stdio.h>
#include <htmlhelp.h>
#include "resource.h"
#include "QuickEdit.h"
#include "StrData.h"
#include "OpeString.h"
#include "TableInfo.h"
#include "SubProc.h"
#include "DateCtrl.h"
#include "Formating.h"
#include "DialogBox.h"
#include "TreeInfo.h"
#include "Variable.h"

//==============================================================================
// �Œ�l 
#define VERSION		"1.37"
#define DATE		"2021/2/8"

#define MAX_BUF		1024
#define MAX_KEYSEQ	5
#define PARAM_FILE	"QuickEdit.ini"
#define HELP_FILE	"QuickEdit.chm"

// �O���[�o���ϐ�
HWND hMainWnd; 
HWND hNextViewr;
int g_nAutoAct = 0;				// ClipBoard���ω��ɔ�������Active�\��
int g_nNwDrvNam = 0;			// �l�b�g���[�N�h���C�u���W�J�\��
int g_nSelfEvnt = 0;			// �v���O�������̑���ɔ����C�x���g�\��
int g_nExcMode = MODE_CMD;		// ���s���[�h
int g_nCtrlStat = CTRL_NONE;	// ���萧����
int g_nIconFlg = 0;				// �E�B���h�E�ŏ����\��
STRLIST g_ExchHist;				// �u����������
STRLIST EditHist;				// ���엚��
CharData KeySeq(MAX_KEYSEQ);	// �L�[�V�[�P���X
WORD g_wHotKey;					// �z�b�g�L�[
FormatParam g_FmtInfo;			// �t�H�[�}�b�g���
MacroParam g_McrInfo;			// �}�N�����
DateWndParam g_DateInfo;		// �����ݒ���

char szDebug[MAX_BUF+1];		// �f�o�b�O�\���p

// �t�@�C�����[�J���ϐ�
static HFONT hFont_MainWnd;
static WNDPROC lpfnEditProc;		// CallBack�֐�
static char ParamFile[MAX_PATH+1];	// �p�����[�^�t�@�C����
static int nMacroExc = 0;			// �}�N�����s���(1:���s��, -1:�G���[)
static STRLIST FncParam;			// �}�N�����s���̃p�����[�^
 
//----------------------------------------------------------------------
//�����C���̊֐� 
//----------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpszCmdLine, int nCmdShow)
{
    HWND hWnd;		// ���C���E�C���h�E�̃n���h��
    WNDCLASS wc;	// WNDCLASS�\����
	MSG msg;

	// WNDCLASS�\���̂�0�ŏ�����
	ZeroMemory(&wc,sizeof(WNDCLASS));
	// WNDCLASS�\���̂̐ݒ�
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= EventCall_MainWnd;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon(hInstance, "IDR_ICON");
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)COLOR_WINDOW;
	wc.lpszMenuName		= MAKEINTRESOURCE(IDR_MENU);	/* ���j���[��t���� */
	wc.lpszClassName	= "QuickEdit";
	// �E�B���h�E �N���X��o�^
	if (RegisterClass(&wc) == 0)
		return 0; 

	// ���C���E�C���h�E�̐���
	hWnd = CreateWindowEx(
	           WS_EX_ACCEPTFILES | WS_EX_CONTROLPARENT | WS_EX_WINDOWEDGE,
	           wc.lpszClassName, "QuickEdit",
	           WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
	           600, 180, NULL, NULL, hInstance, NULL); 
	if (hWnd == NULL)
		return 0;
	hMainWnd = hWnd;
 
    // �E�C���h�E�̕\��
    ShowWindow(hWnd, nCmdShow);
 
	// ���b�Z�[�W���[�v
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg); 
		DispatchMessage(&msg);   
	} 
	return (msg.wParam);
}

//----------------------------------------------------------------------
//�����C���E�C���h�E�̃��b�Z�[�W����
//----------------------------------------------------------------------
LRESULT CALLBACK EventCall_MainWnd(HWND hWnd,UINT uMsg,WPARAM wParam, LPARAM lParam)
{
	INITCOMMONCONTROLSEX ic;
	LONG nStyle;

	switch (uMsg) {
		case WM_CREATE: 
			MainWnd_Create(hWnd, (LPCREATESTRUCT)lParam);
			PostMessage(hWnd, WM_COMMAND, IDM_DISP_EDIT, 0);

			// default�z�b�g�L�[(Ctrl+Shift+Q)��ݒ肷��
			g_wHotKey = MAKEWORD('Q', HOTKEYF_CONTROL | HOTKEYF_SHIFT);
			SendMessage(hWnd, WM_SETHOTKEY, g_wHotKey , 0);
			ic.dwSize = sizeof(INITCOMMONCONTROLSEX);
			ic.dwICC = ICC_HOTKEY_CLASS;
			InitCommonControlsEx(&ic);

			// �N���b�v�{�[�h�r���[�A�̃`�F�C���Ɏ��E�C���h�E��ǉ�����
			hNextViewr = SetClipboardViewer(hWnd);
			break;

		case WM_SIZE:
			switch (wParam) {
				case SIZE_RESTORED:
					if (g_nIconFlg == 1) {
						// �E�B���h�E�ŏ�������߂����ꍇ�ɃN���b�v�{�[�h�Ǎ���
						PostMessage(hWnd, WM_COMMAND, IDM_DISP_EDIT, 0);
					}
					g_nIconFlg = 0;
					break;
				case SIZE_MINIMIZED:
					g_nIconFlg = 1;
					break;
			}
			MainWnd_Resize(hWnd, wParam, LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_ACTIVATE:
			MainWnd_Activate(hWnd, LOWORD(wParam), HIWORD(wParam));
			break;
			
		case WM_DROPFILES:
			MainWnd_DropFiles(hWnd, (HDROP)wParam);
			break;

		case WM_CHANGECBCHAIN:
			// ���E�C���h�E�̎��̃r���[�����폜���ꂽ�ꍇ��
			// ���̎��̃r���[����ݒ肷��
			if ((HWND)wParam == hNextViewr) {
				hNextViewr = (HWND)lParam;
				return 0; // �����̃��b�Z�[�W�����������鎞�͖߂�l��0��Ԃ�
			} else {
				// ���̃r���[���Ƀ��b�Z�[�W�𑗂�
				return SendMessage(hNextViewr, WM_CHANGECBCHAIN, wParam, lParam);
			}

		case WM_DRAWCLIPBOARD:
			if (!IsIconic(hWnd)) {
				// �E�B���h�E�ŏ����ȊO�̏ꍇ�ɃN���b�v�{�[�h�Ǎ���
				PostMessage(hWnd, WM_COMMAND, IDM_DISP_EDIT, 0);
			}

			// ���̃r���[���Ƀ��b�Z�[�W�𑗂�
			// (����Ȃ��Ƒ��̃A�v���P�[�V�����ŉe�����o��)
			if (hNextViewr != 0)
				SendMessage(hNextViewr, WM_DRAWCLIPBOARD, 0, 0);

			break;

		case WM_DESTROY:
			// ���E�B���h�E���N���b�v�{�[�h�r���[�A�̃`�F�C�����珜��
			ChangeClipboardChain(hWnd, hNextViewr);

			SaveParamFile();
			DeleteObject(hFont_MainWnd);
			PostQuitMessage(0);
			break;

		case WM_COMMAND:
			MainWnd_Command(hWnd, HIWORD(wParam), LOWORD(wParam), (HWND)lParam);
			break;

		case WM_MENUSELECT:
			MainWnd_MenuSelect(hWnd);
			break;

		case WM_CLOSE:
			// �E�B���h�E���A�N�e�B�u�ɂ��A���̈ʒu�ƃT�C�Y�ŕ\��
			ShowWindow(hWnd, SW_SHOWNORMAL);
			// break�Ȃ�
		default:
			return(DefWindowProc(hWnd, uMsg, wParam, lParam)); 
	}
	return 0;
}

LRESULT MainWnd_Create(HWND hWnd, LPCREATESTRUCT cs)
{
	HWND hEdit, hStatus;

	// �t�H���g�I�u�W�F�N�g�̍쐬
	hFont_MainWnd = CreateFont(-15, 0, 0, 0, 400, 0, 0, 0,
							   128, 3, 2, 1, 49, "�l�r �S�V�b�N");

	// EditBox�̍쐬
	/*
	LoadLibrary("RICHED20.DLL");
	hEdit = CreateWindowEx(WS_EX_ACCEPTFILES | WS_EX_CLIENTEDGE,
						   "RichEdit20W", "",
						   WS_CHILD | WS_VISIBLE | \
						   WS_VSCROLL | WS_HSCROLL | \
						   WS_TABSTOP | ES_WANTRETURN | \
						   ES_AUTOHSCROLL | ES_MULTILINE,
						   0, 0, 380, 260, hMainWnd, (HMENU)EditBox,
						   cs->hInstance, 0);
	*/
	hEdit = CreateWindowEx(WS_EX_ACCEPTFILES|WS_EX_CLIENTEDGE,
	                       "EDIT", NULL,
	                       WS_CHILD|WS_VISIBLE|WS_BORDER| \
	                       WS_VSCROLL|WS_HSCROLL|ES_AUTOHSCROLL| \
	                       ES_MULTILINE,
	                       0, 0, 600, 180, hWnd, (HMENU)EditBox,
	                       cs->hInstance, 0);
	SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont_MainWnd, 0);
	lpfnEditProc = (WNDPROC)SetWindowLong(hEdit, GWL_WNDPROC,
										  (int)EventCall_EditBox);

	// �X�e�[�^�X�o�[�̍쐬
	hStatus = CreateWindowEx(0, STATUSCLASSNAME, NULL,
	                         WS_CHILD|SBARS_SIZEGRIP|CCS_BOTTOM|WS_VISIBLE,
	                         0, 0, 0, 0, hWnd, (HMENU)StatusBar,
	                         cs->hInstance, NULL);
	if (hStatus != NULL) {
		SendMessage(hStatus, SB_SIMPLE, TRUE, 0L);
	}

	PostMessage(hWnd, WM_COMMAND, IDM_INIT_PARAM, 0);

	return 0;
}

LRESULT MainWnd_Resize(HWND hWnd, LONG SizeType, WORD cx, WORD cy)
{
	HWND hEdit, hStatus;
	RECT rc;
	LONG width, height;
	int nOfset;
	static DWORD dwMajor = 0, dwMinor = 0;

	if (dwMajor == 0) {
		// Windows��OS�o�[�W�����擾
		GetWinVer(&dwMajor, &dwMinor);
	}

	/* �N���C�A���g�̈�̃T�C�Y���擾 */
	GetClientRect(hWnd, &rc);
	nOfset = GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION);
	width = rc.right - rc.left + 1;
	height = rc.bottom - rc.top + 1 - nOfset;

	// Windows10�̏ꍇ�̂ݕ␳
	if (dwMajor == 10) {
		height += 10;
	}

	/* EditBox�̃E�B���h�E�̑傫����ύX���� */
	hEdit = GetDlgItem(hWnd, EditBox);
	MoveWindow(hEdit, 0, 0, width, height, 0);

	// �E�C���h�E�̍X�V
	SendMessage(hWnd, WM_ACTIVATE, 0, 0);

	// Status�o�[�̍X�V
	hStatus = GetDlgItem(hMainWnd, StatusBar);
	SendMessage(hStatus, WM_SIZE, 0, 0);

/*
sprintf(szDebug, "height=%d, caption=%d, frame=%d, scroll=%d, menu=%d",
		height,
		GetSystemMetrics(SM_CYCAPTION),
		GetSystemMetrics(SM_CYFRAME),
		GetSystemMetrics(SM_CYHSCROLL),
		GetSystemMetrics(SM_CYMENU)
		);
*/
	return 0;
}

LRESULT MainWnd_Activate(HWND hWnd, WORD state, WORD minimized)
{
	HWND hEdit;

	hEdit = GetDlgItem(hMainWnd, EditBox);
	SetFocus(hEdit);

	return 0;
}

LRESULT MainWnd_DropFiles(HWND hWnd, HDROP hDrop)
{
#define MAX_FULL_PATH	32767 
	int i, nMax, nStat;
	char *pFile, *pList;
	char szDrv[MAX_BUF+1], szNwDrv[MAX_BUF+1], *pSave;
   	DWORD nSize;
	HWND hEdit;
	//STRLIST Files;
	Variable Files;

	if (g_nExcMode == MODE_EDIT) return 0;

	/* �h���b�v���ꂽ�t�@�C�������擾 */
	nMax = DragQueryFile(hDrop, -1, 0, 0);

	pFile = (char *)malloc(MAX_FULL_PATH + 1);
	if (pFile == NULL) return 0;
	for (i = 0; i < nMax; ++i) {
		/* �h���b�v���ꂽ�t�@�C�������擾 */
		nStat = DragQueryFile(hDrop, i, pFile, MAX_FULL_PATH);
		pFile[nStat] = '\0';

		/* �l�b�g���[�N�h���C�u���̓W�J */
		if (g_nNwDrvNam == 1 && *pFile) {
			if (pFile[1] == ':') {
				*szDrv = *pFile;
				strcpy(szDrv + 1, ":");
				*szNwDrv = '\0';
				nSize = MAX_BUF;
				WNetGetConnection(szDrv, szNwDrv, &nSize);
				if (*szNwDrv) {
					pSave = strdup(pFile + 2);
					strcpy(pFile, szNwDrv);
					strcat(pFile, pSave);
					free(pSave);
				}
			}
		}

		/* �t�@�C������o�^���� */
		Files.add(pFile);
	}
	free(pFile);
	DragFinish(hDrop);

	// �t�@�C�����X�g���쐬���A��ʂ�Clipboard�֐ݒ�
	if (nMax == 1) {
		pList = Files.get(0);
	} else {
		// ���R���\�[�g
		Files.sort(TRUE, TRUE);
		pList = Files.join("\r\n");
	}
	SetEditBox(pList);
	SetClipText(pList);
	if (nMax > 1) delete pList;
	// �X�e�[�^�X�o�[�֏������e�ݒ�
	DspStatus("DropFiles.");

	// ���엚���̃N���A
	EditHist.Clear();
	// �L�[�V�[�P���X�̃N���A
	KeySeq.Clear();
	// �w���v�̖߂�
	DspHelp(0);

	// Window��O�ʂɕ\��
	SetForegroundWindow(hMainWnd);
	SendMessage(hWnd, WM_ACTIVATE, 0, 0);

	return 0;
}

LRESULT MainWnd_DrawClipBoard(HWND hWnd)
{
	char *pText;

	// �N���b�v�{�[�h���當������擾
	pText = GetClipText(hWnd);

	// �擾�������������ʂɕ\��
	if (pText != NULL) {
		SetEditBox(pText);
		free(pText);
	} else {
		SetEditBox("");
	}

	if (g_nAutoAct == 1) {
		// �őO�ʃE�B���h�E�̃v���Z�X�ɃA�^�b�`
		HWND hFgWnd;
		DWORD dwProcId, dwCurId;
		hFgWnd = GetForegroundWindow();
		dwProcId = GetWindowThreadProcessId(hFgWnd, NULL); 
		dwCurId = GetCurrentThreadId();
		AttachThreadInput(dwCurId, dwProcId, TRUE);

		// Window��O�ʂɕ\��
		SetForegroundWindow(hMainWnd);
		SendMessage(hMainWnd, WM_ACTIVATE, 0, 0);

		// �f�^�b�`
		//AttachThreadInput(dwCurId, dwProcId, FALSE);
	}

	if (g_nSelfEvnt > 0) {
		--g_nSelfEvnt;
	} else {
		// ���엚���̃N���A
		EditHist.Clear();
		// ���萧���Ԃ̏�����
		g_nCtrlStat = CTRL_NONE;
		// ���t��Ԃ̏�����
		InitDateStat();

		DspStatus("");
//*szDebug = 0;
	}
	// �L�[�V�[�P���X�̃N���A
	KeySeq.Clear();

	// �w���v�̖߂�
	DspHelp(0);

	// �ҏW���[�h�̖߂�
	if (g_nExcMode == MODE_EDIT) {
		g_nExcMode = MODE_CMD;
	}

	return 0;
}

LRESULT MainWnd_Command(HWND hWnd, WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	HWND hEdit, hHelp;
	HINSTANCE hInstance;
	char *pCmd, *pArg;
	int nMax;

	hEdit = GetDlgItem(hMainWnd, EditBox);
	switch (wID) {
		case IDM_FILE_EXEC:
			// 'e'�L�[���o
			PostMessage(hEdit, WM_CHAR, 'e', 0);
			break;
		case IDM_FILE_AUTO:
			// 'a'�L�[���o
			PostMessage(hEdit, WM_CHAR, 'a', 0);
			break;
		case IDM_FILE_NWDRV:
			// �l�b�g���[�N�h���C�u���W�J�\���ؑւ�
			if (g_nNwDrvNam) {
				g_nNwDrvNam = 0;
			} else {
				g_nNwDrvNam = 1;
			}
			break;
		case IDM_FILE_EXIT:
			if (g_nExcMode != MODE_EDIT) {
				// 'Q'�L�[���o
				PostMessage(hEdit, WM_CHAR, 'Q', 0);
			} else {
				// ESC�L�[���o
				PostMessage(hEdit, WM_CHAR, 0x1B, 0);
			}
			break;
		case IDM_EDIT_UNDO:
			// ^Z�L�[���o
			PostMessage(hEdit, WM_CHAR, 0x1a, 0);
			break;
		case IDM_EDIT_TRIM_SUIT:
			// 't'�L�[���o
			PostMessage(hEdit, WM_CHAR, 't', 0);
			break;
		case IDM_EDIT_TRIM_BOTH:
			// 'Tb'�L�[���o
			PostMessage(hEdit, WM_CHAR, 'T', 0);
			PostMessage(hEdit, WM_CHAR, 'b', 0);
			break;
		case IDM_EDIT_TRIM_HEAD:
			// 'Th'�L�[���o
			PostMessage(hEdit, WM_CHAR, 'T', 0);
			PostMessage(hEdit, WM_CHAR, 'h', 0);
			break;
		case IDM_EDIT_TRIM_TAIL:
			// 'Te'�L�[���o
			PostMessage(hEdit, WM_CHAR, 'T', 0);
			PostMessage(hEdit, WM_CHAR, 'e', 0);
			break;
		case IDM_EDIT_TRIM_MAIL:
			// 'T>'�L�[���o
			PostMessage(hEdit, WM_CHAR, 'T', 0);
			PostMessage(hEdit, WM_CHAR, '>', 0);
			break;
		case IDM_EDIT_TRIM_ZEN:
			// 'Tz'�L�[���o
			PostMessage(hEdit, WM_CHAR, 'T', 0);
			PostMessage(hEdit, WM_CHAR, 'z', 0);
			break;
		case IDM_EDIT_IND_INC:
			// '+'�L�[���o
			PostMessage(hEdit, WM_CHAR, '+', 0);
			break;
		case IDM_EDIT_IND_DEC:
			// '-'�L�[���o
			PostMessage(hEdit, WM_CHAR, '-', 0);
			break;
		case IDM_EDIT_JOIN:
			// 'j'�L�[���o
			PostMessage(hEdit, WM_CHAR, 'j', 0);
			break;
		case IDM_EDIT_HAN_ASC:
			// 'h'�L�[���o
			PostMessage(hEdit, WM_CHAR, 'h', 0);
			break;
		case IDM_EDIT_HAN_ALL:
			// 'H'�L�[���o
			PostMessage(hEdit, WM_CHAR, 'H', 0);
			break;
		case IDM_EDIT_ZEN:
			// 'z'�L�[���o
			PostMessage(hEdit, WM_CHAR, 'z', 0);
			break;
		case IDM_EDIT_UPPER:
			// 'u'�L�[���o
			PostMessage(hEdit, WM_CHAR, 'u', 0);
			break;
		case IDM_EDIT_LOWER:
			// 'l'�L�[���o
			PostMessage(hEdit, WM_CHAR, 'l', 0);
			break;
		case IDM_EDIT_ADJ_QUOT:
			// '"'�L�[���o
			PostMessage(hEdit, WM_CHAR, '\"', 0);
			break;
		case IDM_EDIT_ADJ_DATE:
			// 'd'�L�[���o
			PostMessage(hEdit, WM_CHAR, 'd', 0);
			break;
		case IDM_EDIT_ADJ_TREE:
			// 'y'�L�[���o
			PostMessage(hEdit, WM_CHAR, 'y', 0);
			break;
		case IDM_EDIT_ADJ_CRLF:
			// 'Cr'�L�[���o
			PostMessage(hEdit, WM_CHAR, 'C', 0);
			PostMessage(hEdit, WM_CHAR, 'r', 0);
			break;
		case IDM_EDIT_ADJ_LF:
			// 'Cl'�L�[���o
			PostMessage(hEdit, WM_CHAR, 'C', 0);
			PostMessage(hEdit, WM_CHAR, 'l', 0);
			break;
		case IDM_EDIT_SUB:
			// 'R'�L�[���o
			PostMessage(hEdit, WM_CHAR, 'R', 0);
			break;
		case IDM_EDIT_SUB_REP:
			// 'r'�L�[���o
			PostMessage(hEdit, WM_CHAR, 'r', 0);
			break;
		case IDM_EDIT_FMT:
			// 'F'�L�[���o
			PostMessage(hEdit, WM_CHAR, 'F', 0);
			break;
		case IDM_EDIT_FMT_REP:
			// 'f'�L�[���o
			PostMessage(hEdit, WM_CHAR, 'f', 0);
			break;
		case IDM_EDIT_MCR:
			// 'M'�L�[���o
			PostMessage(hEdit, WM_CHAR, 'M', 0);
			break;
		case IDM_EDIT_MCR_REP:
			// 'm'�L�[���o
			PostMessage(hEdit, WM_CHAR, 'm', 0);
			break;
		case IDM_EDIT_INS_DATE:
			// 'D'�L�[���o
			PostMessage(hEdit, WM_CHAR, 'D', 0);
			break;
		case IDM_EDIT_EDIT_MODE:
			// 'i'�L�[���o
			PostMessage(hEdit, WM_CHAR, 'i', 0);
			break;
		case IDM_EDIT_SORT_ASC_N:
			// 'Sa'�L�[���o
			PostMessage(hEdit, WM_CHAR, 'S', 0);
			PostMessage(hEdit, WM_CHAR, 'a', 0);
			break;
		case IDM_EDIT_SORT_DSC_N:
			// 'Sd'�L�[���o
			PostMessage(hEdit, WM_CHAR, 'S', 0);
			PostMessage(hEdit, WM_CHAR, 'd', 0);
			break;
		case IDM_EDIT_SORT_ASC:
			// 'SA'�L�[���o
			PostMessage(hEdit, WM_CHAR, 'S', 0);
			PostMessage(hEdit, WM_CHAR, 'A', 0);
			break;
		case IDM_EDIT_SORT_DSC:
			// 'SD'�L�[���o
			PostMessage(hEdit, WM_CHAR, 'S', 0);
			PostMessage(hEdit, WM_CHAR, 'D', 0);
			break;
		case IDM_HELP_HELP:
			hHelp = HtmlHelp(hWnd, HELP_FILE, HH_DISPLAY_TOC, (DWORD)"");
			if (hHelp == NULL) {
				// �w���v�\���ׁ̈A'?'�L�[���o
				PostMessage(hEdit, WM_CHAR, 0x3F, 0);
			}
			break;
		case IDM_HELP_VERSION:
			hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
			DialogBox(hInstance, "IDD_DIALOG1",
			          hWnd, (DLGPROC)EventCall_MsgWnd);
			break;
		case IDM_HELP_HOTKEY:
			hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
			DialogBox(hInstance, "IDD_DIALOG3",
			          hWnd, (DLGPROC)EventCall_HotkeyWnd);
			break;
		case IDM_INIT_PARAM:
			// �p�����[�^�t�@�C���Ǎ���
			pCmd = GetCommandLine();
			if (pCmd != NULL) {
				nMax = GetElement(pCmd, 0, NULL);
				GetElement(NULL, 1, &pArg);
				LoadParamFile(pArg);
			}
			break;
		case IDM_DISP_EDIT:
			MainWnd_DrawClipBoard(hWnd);
			break;
	}
	return 0;
}

LRESULT MainWnd_MenuSelect(HWND hWnd)
{
	int nMax;
	HMENU hMenu;

	hMenu = GetMenu(hWnd);

	if (g_nAutoAct == 0) {
		CheckMenuItem(hMenu, IDM_FILE_AUTO, MF_BYCOMMAND|MF_UNCHECKED);
	} else {
		CheckMenuItem(hMenu, IDM_FILE_AUTO, MF_BYCOMMAND|MF_CHECKED);
	}

	if (g_nNwDrvNam == 0) {
		CheckMenuItem(hMenu, IDM_FILE_NWDRV, MF_BYCOMMAND|MF_UNCHECKED);
	} else {
		CheckMenuItem(hMenu, IDM_FILE_NWDRV, MF_BYCOMMAND|MF_CHECKED);
	}

	if (g_nExcMode != MODE_CMD) {
		EnableMenuItem(hMenu, IDM_FILE_EXEC, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_FILE_AUTO, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_FILE_NWDRV, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_EDIT_UNDO, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_EDIT_TRIM_SUIT, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_EDIT_TRIM_BOTH, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_EDIT_TRIM_HEAD, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_EDIT_TRIM_TAIL, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_EDIT_TRIM_MAIL, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_EDIT_TRIM_ZEN, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_EDIT_IND_INC, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_EDIT_IND_DEC, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_EDIT_JOIN, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_EDIT_EDIT_MODE, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_EDIT_ADJ_QUOT, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_EDIT_ADJ_DATE, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_EDIT_ADJ_TREE, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_EDIT_ADJ_CRLF, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_EDIT_ADJ_LF, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_EDIT_SORT_ASC_N, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_EDIT_SORT_DSC_N, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_EDIT_SORT_ASC, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_EDIT_SORT_DSC, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_EDIT_HAN_ASC, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_EDIT_HAN_ALL, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_EDIT_ZEN, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_EDIT_UPPER, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_EDIT_LOWER, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_EDIT_SUB, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_EDIT_SUB_REP, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_EDIT_FMT, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_EDIT_FMT_REP, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_EDIT_MCR, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_EDIT_MCR_REP, MF_GRAYED);

	} else {
		EnableMenuItem(hMenu, IDM_FILE_EXEC, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_FILE_AUTO, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_FILE_NWDRV, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_EDIT_TRIM_SUIT, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_EDIT_TRIM_BOTH, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_EDIT_TRIM_HEAD, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_EDIT_TRIM_TAIL, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_EDIT_TRIM_MAIL, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_EDIT_TRIM_ZEN, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_EDIT_IND_INC, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_EDIT_IND_DEC, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_EDIT_JOIN, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_EDIT_EDIT_MODE, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_EDIT_ADJ_QUOT, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_EDIT_ADJ_DATE, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_EDIT_ADJ_TREE, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_EDIT_ADJ_CRLF, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_EDIT_ADJ_LF, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_EDIT_SORT_ASC_N, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_EDIT_SORT_DSC_N, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_EDIT_SORT_ASC, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_EDIT_SORT_DSC, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_EDIT_HAN_ASC, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_EDIT_HAN_ALL, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_EDIT_ZEN, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_EDIT_UPPER, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_EDIT_LOWER, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_EDIT_SUB, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_EDIT_SUB_REP, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_EDIT_FMT, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_EDIT_FMT_REP, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_EDIT_MCR, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_EDIT_MCR_REP, MF_ENABLED);

		nMax = EditHist.GetMaxCount();
		if (nMax > 1) {
			EnableMenuItem(hMenu, IDM_EDIT_UNDO, MF_ENABLED);
		} else {
			EnableMenuItem(hMenu, IDM_EDIT_UNDO, MF_GRAYED);
		}
	}

	return 0;
}

//----------------------------------------------------------------------
//��EditBox�̃��b�Z�[�W����
//----------------------------------------------------------------------
LRESULT CALLBACK EventCall_EditBox(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int nStat;

	switch(message) {
		case WM_CHAR:
			nStat = EditBox_KeyDown(hWnd, wParam);
			if (nStat == 0) return 0;
		case WM_KEYDOWN:
			if (g_nExcMode != MODE_EDIT) {
				switch(wParam) {
					case VK_DELETE:	// DEL�L�[�̗}��
					case VK_BACK:	// BS�L�[�̗}��
					case VK_RETURN:	// Enter�L�[�̗}��
						return 0;
				}
			}
			break;
		case WM_DROPFILES:
			MainWnd_DropFiles(hWnd, (HDROP)wParam);
			return 0;
	}

	return CallWindowProc(lpfnEditProc, hWnd, message, wParam, lParam);
}

LRESULT EditBox_KeyDown(HWND hWnd, DWORD KeyCode)
{
	int nStat, nActKey, nConv, i, nMax, bUpdText;
	char *pText, *pConv, *pConv2, *pNow, *pStat, *pExch, *pDlmt, *pFmt, *pArg;
	char szMsg[MAX_BUF+1];
	ConvParam Param;
	Variable Lines;
	HINSTANCE hInstance;
	HWND hEdit;
#define LOWER_ALPHA	"a-z��-��"
#define UPPER_ALPHA	"A-Z�`-�y"
#define HAN_ASCII1	"a-zA-Z0-9"
#define HAN_ASCII2	"@ !\"\"#$%&'()*+,-./:;<=>?[\\]^_`{|}~"
#define ZEN_ASCII1	"��-���`-�y�O-�X"
#define ZEN_ASCII2	"���@�I�h�g���������f�i�j���{�C�|�D�^�F�G�������H�m���n�O�Q�e�o�b�p�`"
#define ZEN_KANA1	"�K,�M,�O,�Q,�S,�U,�W,�Y,�[,�],�_,�a,�d,�f,�h,�o,�r,�u,�x,�{,�p,�s,�v,�y,�|,��"
#define HAN_KANA2	"���������������������������������������������������������������"
#define HAN_KANA1	"��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,��"
#define ZEN_KANA2	"�B�u�v�A�E���@�B�D�F�H�������b�[�A�C�E�G�I�J�L�N�P�R�T�V�X�Z�\�^�`�c�e�g�i�j�k�l�m�n�q�t�w�z�}�~���������������������������J�K"

	if (g_nExcMode == MODE_HELP) {
		// �w���v������
		switch ((char)KeyCode) {
			case 'q':
			case 'Q':
				DspHelp(0);
				DspStatus("");
				break;
			case ' ':
				hEdit = GetDlgItem(hMainWnd, EditBox);
				SendMessage(hEdit, EM_SCROLL, SB_PAGEDOWN, 0); 
				break;
			case 'b':
				hEdit = GetDlgItem(hMainWnd, EditBox);
				SendMessage(hEdit, EM_SCROLL, SB_PAGEUP, 0); 
				break;
			case '>':
				hEdit = GetDlgItem(hMainWnd, EditBox);
				SendMessage(hEdit, WM_HSCROLL, SB_PAGERIGHT, 0); 
				break;
			case '<':
				hEdit = GetDlgItem(hMainWnd, EditBox);
				SendMessage(hEdit, WM_HSCROLL, SB_PAGELEFT, 0); 
				break;
			default:
				DspStatus("Press SPACE-key for more, or q when done.");
		}
		return 0;
	} else if (g_nExcMode == MODE_EDIT) {
		// �ҏW���[�h������
		switch ((char)KeyCode) {
			case 0x1B: // ESC
				// Clipboard�֕ҏW��̕�����ݒ�
				pText = GetEditBox();
				SetClipText(pText);
				// ���엚���֒ǉ�
				nMax = EditHist.GetMaxCount();
				if (nMax == 0) {
					EditHist.AddString(pText);
				} else {
					if (strcmp(pText, EditHist.GetString(nMax - 1))) {
						EditHist.AddString(pText);
					}
				}
				// �R�}���h���[�h��ݒ�
				g_nExcMode = MODE_CMD;
				DspStatus("[Command Mode]");

				free(pText);
				return 0;
			case 0x03: // Ctrl+C
			case 0x16: // Ctrl+V
			case 0x18: // Ctrl+X
			case 0x1A: // Ctrl+Z
				// ����L�[�̗}�~
				return 0;
			default:
				DspStatus("[Edit Mode] Press ESC when done.");
				return 1;
		}
	}

	nActKey = 1;
	nConv = 0;
	pStat = "";
	pText = GetEditBox();
	KeySeq.Add((char)KeyCode);
	switch (KeySeq.Get(0)) {
		case 'a':
			// ����Activate�\���ؑ�
			if (g_nAutoAct == 0) {
				g_nAutoAct = 1;
				pStat = "Auto Activate:On";
			} else {
				g_nAutoAct = 0;
				pStat = "Auto Activate:Off";
			}
			break;

		case 'e':
			// �t�@�C���̎��s
			nStat = ExecFile(pText);
			if (nStat == 1) {
				pStat = "Execute.";
			} else {
				pStat = "";
			}
			break;

		case 'Q':
			SendMessage(hMainWnd, WM_CLOSE, 0, 0);
			pStat = "Exit.";
			break;

		case ' ':
			if (*pText) {
				hEdit = GetDlgItem(hMainWnd, EditBox);
				SendMessage(hEdit, EM_SCROLL, SB_PAGEDOWN, 0); 
				pStat = "Page Down.";
			}
			break;
		case 'b':
			if (*pText) {
				hEdit = GetDlgItem(hMainWnd, EditBox);
				SendMessage(hEdit, EM_SCROLL, SB_PAGEUP, 0); 
				pStat = "Page Up.";
			}
			break;
		case '>':
			if (*pText) {
				hEdit = GetDlgItem(hMainWnd, EditBox);
				SendMessage(hEdit, WM_HSCROLL, SB_PAGERIGHT, 0); 
				pStat = "Scroll Right.";
			}
			break;
		case '<':
			if (*pText) {
				hEdit = GetDlgItem(hMainWnd, EditBox);
				SendMessage(hEdit, WM_HSCROLL, SB_PAGELEFT, 0); 
				pStat = "Scroll Left.";
			}
			break;

		case 0x1a: // ^Z�L�[
			nMax = EditHist.GetMaxCount();
			if (nMax > 1) {
				// 1�O�̕ҏW�ɖ߂�
				EditHist.DelLastString();
				--nMax;
				pNow = EditHist.GetString(nMax - 1);
				// EditBox�X�V
				SetEditBox(pNow);
				// Clipboard�֕ϊ�������ݒ�
				SetClipText(pNow);
				// ���쌋�ʐݒ�
				sprintf(szMsg, "return[%d].", nMax);
				pStat = szMsg;
			}
			break;

		case 't':
			pConv = TrimLine(pText, " \t>", TRIM_HEAD);
			if (pConv) {
				pConv2 = TrimLine(pConv, " \t�@", TRIM_BOTH);
				free(pConv);
				pConv = pConv2;
				if (pConv) {
					pStat = "Trim.";
					nConv = 1;
				}
			}
			break;
		case 'T':
			if (KeySeq.Len() == 1) {
				pStat = "Press second-key(h:head, e:end, b:both, >:mail, z:Zenkaku) for Trim.";
				nActKey = 0;
			} else if (KeySeq.Len() == 2) {
				switch (KeySeq.Get(1)) {
					case 'b':
						pConv = TrimLine(pText, " \t", TRIM_BOTH);
						if (pConv) {
							pStat = "Trim line both.";
							nConv = 1;
						}
						break;
					case 'h':
						pConv = TrimLine(pText, " \t", TRIM_HEAD);
						if (pConv) {
							pStat = "Trim line head.";
							nConv = 1;
						}
						break;
					case 'e':
						pConv = TrimLine(pText, " \t", TRIM_TAIL);
						if (pConv) {
							pStat = "Trim line end.";
							nConv = 1;
						}
						break;
					case '>':
						pConv = TrimLine(pText, " \t>", TRIM_HEAD);
						if (pConv) {
							pStat = "Trim line head('>').";
							nConv = 1;
						}
						break;
					case 'z':
						pConv = TrimLine(pText, " \t�@", TRIM_BOTH);
						if (pConv) {
							pStat = "Trim line head(Zenkaku).";
							nConv = 1;
						}
						break;
					default :
						nActKey = 0;
				}
			}
			break;
		case '+':
			pConv = AdjIndent(pText, 1);
			pStat = "Indent(+).";
			nConv = 1;
			break;
		case '-':
			pConv = AdjIndent(pText, -1);
			pStat = "Indent(-).";
			nConv = 1;
			break;
		case 'j':
			pConv = Replace(pText, 0, "\r\n", '1', "", 'a');
			pStat = "Join Lines.";
			nConv = 1;
			break;
		case 'h':
			pConv = Replace(pText, 0, ZEN_ASCII1, 'h', HAN_ASCII1, 'h');
			pConv = Replace(pConv, 1, ZEN_ASCII2, '1', HAN_ASCII2, '1');
			pStat = "Convert Hankaku.";
			nConv = 1;
			break;
		case 'H':
			pConv = Replace(pText, 0, ZEN_ASCII1, 'h', HAN_ASCII1, 'h');
			pConv = Replace(pConv, 1, ZEN_ASCII2, '1', HAN_ASCII2, '1');
			pConv = Replace(pConv, 1, ZEN_KANA1, 'c', HAN_KANA1, 'c');
			pConv = Replace(pConv, 1, ZEN_KANA2, '1', HAN_KANA2, '1');
			pStat = "Convert Hankaku.";
			nConv = 1;
			break;
		case 'z':
			pConv = Replace(pText, 0, HAN_ASCII1, 'h', ZEN_ASCII1, 'h');
			pConv = Replace(pConv, 1, HAN_ASCII2, '1', ZEN_ASCII2, '1');
			pConv = Replace(pConv, 1, HAN_KANA1, 'c', ZEN_KANA1, 'c');
			pConv = Replace(pConv, 1, HAN_KANA2, '1', ZEN_KANA2, '1');
			pStat = "Convert Zenkaku.";
			nConv = 1;
			break;
		case 'u':
			pConv = Replace(pText, 0, LOWER_ALPHA, 'h', UPPER_ALPHA, 'h');
			pStat = "Convert Uppercase.";
			nConv = 1;
			break;
		case 'l':
			pConv = Replace(pText, 0, UPPER_ALPHA, 'h', LOWER_ALPHA, 'h');
			pStat = "Convert Lowercase.";
			nConv = 1;
			break;

		case 'i':
			// ���엚���֒ǉ�
			nMax = EditHist.GetMaxCount();
			if (nMax == 0)	EditHist.AddString(pText);
			// �ҏW���[�h�ݒ�
			g_nExcMode = MODE_EDIT;
			// ���t��Ԃ̏�����
			InitDateStat();

			pStat = "[Edit Mode] Press ESC when done.";
			nActKey = 0;
			break;

		case '\"':
			pConv = AdjQuote(pText);
			if (pConv) {
				pStat = "Adjust Quotes/Brackets.";
				nConv = 1;
			}
			break;
		case 'd':
			pConv = AdjDate(pText);
			if (pConv) {
				pStat = "Adjust Date.";
				nConv = 1;
			}
			break;
		case 'y':
			pConv = CnvTree(pText);
			pStat = "Convert Tree.";
			nConv = 1;
			break;
		case 'C':
			if (KeySeq.Len() == 1) {
				pStat = "Press second-key(r:LF->CRLF, l:CRLF->LF) for Conversion.";
				nActKey = 0;
			} else if (KeySeq.Len() == 2) {
				switch (KeySeq.Get(1)) {
					case 'r':
						pConv = Lines.gsub(pText, "(?<!\r)\n", "\r\n");
						pStat = "Convert LF->CRLF.";
						nConv = 1;
						break;
					case 'l':
						pConv = Lines.gsub(pText, "\r\n", "\n");
						pStat = "Convert CRLF->LF.";
						nConv = 1;
						break;
					default :
						nActKey = 0;
				}
			}
			break;

		case 'S':
			if (KeySeq.Len() == 1) {
				pStat = "Press second-key(a:ascend(natural), d:descend(natural), A:ascend(normal), D:descend(normal)) for Sort.";
				nActKey = 0;
			} else if (KeySeq.Len() == 2) {
				switch (KeySeq.Get(1)) {
					case 'a':
						Lines.split(pText, "\r\n");
						Lines.sort(TRUE, TRUE);
						pConv = Lines.join("\r\n");
						pStat = "Sort lines in natural ascending order.";
						nConv = 1;
						break;
					case 'd':
						Lines.split(pText, "\r\n");
						Lines.sort(FALSE, TRUE);
						pConv = Lines.join("\r\n");
						pStat = "Sort lines in natural descending order.";
						nConv = 1;
						break;
					case 'A':
						Lines.split(pText, "\r\n");
						Lines.sort(TRUE, FALSE);
						pConv = Lines.join("\r\n");
						pStat = "Sort lines in normal ascending order.";
						nConv = 1;
						break;
					case 'D':
						Lines.split(pText, "\r\n");
						Lines.sort(FALSE, FALSE);
						pConv = Lines.join("\r\n");
						pStat = "Sort lines in normal descending order.";
						nConv = 1;
						break;
					default :
						nActKey = 0;
				}
			}
			break;

		case 'r':
		case 'R':
			pExch = g_ExchHist.GetString(0);
			if ((char)KeyCode == 'r' && pExch != NULL) {
				nStat = IDOK;
			} else {
				if (nMacroExc) {
					//g_ExchHist��FncParam�}��
					nStat = IDCANCEL;
					pArg = FncParam.GetString(0);
					if (pArg) {
						if (strcmp(pArg, "R") == 0) {
							pArg = FncParam.GetString(1);
							SortHist(pArg, &g_ExchHist, MAX_EXCHHIST);
							FncParam.DelString(0);
							FncParam.DelString(0);
							nStat = IDOK;
							--nMacroExc;	// �p�����[�^�����I��
						}
					}
					if (nStat != IDOK) {
						pStat = "Macro Parameter Error.";
					}
				} else {
					// ������u��
					hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
					nStat = DialogBox(hInstance, "IDD_DIALOG2",
								  hWnd, (DLGPROC)EventCall_InputWnd);
				}
			}

			if (nStat == IDOK) {
				pExch = g_ExchHist.GetString(0);
				pConv = WildReplaceLine(pText, pExch, &nStat);
				if (nStat) {
					pStat = "Replace.";
					nConv = 1;

					// ���t��Ԃ̏�����
					InitDateStat();
				} else {
					pStat = "Parameter Error.";
				}
			}
			break;

		case 'F':
			if (nMacroExc) { // �}�N������
				//g_FmtInfo.FmtHist��FncParam�}��
				nStat = IDCANCEL;
				pArg = FncParam.GetString(0);
				if (pArg) {
					if (strcmp(pArg, "F") == 0) { // ��1�J������'F'
						pArg = FncParam.GetString(1);
						g_FmtInfo.nFmtId = atoi(pArg);
						switch (g_FmtInfo.nFmtId) { // ��2�J����(���)�ŕ���
							case FMTID_COLFMT:
								pArg = FncParam.GetString(2);
								SortHist(pArg, &g_FmtInfo.FmtHist, MAX_FMTHIST);
								pArg = FncParam.GetString(3);
								g_FmtInfo.nDlmtId = atoi(pArg);
								break;
							case FMTID_MAKTBL:
								pArg = FncParam.GetString(3);
								g_FmtInfo.nDlmtId = atoi(pArg);
								break;
							case FMTID_GETINC:
							case FMTID_GETEXC:
								pArg = FncParam.GetString(2);
								SortHist(pArg, &g_FmtInfo.FmtHist, MAX_FMTHIST);
								break;
						}
						FncParam.DelString(0);
						FncParam.DelString(0);
						FncParam.DelString(0);
						FncParam.DelString(0);
						nStat = IDOK;
						--nMacroExc;	// �p�����[�^�����I��
					}
				}
				if (nStat != IDOK) {
					pStat = "Macro Parameter Error.";
					break;
				}
			} else {
				hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
				nStat = DialogBox(hInstance, "IDD_DIALOG4",
								  hWnd, (DLGPROC)EventCall_FmtWnd);
				if (nStat != IDOK) break;
			}

		case 'f':
			if (g_FmtInfo.nDlmtId == DLMTID_TAB) {
				pDlmt = "\t";
			} else {
				pDlmt = ",";
			}

			switch (g_FmtInfo.nFmtId) {
				case FMTID_ADJHTML: // HTML�R�[�h�̐��`
					pConv = AdjHtml(pText);
					if (pConv) {
						pStat = "Format HTML.";
						nConv = 1;
					}
					break;

				case FMTID_COLFMT: // �J�����f�[�^ �� �����ϊ�
					pFmt = g_FmtInfo.FmtHist.GetString(0);
					if (pFmt) {
						pConv = CnvColDat(pText, pFmt, pDlmt);
						pStat = "Format Data.";
						nConv = 1;
					}
					break;

				case FMTID_MAKTBL: // �J�����f�[�^ �� �\�ϊ�
					pConv = MakeTable(pText, pDlmt);
					pStat = "Make Table.";
					nConv = 1;
					break;

				case FMTID_GETINC: // ��������܂ލs�̒��o
					pFmt = g_FmtInfo.FmtHist.GetString(0);
					if (pFmt) {
						pConv = GrepLine(pText, pFmt, TRUE);
						pStat = "Grep.";
						nConv = 1;
					}
					break;

				case FMTID_GETEXC: // ��������܂܂Ȃ��s�̒��o
					pFmt = g_FmtInfo.FmtHist.GetString(0);
					if (pFmt) {
						pConv = GrepLine(pText, pFmt, FALSE);
						pStat = "Grep -v.";
						nConv = 1;
					}
					break;
			}
			break;

		case 'M':
			if (nMacroExc) {
				pStat = "Macro Error.";
				nMacroExc = 0;
				break;
			}
			hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
			nStat = DialogBox(hInstance, "IDD_DIALOG5",
			                  hWnd, (DLGPROC)EventCall_McrWnd);
			if (nStat != IDOK) break;

		case 'm':
			if (nMacroExc) {
				pStat = "Macro Error.";
				nMacroExc = 0;
				break;
			}
			nStat = ExcMacro(hWnd);
			if (nStat == TRUE) {
				pStat = "Macro Execute.";
			} else {
				pStat = "Macro Error.";
			}
			break;

		case 'D':
			g_DateInfo.bMacro = FALSE;
			if (nMacroExc) {
				pArg = FncParam.GetString(0);
				if (pArg) {
					if (strcmp(pArg, "D") == 0) { // ��1�J������'D'
						pArg = FncParam.GetString(1);
						g_DateInfo.nIdx = atoi(pArg);

						g_DateInfo.bMacro = TRUE;
						--nMacroExc;	// �p�����[�^�����I��
					}
				}
				if (g_DateInfo.bMacro == FALSE) {
					pStat = "Macro Parameter Error.";
					break;
				}
			}

			hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
			nStat = DialogBox(hInstance, "IDD_DIALOG6",
			                  hWnd, (DLGPROC)EventCall_DateWnd);
			if (nStat == IDOK) {
				if (g_DateInfo.pDateTime) {
					pConv = g_DateInfo.pDateTime;
					g_DateInfo.pDateTime = NULL;
					pStat = "Set DateTime.";
					nConv = 1;

					// ���엚���̃N���A
					EditHist.Clear();
					// ���t��Ԃ̏�����
					InitDateStat();
				}
			}
			break;

		case 0x1B:	// ESC
			// �}�N�����s��ԃN���A(�ً}��~�p)
			if (nMacroExc != 0) {
				nMacroExc = 0;
				pStat = "Macro Stopped.";
			}
			if (g_nSelfEvnt > 0) {
				g_nSelfEvnt = 0;
			}
			break;

		case '?':
			// ���엚���֒ǉ�
			nMax = EditHist.GetMaxCount();
			if (nMax == 0)	EditHist.AddString(pText);
			// �w���v�\��
			DspHelp(1);
			nActKey = 0;
			break;

		default:
			nActKey = 0;
			break;
	}

	if (nActKey == 1) {
		DspStatus("");
		if (nConv == 1) { // �ϊ����{
			if (strcmp(pText, pConv)) { // ���e�ύX����
				bUpdText = FALSE;
				pNow = GetClipText(NULL);
				if (pNow != NULL) {
					if (strcmp(pText, pNow) == 0) { // Clipboard�ύX�Ȃ�
						bUpdText = TRUE;
						free(pNow);
					}
				} else {
					// Clipboard�ύX�Ȃ�
					if (*pText == '\0') bUpdText = TRUE;
				}

				if (bUpdText) {
					// EditBox�X�V
					SetEditBox(pConv);
					// Clipboard�֕ϊ�������ݒ�
					SetClipText(pConv);
					// �X�e�[�^�X�o�[�֏������e�ݒ�
					DspStatus(pStat);
					// ���엚���֒ǉ�
					nMax = EditHist.GetMaxCount();
					if (nMax == 0)	EditHist.AddString(pText);
					EditHist.AddString(pConv);
				}
			}
			free(pConv);
		} else {
			DspStatus(pStat);
		}
		KeySeq.Clear();
	} else {
		if (*pStat) {
			DspStatus(pStat);
		} else {
			if (g_nExcMode == MODE_HELP) {
				DspStatus("Press SPACE-key for more, or q when done.");
			} else {
				DspStatus("Press ? for help.");
			}
			KeySeq.Clear();
		}
	}
	free(pText);

	return 0;
}

// �}�N�����s
int ExcMacro(HWND hWnd)
{
	STRLIST LineStr;
	DWORD wParam;
	int nRmax, nCmax, nRidx, nCidx, nStat;
	char *pMcrStr, *pLine, *pRef;
	char szBuf[MAX_BUF];
	
	if (g_McrInfo.Macro.GetMaxCount() > 0) {
		nMacroExc = 1;		// ���s��
		FncParam.Clear();	// �p�����[�^���X�g�N���A
		// �}�N������o���A�s���ɕ������ď���
		pMcrStr = g_McrInfo.Macro.GetString(0);
		StrSplit(pMcrStr, "\r\n", SPLIT_NONE | SPLIT_MULTI, &LineStr);
		nRmax = LineStr.GetMaxCount();
		nStat = TRUE;
		for (nRidx = 0; nRidx < nRmax && nStat == TRUE; ++nRidx) {
			// �}�N���s�̎��o��
			pLine = LineStr.GetString(nRidx);
			switch (*pLine) {
				case '#':
					// �ǂݔ�΂�(�R�����g�s)
					break;
				case 'R':
					nStat = AddFncParam(pLine, &FncParam, "S:R,P");
					if (nStat == TRUE) {
						++nMacroExc;	// �p�����[�^�����\��
						PostMessage(hWnd, WM_CHAR, 'R', 0);
					}
					break;
				case 'F':
					nStat = AddFncParam(pLine, &FncParam, "S:F,N:0-4,O,N:0-1");
					if (nStat == TRUE) {
						++nMacroExc;	// �p�����[�^�����\��
						PostMessage(hWnd, WM_CHAR, 'F', 0);
					}
					break;
				case 'D':
					nStat = AddFncParam(pLine, &FncParam, "S:D,N:0-2");
					if (nStat == TRUE) {
						++nMacroExc;	// �p�����[�^�����\��
						PostMessage(hWnd, WM_CHAR, 'D', 0);
					}
					break;
				default:
					// 1�s���̕�����EditBox�֑��M
					nCmax = strlen(pLine);
					for (nCidx = 0; nCidx < nCmax; ++nCidx) {
						pRef = pLine + nCidx;
						if (IsKanji(*pRef)) {
							nStat = FALSE;
							break;
						} else if (strchr("RFMmi", *pRef)) {
							nStat = FALSE;
							break;
						} else if (*pRef == '#') {
							break;
						} else {
							wParam = *pRef;
							PostMessage(hWnd, WM_CHAR, wParam, 0);
						}
					}
					break;
			}

			if (nStat == FALSE) {
				// �G���[�ʒm���@��v����
				sprintf(szBuf, "�}�N����%d�s�ڂŃG���[���������܂����B\n(%s)",
				        nRidx + 1, pLine);
				MsgBox(hMainWnd, szBuf, "Error", MB_OK);
			}
		}
		--nMacroExc;	// ��~
	}

	return nStat;
}

// ������𕪉����A�p�����[�^���X�g�ɒǉ�
int AddFncParam(char *pStr, STRLIST *pParam, char *pChkPrm)
{
	STRLIST Args, ChkPrm;
	int i, nCnt, nMax, nRet, nStat = TRUE;
	char *pArg;

	StrSplit(pChkPrm, ",", SPLIT_NONE, &ChkPrm);
	nMax = ChkPrm.GetMaxCount();
	StrSplit(pStr, " ", SPLIT_NONE | SPLIT_MULTI | SPLIT_EXCEL, &Args);
	nCnt = Args.GetMaxCount();
	for (i = 0; i < nMax; ++i) {
		if (i < nCnt) {
			pArg = Args.GetString(i);
			nRet = ChkMcrParam(pArg, ChkPrm.GetString(i));
			if (nRet == FALSE) nStat = FALSE;
		} else {
			pArg = "";
			nStat = FALSE;
		}
		pParam->AddString(pArg);
	}

	return nStat;
}

// �}�N���p�����[�^�̃`�F�b�N
int ChkMcrParam(char *pParam, char *pCheck)
{
/* Check���� �c type:value
   type      ��value
   ������������������
   O(�Ȃ�)   ��
   N(���l)   ��m-n
   C(1����)  ��A-Z
   S(������) ��xxx
   P(�擪/����������v)
*/
	STRLIST ChkArg;
	int nStat = FALSE;
	int nMin, nMax, nNum, nCnt, nLen;
	char *pMin, *pMax;

	if (strlen(pCheck) == 1) {
		if (*pCheck == 'O') {
			nStat = TRUE;
		} else if (*pCheck == 'P') {
			// �擪�Ɩ�����1������v�`�F�b�N
			nLen = strlen(pParam);
			if (*pParam == *(pParam + nLen - 1)) nStat = TRUE;
		}
	} else {
		if (*(pCheck + 1) == ':') {
			if (*pCheck == 'N') {
				// ���l�`�F�b�N
				if (IsNumStr(pParam)) {
					StrSplit(pCheck, "-", SPLIT_NONE, &ChkArg);
					nMin = atoi(ChkArg.GetString(0));
					nMax = atoi(ChkArg.GetString(1));
					nNum = atoi(pParam);
					if (nMin <= nNum && nNum <= nMax) nStat = TRUE;
				}
			} else if (*pCheck == 'C') {
				// 1�����`�F�b�N
				if (strlen(pParam) == 1 && strlen(pCheck) == 3) {
					StrSplit(pCheck, "-", SPLIT_NONE, &ChkArg);
					pMin = ChkArg.GetString(0);
					pMax = ChkArg.GetString(1);
					if (*pMin <= *pParam && *pParam <= *pMax) nStat = TRUE;
				}
			} else if (*pCheck == 'S') {
				// ������`�F�b�N
				if (strcmp(pParam, pCheck + 2) == 0) nStat = TRUE;
			}
		}
	}

	return nStat;
}

//----------------------------------------------------------------------
//�����ʏ���
//----------------------------------------------------------------------
// �X�e�[�^�X�\��
void DspStatus(char *pStr)
{
	HWND hStatus;
	char szBuf[MAX_BUF+1];

	sprintf(szBuf, "%s ", pStr);
	hStatus = GetDlgItem(hMainWnd, StatusBar);
	SendMessage(hStatus, SB_SETTEXT, 255 | 0, (WPARAM)szBuf);
	SendMessage(hStatus, WM_SIZE, 0, 0);
}

// EditBox�̕�����擾
char *GetEditBox()
{
	HWND hEdit;
	int cbSize;
	char *pText;

	hEdit = GetDlgItem(hMainWnd, EditBox);
	cbSize = GetWindowTextLength(hEdit);
	pText = (char *)malloc(cbSize + 1);
	GetWindowText(hEdit, pText, cbSize + 1);

	return pText;
}

// EditBox�֕�����ݒ�
void SetEditBox(char *pText)
{
	HWND hEdit;

	hEdit = GetDlgItem(hMainWnd, EditBox);
	SetWindowText(hEdit, pText);
	UpdateWindow(hEdit);
	SetFocus(hEdit);
}

// �N���b�v�{�[�h����̃e�L�X�g�f�[�^�擾
char *GetClipText(HWND hWnd)
{
	HANDLE hClipData = NULL;
	char *pText, *pCopy;
	int nFormat, bRet;
	char szBuf[MAX_BUF+1];

	// �N���b�v�{�[�h���̃e�L�X�g�m�F
	bRet = IsClipboardFormatAvailable(CF_TEXT);
	if (bRet) {
		// �N���b�v�{�[�h�̃I�[�v��
		bRet = OpenClipboard(hWnd);
		if (bRet) {
			// �N���b�v�{�[�h�f�[�^�̎擾
			hClipData = GetClipboardData(CF_TEXT);

			// �N���b�v�{�[�h�̃e�L�X�g���R�s�[(�f�[�^�̃n���h���͉�����Ȃ�)
			if (hClipData != NULL) {
				pText = (char *)GlobalLock(hClipData);
				pCopy = strdup(pText);
				GlobalUnlock(hClipData);
			} else {
				pCopy = NULL;
#if 0
				nFormat = EnumClipboardFormats(0);
				if (nFormat != 0) {
					switch(nFormat) {
					case CF_TEXT: DspStatus("CF_TEXT"); break;
					case CF_BITMAP: DspStatus("CF_BITMAP"); break;
					case CF_METAFILEPICT: DspStatus("CF_METAFILEPICT"); break;
					case CF_SYLK: DspStatus("CF_SYLK"); break;
					case CF_DIF: DspStatus("CF_DIF"); break;
					case CF_TIFF: DspStatus("CF_TIFF"); break;
					case CF_OEMTEXT: DspStatus("CF_OEMTEXT"); break;
					case CF_DIB: DspStatus("CF_DIB"); break;
					case CF_PALETTE: DspStatus("CF_PALETTE"); break;
					case CF_PENDATA: DspStatus("CF_PENDATA"); break;
					case CF_RIFF: DspStatus("CF_RIFF"); break;
					case CF_WAVE: DspStatus("CF_WAVE"); break;
					case CF_UNICODETEXT: DspStatus("CF_UNICODETEXT"); break;
					case CF_ENHMETAFILE: DspStatus("CF_ENHMETAFILE"); break;
					case CF_HDROP: DspStatus("CF_HDROP"); break;
					case CF_LOCALE: DspStatus("CF_LOCALE"); break;
					case CF_MAX: DspStatus("CF_MAX"); break;
					case CF_OWNERDISPLAY: DspStatus("CF_OWNERDISPLAY"); break;
					case CF_DSPTEXT: DspStatus("CF_DSPTEXT"); break;
					case CF_DSPBITMAP: DspStatus("CF_DSPBITMAP"); break;
					case CF_DSPMETAFILEPICT: DspStatus("CF_DSPMETAFILEPICT"); break;
					case CF_DSPENHMETAFILE:	DspStatus("CF_DSPENHMETAFILE"); break;
					default:
						GetClipboardFormatName(nFormat, szBuf, MAX_BUF);
						DspStatus(szBuf);
					}
				}
#endif
			}
			CloseClipboard();
		} else {
			pCopy = NULL;
		}
	} else {
		pCopy = NULL;
	}

	return pCopy;
}

// �N���b�v�{�[�h�փe�L�X�g�f�[�^�ݒ�
void SetClipText(char *pData)
{
	HGLOBAL hText;
	int nSize;
	char *pText;

	nSize = strlen(pData);
	hText = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, nSize + 1);
	pText = (char *)GlobalLock(hText);
	strcpy(pText, pData);
	GlobalUnlock(hText);

	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hText);
	CloseClipboard();
	//g_nSelfEvnt = 1;
	++g_nSelfEvnt;
}

// ������u��
//   pOrg	:�ϊ��O�e�L�X�g
//   nFreFlg:�ϊ��O�e�L�X�g�̃��������(0:������Ȃ��A1:�������)
//   pSrch	:�����f�[�^
//   cStype	:�����f�[�^�^(a:�S�́A1:1�����`���Ah:From-To�`���Ac:CSV�`��)
//   pConv	:�ϊ��f�[�^
//   cCtype	:�ϊ��f�[�^�^(a:�S�́A1:1�����`���Ah:From-To�`���Ac:CSV�`��)
char *Replace(char *pOrg, int nFreFlg, char *pSrch, char cStype, char *pConv, char cCtype)
{
	StrData OrigStr, SrchStr, ConvStr;
	char *pOrigPtr, *pSrchPtr, *pConvPtr;
	int nOrigLen, nSrchLen, nConvLen;
	StrEdit OutBuf;
	char *pOutStr, *pConvBuf;
	int nIdx, nOfset;
	int nMchId;
char buf[MAX_BUF+1];
#define MCH_NONE	0	// ��v�Ȃ�
#define MCH_VALUE	1	// ��v(�l)
#define MCH_FROMTO	2	// ��v(From-To)

	// �o�̓o�b�t�@������
	pOutStr = OutBuf.AddEdit("", 0);

	// �p�����[�^�ݒ�
	OrigStr.SetData(pOrg, '1');
	SrchStr.SetData(pSrch, cStype);
	ConvStr.SetData(pConv, cCtype);

	// �ݒ肷��ϊ��������i�[���郁�����̈�m��(�R�s�[�����l�͏㏑���Ŏg�p)
	pConvBuf = strdup(ConvStr.GetCurPtr());

	// �p�����[�^�̎Q�ƈʒu�擾
	pOrigPtr = OrigStr.GetCurPtr();
	pSrchPtr = SrchStr.GetCurPtr();
	nSrchLen = SrchStr.GetCurLen();
	while (*pOrigPtr) {
		nMchId = MCH_NONE;
		while (*pSrchPtr) {
			// �Ώە�����ƌ��������̈�v�𔻒�
			if (SrchStr.GetType() == 'h') {
				nOfset = SrchStr.ChkRange(pOrigPtr);
				if (nOfset != -1) {
					nMchId = MCH_FROMTO;
					break;
				}
			} else {
				if (memcmp(pOrigPtr, pSrchPtr, nSrchLen) == 0) {
					nMchId = MCH_VALUE;
					break;
				}
			}

			// ���̌��������ɍX�V
			SrchStr.MovNextPtr();
			pSrchPtr = SrchStr.GetCurPtr();
			nSrchLen = SrchStr.GetCurLen();
		}

		if (nMchId == MCH_VALUE) {
			// ���������ƈ�v������ϊ��������o��
			if (ConvStr.GetType() == 'a') {
				pConvPtr = ConvStr.GetCurPtr();
			} else {
				nIdx = SrchStr.GetCurIdx();
				pConvPtr = ConvStr.GetIdxPtr(nIdx);
			}
			nConvLen = ConvStr.GetCurLen();
			pOutStr = OutBuf.AddEdit(pConvPtr, nConvLen);
			OrigStr.UpdCurPtr(nSrchLen);
		} else if (nMchId == MCH_FROMTO) {
			// From-To�`���̌��������ƈ�v������ϊ��������o��
			nIdx = SrchStr.GetCurIdx();
			ConvStr.GetRangeStr(nIdx, nOfset, pConvBuf);
			pOutStr = OutBuf.AddEdit(pConvBuf, 0);
			if (nSrchLen > 2) nSrchLen /= 2;
			OrigStr.UpdCurPtr(nSrchLen);
		} else {
			// ��v���Ȃ���ΑΏە������o��
			nOrigLen = OrigStr.GetCurLen();
			pOutStr = OutBuf.AddEdit(pOrigPtr, nOrigLen);
			OrigStr.MovNextPtr();
		}
		// ����������擪�ɖ߂�
		SrchStr.MovTopPtr();

		pOrigPtr = OrigStr.GetCurPtr();
		pSrchPtr = SrchStr.GetCurPtr();
		nSrchLen = SrchStr.GetCurLen();
	}

	// ���������
	free(pConvBuf);
	if (nFreFlg == 1) free(pOrg);

	return pOutStr;
}

// �]���ȃX�y�[�X/�^�u�̍폜
char *TrimLine(char *pStr, char *pDel, int nType)
{
	StrData OrigStr;
	StrEdit Conv, Save;
	char *pOrig, *pConv = NULL, *pSave = NULL;
	int nLen, nEdge;

	nEdge = 1;
	OrigStr.SetData(pStr, '1');
	pOrig = OrigStr.GetCurPtr();
	nLen = OrigStr.GetCurLen();
	while (*pOrig) {
		if (nEdge == 1) {
			if (StrChr(pDel, pOrig)) {
				if (nType == TRIM_TAIL) {
					pConv = Conv.AddEdit(pOrig, nLen);
				}
			} else if (*pOrig == '\n') {
				pConv = Conv.AddEdit(pOrig, nLen);
			} else {
				pConv = Conv.AddEdit(pOrig, nLen);
				nEdge = 0;
			}
		} else {
			if (StrChr(pDel, pOrig)) {
				pSave = Save.AddEdit(pOrig, nLen);
			} else if (*pOrig == '\r' || *pOrig == '\n') {
				if (nType == TRIM_HEAD) {
					pConv = Conv.AddEdit(pSave, 0);
				}
				pSave = Save.Clear();
				pConv = Conv.AddEdit(pOrig, nLen);
				nEdge = 1;
			} else {
				pConv = Conv.AddEdit(pSave, 0);
				pConv = Conv.AddEdit(pOrig, nLen);
				pSave = Save.Clear();
			}
		}

		OrigStr.MovNextPtr();
		pOrig = OrigStr.GetCurPtr();
		nLen = OrigStr.GetCurLen();
	}

	// �ϊ����镶����̍Ōオ���s�Ŗ����ꍇ�̑Ώ�
	if (nType == TRIM_HEAD) {
		pConv = Conv.AddEdit(pSave, 0);
		pSave = Save.Clear();
	}

	if (pSave != NULL) free(pSave);

	return pConv;
}

// �C���f���g�̒���
char *AdjIndent(char *pStr, int nType)
{
	StrData OrigStr;
	StrEdit Conv;
	char *pOrig, *pConv;
	int nLen;
	BOOL bHead;

	bHead = TRUE;
	OrigStr.SetData(pStr, '1');
	pOrig = OrigStr.GetCurPtr();
	nLen = OrigStr.GetCurLen();
	while (*pOrig) {
		if (bHead) {
			if (*pOrig == '\r' || *pOrig == '\n') {
				pConv = Conv.AddEdit(pOrig, nLen);
			} else if (nType > 0) {
				pConv = Conv.AddEdit(" ", 0);
				pConv = Conv.AddEdit(pOrig, nLen);
				bHead = FALSE;
			} else if (nType < 0) {
				if (*pOrig != ' ') {
					pConv = Conv.AddEdit(pOrig, nLen);
				}
				bHead = FALSE;
			}
		} else {
			if (*pOrig == '\r' || *pOrig == '\n') bHead = TRUE;
			pConv = Conv.AddEdit(pOrig, nLen);
		}
		OrigStr.MovNextPtr();
		pOrig = OrigStr.GetCurPtr();
		nLen = OrigStr.GetCurLen();
	}

	return pConv;
}

// ����̒���
char *AdjQuote(char *pStr)
{
	char *pRet = NULL;
	char *pFind, *pConv, *pLast;
	int nIdx, nNext, nLen;
	char *HEAD_QUOTE = "\"(<[�u�y�i���g";
	char *TAIL_QUOTE = "\")>]�v�z�j���h";

	pFind = StrChr(HEAD_QUOTE, pStr);
	if (pFind) { // �s���͊���̊J�n�H
		// �s���̕s�v�����폜
		pConv = TrimLine(pStr, "\t\r\n", TRIM_TAIL);

		nIdx = (int)(pFind - HEAD_QUOTE);
		pLast = GetLastChar(pConv);
		nLen = strlen(pLast);
		if (!memcmp(pLast, TAIL_QUOTE + nIdx, nLen)) {	// �s���͊���̏I���H
			// �s��/�s���̊���폜
			DelHeadTailChar(pConv);

			if (g_nCtrlStat == CTRL_DEL) { // �폜���
				nNext = nIdx + (IsKanji(*pFind) ? 2 : 1);
				if (*(HEAD_QUOTE + nNext)) {
					// ���̕����Ŋ���
					pRet = AddHeadTailChar(pConv, HEAD_QUOTE + nNext,
					                              TAIL_QUOTE + nNext);
					free(pConv);
				} else {
					pRet = pConv;
				}
			} else {
				pRet = pConv;
				// �폜��Ԑݒ�
				g_nCtrlStat = CTRL_DEL;
			}
		} else {
			// ""�Ŋ���
			pRet = AddHeadTailChar(pConv, "\"", "\"");
			// �폜��Ԑݒ�
			g_nCtrlStat = CTRL_DEL;

			free(pConv);
		}
	} else {
		// ""�Ŋ���
		pRet = AddHeadTailChar(pStr, "\"", "\"");
		// �폜��Ԑݒ�
		g_nCtrlStat = CTRL_DEL;
	}

	return pRet;
}

// �s��/�s���ɕ����̕t�^
char *AddHeadTailChar(char *pStr, char *pHead, char *pTail)
{
	int nStrLen, nHeadLen, nTailLen;
	char *pRet = NULL;

	nStrLen = strlen(pStr);
	nHeadLen = (IsKanji(*pHead) ? 2 : 1);
	nTailLen = (IsKanji(*pTail) ? 2 : 1);
	pRet = (char *)malloc(nStrLen + nHeadLen + nTailLen + 1);

	memcpy(pRet, pHead, nHeadLen);
	strcpy(pRet + nHeadLen, pStr);
	memcpy(pRet + nHeadLen + nStrLen, pTail, nTailLen);
	*(pRet + nHeadLen + nStrLen + nTailLen) = '\0';

	return pRet;
}

// �s��/�s���̕����폜
void DelHeadTailChar(char *pStr)
{
	int nLen;
	char *pLast;

	// �s���폜
	nLen = (IsKanji(*pStr) ? 2 : 1); 
	strcpy(pStr, pStr + nLen);

	// �s���폜
	pLast = GetLastChar(pStr);
	*pLast = '\0';
}

// �Ō��1�����擾
char *GetLastChar(char *pStr)
{
	char *pCur, *pPrev = NULL;

	pCur = pStr;
	while (*pCur) {
		pPrev = pCur;
		if (IsKanji(*pCur)) {
			pCur += 2;
		} else {
			++pCur;
		}
	}

	return pPrev;
}

// �t�@�C��������c���[�\���ւ̕ϊ�
char *CnvTree(char *pStr)
{
	int nLayer, nMargin, nDepth;
	char *pFile, *pTree;
	TREEINFO Tree;

	// �c���[�\������t�@�C�����֕ϊ�
	pFile = Tree.Tree2File(pStr, &nLayer, &nMargin);

	// �t�@�C�����̓o�^
	Tree.RegFile(pFile);
	free(pFile);

	// �t�@�C����񂩂�c���[�\���ւ̕ϊ�
	nDepth = -1 * nLayer;
	pTree = Tree.GetTree(nDepth, nMargin);

	return pTree;
}

// �t�@�C���̎��s
int ExecFile(char *pFile)
{
	HWND hEdit;
	int nStat, nLen, nLidx, nMax;
	char exec[MAX_PATH+1], *pParam, *pLine;
	STRLIST ClpLine;

	// �J�[�\���ʒu�̍s�擾
	hEdit = GetDlgItem(hMainWnd, EditBox);
	nLidx = GetCurLine(hEdit);
	StrSplit(pFile, "\r\n", SPLIT_NONE | SPLIT_SINGLE, &ClpLine);
	nMax = ClpLine.GetMaxCount();
	if (nLidx < nMax) {
		pLine = ClpLine.GetString(nLidx);
	} else {
		return 0;
	}

	// �t�@�C�������_�u���N�H�[�g�Ŋ���
	nLen = strlen(pLine);
	pParam = (char *)malloc(nLen + 3);
	sprintf(pParam, "\"%s\"", pLine);

	/* �t�@�C�������擾 */
	nStat = GetFileAttributes(pLine);
	if (nStat != -1 && (nStat & FILE_ATTRIBUTE_DIRECTORY)) {
		/* �f�B���N�g���̏ꍇ�AExplorer�N�� */
		nStat = (int)ShellExecute(NULL, "explore", pParam,
		                          NULL, NULL, SW_SHOWNORMAL);
	} else {
		/* �֘A�t����ꂽ���s�t�@�C��������΋N�� */
		nStat = (int)FindExecutable(pParam, NULL, exec);
		if (nStat > 32) {
			nStat = (int)ShellExecute(NULL, NULL, exec, pParam,
			                          NULL, SW_SHOWNORMAL);
		} else if (!CmpStr(TailStr(pLine, 4), ".lnk") ||
		           !CmpStr(TailStr(pLine, 5), ".html") ||
		           !CmpStr(TailStr(pLine, 4), ".htm") ||
		           !memcmp(pLine, "http://", 7) ||
		           !memcmp(pLine, "https://", 8)) {
			nStat = (int)ShellExecute(NULL, NULL, pLine,
			                          NULL, NULL, SW_SHOWNORMAL);
		} else {
			nStat = -1;
		}
	}

	free(pParam);

	if (nStat > 32) {
		return 1;
	} else {
		return 0;
	}
}

// TexBox���J�[�\���̍s�ʒu�擾
int GetCurLine(HWND hEdit)
{
	DWORD wParam, lParam;
	int nCurPos, nCsrPos = 0, nBytePos = 0, nLine = 0;
	char *pText, ch;

	// TexBox���J�[�\���ʒu�擾
	SendMessage(hEdit, EM_GETSEL, (DWORD)&wParam, (DWORD)&lParam);
	nCurPos = wParam;

	pText = GetEditBox();
	if (pText) {
		ch = pText[nBytePos];
		while (ch != 0 && nBytePos < nCurPos) {
			// �Q�ƕ����ʒu�X�V
			if (IsKanji(ch)) {
				nBytePos += 2;
			} else {
				nBytePos++;
			}
			if (ch != 0x0d) nCsrPos++;

			// �s�ʒu�X�V
			if (ch == 0x0a) ++nLine;

			ch = pText[nBytePos];
		}

		free(pText);
	}

	return nLine;
}

// �s�P�ʂ̃��C���h�J�[�h�u��
char *WildReplaceLine(char *pStr, char *pParam, int *lpStat)
{
#define CTRL_CHAR "\\t,\\n,\\r,\\f"
#define CTRL_DATA "\t,\r\n,\r,\f"
	int nStat = -1, nCnt, Lidx, Ridx, nOneLf;
	BOOL bCrLf;
	char szDlmt[MAX_BUF+1];
	char *pOrg, *pValue, *pSrch, *pConv, *pOrgLine, *pDstLine;
	char *pRet = NULL;
	STRLIST InParam, OrgLine, DstLine;

	// ����/�u�������̕���
	if (*pParam) {
		// �f���~�^�ݒ�
		szDlmt[0] = *pParam;
		szDlmt[1] = '\0';
		// ���䕶���ϊ�
		pValue = Replace(pParam + 1, 0, CTRL_CHAR, 'c', CTRL_DATA, 'c');
		// �f���~�^�ŕ���
		StrSplit(pValue, szDlmt, SPLIT_NONE | SPLIT_SINGLE, &InParam);
		nCnt = InParam.GetMaxCount();

		free(pValue);
		if (nCnt > 1) nStat = 0;
	}

	if (nStat < 0) {
		*lpStat = 0;
	} else {
		pOrg = pStr;
		// ���͂��ꂽ����/�u���̑g�������ɕϊ�
		for (Ridx = 0; (Ridx + 1) < nCnt; Ridx += 2) {
			// ���s�ŕ���
			StrSplit(pOrg, "\r\n", SPLIT_SEP | SPLIT_MULTI, &OrgLine);

			// ����/�u���p�����[�^�擾
			pSrch = InParam.GetString(Ridx);
			pConv = InParam.GetString(Ridx + 1);

			// �����p�����[�^�̒P�Ɖ��s����
			if (strcmp(pSrch, "\r\n") == 0) {
				bCrLf = TRUE;
			} else {
				bCrLf = FALSE;
			}

			// �s���ɒu��
			for (Lidx = 0; Lidx < OrgLine.GetMaxCount(); ++Lidx) {
				pOrgLine = OrgLine.GetString(Lidx);

				pDstLine = WildReplace(pOrgLine, pSrch, pConv);
				if (bCrLf == TRUE) {
					// �P�Ɖ��s�̓��ꏈ��
					pOrgLine = pDstLine;
					pDstLine = OnlyLfReplace(pOrgLine, pConv);
					free(pOrgLine);
				}
				DstLine.AddString(pDstLine);
				free(pDstLine);
			}

			// �s���̒u�����}�[�W
			if (Ridx > 0) free(pOrg);
			pOrg = DstLine.MergeAllStr("");
			OrgLine.Clear();
			DstLine.Clear();
		}

		pRet = pOrg;
		*lpStat = 1;
	}

	return pRet;
}

// �p�����[�^�t�@�C���̎捞��
void LoadParamFile(char *pExcFile)
{
	int nMax, nIdx, nStt, nType, nPosFlg = 0;
	char szBuf[4*MAX_BUF+1];
	FILE *fp;
	enum Pid {ID_NONE, ID_WINDOW, ID_OPTION, ID_MACROS};
	Pid nPid = ID_NONE;
	INT cx = 0, cy = 0, sx = 0, sy = 0;
	int nHotKey;
	int bName = TRUE, nMcrIdx = -1;
	char *pOld, *pNew;
	
	/* �p�����[�^�t�@�C�����̎擾 */
	nMax = CountChar(pExcFile, '\\');
	nIdx = SrchCharPos(pExcFile, '\\', nMax);
	if (*pExcFile == '"') {
		nStt = 1;
	} else {
		nStt = 0;
	}
	memcpy(ParamFile, pExcFile + nStt, nIdx - nStt + 1);
	strcpy(ParamFile + nIdx - nStt + 1, PARAM_FILE);
	/* �p�����[�^�t�@�C���Ǎ��� */
	fp = fopen(ParamFile, "r");
	if (fp == NULL)	return;
	while (fgets(szBuf, 4*MAX_BUF, fp) != NULL) {
		/* ���s�폜*/
		SubStr(szBuf, "\n", "", 0);

		if (nPid == ID_NONE) {
			/* �J�e�S���̔��� */
			if (!strcmp(szBuf, "[Window]")) {
				nPid = ID_WINDOW;
				nPosFlg = 1;
			} else if (!strcmp(szBuf, "[Option]")) {
				nPid = ID_OPTION;
			} else if (!strcmp(szBuf, "[Macros]")) {
				nPid = ID_MACROS;
			}
		} else if (nPid == ID_WINDOW) {
			/* �E�B���h�E�ʒu/�T�C�Y�擾 */
			if (*szBuf == '\0') {
				nPid = ID_NONE;
			} else if ((nIdx = ChkParam(szBuf, "Left")) > 0) {
				sx = atoi(szBuf + nIdx);
			} else if ((nIdx = ChkParam(szBuf, "Top")) > 0) {
				sy = atoi(szBuf + nIdx);
			} else if ((nIdx = ChkParam(szBuf, "Width")) > 0) {
				cx = atoi(szBuf + nIdx);
			} else if ((nIdx = ChkParam(szBuf, "Height")) > 0) {
				cy = atoi(szBuf + nIdx);
			}
		} else if (nPid == ID_OPTION) {
			/* �I�v�V�����擾 */
			if (*szBuf == '\0') {
				nPid = ID_NONE;
			} else if ((nIdx = ChkParam(szBuf, "AutoActive")) > 0) {
				if (atoi(szBuf + nIdx) == 1) {
					g_nAutoAct = 1;
				} else {
					g_nAutoAct = 0;
				}
			} else if ((nIdx = ChkParam(szBuf, "NetworkDrive")) > 0) {
				if (atoi(szBuf + nIdx) == 1) {
					g_nNwDrvNam = 1;
				} else {
					g_nNwDrvNam = 0;
				}
			} else if ((nIdx = ChkParam(szBuf, "HotKey")) > 0) {
				sscanf(szBuf + nIdx, "%x", &nHotKey);
				g_wHotKey = (WORD)nHotKey;
				SendMessage(hMainWnd, WM_SETHOTKEY, g_wHotKey , 0);
			} else if ((nIdx = ChkParam(szBuf, "ReplaceExpr")) > 0) {
				g_ExchHist.AddString(szBuf + nIdx);
			} else if ((nIdx = ChkParam(szBuf, "FormatId")) > 0) {
				g_FmtInfo.nFmtId = atoi(szBuf + nIdx);
			} else if ((nIdx = ChkParam(szBuf, "FormatParam")) > 0) {
				g_FmtInfo.FmtHist.AddString(szBuf + nIdx);
			} else if ((nIdx = ChkParam(szBuf, "FormatDlmt")) > 0) {
				if (atoi(szBuf + nIdx) == 0) {
					g_FmtInfo.nDlmtId = 0;
				} else {
					g_FmtInfo.nDlmtId = 1;
				}
			} else if ((nIdx = ChkParam(szBuf, "DateTimeFormat")) > 0) {
				g_DateInfo.nIdx = atoi(szBuf + nIdx);
			}
		} else if (nPid == ID_MACROS) {
			/* �}�N���擾 */
			if (*szBuf == '*') {
				++nMcrIdx;
				g_McrInfo.Name.AddString(szBuf + 1);
				g_McrInfo.Macro.AddString("");
			} else if (*szBuf) {
				pOld = g_McrInfo.Macro.GetString(nMcrIdx);
				pNew = (char *)malloc(strlen(pOld) + strlen(szBuf) + 3);
				strcpy(pNew, pOld);
				if (*pNew) strcat(pNew, "\r\n");
				strcat(pNew, szBuf);
				g_McrInfo.Macro.InsString(nMcrIdx, pNew);
				g_McrInfo.Macro.DelString(nMcrIdx + 1);
				free(pNew);
			}
		}
	}
	fclose(fp);

	// �E�B���h�E�ʒu/�T�C�Y�̕ύX
	if (nPosFlg == 1) {
		SetWindowPos(hMainWnd, HWND_TOP, sx, sy, cx, cy, 0);
	}

	return;
}

// �p�����[�^�����̔���
int ChkParam(char *pLine, char *pParam)
{
	int nLen;

	nLen = strlen(pParam);
	if (!strncmp(pLine, pParam, nLen) && pLine[nLen] == '=') {
		return nLen + 1;
	} else {
		return 0;
	}
}

// �p�����[�^�t�@�C���ւ̕ۑ�
void SaveParamFile()
{
	FILE *fp;
	char *pExch, *pFmt, *pName, *pMacro, *pConv;
	RECT OwnWin;
	WORD wHotKey;

	fp = fopen(ParamFile, "w");
	if (fp == NULL)	return;

	GetWindowRect(hMainWnd, &OwnWin);

	fprintf(fp, "[Window]\n");
	fprintf(fp, "Left=%d\n", OwnWin.left);
	fprintf(fp, "Top=%d\n", OwnWin.top);
	fprintf(fp, "Width=%d\n", OwnWin.right - OwnWin.left);
	fprintf(fp, "Height=%d\n", OwnWin.bottom - OwnWin.top);
	fprintf(fp, "\n");

	fprintf(fp, "[Option]\n");
	fprintf(fp, "AutoActive=%d\n", g_nAutoAct);
	fprintf(fp, "NetworkDrive=%d\n", g_nNwDrvNam);
	fprintf(fp, "HotKey=%08X\n", g_wHotKey);
	for (int i = 0; i < g_ExchHist.GetMaxCount(); ++i) {
		pExch = g_ExchHist.GetString(i);
		fprintf(fp, "ReplaceExpr=%s\n", pExch);
	}
	fprintf(fp, "FormatId=%d\n", g_FmtInfo.nFmtId);
	for (int i = 0; i < g_FmtInfo.FmtHist.GetMaxCount(); ++i) {
		pFmt = g_FmtInfo.FmtHist.GetString(i);
		fprintf(fp, "FormatParam=%s\n", pFmt);
	}
	fprintf(fp, "FormatDlmt=%d\n", g_FmtInfo.nDlmtId);
	fprintf(fp, "DateTimeFormat=%d\n", g_DateInfo.nIdx);
	fprintf(fp, "\n");

	fprintf(fp, "[Macros]\n");
	for (int i = 0; i < g_McrInfo.Name.GetMaxCount(); ++i) {
		pName = g_McrInfo.Name.GetString(i);
		fprintf(fp, "*%s\n", pName);
		pMacro = g_McrInfo.Macro.GetString(i);
		pConv = Replace(pMacro, 0, "\r", '1', "", 'a');
		fprintf(fp, "%s\n", pConv);
		free(pConv);
	}

	fclose(fp);
	return;
}

// Windows��OS�o�[�W�����擾
int GetWinVer(DWORD *lpMajor, DWORD *lpMinor)
{
typedef void (WINAPI *RtlGetVersion_FUNC)(OSVERSIONINFOEX *);
	HMODULE hMod;
	RtlGetVersion_FUNC func;
	OSVERSIONINFO Ver;
	OSVERSIONINFOEX VerEx;

	hMod = LoadLibrary(TEXT("ntdll.dll"));
	if (hMod) {
		func = (RtlGetVersion_FUNC)GetProcAddress(hMod, "RtlGetVersion");
		if (func == 0) {
			FreeLibrary(hMod);
			return FALSE;
		}

		ZeroMemory(&VerEx, sizeof(VerEx));
		VerEx.dwOSVersionInfoSize = sizeof(VerEx);
		func(&VerEx);
		*lpMajor = VerEx.dwMajorVersion;
		*lpMinor = VerEx.dwMinorVersion;

		FreeLibrary(hMod);
	} else {
		VerEx.dwOSVersionInfoSize = sizeof(VerEx);
		if (GetVersionEx((OSVERSIONINFO *)&VerEx)) {
			*lpMajor = VerEx.dwMajorVersion;
			*lpMinor = VerEx.dwMinorVersion;
		} else {
			Ver.dwOSVersionInfoSize = sizeof(Ver);
			GetVersionEx(&Ver);
			*lpMajor = Ver.dwMajorVersion;
			*lpMinor = Ver.dwMinorVersion;
		}
	}

	return TRUE;
}

// �o�[�W�������̎擾
void GetVerInfo(char *pFmt, char *pOutStr)
{
	sprintf(pOutStr, pFmt, VERSION, DATE);
}

// �w���v�̕\��
void DspHelp(int nAct)
{
#define GetMonitorRect(rc)  SystemParametersInfo(SPI_GETWORKAREA,0,rc,0)
	int i, nMax;
	char *pNow, *pText;
	STRLIST HelpStr;
	INT cx, cy, sx, sy;
	RECT Screen;
	static RECT OrgWin;
	char *pHelp[] = {
		"�yKey�R�}���h�z",
		"  a    : ClipBoard�ύX���̎���Activate�ؑ�",
		"  e    : �t�@�C���Ƃ��Ď��s",
		"  ^Z   : �ҏW��߂�",
		" -------------------------------------------------------------------",
		"  t    : �s���̋�(�S�p�܂�)/�^�u/'>'+�s���̋�(�S�p�܂�)/�^�u�폜",
		"  Tb   : �s��/�s���̋�/�^�u�폜",
		"  Tz   : �s��/�s���̋�(�S�p�܂�)/�^�u�폜",
		"  Th   : �s���̋�/�^�u�폜",
		"  T>   : �s���̋�/�^�u/'>'�폜",
		"  Te   : �s���̋�/�^�u�폜",
		" -------------------------------------------------------------------",
		"  +    : �s���̋󔒃C���f���g����",
		"  -    : �s���̋󔒃C���f���g����",
		" -------------------------------------------------------------------",
		"  z    : �S�p�ϊ�",
		"  H    : ���p�ϊ�(�J�i�܂�)",
		"  h    : ���p�ϊ�(�J�i����)",
		"  u    : �啶���ϊ�",
		"  l    : �������ϊ�",
		" -------------------------------------------------------------------",
		"  j    : �s�̌���",
		"  i    : �ҏW�J�n(ESC�L�[�ŕҏW�I��)",
		"  \"    : ���蕶���̕ύX �c \"\",(),<>,[],�u�v,�y�z,�i�j,����,�g�h",
		"  d    : ���t�����̕ύX �c m/d, yyyy/m/d, m��d��, yyyy�Nm��d��, ...",
		"  y    : �t�@�C�������X�g���c���[�\���̕ύX",
		"  Cr   : ���s�R�[�h�ϊ�(LF��CRLF)",
		"  Cl   : ���s�R�[�h�ϊ�(CRLF��LF)",
		" -------------------------------------------------------------------",
		"  Sa   : �����\�[�g(���R��)",
		"  Sd   : �~���\�[�g(���R��)",
		"  SA   : �����\�[�g(�����R�[�h��)",
		"  SD   : �~���\�[�g(�����R�[�h��)",
		" -------------------------------------------------------------------",
		"  R    : �u��(����/�u����������)",
		"  r    : �u��(�J�Ԃ�)",
		"  F    : �����ϊ�(�ϊ����/�p�����[�^����)",
		"  f    : �����ϊ�(�J�Ԃ�)",
		"  D    : �����}��(�ݒ�`��/�N����/�����I��)",
		"  M    : �}�N�����s(�V�K/�捞/�ҏW/�폜/���s)",
		"  m    : �}�N�����s(�J�Ԃ�)",
		" -------------------------------------------------------------------",
		"  Space: 1�y�[�W���ɃX�N���[��",
		"  b    : 1�y�[�W��ɃX�N���[��",
		"  >    : 1��ʉE�ɃX�N���[��",
		"  <    : 1��ʍ��ɃX�N���[��",
		NULL};

	if (nAct == 1 && g_nExcMode != MODE_HELP) {
		// �w���v��EditBox�֐ݒ�
		for (i = 0; pHelp[i]; ++i) {
			HelpStr.AddString(pHelp[i]);
		}
		pText = HelpStr.MergeAllStr("\r\n");
		SetEditBox(pText);
		free(pText);

		// Window�ʒu����
		GetWindowRect(hMainWnd, &OrgWin);
		GetMonitorRect(&Screen);
		sx = OrgWin.left;
		sy = OrgWin.top;
		cx = OrgWin.right - OrgWin.left;
		cy = OrgWin.bottom - OrgWin.top;
		if (cy < 400) cy = 400;
		if (Screen.bottom < (sy + cy)) {
			sy = Screen.bottom - cy;
		}
		SetWindowPos(hMainWnd, HWND_TOP, sx, sy, cx, cy, 0);

		// �w���v�\�����\��On
		g_nExcMode = MODE_HELP;
	} else if (nAct == 0 && g_nExcMode == MODE_HELP) {
		// ���엚������EditBox�����ɖ߂�
		nMax = EditHist.GetMaxCount();
		if (nMax > 0) {
			pNow = EditHist.GetString(nMax - 1);
			SetEditBox(pNow);
		}

		// Window�ʒu����
		sx = OrgWin.left;
		sy = OrgWin.top;
		cx = OrgWin.right - OrgWin.left;
		cy = OrgWin.bottom - OrgWin.top;
		SetWindowPos(hMainWnd, HWND_TOP, sx, sy, cx, cy, 0);
		
		// �w���v�\�����\��Off
		g_nExcMode = MODE_CMD;
	}
}
