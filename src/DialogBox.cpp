/*
 * DialogBox�Ɋւ��鏈��
 */
 
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <time.h>
#include <stdio.h>
#include "resource.h"
#include "QuickEdit.h"
#include "DialogBox.h"
#include "Variable.h"
#include "OpeString.h"
#include "CalTime.h"

// �Œ�l 
#define MAX_BUF		1024

// �}�N���ݒ�̏��
enum MCRSTAT {
	TOP = 0, NEW, EDIT, 
};

static MCRSTAT l_eMcrStat;

// MsgBox�p�����[�^
static char *l_pMsgStr = NULL;
static char *l_pMsgTitle = NULL;
static int l_nMsgType;

// CallBack�֐�
static WNDPROC lpfnDateProc;
static WNDPROC lpfnStimeProc;
static WNDPROC lpfnEtimeProc;

//----------------------------------------------------------------------
//��Dialog�̃��b�Z�[�W����
//----------------------------------------------------------------------
// �_�C�A���O1(�o�[�W�����\��)�v���V�[�W��
BOOL CALLBACK EventCall_MsgWnd(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	int stat;

	switch (msg) {
		case WM_INITDIALOG:
			return MsgWnd_Init(hWnd);
		case WM_COMMAND:
			stat = MsgWnd_Command(hWnd, HIWORD(wparam),
			                      LOWORD(wparam), (HWND)lparam);
			SendMessage(hMainWnd, WM_ACTIVATE, 0, 0);
			return stat;
	}
	return FALSE;
}

BOOL MsgWnd_Init(HWND hWnd)
{
	int x, y;
	char szBuf[MAX_BUF+1];

	// �_�C�A���O�̕\���ʒu�擾
	GetDispLoc(hWnd, &x, &y);
	// �_�C�A���O��K�؂Ȉʒu�Ɉړ�
	SetWindowPos(hWnd, NULL, x, y, 0, 0, SWP_NOSIZE);

	// �o�[�W�������̕\��
	GetVerInfo("Version %s (%s)", szBuf);
	SetDlgItemText(hWnd, IDC_VERSION, szBuf);

	return FALSE;
}

BOOL MsgWnd_Command(HWND hWnd, WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	switch (wID) {
		case IDOK:
			EndDialog(hWnd, IDOK);
			break;
		default:
			return FALSE;
	}
	return TRUE;
}

// �_�C�A���O2(����/�u����������)�v���V�[�W��
BOOL CALLBACK EventCall_InputWnd(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	int stat;

	switch (msg) {
		case WM_INITDIALOG:
			return InputWnd_Init(hWnd);
		case WM_COMMAND:
			stat = InputWnd_Command(hWnd, HIWORD(wparam),
			                        LOWORD(wparam), (HWND)lparam);
			SendMessage(hMainWnd, WM_ACTIVATE, 0, 0);
			return stat;
	}
	return FALSE;
}

BOOL InputWnd_Init(HWND hWnd)
{	
	HWND hCombo;
	int x, y, i;

	// �_�C�A���O�̕\���ʒu�擾
	GetDispLoc(hWnd, &x, &y);
	// �_�C�A���O��K�؂Ȉʒu�Ɉړ�
	SetWindowPos(hWnd, NULL, x, y, 0, 0, SWP_NOSIZE);

	SetDlgItemText(hWnd, IDC_NOTE,
	               "�u/����1/�u��1/[����2/�u��2/...]�v�`���œ���");
	hCombo = GetDlgItem(hWnd, IDC_INPUT);
	for (i = 0; i < g_ExchHist.GetMaxCount(); ++i) {
		SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)g_ExchHist.GetString(i));
	}
	SendMessage(hCombo, CB_SETCURSEL, 0, 0);

	return TRUE;
}

BOOL InputWnd_Command(HWND hWnd, WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	int nIdx, nMax, i;
	char szBuf[MAX_BUF+1];

	switch (wID) {
		case IDOK:
			// ���͕�����̕ۑ�
			GetDlgItemText(hWnd, IDC_INPUT, szBuf, MAX_BUF);
			if (*szBuf) {
				SortHist(szBuf, &g_ExchHist, MAX_EXCHHIST);
			}

			EndDialog(hWnd, IDOK);
			break;
		case IDCANCEL:
			EndDialog(hWnd, IDCANCEL);
			break;
		default:
			return FALSE;
	}
	return TRUE;
}

// �_�C�A���O3(�z�b�g�L�[�ݒ�)�v���V�[�W��
BOOL CALLBACK EventCall_HotkeyWnd(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		case WM_INITDIALOG:
			return HotkeyWnd_Init(hDlg);

		case WM_COMMAND:
			return HotkeyWnd_Command(hDlg, HIWORD(wParam),
			                         LOWORD(wParam), (HWND)lParam);
	}
	return FALSE;
}

BOOL HotkeyWnd_Init(HWND hDlg)
{	
	int x, y;
	RECT BasRc, OwnRc;
	HWND hHot, hParent;
	WORD wHotKey;

	// �_�C�A���O�̕\���ʒu�擾
	GetDispLoc(hDlg, &x, &y);
	// �_�C�A���O��K�؂Ȉʒu�Ɉړ�
	SetWindowPos(hDlg, NULL, x, y, 0, 0, SWP_NOSIZE);

	// ���ݐݒ肳��Ă���z�b�g�L�[��\��
	hHot = GetDlgItem(hDlg, IDC_HOTKEY);
	hParent = GetParent(hDlg);
	SendMessage(hHot, HKM_SETRULES,
				(WPARAM)HKCOMB_NONE, MAKELPARAM(HOTKEYF_ALT, 0));
	wHotKey = (WORD)SendMessage(hParent, WM_GETHOTKEY, 0, 0);
	SendMessage(hHot, HKM_SETHOTKEY, (WPARAM)wHotKey, 0);

	return TRUE;
}

BOOL HotkeyWnd_Command(HWND hDlg, WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	HWND hHot, hParent;
	WORD wHotKey;

	hHot = GetDlgItem(hDlg, IDC_HOTKEY);
	hParent = GetParent(hDlg);
	switch (wID) {
		case IDOK:
			wHotKey = (WORD)SendMessage(hHot, HKM_GETHOTKEY, 0, 0);
			if (SendMessage(hParent, WM_SETHOTKEY, (WPARAM)wHotKey, 0) != 1) {
				MsgBox(hDlg, "�z�b�g�L�[�̐ݒ�Ɏ��s���܂����B", "ERROR", MB_OK);
				return TRUE;
			}
			g_wHotKey = wHotKey;
			EndDialog(hDlg, IDOK);
			return TRUE;

		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
	}
	return FALSE;
}

// �_�C�A���O4(�����p�����[�^����)�v���V�[�W��
BOOL CALLBACK EventCall_FmtWnd(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	int stat;

	switch (msg) {
		case WM_INITDIALOG:
			return FmtWnd_Init(hWnd);
		case WM_COMMAND:
			stat = FmtWnd_Command(hWnd, HIWORD(wparam),
			                      LOWORD(wparam), (HWND)lparam);
			SendMessage(hMainWnd, WM_ACTIVATE, 0, 0);
			return stat;
	}
	return FALSE;
}

BOOL FmtWnd_Init(HWND hWnd)
{
	HWND hList, hCombo;
	int x, y, i, id;

	// �_�C�A���O�̕\���ʒu�擾
	GetDispLoc(hWnd, &x, &y);
	// �_�C�A���O��K�؂Ȉʒu�Ɉړ�
	SetWindowPos(hWnd, NULL, x, y, 0, 0, SWP_NOSIZE);

	// ��ʃ��X�g�̐ݒ�
	hList = GetDlgItem(hWnd, IDC_LIST);
	ComboBox_AddString(hList, "HTML�R�[�h�̐��`");
	ComboBox_AddString(hList, "�J�����f�[�^ �� �����ϊ�");
	ComboBox_AddString(hList, "�J�����f�[�^ �� �\�ϊ�");
	ComboBox_AddString(hList, "��������܂ލs�𒊏o(grep)");
	ComboBox_AddString(hList, "��������܂܂Ȃ��s�𒊏o(grep -v)");
	ComboBox_SetCurSel(hList, g_FmtInfo.nFmtId);

	// ���������̐ݒ�
	hCombo = GetDlgItem(hWnd, IDC_INPUT);
	for (i = 0; i < g_FmtInfo.FmtHist.GetMaxCount(); ++i) {
		ComboBox_AddString(hCombo, g_FmtInfo.FmtHist.GetString(i));
	}
	ComboBox_SetCurSel(hCombo, 0);

	// ���W�I�{�^���̃`�F�b�N�ݒ�
	if (g_FmtInfo.nDlmtId == DLMTID_TAB) {
		id = IDC_RADIO1;
	} else {
		id = IDC_RADIO2;
	}
	Button_SetCheck(GetDlgItem(hWnd, id), BST_CHECKED);

	// ��ʂɉ��������ڂ̗L��/�����ݒ�
	FmtWnd_SelChange(hWnd);

	return FALSE;
}

BOOL FmtWnd_Command(HWND hWnd, WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	HWND hList, hCombo;
	int nIdx, iCheck;
	char szBuf[MAX_BUF+1];

	if (wID == IDC_LIST && wNotifyCode == CBN_SELCHANGE) {
		FmtWnd_SelChange(hWnd);
	}

	switch (wID) {
		case IDOK:
			// ��ʂ̕ۑ�
			hList = GetDlgItem(hWnd, IDC_LIST);
			g_FmtInfo.nFmtId = SendMessage(hList, CB_GETCURSEL, 0, 0);

			// �����̕ۑ�
			GetDlgItemText(hWnd, IDC_INPUT, szBuf, MAX_BUF);
			/*
			if (*szBuf) {
				SortHist(szBuf, &(g_FmtInfo.FmtHist), MAX_FMTHIST);
			}
			*/
			SortHist(szBuf, &(g_FmtInfo.FmtHist), MAX_FMTHIST); // �󔒂��o�^

			// ��؂蕶���̕ۑ�
			iCheck = Button_GetCheck(GetDlgItem(hWnd, IDC_RADIO1));
			if (iCheck == BST_CHECKED) {
				g_FmtInfo.nDlmtId = DLMTID_TAB;
			} else {
				g_FmtInfo.nDlmtId = DLMTID_CSV;
			}

			EndDialog(hWnd, IDOK);
			break;
		case IDCANCEL:
			EndDialog(hWnd, IDCANCEL);
			break;
		default:
			return FALSE;
	}
	return TRUE;
}

void FmtWnd_SelChange(HWND hWnd)
{
	HWND hList, hInp, hLabel, hRad1, hRad2, hNote;
	int nSel;

	hList  = GetDlgItem(hWnd, IDC_LIST);
	hLabel = GetDlgItem(hWnd, IDC_LABEL);
	hInp   = GetDlgItem(hWnd, IDC_INPUT);
	hNote  = GetDlgItem(hWnd, IDC_NOTE);
	hRad1  = GetDlgItem(hWnd, IDC_RADIO1);
	hRad2  = GetDlgItem(hWnd, IDC_RADIO2);

	nSel = SendMessage(hList, CB_GETCURSEL, 0, 0);
	switch (nSel) {
		case FMTID_ADJHTML:
			EnableWindow(hInp, FALSE);
			EnableWindow(hNote, FALSE);
			EnableWindow(hRad1, FALSE);
			EnableWindow(hRad2, FALSE);
			break;
		case FMTID_COLFMT:
			SetWindowText(hLabel, "�����F");
			EnableWindow(hInp, TRUE);
			EnableWindow(hNote, TRUE);
			EnableWindow(hRad1, TRUE);
			EnableWindow(hRad2, TRUE);

			UpdateWindow(hInp);
			break;
		case FMTID_MAKTBL:
			EnableWindow(hInp, FALSE);
			EnableWindow(hNote, FALSE);
			EnableWindow(hRad1, TRUE);
			EnableWindow(hRad2, TRUE);
			break;
		case FMTID_GETINC:
		case FMTID_GETEXC:
			SetWindowText(hLabel, "�����F");
			EnableWindow(hInp, TRUE);
			EnableWindow(hNote, FALSE);
			EnableWindow(hRad1, FALSE);
			EnableWindow(hRad2, FALSE);

			UpdateWindow(hInp);
			break;
	}
}

// �_�C�A���O5(�}�N���ݒ�)�v���V�[�W��
BOOL CALLBACK EventCall_McrWnd(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	int stat;

	switch (msg) {
		case WM_INITDIALOG:
			return McrWnd_Init(hWnd);
		case WM_COMMAND:
			stat = McrWnd_Command(hWnd, HIWORD(wparam),
			                      LOWORD(wparam), (HWND)lparam);
			SendMessage(hMainWnd, WM_ACTIVATE, 0, 0);
			return stat;
	}
	return FALSE;
}

BOOL McrWnd_Init(HWND hWnd)
{
	HWND hList, hCombo;
	int x, y, i, id;

	// �_�C�A���O�̕\���ʒu�擾
	GetDispLoc(hWnd, &x, &y);
	// �_�C�A���O��K�؂Ȉʒu�Ɉړ�
	SetWindowPos(hWnd, NULL, x, y, 0, 0, SWP_NOSIZE);

	// �}�N�����ݒ�
	SetMcrData(hWnd, 0);

	// �L���{�^���ݒ�
	McrWnd_SetCtrl(hWnd, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE);
	l_eMcrStat = TOP;

	return FALSE;
}

BOOL McrWnd_Command(HWND hWnd, WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	HWND hList, hCombo, hEdit;
	int nIdx, nStat;
	char *pName, *pMacro, szBuf[MAX_BUF];

	hCombo = GetDlgItem(hWnd, IDC_LIST);
	nIdx = SendMessage(hCombo, CB_GETCURSEL, 0, 0);

	switch (wID) {
		case IDEXC:
			if (g_McrInfo.Name.GetMaxCount() > 0) {
				// �I���������̂Ɠ��e�����X�g�擪�ֈړ�
				g_McrInfo.Name.MoveString(nIdx, 0);
				g_McrInfo.Macro.MoveString(nIdx, 0);
			}
			EndDialog(hWnd, IDOK);
			break;
		case IDCANCEL:
			if (l_eMcrStat == TOP) {
				EndDialog(hWnd, IDCANCEL);
			} else {
				l_eMcrStat = TOP;
				McrWnd_SetCtrl(hWnd, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE);
				// �}�N�����ݒ�
				SetMcrData(hWnd, nIdx);
			}
			break;
		case IDREG:
			// �}�N�����̓o�^
			pName  = GetDlgText(hWnd, IDC_NOTE);
			pMacro = GetDlgText(hWnd, IDC_INPUT);
			if (l_eMcrStat == NEW) nIdx = -1;
			nStat = AddMcrInfo(pName, pMacro, nIdx);
			if (nStat) {
				if (l_eMcrStat == NEW) nIdx = 0;
				l_eMcrStat = TOP;
				McrWnd_SetCtrl(hWnd, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE);
				// �}�N�����ݒ�
				SetMcrData(hWnd, nIdx);
			}
			free(pName);
			free(pMacro);

			break;
		case IDNEW:
			l_eMcrStat = NEW;
			McrWnd_SetCtrl(hWnd, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE);
			SetDlgItemText(hWnd, IDC_NOTE, "");
			SetDlgItemText(hWnd, IDC_INPUT, "");
			break;
		case IDIMPORT:
			// �N���b�v�{�[�h����}�N���̖��̂Ɠ��e���擾
			nStat = GetMcrFromCb(&pName, &pMacro, TRUE);
			while (nStat) {
				// �}�N�����̓o�^
				nStat = AddMcrInfo(pName, pMacro, -1);
				free(pName);
				free(pMacro);

				nStat = GetMcrFromCb(&pName, &pMacro, FALSE);
			}

			McrWnd_SetCtrl(hWnd, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE);
			// �}�N�����ݒ�
			SetMcrData(hWnd, 0);
			break;
		case IDEDIT:
			l_eMcrStat = EDIT;
			McrWnd_SetCtrl(hWnd, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE);

			if (nIdx != CB_ERR) {
				SetDlgItemText(hWnd, IDC_NOTE, g_McrInfo.Name.GetString(nIdx));
			}
			break;
		case IDDEL:
			sprintf(szBuf, "�}�N���u%s�v���폜���܂����H",
			        g_McrInfo.Name.GetString(nIdx));
			nStat = MsgBox(hWnd, szBuf, "�m�F", MB_YESNO | MB_DEFBUTTON2);
			if (nStat == IDYES) {
				g_McrInfo.Name.DelString(nIdx);
				g_McrInfo.Macro.DelString(nIdx);
				// �}�N�����ݒ�
				SetMcrData(hWnd, 0);
			} else {
				// �}�N�����ݒ�
				SetMcrData(hWnd, nIdx);
			}
			McrWnd_SetCtrl(hWnd, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE);
			break;
		case IDC_LIST:
			if (wNotifyCode == CBN_SELCHANGE) {
				if (nIdx != CB_ERR) {
					SetDlgItemText(hWnd, IDC_INPUT,
					               g_McrInfo.Macro.GetString(nIdx));
			   }
		   }
		default:
			return FALSE;
	}
	return TRUE;
}

// �_�C�A���O�̐���ݒ�
void McrWnd_SetCtrl(HWND hWnd, int bNew, int bImp, int bEdt, int bDel, int bReg, int bExc)
{
	HWND hDlg;
	int nCnt;

	// �}�N���f�[�^��������΁A�ҏW/�폜�{�^������
	nCnt = g_McrInfo.Name.GetMaxCount();
	if (nCnt == 0) {
		bEdt = FALSE;
		bDel = FALSE;
	}

	// �e�{�^���̗L��/�����ݒ�
	SetButton(hWnd, IDNEW   , bNew, FALSE);
	SetButton(hWnd, IDIMPORT, bImp, FALSE);
	SetButton(hWnd, IDEDIT  , bEdt, FALSE);
	SetButton(hWnd, IDDEL   , bDel, FALSE);
	SetButton(hWnd, IDREG   , bReg, FALSE);
	SetButton(hWnd, IDEXC   , bExc, bExc);

	switch (l_eMcrStat) {
		case TOP:
			SetComboBox(hWnd, IDC_LIST, TRUE);
			SetComboBox(hWnd, IDC_NOTE, FALSE);
			SetEditBox(hWnd, IDC_INPUT, FALSE);
			hDlg = GetDlgItem(hWnd, IDC_LIST);
			SetFocus(hDlg);
			break;
		case NEW:
			SetComboBox(hWnd, IDC_LIST, FALSE);
			SetComboBox(hWnd, IDC_NOTE, TRUE);
			SetEditBox(hWnd, IDC_INPUT, TRUE);
			hDlg = GetDlgItem(hWnd, IDC_NOTE);
			SetFocus(hDlg);
			break;
		case EDIT:
			SetComboBox(hWnd, IDC_LIST, FALSE);
			SetComboBox(hWnd, IDC_NOTE, TRUE);
			SetEditBox(hWnd, IDC_INPUT, TRUE);
			hDlg = GetDlgItem(hWnd, IDC_INPUT);
			SetFocus(hDlg);
			break;
	}
}

// �{�^����Style�ݒ�
void SetButton(HWND hWnd, int Id, bool bEnable, bool bDefBtn)
{
	HWND hBtn;
	DWORD nStyle;

	hBtn = GetDlgItem(hWnd, Id);

	nStyle = GetWindowLong(hBtn, GWL_STYLE);
	if (bEnable) {
		nStyle &= ~WS_DISABLED;
	} else {
		nStyle |= WS_DISABLED;
	}
	if (bDefBtn) {
		nStyle |= BS_DEFPUSHBUTTON;
	} else {
		nStyle &= ~BS_DEFPUSHBUTTON;
	}
	SetWindowLong(hBtn, GWL_STYLE, nStyle);
	InvalidateRect(hBtn, NULL, TRUE);
}

// EditBox��Style�ݒ�
void SetEditBox(HWND hWnd, int Id, bool bEdit)
{
	HWND hEdt;
	DWORD nStyle;

	hEdt = GetDlgItem(hWnd, Id);
	if (bEdit) {
		SendMessage(hEdt, EM_SETREADONLY, FALSE, 0);
	} else {
		SendMessage(hEdt, EM_SETREADONLY, TRUE, 0);
	}
}

// ComboBox��Style�ݒ�
void SetComboBox(HWND hWnd, int Id, bool bEdit)
{
	HWND hCombo;
	DWORD nStyle;

	hCombo = GetDlgItem(hWnd, Id);
	nStyle = GetWindowLong(hCombo, GWL_STYLE);
	if (bEdit) {
		nStyle |= WS_VISIBLE;
	} else {
		nStyle &= ~WS_VISIBLE;
	}
	SetWindowLong(hCombo, GWL_STYLE, nStyle);
	EnableWindow(hCombo, bEdit);
}

// �}�N�������_�C�A���O�ɐݒ�
void SetMcrData(HWND hWnd, int nIdx)
{
	HWND hCombo, hEdit;
	int i, nCnt;

	// ����/���e�̃N���A
	hCombo = GetDlgItem(hWnd, IDC_LIST);
	hEdit = GetDlgItem(hWnd, IDC_INPUT);
	SendMessage(hCombo, CB_RESETCONTENT, 0, 0);
	SetWindowText(hEdit, "");

	// ���̂̐ݒ�
	nCnt = g_McrInfo.Name.GetMaxCount();
	for (i = 0; i < nCnt; ++i) {
		SendMessage(hCombo, CB_ADDSTRING, 0,
		            (LPARAM)g_McrInfo.Name.GetString(i));
	}

	if (nIdx >= 0 && nCnt > 0) {
		SendMessage(hCombo, CB_SETCURSEL, nIdx, 0);
		// ���e�̐ݒ�
		SetWindowText(hEdit, g_McrInfo.Macro.GetString(nIdx));
	}
}

// �}�N������ǉ�
int AddMcrInfo(char *pName, char *pMacro, int nCurIdx)
{
	HWND hCombo;
	int nIdx, nStat, bChng = FALSE;
	char szMsg[MAX_BUF];

	if (nCurIdx < 0) { // �V�K
		// ���̂�����
		nIdx = g_McrInfo.Name.SrchString(pName);
		if (nIdx == -1) {
			// ���̂Ɠ��e�̒ǉ�
			g_McrInfo.Name.InsString(0, pName);
			g_McrInfo.Macro.InsString(0, pMacro);
			bChng = TRUE;
		} else {
			sprintf(szMsg, "�}�N���u%s�v�͓o�^�ς݂ł��B�㏑�����܂����H", pName);
			nStat = MsgBox(hMainWnd, szMsg, "�m�F", MB_YESNO | MB_DEFBUTTON2);
			if (nStat == IDYES) {
				// ���̂Ɠ��e�̏�����
				g_McrInfo.Name.DelString(nIdx);
				g_McrInfo.Name.InsString(nIdx, pName);
				g_McrInfo.Macro.DelString(nIdx);
				g_McrInfo.Macro.InsString(nIdx, pMacro);
				bChng = TRUE;
			}
		}
	} else { // �ҏW
		// ���̂Ɠ��e�̏�����
		g_McrInfo.Name.DelString(nCurIdx);
		g_McrInfo.Name.InsString(nCurIdx, pName);
		g_McrInfo.Macro.DelString(nCurIdx);
		g_McrInfo.Macro.InsString(nCurIdx, pMacro);
		bChng = TRUE;
	}
	
	return bChng;
}

// �N���b�v�{�[�h����}�N���̖��̂Ɠ��e���擾
int GetMcrFromCb(char **pName, char **pMacro, int bNew)
{
#define DLMT "\r\n"
	HWND hDlg;
	int nLen, nCnt, i, bOutput = FALSE;
#define STAT_SRCH_NAME	0
#define STAT_GET_MACRO	1
#define STAT_SAVE_REST	2
	int nStat;
	char *pCbStr, *pLine;
	STRLIST LineStr, MacroStr, RestStr;
	static char *pRestStr = NULL;

	if (bNew) {
		// �c�蕶����N���A
		if (pRestStr) {
			free(pRestStr);
			pRestStr = NULL;
		}
		// �N���b�v�{�[�h�̕�����擾
		hDlg = GetDlgItem(hMainWnd, EditBox);
		nLen = GetWindowTextLength(hDlg);
		pCbStr = (char *)malloc(nLen + 1);
		GetWindowText(hDlg, pCbStr, nLen + 1);
	} else {
		// �c�蕶�����ݒ�
		pCbStr = pRestStr;
	}

	// �f���~�^�ŕ���
	StrSplit(pCbStr, DLMT, SPLIT_NONE | SPLIT_MULTI, &LineStr);
	nCnt = LineStr.GetMaxCount();

	nStat = STAT_SRCH_NAME;
	for (i = 0; i < nCnt; ++i) {
		pLine = LineStr.GetString(i);
		if (nStat == STAT_SRCH_NAME) {
			if (*pLine == '*') {
				// ���̂̐ݒ�
				*pName = strdup(pLine + 1);
				nStat = STAT_GET_MACRO;
				bOutput = TRUE;
			}
		} else if (nStat == STAT_GET_MACRO) {
			// ���e�̐ݒ�
			if (*pLine == '*') {
				RestStr.AddString(pLine);
				nStat = STAT_SAVE_REST;
			} else if (*pLine != '\0') {
				MacroStr.AddString(pLine);
			}
		} else {
			// �c�蕶����̕ۑ�
			RestStr.AddString(pLine);
		}
	}
	*pMacro = MacroStr.MergeAllStr(DLMT);
	free(pCbStr);
	pRestStr = RestStr.MergeAllStr(DLMT);

	return bOutput;
}

// �_�C�A���O6(�����ݒ�)�v���V�[�W��
BOOL CALLBACK EventCall_DateWnd(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int nStat;
	char szBuf[MAX_BUF];

	switch (msg) {
		case WM_INITDIALOG:
			return DateWnd_Init(hWnd, lParam);
		case WM_COMMAND:
			nStat = DateWnd_Command(hWnd, HIWORD(wParam),
			                       LOWORD(wParam), (HWND)lParam);
			SendMessage(hMainWnd, WM_ACTIVATE, 0, 0);
			return nStat;
	}
	return FALSE;
}

BOOL DateWnd_Init(HWND hWnd, LPARAM lParam)
{
	int x, y, nHalf;
	char szDate[MAX_BUF+1], szTime[MAX_BUF+1];
	Variable Edit;

	// �_�C�A���O�̕\���ʒu�擾
	GetDispLoc(hWnd, &x, &y);
	// �_�C�A���O��K�؂Ȉʒu�Ɉړ�
	SetWindowPos(hWnd, NULL, x, y, 0, 0, SWP_NOSIZE);

	g_DateInfo.hDate = GetDlgItem(hWnd, IDC_DATE);
	g_DateInfo.hStime = GetDlgItem(hWnd, IDC_STIME);
	g_DateInfo.hEtime = GetDlgItem(hWnd, IDC_ETIME);

	lpfnDateProc = (WNDPROC)SetWindowLong(g_DateInfo.hDate, GWL_WNDPROC,
	                                      (int)EventCall_DateDlg);
	lpfnStimeProc = (WNDPROC)SetWindowLong(g_DateInfo.hStime, GWL_WNDPROC,
	                                       (int)EventCall_StimeDlg);
	lpfnEtimeProc = (WNDPROC)SetWindowLong(g_DateInfo.hEtime, GWL_WNDPROC,
	                                       (int)EventCall_EtimeDlg);

	// �ݒ�`���̑I�����X�g�ݒ�
	g_DateInfo.hCombo = GetDlgItem(hWnd, IDC_FORMAT);
	ComboBox_AddString(g_DateInfo.hCombo, "�N����+�J�n�����`�I������");
	ComboBox_AddString(g_DateInfo.hCombo, "�N����+����");
	ComboBox_AddString(g_DateInfo.hCombo, "�N����");
	ComboBox_SetCurSel(g_DateInfo.hCombo, g_DateInfo.nIdx);

	// ���ݔN�����̐ݒ�
	GetTime(szDate);
	Edit.split(szDate, " ");
	SetWindowText(g_DateInfo.hDate, Edit.get(0));
	Edit_SetSel(g_DateInfo.hDate, 0, 10);

	// ���ݎ����̐ݒ�
	nHalf = DateWnd_GetHalfHour(Edit.get(1));	// �J�n����
	if (g_DateInfo.nIdx != DTFMT_DATETIME) {
		nHalf = DateWnd_AddHalfHour(nHalf, 0);		// 30���l�ŕ␳
		DateWnd_Half2Str(nHalf, szTime);			// hh:mm�֕ϊ�
		SetWindowText(g_DateInfo.hStime, szTime);	// �J�n�����֐ݒ�
	} else {
		// �J�n�����֌��ݎ��������̂܂ܐݒ�
		SetWindowText(g_DateInfo.hStime, Edit.get(1));
	}
	Edit_SetSel(g_DateInfo.hStime, 0, 5);

	nHalf = DateWnd_AddHalfHour(nHalf, 2);		// ���ݎ���+1����
	DateWnd_Half2Str(nHalf, szTime);			// hh:mm�֕ϊ�
	SetWindowText(g_DateInfo.hEtime, szTime);	// �I�������֐ݒ�
	Edit_SetSel(g_DateInfo.hEtime, 0, 5);

	// �N�����ɃJ�[�\�������킹��
	SetFocus(g_DateInfo.hDate);

	// �_�C�A���O�̐���ݒ�
	DateWnd_SetCtrl(hWnd);

	// �}�N�����s
	if (g_DateInfo.bMacro) {
		SendMessage(hWnd, WM_COMMAND, MAKEWORD(IDOK, 0), 0);
	}
	return FALSE;
}

BOOL DateWnd_Command(HWND hWnd, WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	int nIdx;
	char *pDate, *pStime, *pEtime, szBuf[MAX_BUF];

	nIdx = ComboBox_GetCurSel(g_DateInfo.hCombo);
	switch (wID) {
		case IDOK:
			pDate = GetDlgText(hWnd, IDC_DATE);
			pStime = GetDlgText(hWnd, IDC_STIME);
			pEtime = GetDlgText(hWnd, IDC_ETIME);

			if (nIdx == 0) {
				sprintf(szBuf, "%s %s�`%s", pDate, pStime, pEtime);
			} else if (nIdx == 1) {
				sprintf(szBuf, "%s %s", pDate, pStime);
			} else if (nIdx == 2) {
				sprintf(szBuf, "%s", pDate);
			}
			g_DateInfo.pDateTime = strdup(szBuf);
			g_DateInfo.nIdx = nIdx;

			EndDialog(hWnd, IDOK);
			break;
		case IDCANCEL:
			EndDialog(hWnd, IDCANCEL);
			break;
		case IDC_FORMAT:
			if (wNotifyCode == CBN_SELCHANGE) {
				if (nIdx != CB_ERR) {
					DateWnd_SetCtrl(hWnd);
			   }
		   }

			break;
		default:
			return FALSE;
	}
	return TRUE;
}

// �_�C�A���O�̐���ݒ�
void DateWnd_SetCtrl(HWND hWnd)
{
	int nIdx;

	nIdx = ComboBox_GetCurSel(g_DateInfo.hCombo);
	g_DateInfo.nIdx = nIdx;
	switch (nIdx) {
		case 0:
			EnableWindow(g_DateInfo.hStime, TRUE);
			EnableWindow(g_DateInfo.hEtime, TRUE);
			break;
		case 1:
			EnableWindow(g_DateInfo.hStime, TRUE);
			EnableWindow(g_DateInfo.hEtime, FALSE);
			break;
		case 2:
			EnableWindow(g_DateInfo.hStime, FALSE);
			EnableWindow(g_DateInfo.hEtime, FALSE);
			break;
	}
}

// ����EditBox�Ԃ̃J�[�\���ړ�
// Args:
// 1	HWND 	���݈ʒu�̃n���h��
// 2	int 	1:�E�����ֈړ��A-1:�������ֈړ�
// Return:		�Ȃ�
void DateWnd_MoveCursor(HWND hWnd, int nWay)
{
	HWND hNext;
	int nIdx, nMax, nPos, nNext;

	// �J�[�\���ړ��\�ȍ��ڐ��Z�o
	nIdx = ComboBox_GetCurSel(g_DateInfo.hCombo);
	nMax = 3 - nIdx;

	// �J�[�\���ʒu�擾
	nPos = DateWnd_GetCsrPos(hWnd);

	// ���̃J�[�\���ʒu�ݒ�
	nNext = (nPos + nWay + nMax) % nMax;
	switch (nNext) {
		case 0:
			hNext = g_DateInfo.hDate; break;
		case 1:
			hNext = g_DateInfo.hStime; break;
		case 2:
			hNext = g_DateInfo.hEtime; break;
	}
	SetFocus(hNext);
}

// EditBox�̃J�[�\���ʒu�擾
// Args:
// 1	HWND 	���݈ʒu�̃n���h��
// Return:		0:�N�����A1:�J�n�����A2:�I������
int DateWnd_GetCsrPos(HWND hWnd)
{
	int nPos;

	// �J�[�\���ʒu�擾
	if (hWnd == g_DateInfo.hDate) {
		nPos = 0;
	} else if (hWnd == g_DateInfo.hStime) {
		nPos = 1;
	} else if (hWnd == g_DateInfo.hEtime) {
		nPos = 2;
	}

	return nPos;
}
// �N������N���X�V
// Args:
// 1	int 	�X�V���鍷������
// Return:		�Ȃ�
void DateWnd_UpdDate(int nDiff)
{
	double dDate;
	char szDate[MAX_BUF+1];
	Variable Edit;

	GetWindowText(g_DateInfo.hDate, szDate, MAX_BUF);
	dDate = Date2Bin(szDate);
	dDate += nDiff;
	Bin2Date(dDate, szDate);
	Edit.split(szDate, " ");
	SetWindowText(g_DateInfo.hDate, Edit.get(0));
	Edit_SetSel(g_DateInfo.hDate, 0, 10);
}

// ������N���X�V
// Args:
// 1	HWND 	���݈ʒu�̃n���h��
// 2	int 	�X�V����30���l(1:30���A2:1����)
// Return:		�Ȃ�
void DateWnd_UpdTime(HWND hWnd, int nDiff)
{
	int nPos, nStime, nEtime, nHour, nMin;
	char szStime[MAX_BUF+1], szEtime[MAX_BUF+1];

	// �_�C�A���O�̎������擾
	GetWindowText(g_DateInfo.hStime, szStime, MAX_BUF);
	GetWindowText(g_DateInfo.hEtime, szEtime, MAX_BUF);

	// ������30���l�֕ϊ�
	nStime = DateWnd_GetHalfHour(szStime);
	nEtime = DateWnd_GetHalfHour(szEtime);

	// 30���l�̍����X�V
	nPos = DateWnd_GetCsrPos(hWnd);
	if (nPos == 1) { // �J�n����
		// �J�n�����̍X�V
		nStime = DateWnd_AddHalfHour(nStime, nDiff);
		DateWnd_Half2Str(nStime, szStime);
		SetWindowText(g_DateInfo.hStime, szStime);

		// �I���������A�����čX�V
		nEtime = DateWnd_AddHalfHour(nEtime, nDiff);
		DateWnd_Half2Str(nEtime, szEtime);
		SetWindowText(g_DateInfo.hEtime, szEtime);
	} else if (nPos == 2) { // �I������
		// �I�������̍X�V
		nEtime = DateWnd_AddHalfHour(nEtime, nDiff);
		DateWnd_Half2Str(nEtime, szEtime);
		SetWindowText(g_DateInfo.hEtime, szEtime);

		// �J�n�����ƏI���������t�]����ꍇ�A�J�n�������X�V
		if (nStime >= nEtime) {
			nStime = DateWnd_AddHalfHour(nStime, nDiff);
			DateWnd_Half2Str(nStime, szStime);
			SetWindowText(g_DateInfo.hStime, szStime);
		}
	}
	Edit_SetSel(g_DateInfo.hStime, 0, 5);
	Edit_SetSel(g_DateInfo.hEtime, 0, 5);
}

// ������30���l�ϊ�
// Args:
// 1	char * 	�����̕�����(hh:�o)
// Return:		30���l(1:30��, 2:1����, ...)
int DateWnd_GetHalfHour(char *pTime)
{
	int nHour, nMin, nHalf;
	Variable Edit;

	// �����𐔒l�ϊ�
	Edit.split(pTime, ":");
	nHour = atoi(Edit.get(0));
	nMin  = atoi(Edit.get(1));

	// 30���l�֕ϊ�
	nMin  = int((nMin +15) / 30);
	nHalf = 2 * nHour + nMin;

	return nHalf;
}

// 30���l�̉����Z
// Args:
// 1	int   	����30���l
// 2	int   	�����Z����30���l
// Return:		�����Z����30���l(1:30��, 2:1����, ...)
int DateWnd_AddHalfHour(int nOrg, int nAdd)
{
	int nHalf;

	nHalf = nOrg + nAdd;
	if (nHalf < 0) nHalf = 0;
	if (nHalf > 47) nHalf = 47;

	return nHalf;
}

// 30���l����������(hh:�o)�ϊ�
// Args:
// 1	char * 	�����̕�����(hh:�o)
// Return:		30���l(1:30��, 2:1����, ...)
void DateWnd_Half2Str(int nHalf, char *pTime)
{
	int nHour, nMin;

	nHour = int(nHalf / 2);
	nMin  = (nHalf % 2 == 0) ? 0 : 30;
	sprintf(pTime, "%d:%02d", nHour, nMin);
}

LRESULT CALLBACK EventCall_DateDlg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nStat, nCtrl;

	switch(uMsg) {
		case WM_CHAR:
			switch((char)wParam) {
				case 'j':
					DateWnd_UpdDate(1); break;
				case 'k':
					DateWnd_UpdDate(-1); break;
				case 'h':
					DateWnd_MoveCursor(hWnd, -1); break;
				case 'l':
					DateWnd_MoveCursor(hWnd, 1); break;
				case 'J':
					DateWnd_UpdDate(7); break;
				case 'K':
					DateWnd_UpdDate(-7); break;
			}
			return 0;

		case WM_KEYDOWN:
			// Ctrl�L�[��Ԏ擾
			nCtrl = GetKeyState(VK_LCONTROL);
			nCtrl |= GetKeyState(VK_RCONTROL);

			switch(wParam) {
				case VK_DOWN:	// ���L�[
					if(nCtrl & 0x80) {	// Ctrl�L�[����
						DateWnd_UpdDate(7); break;
					} else {
						DateWnd_UpdDate(1); break;
					}
				case VK_UP:		// ���L�[
					if(nCtrl & 0x80) {	// Ctrl�L�[����
						DateWnd_UpdDate(-7); break;
					} else {
						DateWnd_UpdDate(-1); break;
					}
				case VK_LEFT:	// ���L�[
					DateWnd_MoveCursor(hWnd, -1); break;
				case VK_RIGHT:	// ���L�[
					DateWnd_MoveCursor(hWnd, 1); break;
			}
			return 0;

		case WM_LBUTTONDBLCLK:
			if (wParam & MK_SHIFT) {
				if (wParam & MK_CONTROL) {
					DateWnd_UpdDate(-7);
				} else {
					DateWnd_UpdDate(-1);
				}
			} else {
				if (wParam & MK_CONTROL) {
					DateWnd_UpdDate(7);
				} else {
					DateWnd_UpdDate(1);
				}
			}
			return 0;
	}

	return CallWindowProc(lpfnDateProc, hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK EventCall_StimeDlg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nStat, nCtrl;

	switch(uMsg) {
		case WM_CHAR:
			switch((char)wParam) {
				case 'j':
					DateWnd_UpdTime(hWnd, 1); break;
				case 'k':
					DateWnd_UpdTime(hWnd, -1); break;
				case 'h':
					DateWnd_MoveCursor(hWnd, -1); break;
				case 'l':
					DateWnd_MoveCursor(hWnd, 1); break;
				case 'J':
					DateWnd_UpdTime(hWnd, 2); break;
				case 'K':
					DateWnd_UpdTime(hWnd, -2); break;
			}
			return 0;

		case WM_KEYDOWN:
			// Ctrl�L�[��Ԏ擾
			nCtrl = GetKeyState(VK_LCONTROL);
			nCtrl |= GetKeyState(VK_RCONTROL);

			switch(wParam) {
				case VK_UP:		// ���L�[
					if(nCtrl & 0x80) {	// Ctrl�L�[����
						DateWnd_UpdTime(hWnd, -2); break;
					} else {
						DateWnd_UpdTime(hWnd, -1); break;
					}
				case VK_DOWN:	// ���L�[
					if(nCtrl & 0x80) {	// Ctrl�L�[����
						DateWnd_UpdTime(hWnd, 2); break;
					} else {
						DateWnd_UpdTime(hWnd, 1); break;
					}
				case VK_LEFT:	// ���L�[
					DateWnd_MoveCursor(hWnd, -1); break;
				case VK_RIGHT:	// ���L�[
					DateWnd_MoveCursor(hWnd, 1); break;
			}
			return 0;

		case WM_LBUTTONDBLCLK:
			if (wParam & MK_SHIFT) {
				if (wParam & MK_CONTROL) {
					DateWnd_UpdTime(hWnd, -2);
				} else {
					DateWnd_UpdTime(hWnd, -1);
				}
			} else {
				if (wParam & MK_CONTROL) {
					DateWnd_UpdTime(hWnd, 2);
				} else {
					DateWnd_UpdTime(hWnd, 1);
				}
			}
			return 0;
	}

	return CallWindowProc(lpfnStimeProc, hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK EventCall_EtimeDlg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nStat, nCtrl;

	switch(uMsg) {
		case WM_CHAR:
			switch((char)wParam) {
				case 'j':
					DateWnd_UpdTime(hWnd, 1); break;
				case 'k':
					DateWnd_UpdTime(hWnd, -1); break;
				case 'h':
					DateWnd_MoveCursor(hWnd, -1); break;
				case 'l':
					DateWnd_MoveCursor(hWnd, 1); break;
				case 'J':
					DateWnd_UpdTime(hWnd, 2); break;
				case 'K':
					DateWnd_UpdTime(hWnd, -2); break;
			}
			return 0;

		case WM_KEYDOWN:
			// Ctrl�L�[��Ԏ擾
			nCtrl = GetKeyState(VK_LCONTROL);
			nCtrl |= GetKeyState(VK_RCONTROL);

			switch(wParam) {
				case VK_UP:		// ���L�[
					if(nCtrl & 0x80) {	// Ctrl�L�[����
						DateWnd_UpdTime(hWnd, -2); break;
					} else {
						DateWnd_UpdTime(hWnd, -1); break;
					}
				case VK_DOWN:	// ���L�[
					if(nCtrl & 0x80) {	// Ctrl�L�[����
						DateWnd_UpdTime(hWnd, 2); break;
					} else {
						DateWnd_UpdTime(hWnd, 1); break;
					}
				case VK_LEFT:	// ���L�[
					DateWnd_MoveCursor(hWnd, -1); break;
				case VK_RIGHT:	// ���L�[
					DateWnd_MoveCursor(hWnd, 1); break;
			}
			return 0;

		case WM_LBUTTONDBLCLK:
			if (wParam & MK_SHIFT) {
				if (wParam & MK_CONTROL) {
					DateWnd_UpdTime(hWnd, -2);
				} else {
					DateWnd_UpdTime(hWnd, -1);
				}
			} else {
				if (wParam & MK_CONTROL) {
					DateWnd_UpdTime(hWnd, 2);
				} else {
					DateWnd_UpdTime(hWnd, 1);
				}
			}
			return 0;
	}

	return CallWindowProc(lpfnEtimeProc, hWnd, uMsg, wParam, lParam);
}

// �_�C�A���O0(MsgBox�ݒ�)�v���V�[�W��
int MsgBox(HWND hWnd, char *pMsg, char *pTitle, int nType)
{
	HINSTANCE hInstance;
	int nStat;

	// �p�����[�^�̕ۑ�
	if (l_pMsgStr) free(l_pMsgStr);
	l_pMsgStr = strdup(pMsg);
	if (l_pMsgTitle) free(l_pMsgTitle);
	l_pMsgTitle = strdup(pTitle);
	l_nMsgType = nType;

	// �_�C�A���O�\��
	hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
	nStat = DialogBox(hInstance, "IDD_DIALOG0",
	                  hWnd, (DLGPROC)EventCall_MsgBox);

	return nStat;
}

BOOL CALLBACK EventCall_MsgBox(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int nStat;

	switch (msg) {
		case WM_INITDIALOG:
			return MsgBox_Init(hWnd);
		case WM_COMMAND:
			nStat = MsgBox_Command(hWnd, HIWORD(wParam),
			                      LOWORD(wParam), (HWND)lParam);
			SendMessage(hMainWnd, WM_ACTIVATE, 0, 0);
			return nStat;
	}
	return FALSE;
}

BOOL MsgBox_Init(HWND hWnd)
{
	HWND hDlg, hBtn1, hBtn2, hBtn3;
	int x, y, style;
	int nCurWd, nCurHt, nNedWd, nNedHt, nFntHt, nDifHt;

	// �{�^���̐ݒ�
	hBtn1 = GetDlgItem(hWnd, IDBTN1);
	hBtn2 = GetDlgItem(hWnd, IDBTN2);
	hBtn3 = GetDlgItem(hWnd, IDBTN3);
	switch (l_nMsgType & MB_TYPEMASK) {
		case MB_OK:
			EnableWindow(hBtn1, FALSE);
			EnableWindow(hBtn3, FALSE);
			SetWindowText(hBtn2, "OK");
			ChgWinStyle(hBtn1, 0, WS_VISIBLE);
			ChgWinStyle(hBtn2, BS_DEFPUSHBUTTON, 0xF);
			ChgWinStyle(hBtn3, 0, WS_VISIBLE);
			break;
		case MB_OKCANCEL:
			EnableWindow(hBtn1, FALSE);
			ChgWinStyle(hBtn1, 0, WS_VISIBLE);
			SetWindowText(hBtn2, "OK(&O)");
			SetWindowText(hBtn3, "��ݾ�(&C)");
			if ((l_nMsgType & MB_DEFMASK) == MB_DEFBUTTON2) {
				ChgWinStyle(hBtn3, BS_DEFPUSHBUTTON, 0xF);
			} else {
				ChgWinStyle(hBtn2, BS_DEFPUSHBUTTON, 0xF);
			}
			break;
		case MB_YESNOCANCEL:
			SetWindowText(hBtn1, "�͂�(&Y)");
			SetWindowText(hBtn2, "������(&N)");
			SetWindowText(hBtn3, "��ݾ�(&C)");
			if ((l_nMsgType & MB_DEFMASK) == MB_DEFBUTTON2) {
				ChgWinStyle(hBtn2, BS_DEFPUSHBUTTON, 0xF);
			} else if ((l_nMsgType & MB_DEFMASK) == MB_DEFBUTTON3) {
				ChgWinStyle(hBtn3, BS_DEFPUSHBUTTON, 0xF);
			} else {
				ChgWinStyle(hBtn1, BS_DEFPUSHBUTTON, 0xF);
			}
			break;
		case MB_YESNO:
			EnableWindow(hBtn1, FALSE);
			ChgWinStyle(hBtn1, 0, WS_VISIBLE);
			SetWindowText(hBtn2, "�͂�(&Y)");
			SetWindowText(hBtn3, "������(&N)");
			if ((l_nMsgType & MB_DEFMASK) == MB_DEFBUTTON2) {
				ChgWinStyle(hBtn3, BS_DEFPUSHBUTTON, 0xF);
			} else {
				ChgWinStyle(hBtn2, BS_DEFPUSHBUTTON, 0xF);
			}
			break;
	}

	// �\���ɕK�v�ȃT�C�Y�擾
	hDlg = MsgBox_GetDispSize(hWnd, IDC_NOTE, &nCurWd, &nCurHt,
	                                          &nNedWd, &nNedHt, &nFntHt);
	// �e�_�C�A���O�̈ʒu�A�T�C�Y����
	nDifHt = nNedHt - nCurHt;
	if (nDifHt > 0) {
		MsgBox_ChgWinRel(hWnd, 0, 0, 0, nDifHt);
		MsgBox_ChgWinRel(hDlg, 0, 0, 0, nDifHt);
		MsgBox_ChgWinRel(hBtn1, 0, nDifHt, 0, 0);
		MsgBox_ChgWinRel(hBtn2, 0, nDifHt, 0, 0);
		MsgBox_ChgWinRel(hBtn3, 0, nDifHt, 0, 0);
	} else if (nNedHt + nFntHt == 0) {
		MsgBox_ChgWinRel(hDlg, 0, nNedHt, 0, 0);
		ChgWinStyle(hDlg, SS_CENTER, SS_TYPEMASK);
	}

	// �_�C�A���O�̕\���ʒu�擾
	GetDispLoc(hWnd, &x, &y);
	// �_�C�A���O��K�؂Ȉʒu�Ɉړ�
	SetWindowPos(hWnd, NULL, x, y, 0, 0, SWP_NOSIZE);

	// �^�C�g���ƃ��b�Z�[�W�ݒ�
	SetWindowText(hWnd, l_pMsgTitle);
	hDlg = GetDlgItem(hWnd, IDC_NOTE);
	SetWindowText(hDlg, l_pMsgStr);

	return FALSE;
}

// �\���ɕK�v�ȃT�C�Y�擾
HWND MsgBox_GetDispSize(HWND hWnd, int nId, int *pCurWd, int *pCurHt, int *pNedWd, int *pNedHt, int *pFntHt)
{
	HWND hDlg;
	HDC hdc;
	HFONT hFont;
	LOGFONT lf;
	RECT CltRc;
	int w, h, dh, nh, style;

	// ���b�Z�[�W�̈�̃f�o�C�X �R���e�L�X�g�擾
	hDlg = GetDlgItem(hWnd, nId);
	hdc = GetDC(hDlg);

	// ���b�Z�[�W�̈�̃t�H���g��I��
	hFont = (HFONT)SendMessage(hDlg, WM_GETFONT, 0, 0);
	if (hFont) SelectObject(hdc, hFont);

	// �t�H���g���̎擾
	GetObject(hFont, sizeof(LOGFONT), &lf);
	*pFntHt = lf.lfHeight;
	
	// ���b�Z�[�W�̈�̃T�C�Y�擾
	GetClientRect(hDlg, &CltRc);
	*pCurWd = CltRc.right - CltRc.left;
	*pCurHt = CltRc.bottom - CltRc.top;

	// ���b�Z�[�W�̈�ŕK�v�ȕ\���T�C�Y�擾
	DrawText(hdc, l_pMsgStr, -1, &CltRc, DT_EDITCONTROL | DT_WORDBREAK | DT_NOPREFIX | DT_CALCRECT | DT_EXTERNALLEADING);
	*pNedWd = CltRc.right - CltRc.left;
	*pNedHt = CltRc.bottom - CltRc.top;
	ReleaseDC(hDlg, hdc);

	return hDlg;
}

// ���ΓI�ȃT�C�Y�A�ʒu�ύX
int MsgBox_ChgWinRel(HWND hWnd, int dx, int dy, int dw, int dh)
{
	HWND hPar;
	RECT rc, rc2;
	int ofset;

	// �E�B���h�E�̍��W�擾
	GetWindowRect(hWnd, &rc);

	// �T�C�Y�ύX
	if (dw != 0 || dh != 0) {
		SetWindowPos(hWnd, NULL, 0, 0, 
		             rc.right - rc.left + dw,
		             rc.bottom - rc.top + dh, SWP_NOMOVE);
	}
	
	// �ʒu�ύX
	if (dx != 0 || dy != 0) {
		hPar = GetParent(hWnd);
		GetWindowRect(hPar, &rc2);
		ofset =  GetSystemMetrics(SM_CYDLGFRAME)
		       + GetSystemMetrics(SM_CYCAPTION);
		SetWindowPos(hWnd, NULL,
		             rc.left - rc2.left + dx,
		             rc.top - rc2.top + dy - ofset, 0, 0, SWP_NOSIZE);
	}

	return TRUE;
}

BOOL MsgBox_Command(HWND hWnd, WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	WORD wRetId;

	switch (wID) {
		case IDBTN1:
		case IDBTN2:
		case IDBTN3:
			switch (l_nMsgType & MB_TYPEMASK) {
				case MB_OK:
					wRetId = IDOK;
					break;
				case MB_OKCANCEL:
					if (wID == IDBTN2) {
						wRetId = IDOK;
					} else {
						wRetId = IDCANCEL;
					}
					break;
				case MB_YESNOCANCEL:
					if (wID == IDBTN1) {
						wRetId = IDYES;
					} else if (wID == IDBTN2) {
						wRetId = IDNO;
					} else {
						wRetId = IDCANCEL;
					}
					break;
				case MB_YESNO:
					if (wID == IDBTN2) {
						wRetId = IDYES;
					} else {
						wRetId = IDNO;
					}
					break;
				default:
					wRetId = IDCANCEL;
			}
			EndDialog(hWnd, wRetId);
			break;
		default:
			return FALSE;
	}
	return TRUE;
}

// ���ʏ��� --------------------------------------------------------------------
// �_�C�A���O�̕\���ʒu�擾
void GetDispLoc(HWND hWnd, int *lpX, int *lpY)
{
#define GetMonitorRect(rc)  SystemParametersInfo(SPI_GETWORKAREA,0,rc,0)
	RECT BasRc, OwnRc, Screen;
	int x, y, OwnHt;

	GetWindowRect(hMainWnd, &BasRc);
	GetWindowRect(hWnd, &OwnRc);
	GetMonitorRect(&Screen);

	x = BasRc.left + int(((BasRc.right - BasRc.left + 1)
	                       - (OwnRc.right - OwnRc.left + 1)) / 2);
	if (x < 0) x = BasRc.right;
	OwnHt = OwnRc.bottom - OwnRc.top;
	if (Screen.bottom < (BasRc.top + OwnHt)) {
		y = Screen.bottom - OwnHt - GetSystemMetrics(SM_CYFRAME);
	} else {
		y = BasRc.top + GetSystemMetrics(SM_CYFRAME);
	}

	*lpX = x;
	*lpY = y;
}

// �����̕��בւ�
void SortHist(char *pInput, STRLIST *pHist, int nHstMax)
{
	int nIdx, nMax, i;

	// �����������
	nIdx = pHist->SrchString(pInput);
	if (nIdx == -1) {
		// �����ɒǉ�
		pHist->InsString(0, pInput);
	} else {
		// �����̏��ԓ��ւ�
		pHist->DelString(nIdx);
		pHist->InsString(0, pInput);
	}

	// �ő吔�𒴂����������폜
	nMax = pHist->GetMaxCount();
	if (nHstMax < nMax) {
		for (i = nHstMax; i < nMax; ++i) {
			pHist->DelString(nHstMax);
		}
	}
}

// �_�C�A���O�̃e�L�X�g�擾
char *GetDlgText(HWND hWnd, int nDlgId)
{
	HWND hDlg;
	int nLen;
	char *pStr;

	hDlg = GetDlgItem(hWnd, nDlgId);
	nLen = GetWindowTextLength(hDlg);
	pStr = (char *)malloc(nLen + 1);
	GetWindowText(hDlg, pStr, nLen + 1);

	return pStr;
}

// �E�B���h�E�̃X�^�C���ύX
void ChgWinStyle(HWND hWnd, int nValue, int nMask)
{
	int nStyle;

	nStyle = GetWindowLong(hWnd, GWL_STYLE);
	nStyle &= ~nMask;
	nStyle |= (nValue & nMask);
	SetWindowLong(hWnd, GWL_STYLE, nStyle);
}

// ���ݎ����擾
// Args:
// 1	char *	���ݎ���������(YYYY/M/D h:mm)������|�C���^
// Return:		�Ȃ�
void GetTime(char *pCurTime)
{
	time_t now = time(NULL);
	struct tm *pNow = localtime(&now);

	// sprintf(pCurTime, "%d/%d/%d %d:%02d:%02d", pNow->tm_year + 1900, pNow->tm_mon + 1, pNow->tm_mday, pNow->tm_hour, pNow->tm_min, pNow->tm_sec);
	sprintf(pCurTime, "%d/%d/%d %d:%02d", pNow->tm_year + 1900, pNow->tm_mon + 1, pNow->tm_mday, pNow->tm_hour, pNow->tm_min);
}
