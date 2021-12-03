/*
 * DialogBoxに関する処理
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

// 固定値 
#define MAX_BUF		1024

// マクロ設定の状態
enum MCRSTAT {
	TOP = 0, NEW, EDIT, 
};

static MCRSTAT l_eMcrStat;

// MsgBoxパラメータ
static char *l_pMsgStr = NULL;
static char *l_pMsgTitle = NULL;
static int l_nMsgType;

// CallBack関数
static WNDPROC lpfnDateProc;
static WNDPROC lpfnStimeProc;
static WNDPROC lpfnEtimeProc;

//----------------------------------------------------------------------
//■Dialogのメッセージ処理
//----------------------------------------------------------------------
// ダイアログ1(バージョン表示)プロシージャ
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

	// ダイアログの表示位置取得
	GetDispLoc(hWnd, &x, &y);
	// ダイアログを適切な位置に移動
	SetWindowPos(hWnd, NULL, x, y, 0, 0, SWP_NOSIZE);

	// バージョン情報の表示
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

// ダイアログ2(検索/置換条件入力)プロシージャ
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

	// ダイアログの表示位置取得
	GetDispLoc(hWnd, &x, &y);
	// ダイアログを適切な位置に移動
	SetWindowPos(hWnd, NULL, x, y, 0, 0, SWP_NOSIZE);

	SetDlgItemText(hWnd, IDC_NOTE,
	               "「/検索1/置換1/[検索2/置換2/...]」形式で入力");
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
			// 入力文字列の保存
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

// ダイアログ3(ホットキー設定)プロシージャ
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

	// ダイアログの表示位置取得
	GetDispLoc(hDlg, &x, &y);
	// ダイアログを適切な位置に移動
	SetWindowPos(hDlg, NULL, x, y, 0, 0, SWP_NOSIZE);

	// 現在設定されているホットキーを表示
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
				MsgBox(hDlg, "ホットキーの設定に失敗しました。", "ERROR", MB_OK);
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

// ダイアログ4(書式パラメータ入力)プロシージャ
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

	// ダイアログの表示位置取得
	GetDispLoc(hWnd, &x, &y);
	// ダイアログを適切な位置に移動
	SetWindowPos(hWnd, NULL, x, y, 0, 0, SWP_NOSIZE);

	// 種別リストの設定
	hList = GetDlgItem(hWnd, IDC_LIST);
	ComboBox_AddString(hList, "HTMLコードの整形");
	ComboBox_AddString(hList, "カラムデータ → 書式変換");
	ComboBox_AddString(hList, "カラムデータ → 表変換");
	ComboBox_AddString(hList, "文字列を含む行を抽出(grep)");
	ComboBox_AddString(hList, "文字列を含まない行を抽出(grep -v)");
	ComboBox_SetCurSel(hList, g_FmtInfo.nFmtId);

	// 書式履歴の設定
	hCombo = GetDlgItem(hWnd, IDC_INPUT);
	for (i = 0; i < g_FmtInfo.FmtHist.GetMaxCount(); ++i) {
		ComboBox_AddString(hCombo, g_FmtInfo.FmtHist.GetString(i));
	}
	ComboBox_SetCurSel(hCombo, 0);

	// ラジオボタンのチェック設定
	if (g_FmtInfo.nDlmtId == DLMTID_TAB) {
		id = IDC_RADIO1;
	} else {
		id = IDC_RADIO2;
	}
	Button_SetCheck(GetDlgItem(hWnd, id), BST_CHECKED);

	// 種別に応じた項目の有効/無効設定
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
			// 種別の保存
			hList = GetDlgItem(hWnd, IDC_LIST);
			g_FmtInfo.nFmtId = SendMessage(hList, CB_GETCURSEL, 0, 0);

			// 書式の保存
			GetDlgItemText(hWnd, IDC_INPUT, szBuf, MAX_BUF);
			/*
			if (*szBuf) {
				SortHist(szBuf, &(g_FmtInfo.FmtHist), MAX_FMTHIST);
			}
			*/
			SortHist(szBuf, &(g_FmtInfo.FmtHist), MAX_FMTHIST); // 空白も登録

			// 区切り文字の保存
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
			SetWindowText(hLabel, "書式：");
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
			SetWindowText(hLabel, "文字：");
			EnableWindow(hInp, TRUE);
			EnableWindow(hNote, FALSE);
			EnableWindow(hRad1, FALSE);
			EnableWindow(hRad2, FALSE);

			UpdateWindow(hInp);
			break;
	}
}

// ダイアログ5(マクロ設定)プロシージャ
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

	// ダイアログの表示位置取得
	GetDispLoc(hWnd, &x, &y);
	// ダイアログを適切な位置に移動
	SetWindowPos(hWnd, NULL, x, y, 0, 0, SWP_NOSIZE);

	// マクロ情報設定
	SetMcrData(hWnd, 0);

	// 有効ボタン設定
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
				// 選択した名称と内容をリスト先頭へ移動
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
				// マクロ情報設定
				SetMcrData(hWnd, nIdx);
			}
			break;
		case IDREG:
			// マクロ情報の登録
			pName  = GetDlgText(hWnd, IDC_NOTE);
			pMacro = GetDlgText(hWnd, IDC_INPUT);
			if (l_eMcrStat == NEW) nIdx = -1;
			nStat = AddMcrInfo(pName, pMacro, nIdx);
			if (nStat) {
				if (l_eMcrStat == NEW) nIdx = 0;
				l_eMcrStat = TOP;
				McrWnd_SetCtrl(hWnd, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE);
				// マクロ情報設定
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
			// クリップボードからマクロの名称と内容を取得
			nStat = GetMcrFromCb(&pName, &pMacro, TRUE);
			while (nStat) {
				// マクロ情報の登録
				nStat = AddMcrInfo(pName, pMacro, -1);
				free(pName);
				free(pMacro);

				nStat = GetMcrFromCb(&pName, &pMacro, FALSE);
			}

			McrWnd_SetCtrl(hWnd, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE);
			// マクロ情報設定
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
			sprintf(szBuf, "マクロ「%s」を削除しますか？",
			        g_McrInfo.Name.GetString(nIdx));
			nStat = MsgBox(hWnd, szBuf, "確認", MB_YESNO | MB_DEFBUTTON2);
			if (nStat == IDYES) {
				g_McrInfo.Name.DelString(nIdx);
				g_McrInfo.Macro.DelString(nIdx);
				// マクロ情報設定
				SetMcrData(hWnd, 0);
			} else {
				// マクロ情報設定
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

// ダイアログの制御設定
void McrWnd_SetCtrl(HWND hWnd, int bNew, int bImp, int bEdt, int bDel, int bReg, int bExc)
{
	HWND hDlg;
	int nCnt;

	// マクロデータが無ければ、編集/削除ボタン無効
	nCnt = g_McrInfo.Name.GetMaxCount();
	if (nCnt == 0) {
		bEdt = FALSE;
		bDel = FALSE;
	}

	// 各ボタンの有効/無効設定
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

// ボタンのStyle設定
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

// EditBoxのStyle設定
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

// ComboBoxのStyle設定
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

// マクロ情報をダイアログに設定
void SetMcrData(HWND hWnd, int nIdx)
{
	HWND hCombo, hEdit;
	int i, nCnt;

	// 名称/内容のクリア
	hCombo = GetDlgItem(hWnd, IDC_LIST);
	hEdit = GetDlgItem(hWnd, IDC_INPUT);
	SendMessage(hCombo, CB_RESETCONTENT, 0, 0);
	SetWindowText(hEdit, "");

	// 名称の設定
	nCnt = g_McrInfo.Name.GetMaxCount();
	for (i = 0; i < nCnt; ++i) {
		SendMessage(hCombo, CB_ADDSTRING, 0,
		            (LPARAM)g_McrInfo.Name.GetString(i));
	}

	if (nIdx >= 0 && nCnt > 0) {
		SendMessage(hCombo, CB_SETCURSEL, nIdx, 0);
		// 内容の設定
		SetWindowText(hEdit, g_McrInfo.Macro.GetString(nIdx));
	}
}

// マクロ情報を追加
int AddMcrInfo(char *pName, char *pMacro, int nCurIdx)
{
	HWND hCombo;
	int nIdx, nStat, bChng = FALSE;
	char szMsg[MAX_BUF];

	if (nCurIdx < 0) { // 新規
		// 名称を検索
		nIdx = g_McrInfo.Name.SrchString(pName);
		if (nIdx == -1) {
			// 名称と内容の追加
			g_McrInfo.Name.InsString(0, pName);
			g_McrInfo.Macro.InsString(0, pMacro);
			bChng = TRUE;
		} else {
			sprintf(szMsg, "マクロ「%s」は登録済みです。上書きしますか？", pName);
			nStat = MsgBox(hMainWnd, szMsg, "確認", MB_YESNO | MB_DEFBUTTON2);
			if (nStat == IDYES) {
				// 名称と内容の書換え
				g_McrInfo.Name.DelString(nIdx);
				g_McrInfo.Name.InsString(nIdx, pName);
				g_McrInfo.Macro.DelString(nIdx);
				g_McrInfo.Macro.InsString(nIdx, pMacro);
				bChng = TRUE;
			}
		}
	} else { // 編集
		// 名称と内容の書換え
		g_McrInfo.Name.DelString(nCurIdx);
		g_McrInfo.Name.InsString(nCurIdx, pName);
		g_McrInfo.Macro.DelString(nCurIdx);
		g_McrInfo.Macro.InsString(nCurIdx, pMacro);
		bChng = TRUE;
	}
	
	return bChng;
}

// クリップボードからマクロの名称と内容を取得
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
		// 残り文字列クリア
		if (pRestStr) {
			free(pRestStr);
			pRestStr = NULL;
		}
		// クリップボードの文字列取得
		hDlg = GetDlgItem(hMainWnd, EditBox);
		nLen = GetWindowTextLength(hDlg);
		pCbStr = (char *)malloc(nLen + 1);
		GetWindowText(hDlg, pCbStr, nLen + 1);
	} else {
		// 残り文字列を設定
		pCbStr = pRestStr;
	}

	// デリミタで分解
	StrSplit(pCbStr, DLMT, SPLIT_NONE | SPLIT_MULTI, &LineStr);
	nCnt = LineStr.GetMaxCount();

	nStat = STAT_SRCH_NAME;
	for (i = 0; i < nCnt; ++i) {
		pLine = LineStr.GetString(i);
		if (nStat == STAT_SRCH_NAME) {
			if (*pLine == '*') {
				// 名称の設定
				*pName = strdup(pLine + 1);
				nStat = STAT_GET_MACRO;
				bOutput = TRUE;
			}
		} else if (nStat == STAT_GET_MACRO) {
			// 内容の設定
			if (*pLine == '*') {
				RestStr.AddString(pLine);
				nStat = STAT_SAVE_REST;
			} else if (*pLine != '\0') {
				MacroStr.AddString(pLine);
			}
		} else {
			// 残り文字列の保存
			RestStr.AddString(pLine);
		}
	}
	*pMacro = MacroStr.MergeAllStr(DLMT);
	free(pCbStr);
	pRestStr = RestStr.MergeAllStr(DLMT);

	return bOutput;
}

// ダイアログ6(日時設定)プロシージャ
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

	// ダイアログの表示位置取得
	GetDispLoc(hWnd, &x, &y);
	// ダイアログを適切な位置に移動
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

	// 設定形式の選択リスト設定
	g_DateInfo.hCombo = GetDlgItem(hWnd, IDC_FORMAT);
	ComboBox_AddString(g_DateInfo.hCombo, "年月日+開始時刻～終了時刻");
	ComboBox_AddString(g_DateInfo.hCombo, "年月日+時刻");
	ComboBox_AddString(g_DateInfo.hCombo, "年月日");
	ComboBox_SetCurSel(g_DateInfo.hCombo, g_DateInfo.nIdx);

	// 現在年月日の設定
	GetTime(szDate);
	Edit.split(szDate, " ");
	SetWindowText(g_DateInfo.hDate, Edit.get(0));
	Edit_SetSel(g_DateInfo.hDate, 0, 10);

	// 現在時刻の設定
	nHalf = DateWnd_GetHalfHour(Edit.get(1));	// 開始時間
	if (g_DateInfo.nIdx != DTFMT_DATETIME) {
		nHalf = DateWnd_AddHalfHour(nHalf, 0);		// 30分値で補正
		DateWnd_Half2Str(nHalf, szTime);			// hh:mmへ変換
		SetWindowText(g_DateInfo.hStime, szTime);	// 開始時刻へ設定
	} else {
		// 開始時刻へ現在時刻をそのまま設定
		SetWindowText(g_DateInfo.hStime, Edit.get(1));
	}
	Edit_SetSel(g_DateInfo.hStime, 0, 5);

	nHalf = DateWnd_AddHalfHour(nHalf, 2);		// 現在時刻+1時間
	DateWnd_Half2Str(nHalf, szTime);			// hh:mmへ変換
	SetWindowText(g_DateInfo.hEtime, szTime);	// 終了時刻へ設定
	Edit_SetSel(g_DateInfo.hEtime, 0, 5);

	// 年月日にカーソルを合わせる
	SetFocus(g_DateInfo.hDate);

	// ダイアログの制御設定
	DateWnd_SetCtrl(hWnd);

	// マクロ実行
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
				sprintf(szBuf, "%s %s～%s", pDate, pStime, pEtime);
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

// ダイアログの制御設定
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

// 日時EditBox間のカーソル移動
// Args:
// 1	HWND 	現在位置のハンドル
// 2	int 	1:右方向へ移動、-1:左方向へ移動
// Return:		なし
void DateWnd_MoveCursor(HWND hWnd, int nWay)
{
	HWND hNext;
	int nIdx, nMax, nPos, nNext;

	// カーソル移動可能な項目数算出
	nIdx = ComboBox_GetCurSel(g_DateInfo.hCombo);
	nMax = 3 - nIdx;

	// カーソル位置取得
	nPos = DateWnd_GetCsrPos(hWnd);

	// 次のカーソル位置設定
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

// EditBoxのカーソル位置取得
// Args:
// 1	HWND 	現在位置のハンドル
// Return:		0:年月日、1:開始時刻、2:終了時刻
int DateWnd_GetCsrPos(HWND hWnd)
{
	int nPos;

	// カーソル位置取得
	if (hWnd == g_DateInfo.hDate) {
		nPos = 0;
	} else if (hWnd == g_DateInfo.hStime) {
		nPos = 1;
	} else if (hWnd == g_DateInfo.hEtime) {
		nPos = 2;
	}

	return nPos;
}
// 年月日のN日更新
// Args:
// 1	int 	更新する差分日数
// Return:		なし
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

// 時刻のN分更新
// Args:
// 1	HWND 	現在位置のハンドル
// 2	int 	更新する30分値(1:30分、2:1時間)
// Return:		なし
void DateWnd_UpdTime(HWND hWnd, int nDiff)
{
	int nPos, nStime, nEtime, nHour, nMin;
	char szStime[MAX_BUF+1], szEtime[MAX_BUF+1];

	// ダイアログの時刻情報取得
	GetWindowText(g_DateInfo.hStime, szStime, MAX_BUF);
	GetWindowText(g_DateInfo.hEtime, szEtime, MAX_BUF);

	// 時刻を30分値へ変換
	nStime = DateWnd_GetHalfHour(szStime);
	nEtime = DateWnd_GetHalfHour(szEtime);

	// 30分値の差分更新
	nPos = DateWnd_GetCsrPos(hWnd);
	if (nPos == 1) { // 開始時刻
		// 開始時刻の更新
		nStime = DateWnd_AddHalfHour(nStime, nDiff);
		DateWnd_Half2Str(nStime, szStime);
		SetWindowText(g_DateInfo.hStime, szStime);

		// 終了時刻も連動して更新
		nEtime = DateWnd_AddHalfHour(nEtime, nDiff);
		DateWnd_Half2Str(nEtime, szEtime);
		SetWindowText(g_DateInfo.hEtime, szEtime);
	} else if (nPos == 2) { // 終了時刻
		// 終了時刻の更新
		nEtime = DateWnd_AddHalfHour(nEtime, nDiff);
		DateWnd_Half2Str(nEtime, szEtime);
		SetWindowText(g_DateInfo.hEtime, szEtime);

		// 開始時刻と終了時刻が逆転する場合、開始時刻を更新
		if (nStime >= nEtime) {
			nStime = DateWnd_AddHalfHour(nStime, nDiff);
			DateWnd_Half2Str(nStime, szStime);
			SetWindowText(g_DateInfo.hStime, szStime);
		}
	}
	Edit_SetSel(g_DateInfo.hStime, 0, 5);
	Edit_SetSel(g_DateInfo.hEtime, 0, 5);
}

// 時刻の30分値変換
// Args:
// 1	char * 	時刻の文字列(hh:㎜)
// Return:		30分値(1:30分, 2:1時間, ...)
int DateWnd_GetHalfHour(char *pTime)
{
	int nHour, nMin, nHalf;
	Variable Edit;

	// 時分を数値変換
	Edit.split(pTime, ":");
	nHour = atoi(Edit.get(0));
	nMin  = atoi(Edit.get(1));

	// 30分値へ変換
	nMin  = int((nMin +15) / 30);
	nHalf = 2 * nHour + nMin;

	return nHalf;
}

// 30分値の加減算
// Args:
// 1	int   	元の30分値
// 2	int   	加減算する30分値
// Return:		加減算した30分値(1:30分, 2:1時間, ...)
int DateWnd_AddHalfHour(int nOrg, int nAdd)
{
	int nHalf;

	nHalf = nOrg + nAdd;
	if (nHalf < 0) nHalf = 0;
	if (nHalf > 47) nHalf = 47;

	return nHalf;
}

// 30分値→時刻文字(hh:㎜)変換
// Args:
// 1	char * 	時刻の文字列(hh:㎜)
// Return:		30分値(1:30分, 2:1時間, ...)
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
			// Ctrlキー状態取得
			nCtrl = GetKeyState(VK_LCONTROL);
			nCtrl |= GetKeyState(VK_RCONTROL);

			switch(wParam) {
				case VK_DOWN:	// ↓キー
					if(nCtrl & 0x80) {	// Ctrlキー押下
						DateWnd_UpdDate(7); break;
					} else {
						DateWnd_UpdDate(1); break;
					}
				case VK_UP:		// ↑キー
					if(nCtrl & 0x80) {	// Ctrlキー押下
						DateWnd_UpdDate(-7); break;
					} else {
						DateWnd_UpdDate(-1); break;
					}
				case VK_LEFT:	// ←キー
					DateWnd_MoveCursor(hWnd, -1); break;
				case VK_RIGHT:	// →キー
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
			// Ctrlキー状態取得
			nCtrl = GetKeyState(VK_LCONTROL);
			nCtrl |= GetKeyState(VK_RCONTROL);

			switch(wParam) {
				case VK_UP:		// ↑キー
					if(nCtrl & 0x80) {	// Ctrlキー押下
						DateWnd_UpdTime(hWnd, -2); break;
					} else {
						DateWnd_UpdTime(hWnd, -1); break;
					}
				case VK_DOWN:	// ↓キー
					if(nCtrl & 0x80) {	// Ctrlキー押下
						DateWnd_UpdTime(hWnd, 2); break;
					} else {
						DateWnd_UpdTime(hWnd, 1); break;
					}
				case VK_LEFT:	// ←キー
					DateWnd_MoveCursor(hWnd, -1); break;
				case VK_RIGHT:	// →キー
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
			// Ctrlキー状態取得
			nCtrl = GetKeyState(VK_LCONTROL);
			nCtrl |= GetKeyState(VK_RCONTROL);

			switch(wParam) {
				case VK_UP:		// ↑キー
					if(nCtrl & 0x80) {	// Ctrlキー押下
						DateWnd_UpdTime(hWnd, -2); break;
					} else {
						DateWnd_UpdTime(hWnd, -1); break;
					}
				case VK_DOWN:	// ↓キー
					if(nCtrl & 0x80) {	// Ctrlキー押下
						DateWnd_UpdTime(hWnd, 2); break;
					} else {
						DateWnd_UpdTime(hWnd, 1); break;
					}
				case VK_LEFT:	// ←キー
					DateWnd_MoveCursor(hWnd, -1); break;
				case VK_RIGHT:	// →キー
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

// ダイアログ0(MsgBox設定)プロシージャ
int MsgBox(HWND hWnd, char *pMsg, char *pTitle, int nType)
{
	HINSTANCE hInstance;
	int nStat;

	// パラメータの保存
	if (l_pMsgStr) free(l_pMsgStr);
	l_pMsgStr = strdup(pMsg);
	if (l_pMsgTitle) free(l_pMsgTitle);
	l_pMsgTitle = strdup(pTitle);
	l_nMsgType = nType;

	// ダイアログ表示
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

	// ボタンの設定
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
			SetWindowText(hBtn3, "ｷｬﾝｾﾙ(&C)");
			if ((l_nMsgType & MB_DEFMASK) == MB_DEFBUTTON2) {
				ChgWinStyle(hBtn3, BS_DEFPUSHBUTTON, 0xF);
			} else {
				ChgWinStyle(hBtn2, BS_DEFPUSHBUTTON, 0xF);
			}
			break;
		case MB_YESNOCANCEL:
			SetWindowText(hBtn1, "はい(&Y)");
			SetWindowText(hBtn2, "いいえ(&N)");
			SetWindowText(hBtn3, "ｷｬﾝｾﾙ(&C)");
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
			SetWindowText(hBtn2, "はい(&Y)");
			SetWindowText(hBtn3, "いいえ(&N)");
			if ((l_nMsgType & MB_DEFMASK) == MB_DEFBUTTON2) {
				ChgWinStyle(hBtn3, BS_DEFPUSHBUTTON, 0xF);
			} else {
				ChgWinStyle(hBtn2, BS_DEFPUSHBUTTON, 0xF);
			}
			break;
	}

	// 表示に必要なサイズ取得
	hDlg = MsgBox_GetDispSize(hWnd, IDC_NOTE, &nCurWd, &nCurHt,
	                                          &nNedWd, &nNedHt, &nFntHt);
	// 各ダイアログの位置、サイズ調整
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

	// ダイアログの表示位置取得
	GetDispLoc(hWnd, &x, &y);
	// ダイアログを適切な位置に移動
	SetWindowPos(hWnd, NULL, x, y, 0, 0, SWP_NOSIZE);

	// タイトルとメッセージ設定
	SetWindowText(hWnd, l_pMsgTitle);
	hDlg = GetDlgItem(hWnd, IDC_NOTE);
	SetWindowText(hDlg, l_pMsgStr);

	return FALSE;
}

// 表示に必要なサイズ取得
HWND MsgBox_GetDispSize(HWND hWnd, int nId, int *pCurWd, int *pCurHt, int *pNedWd, int *pNedHt, int *pFntHt)
{
	HWND hDlg;
	HDC hdc;
	HFONT hFont;
	LOGFONT lf;
	RECT CltRc;
	int w, h, dh, nh, style;

	// メッセージ領域のデバイス コンテキスト取得
	hDlg = GetDlgItem(hWnd, nId);
	hdc = GetDC(hDlg);

	// メッセージ領域のフォントを選択
	hFont = (HFONT)SendMessage(hDlg, WM_GETFONT, 0, 0);
	if (hFont) SelectObject(hdc, hFont);

	// フォント情報の取得
	GetObject(hFont, sizeof(LOGFONT), &lf);
	*pFntHt = lf.lfHeight;
	
	// メッセージ領域のサイズ取得
	GetClientRect(hDlg, &CltRc);
	*pCurWd = CltRc.right - CltRc.left;
	*pCurHt = CltRc.bottom - CltRc.top;

	// メッセージ領域で必要な表示サイズ取得
	DrawText(hdc, l_pMsgStr, -1, &CltRc, DT_EDITCONTROL | DT_WORDBREAK | DT_NOPREFIX | DT_CALCRECT | DT_EXTERNALLEADING);
	*pNedWd = CltRc.right - CltRc.left;
	*pNedHt = CltRc.bottom - CltRc.top;
	ReleaseDC(hDlg, hdc);

	return hDlg;
}

// 相対的なサイズ、位置変更
int MsgBox_ChgWinRel(HWND hWnd, int dx, int dy, int dw, int dh)
{
	HWND hPar;
	RECT rc, rc2;
	int ofset;

	// ウィンドウの座標取得
	GetWindowRect(hWnd, &rc);

	// サイズ変更
	if (dw != 0 || dh != 0) {
		SetWindowPos(hWnd, NULL, 0, 0, 
		             rc.right - rc.left + dw,
		             rc.bottom - rc.top + dh, SWP_NOMOVE);
	}
	
	// 位置変更
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

// 共通処理 --------------------------------------------------------------------
// ダイアログの表示位置取得
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

// 履歴の並べ替え
void SortHist(char *pInput, STRLIST *pHist, int nHstMax)
{
	int nIdx, nMax, i;

	// 履歴内を検索
	nIdx = pHist->SrchString(pInput);
	if (nIdx == -1) {
		// 履歴に追加
		pHist->InsString(0, pInput);
	} else {
		// 履歴の順番入替え
		pHist->DelString(nIdx);
		pHist->InsString(0, pInput);
	}

	// 最大数を超えた履歴を削除
	nMax = pHist->GetMaxCount();
	if (nHstMax < nMax) {
		for (i = nHstMax; i < nMax; ++i) {
			pHist->DelString(nHstMax);
		}
	}
}

// ダイアログのテキスト取得
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

// ウィンドウのスタイル変更
void ChgWinStyle(HWND hWnd, int nValue, int nMask)
{
	int nStyle;

	nStyle = GetWindowLong(hWnd, GWL_STYLE);
	nStyle &= ~nMask;
	nStyle |= (nValue & nMask);
	SetWindowLong(hWnd, GWL_STYLE, nStyle);
}

// 現在時刻取得
// Args:
// 1	char *	現在時刻文字列(YYYY/M/D h:mm)を受取るポインタ
// Return:		なし
void GetTime(char *pCurTime)
{
	time_t now = time(NULL);
	struct tm *pNow = localtime(&now);

	// sprintf(pCurTime, "%d/%d/%d %d:%02d:%02d", pNow->tm_year + 1900, pNow->tm_mon + 1, pNow->tm_mday, pNow->tm_hour, pNow->tm_min, pNow->tm_sec);
	sprintf(pCurTime, "%d/%d/%d %d:%02d", pNow->tm_year + 1900, pNow->tm_mon + 1, pNow->tm_mday, pNow->tm_hour, pNow->tm_min);
}
