/*
 * クリップボード編集ツール
 *======================================================================
 *[変更履歴]
 * Ver0.00  2016/10/10 作成開始
 * Ver1.00  2016/12/24 新規作成
 * Ver1.01  2016/12/30 ダイアログの表示位置を中央上部へ変更
 * Ver1.02  2017/04/02 ホットキーの追加
 * Ver1.03  2017/04/08 StrDataクラスのバグ修正(CSV形式の現在位置データ長算出)
 * Ver1.04  2017/04/09 インデント機能の追加
 * Ver1.05  2017/04/15 ホットキー保存機能、DropFile対応追加
 * Ver1.06  2017/04/18 EditBox内編集機能の追加
 * Ver1.07  2017/04/20 単独の全角濁点/半濁点の半角変換追加
 * Ver1.08  2017/04/22 最小化状態で終了時の保存するWindow位置修正
 * Ver1.09  2017/04/25 全角半角カナ変換の不具合修正(ノ→モ、ヴ追加)
 * Ver1.10  2017/04/30 括り制御機能の追加
 * Ver1.11  2017/05/04 日付変換機能の追加
 * Ver1.12  2017/05/08 日付変換機能の不具合修正(日付の正規表現)
 * Ver1.13  2017/05/09 日付変換機能の不具合修正(正規表現ミス、状態比較)
 * Ver1.14  2017/05/11 タスクバーの場所による終了時の保存位置不具合修正
 * Ver1.15  2017/05/23 HTMLの整形機能追加
 * Ver1.16  2017/05/28 文字列置換の履歴機能追加
 * Ver1.17  2017/05/30 HTML整形の不具合修正(LIK→LINK)
 *                     ドロップ時のファイル名取得改善(1ファイル時は改行無し)
 * Ver1.18  2017/06/04 文字列置換にワイルドカード検索/連続置換機能追加
 * Ver1.19  2017/06/16 カラムデータの書式変換機能追加
 * Ver1.20  2017/06/18 ファイルドロップ時にActivate処理追加
 * Ver1.21  2017/06/21 カラムデータのEXCEL形式対応
 * Ver1.22  2017/06/25 HTML整形不具合修正(クリップボードが空の場合)
 * Ver1.23  2017/07/01 スペースを含むファイル名実行の不具合修正
 * Ver1.24  2017/07/06 カーソル位置の行データでファイル名実行機能追加
 * Ver1.25  2017/07/17 Trim機能のパターン追加
 * Ver1.26  2017/07/24 ファイルドロップ時の260byteを超えるパス名対応
 * Ver1.27  2017/08/29 単独1バイト改行(0x0a)の置換機能追加
 * Ver1.28  2017/10/31 マクロ機能追加
 * Ver1.29  2018/02/05 ウィンドウ最小化時のクリップボード読込み抑止機能追加
 *                     ClipBoard内変化に伴う自動Active表示不具合修正
 * Ver1.30  2018/02/15 DropFile時のネットワークドライブ名展開機能追加
 * Ver1.31  2018/09/17 DropFileのパス名→ツリー表示変換機能追加
 * Ver1.32  2019/04/01 DropFile時にファイル名を自然順ソート
 * Ver1.33  2019/04/30 ソート機能/改行コード変換(CRLF⇔LF)機能追加
 *                     Windows10対応(表示位置ズレ補正) 
 * Ver1.34  2019/05/04 split,sort処理の不具合修正
 * Ver1.35  2019/05/23 Trim処理の不具合修正(クリップボード末尾に改行が無い場合)
 * Ver1.36  2019/09/01 他アプリとのクリップボード競合回避
 * Ver1.37  2021/02/08 文字列を含む/含まない行の抽出機能追加、日時設定機能追加
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
// 固定値 
#define VERSION		"1.37"
#define DATE		"2021/2/8"

#define MAX_BUF		1024
#define MAX_KEYSEQ	5
#define PARAM_FILE	"QuickEdit.ini"
#define HELP_FILE	"QuickEdit.chm"

// グローバル変数
HWND hMainWnd; 
HWND hNextViewr;
int g_nAutoAct = 0;				// ClipBoard内変化に伴う自動Active表示
int g_nNwDrvNam = 0;			// ネットワークドライブ名展開表示
int g_nSelfEvnt = 0;			// プログラム内の操作に伴うイベント表示
int g_nExcMode = MODE_CMD;		// 実行モード
int g_nCtrlStat = CTRL_NONE;	// 括り制御状態
int g_nIconFlg = 0;				// ウィンドウ最小化表示
STRLIST g_ExchHist;				// 置換条件履歴
STRLIST EditHist;				// 操作履歴
CharData KeySeq(MAX_KEYSEQ);	// キーシーケンス
WORD g_wHotKey;					// ホットキー
FormatParam g_FmtInfo;			// フォーマット情報
MacroParam g_McrInfo;			// マクロ情報
DateWndParam g_DateInfo;		// 日時設定情報

char szDebug[MAX_BUF+1];		// デバッグ表示用

// ファイルローカル変数
static HFONT hFont_MainWnd;
static WNDPROC lpfnEditProc;		// CallBack関数
static char ParamFile[MAX_PATH+1];	// パラメータファイル名
static int nMacroExc = 0;			// マクロ実行状態(1:実行中, -1:エラー)
static STRLIST FncParam;			// マクロ実行時のパラメータ
 
//----------------------------------------------------------------------
//■メインの関数 
//----------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpszCmdLine, int nCmdShow)
{
    HWND hWnd;		// メインウインドウのハンドル
    WNDCLASS wc;	// WNDCLASS構造体
	MSG msg;

	// WNDCLASS構造体を0で初期化
	ZeroMemory(&wc,sizeof(WNDCLASS));
	// WNDCLASS構造体の設定
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= EventCall_MainWnd;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon(hInstance, "IDR_ICON");
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)COLOR_WINDOW;
	wc.lpszMenuName		= MAKEINTRESOURCE(IDR_MENU);	/* メニューを付ける */
	wc.lpszClassName	= "QuickEdit";
	// ウィンドウ クラスを登録
	if (RegisterClass(&wc) == 0)
		return 0; 

	// メインウインドウの生成
	hWnd = CreateWindowEx(
	           WS_EX_ACCEPTFILES | WS_EX_CONTROLPARENT | WS_EX_WINDOWEDGE,
	           wc.lpszClassName, "QuickEdit",
	           WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
	           600, 180, NULL, NULL, hInstance, NULL); 
	if (hWnd == NULL)
		return 0;
	hMainWnd = hWnd;
 
    // ウインドウの表示
    ShowWindow(hWnd, nCmdShow);
 
	// メッセージループ
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg); 
		DispatchMessage(&msg);   
	} 
	return (msg.wParam);
}

//----------------------------------------------------------------------
//■メインウインドウのメッセージ処理
//----------------------------------------------------------------------
LRESULT CALLBACK EventCall_MainWnd(HWND hWnd,UINT uMsg,WPARAM wParam, LPARAM lParam)
{
	INITCOMMONCONTROLSEX ic;
	LONG nStyle;

	switch (uMsg) {
		case WM_CREATE: 
			MainWnd_Create(hWnd, (LPCREATESTRUCT)lParam);
			PostMessage(hWnd, WM_COMMAND, IDM_DISP_EDIT, 0);

			// defaultホットキー(Ctrl+Shift+Q)を設定する
			g_wHotKey = MAKEWORD('Q', HOTKEYF_CONTROL | HOTKEYF_SHIFT);
			SendMessage(hWnd, WM_SETHOTKEY, g_wHotKey , 0);
			ic.dwSize = sizeof(INITCOMMONCONTROLSEX);
			ic.dwICC = ICC_HOTKEY_CLASS;
			InitCommonControlsEx(&ic);

			// クリップボードビューアのチェインに自ウインドウを追加する
			hNextViewr = SetClipboardViewer(hWnd);
			break;

		case WM_SIZE:
			switch (wParam) {
				case SIZE_RESTORED:
					if (g_nIconFlg == 1) {
						// ウィンドウ最小化から戻った場合にクリップボード読込み
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
			// 自ウインドウの次のビューワが削除された場合は
			// その次のビューワを設定する
			if ((HWND)wParam == hNextViewr) {
				hNextViewr = (HWND)lParam;
				return 0; // ↑このメッセージを処理をする時は戻り値に0を返す
			} else {
				// 次のビューワにメッセージを送る
				return SendMessage(hNextViewr, WM_CHANGECBCHAIN, wParam, lParam);
			}

		case WM_DRAWCLIPBOARD:
			if (!IsIconic(hWnd)) {
				// ウィンドウ最小化以外の場合にクリップボード読込み
				PostMessage(hWnd, WM_COMMAND, IDM_DISP_EDIT, 0);
			}

			// 次のビューワにメッセージを送る
			// (送らないと他のアプリケーションで影響が出る)
			if (hNextViewr != 0)
				SendMessage(hNextViewr, WM_DRAWCLIPBOARD, 0, 0);

			break;

		case WM_DESTROY:
			// 自ウィンドウをクリップボードビューアのチェインから除去
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
			// ウィンドウをアクティブにし、元の位置とサイズで表示
			ShowWindow(hWnd, SW_SHOWNORMAL);
			// breakなし
		default:
			return(DefWindowProc(hWnd, uMsg, wParam, lParam)); 
	}
	return 0;
}

LRESULT MainWnd_Create(HWND hWnd, LPCREATESTRUCT cs)
{
	HWND hEdit, hStatus;

	// フォントオブジェクトの作成
	hFont_MainWnd = CreateFont(-15, 0, 0, 0, 400, 0, 0, 0,
							   128, 3, 2, 1, 49, "ＭＳ ゴシック");

	// EditBoxの作成
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

	// ステータスバーの作成
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
		// WindowsのOSバージョン取得
		GetWinVer(&dwMajor, &dwMinor);
	}

	/* クライアント領域のサイズを取得 */
	GetClientRect(hWnd, &rc);
	nOfset = GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION);
	width = rc.right - rc.left + 1;
	height = rc.bottom - rc.top + 1 - nOfset;

	// Windows10の場合のみ補正
	if (dwMajor == 10) {
		height += 10;
	}

	/* EditBoxのウィンドウの大きさを変更する */
	hEdit = GetDlgItem(hWnd, EditBox);
	MoveWindow(hEdit, 0, 0, width, height, 0);

	// ウインドウの更新
	SendMessage(hWnd, WM_ACTIVATE, 0, 0);

	// Statusバーの更新
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

	/* ドロップされたファイル数を取得 */
	nMax = DragQueryFile(hDrop, -1, 0, 0);

	pFile = (char *)malloc(MAX_FULL_PATH + 1);
	if (pFile == NULL) return 0;
	for (i = 0; i < nMax; ++i) {
		/* ドロップされたファイル名を取得 */
		nStat = DragQueryFile(hDrop, i, pFile, MAX_FULL_PATH);
		pFile[nStat] = '\0';

		/* ネットワークドライブ名の展開 */
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

		/* ファイル名を登録する */
		Files.add(pFile);
	}
	free(pFile);
	DragFinish(hDrop);

	// ファイルリストを作成し、画面とClipboardへ設定
	if (nMax == 1) {
		pList = Files.get(0);
	} else {
		// 自然順ソート
		Files.sort(TRUE, TRUE);
		pList = Files.join("\r\n");
	}
	SetEditBox(pList);
	SetClipText(pList);
	if (nMax > 1) delete pList;
	// ステータスバーへ処理内容設定
	DspStatus("DropFiles.");

	// 操作履歴のクリア
	EditHist.Clear();
	// キーシーケンスのクリア
	KeySeq.Clear();
	// ヘルプの戻し
	DspHelp(0);

	// Windowを前面に表示
	SetForegroundWindow(hMainWnd);
	SendMessage(hWnd, WM_ACTIVATE, 0, 0);

	return 0;
}

LRESULT MainWnd_DrawClipBoard(HWND hWnd)
{
	char *pText;

	// クリップボードから文字列を取得
	pText = GetClipText(hWnd);

	// 取得した文字列を画面に表示
	if (pText != NULL) {
		SetEditBox(pText);
		free(pText);
	} else {
		SetEditBox("");
	}

	if (g_nAutoAct == 1) {
		// 最前面ウィンドウのプロセスにアタッチ
		HWND hFgWnd;
		DWORD dwProcId, dwCurId;
		hFgWnd = GetForegroundWindow();
		dwProcId = GetWindowThreadProcessId(hFgWnd, NULL); 
		dwCurId = GetCurrentThreadId();
		AttachThreadInput(dwCurId, dwProcId, TRUE);

		// Windowを前面に表示
		SetForegroundWindow(hMainWnd);
		SendMessage(hMainWnd, WM_ACTIVATE, 0, 0);

		// デタッチ
		//AttachThreadInput(dwCurId, dwProcId, FALSE);
	}

	if (g_nSelfEvnt > 0) {
		--g_nSelfEvnt;
	} else {
		// 操作履歴のクリア
		EditHist.Clear();
		// 括り制御状態の初期化
		g_nCtrlStat = CTRL_NONE;
		// 日付状態の初期化
		InitDateStat();

		DspStatus("");
//*szDebug = 0;
	}
	// キーシーケンスのクリア
	KeySeq.Clear();

	// ヘルプの戻し
	DspHelp(0);

	// 編集モードの戻し
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
			// 'e'キー送出
			PostMessage(hEdit, WM_CHAR, 'e', 0);
			break;
		case IDM_FILE_AUTO:
			// 'a'キー送出
			PostMessage(hEdit, WM_CHAR, 'a', 0);
			break;
		case IDM_FILE_NWDRV:
			// ネットワークドライブ名展開表示切替え
			if (g_nNwDrvNam) {
				g_nNwDrvNam = 0;
			} else {
				g_nNwDrvNam = 1;
			}
			break;
		case IDM_FILE_EXIT:
			if (g_nExcMode != MODE_EDIT) {
				// 'Q'キー送出
				PostMessage(hEdit, WM_CHAR, 'Q', 0);
			} else {
				// ESCキー送出
				PostMessage(hEdit, WM_CHAR, 0x1B, 0);
			}
			break;
		case IDM_EDIT_UNDO:
			// ^Zキー送出
			PostMessage(hEdit, WM_CHAR, 0x1a, 0);
			break;
		case IDM_EDIT_TRIM_SUIT:
			// 't'キー送出
			PostMessage(hEdit, WM_CHAR, 't', 0);
			break;
		case IDM_EDIT_TRIM_BOTH:
			// 'Tb'キー送出
			PostMessage(hEdit, WM_CHAR, 'T', 0);
			PostMessage(hEdit, WM_CHAR, 'b', 0);
			break;
		case IDM_EDIT_TRIM_HEAD:
			// 'Th'キー送出
			PostMessage(hEdit, WM_CHAR, 'T', 0);
			PostMessage(hEdit, WM_CHAR, 'h', 0);
			break;
		case IDM_EDIT_TRIM_TAIL:
			// 'Te'キー送出
			PostMessage(hEdit, WM_CHAR, 'T', 0);
			PostMessage(hEdit, WM_CHAR, 'e', 0);
			break;
		case IDM_EDIT_TRIM_MAIL:
			// 'T>'キー送出
			PostMessage(hEdit, WM_CHAR, 'T', 0);
			PostMessage(hEdit, WM_CHAR, '>', 0);
			break;
		case IDM_EDIT_TRIM_ZEN:
			// 'Tz'キー送出
			PostMessage(hEdit, WM_CHAR, 'T', 0);
			PostMessage(hEdit, WM_CHAR, 'z', 0);
			break;
		case IDM_EDIT_IND_INC:
			// '+'キー送出
			PostMessage(hEdit, WM_CHAR, '+', 0);
			break;
		case IDM_EDIT_IND_DEC:
			// '-'キー送出
			PostMessage(hEdit, WM_CHAR, '-', 0);
			break;
		case IDM_EDIT_JOIN:
			// 'j'キー送出
			PostMessage(hEdit, WM_CHAR, 'j', 0);
			break;
		case IDM_EDIT_HAN_ASC:
			// 'h'キー送出
			PostMessage(hEdit, WM_CHAR, 'h', 0);
			break;
		case IDM_EDIT_HAN_ALL:
			// 'H'キー送出
			PostMessage(hEdit, WM_CHAR, 'H', 0);
			break;
		case IDM_EDIT_ZEN:
			// 'z'キー送出
			PostMessage(hEdit, WM_CHAR, 'z', 0);
			break;
		case IDM_EDIT_UPPER:
			// 'u'キー送出
			PostMessage(hEdit, WM_CHAR, 'u', 0);
			break;
		case IDM_EDIT_LOWER:
			// 'l'キー送出
			PostMessage(hEdit, WM_CHAR, 'l', 0);
			break;
		case IDM_EDIT_ADJ_QUOT:
			// '"'キー送出
			PostMessage(hEdit, WM_CHAR, '\"', 0);
			break;
		case IDM_EDIT_ADJ_DATE:
			// 'd'キー送出
			PostMessage(hEdit, WM_CHAR, 'd', 0);
			break;
		case IDM_EDIT_ADJ_TREE:
			// 'y'キー送出
			PostMessage(hEdit, WM_CHAR, 'y', 0);
			break;
		case IDM_EDIT_ADJ_CRLF:
			// 'Cr'キー送出
			PostMessage(hEdit, WM_CHAR, 'C', 0);
			PostMessage(hEdit, WM_CHAR, 'r', 0);
			break;
		case IDM_EDIT_ADJ_LF:
			// 'Cl'キー送出
			PostMessage(hEdit, WM_CHAR, 'C', 0);
			PostMessage(hEdit, WM_CHAR, 'l', 0);
			break;
		case IDM_EDIT_SUB:
			// 'R'キー送出
			PostMessage(hEdit, WM_CHAR, 'R', 0);
			break;
		case IDM_EDIT_SUB_REP:
			// 'r'キー送出
			PostMessage(hEdit, WM_CHAR, 'r', 0);
			break;
		case IDM_EDIT_FMT:
			// 'F'キー送出
			PostMessage(hEdit, WM_CHAR, 'F', 0);
			break;
		case IDM_EDIT_FMT_REP:
			// 'f'キー送出
			PostMessage(hEdit, WM_CHAR, 'f', 0);
			break;
		case IDM_EDIT_MCR:
			// 'M'キー送出
			PostMessage(hEdit, WM_CHAR, 'M', 0);
			break;
		case IDM_EDIT_MCR_REP:
			// 'm'キー送出
			PostMessage(hEdit, WM_CHAR, 'm', 0);
			break;
		case IDM_EDIT_INS_DATE:
			// 'D'キー送出
			PostMessage(hEdit, WM_CHAR, 'D', 0);
			break;
		case IDM_EDIT_EDIT_MODE:
			// 'i'キー送出
			PostMessage(hEdit, WM_CHAR, 'i', 0);
			break;
		case IDM_EDIT_SORT_ASC_N:
			// 'Sa'キー送出
			PostMessage(hEdit, WM_CHAR, 'S', 0);
			PostMessage(hEdit, WM_CHAR, 'a', 0);
			break;
		case IDM_EDIT_SORT_DSC_N:
			// 'Sd'キー送出
			PostMessage(hEdit, WM_CHAR, 'S', 0);
			PostMessage(hEdit, WM_CHAR, 'd', 0);
			break;
		case IDM_EDIT_SORT_ASC:
			// 'SA'キー送出
			PostMessage(hEdit, WM_CHAR, 'S', 0);
			PostMessage(hEdit, WM_CHAR, 'A', 0);
			break;
		case IDM_EDIT_SORT_DSC:
			// 'SD'キー送出
			PostMessage(hEdit, WM_CHAR, 'S', 0);
			PostMessage(hEdit, WM_CHAR, 'D', 0);
			break;
		case IDM_HELP_HELP:
			hHelp = HtmlHelp(hWnd, HELP_FILE, HH_DISPLAY_TOC, (DWORD)"");
			if (hHelp == NULL) {
				// ヘルプ表示の為、'?'キー送出
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
			// パラメータファイル読込み
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
//■EditBoxのメッセージ処理
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
					case VK_DELETE:	// DELキーの抑制
					case VK_BACK:	// BSキーの抑制
					case VK_RETURN:	// Enterキーの抑制
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
#define LOWER_ALPHA	"a-zａ-ｚ"
#define UPPER_ALPHA	"A-ZＡ-Ｚ"
#define HAN_ASCII1	"a-zA-Z0-9"
#define HAN_ASCII2	"@ !\"\"#$%&'()*+,-./:;<=>?[\\]^_`{|}~"
#define ZEN_ASCII1	"ａ-ｚＡ-Ｚ０-９"
#define ZEN_ASCII2	"＠　！”“＃＄％＆’（）＊＋，－．／：；＜＝＞？［￥］＾＿‘｛｜｝～"
#define ZEN_KANA1	"ガ,ギ,グ,ゲ,ゴ,ザ,ジ,ズ,ゼ,ゾ,ダ,ヂ,ヅ,デ,ド,バ,ビ,ブ,ベ,ボ,パ,ピ,プ,ペ,ポ,ヴ"
#define HAN_KANA2	"｡｢｣､･ｦｧｨｩｪｫｬｭｮｯｰｱｲｳｴｵｶｷｸｹｺｻｼｽｾｿﾀﾁﾂﾃﾄﾅﾆﾇﾈﾉﾊﾋﾌﾍﾎﾏﾐﾑﾒﾓﾔﾕﾖﾗﾘﾙﾚﾛﾜﾝﾞﾟ"
#define HAN_KANA1	"ｶﾞ,ｷﾞ,ｸﾞ,ｹﾞ,ｺﾞ,ｻﾞ,ｼﾞ,ｽﾞ,ｾﾞ,ｿﾞ,ﾀﾞ,ﾁﾞ,ﾂﾞ,ﾃﾞ,ﾄﾞ,ﾊﾞ,ﾋﾞ,ﾌﾞ,ﾍﾞ,ﾎﾞ,ﾊﾟ,ﾋﾟ,ﾌﾟ,ﾍﾟ,ﾎﾟ,ｳﾞ"
#define ZEN_KANA2	"。「」、・ヲァィゥェォャュョッーアイウエオカキクケコサシスセソタチツテトナニヌネノハヒフヘホマミムメモヤユヨラリルレロワン゛゜"

	if (g_nExcMode == MODE_HELP) {
		// ヘルプ内操作
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
		// 編集モード内操作
		switch ((char)KeyCode) {
			case 0x1B: // ESC
				// Clipboardへ編集後の文字列設定
				pText = GetEditBox();
				SetClipText(pText);
				// 操作履歴へ追加
				nMax = EditHist.GetMaxCount();
				if (nMax == 0) {
					EditHist.AddString(pText);
				} else {
					if (strcmp(pText, EditHist.GetString(nMax - 1))) {
						EditHist.AddString(pText);
					}
				}
				// コマンドモードを設定
				g_nExcMode = MODE_CMD;
				DspStatus("[Command Mode]");

				free(pText);
				return 0;
			case 0x03: // Ctrl+C
			case 0x16: // Ctrl+V
			case 0x18: // Ctrl+X
			case 0x1A: // Ctrl+Z
				// 制御キーの抑止
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
			// 自動Activate表示切替
			if (g_nAutoAct == 0) {
				g_nAutoAct = 1;
				pStat = "Auto Activate:On";
			} else {
				g_nAutoAct = 0;
				pStat = "Auto Activate:Off";
			}
			break;

		case 'e':
			// ファイルの実行
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

		case 0x1a: // ^Zキー
			nMax = EditHist.GetMaxCount();
			if (nMax > 1) {
				// 1つ前の編集に戻る
				EditHist.DelLastString();
				--nMax;
				pNow = EditHist.GetString(nMax - 1);
				// EditBox更新
				SetEditBox(pNow);
				// Clipboardへ変換文字列設定
				SetClipText(pNow);
				// 操作結果設定
				sprintf(szMsg, "return[%d].", nMax);
				pStat = szMsg;
			}
			break;

		case 't':
			pConv = TrimLine(pText, " \t>", TRIM_HEAD);
			if (pConv) {
				pConv2 = TrimLine(pConv, " \t　", TRIM_BOTH);
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
						pConv = TrimLine(pText, " \t　", TRIM_BOTH);
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
			// 操作履歴へ追加
			nMax = EditHist.GetMaxCount();
			if (nMax == 0)	EditHist.AddString(pText);
			// 編集モード設定
			g_nExcMode = MODE_EDIT;
			// 日付状態の初期化
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
					//g_ExchHistへFncParam挿入
					nStat = IDCANCEL;
					pArg = FncParam.GetString(0);
					if (pArg) {
						if (strcmp(pArg, "R") == 0) {
							pArg = FncParam.GetString(1);
							SortHist(pArg, &g_ExchHist, MAX_EXCHHIST);
							FncParam.DelString(0);
							FncParam.DelString(0);
							nStat = IDOK;
							--nMacroExc;	// パラメータ処理終了
						}
					}
					if (nStat != IDOK) {
						pStat = "Macro Parameter Error.";
					}
				} else {
					// 文字列置換
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

					// 日付状態の初期化
					InitDateStat();
				} else {
					pStat = "Parameter Error.";
				}
			}
			break;

		case 'F':
			if (nMacroExc) { // マクロ処理
				//g_FmtInfo.FmtHistへFncParam挿入
				nStat = IDCANCEL;
				pArg = FncParam.GetString(0);
				if (pArg) {
					if (strcmp(pArg, "F") == 0) { // 第1カラムが'F'
						pArg = FncParam.GetString(1);
						g_FmtInfo.nFmtId = atoi(pArg);
						switch (g_FmtInfo.nFmtId) { // 第2カラム(種別)で分岐
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
						--nMacroExc;	// パラメータ処理終了
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
				case FMTID_ADJHTML: // HTMLコードの整形
					pConv = AdjHtml(pText);
					if (pConv) {
						pStat = "Format HTML.";
						nConv = 1;
					}
					break;

				case FMTID_COLFMT: // カラムデータ → 書式変換
					pFmt = g_FmtInfo.FmtHist.GetString(0);
					if (pFmt) {
						pConv = CnvColDat(pText, pFmt, pDlmt);
						pStat = "Format Data.";
						nConv = 1;
					}
					break;

				case FMTID_MAKTBL: // カラムデータ → 表変換
					pConv = MakeTable(pText, pDlmt);
					pStat = "Make Table.";
					nConv = 1;
					break;

				case FMTID_GETINC: // 文字列を含む行の抽出
					pFmt = g_FmtInfo.FmtHist.GetString(0);
					if (pFmt) {
						pConv = GrepLine(pText, pFmt, TRUE);
						pStat = "Grep.";
						nConv = 1;
					}
					break;

				case FMTID_GETEXC: // 文字列を含まない行の抽出
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
					if (strcmp(pArg, "D") == 0) { // 第1カラムが'D'
						pArg = FncParam.GetString(1);
						g_DateInfo.nIdx = atoi(pArg);

						g_DateInfo.bMacro = TRUE;
						--nMacroExc;	// パラメータ処理終了
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

					// 操作履歴のクリア
					EditHist.Clear();
					// 日付状態の初期化
					InitDateStat();
				}
			}
			break;

		case 0x1B:	// ESC
			// マクロ実行状態クリア(緊急停止用)
			if (nMacroExc != 0) {
				nMacroExc = 0;
				pStat = "Macro Stopped.";
			}
			if (g_nSelfEvnt > 0) {
				g_nSelfEvnt = 0;
			}
			break;

		case '?':
			// 操作履歴へ追加
			nMax = EditHist.GetMaxCount();
			if (nMax == 0)	EditHist.AddString(pText);
			// ヘルプ表示
			DspHelp(1);
			nActKey = 0;
			break;

		default:
			nActKey = 0;
			break;
	}

	if (nActKey == 1) {
		DspStatus("");
		if (nConv == 1) { // 変換実施
			if (strcmp(pText, pConv)) { // 内容変更あり
				bUpdText = FALSE;
				pNow = GetClipText(NULL);
				if (pNow != NULL) {
					if (strcmp(pText, pNow) == 0) { // Clipboard変更なし
						bUpdText = TRUE;
						free(pNow);
					}
				} else {
					// Clipboard変更なし
					if (*pText == '\0') bUpdText = TRUE;
				}

				if (bUpdText) {
					// EditBox更新
					SetEditBox(pConv);
					// Clipboardへ変換文字列設定
					SetClipText(pConv);
					// ステータスバーへ処理内容設定
					DspStatus(pStat);
					// 操作履歴へ追加
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

// マクロ実行
int ExcMacro(HWND hWnd)
{
	STRLIST LineStr;
	DWORD wParam;
	int nRmax, nCmax, nRidx, nCidx, nStat;
	char *pMcrStr, *pLine, *pRef;
	char szBuf[MAX_BUF];
	
	if (g_McrInfo.Macro.GetMaxCount() > 0) {
		nMacroExc = 1;		// 実行中
		FncParam.Clear();	// パラメータリストクリア
		// マクロを取出し、行毎に分解して処理
		pMcrStr = g_McrInfo.Macro.GetString(0);
		StrSplit(pMcrStr, "\r\n", SPLIT_NONE | SPLIT_MULTI, &LineStr);
		nRmax = LineStr.GetMaxCount();
		nStat = TRUE;
		for (nRidx = 0; nRidx < nRmax && nStat == TRUE; ++nRidx) {
			// マクロ行の取り出し
			pLine = LineStr.GetString(nRidx);
			switch (*pLine) {
				case '#':
					// 読み飛ばし(コメント行)
					break;
				case 'R':
					nStat = AddFncParam(pLine, &FncParam, "S:R,P");
					if (nStat == TRUE) {
						++nMacroExc;	// パラメータ処理表示
						PostMessage(hWnd, WM_CHAR, 'R', 0);
					}
					break;
				case 'F':
					nStat = AddFncParam(pLine, &FncParam, "S:F,N:0-4,O,N:0-1");
					if (nStat == TRUE) {
						++nMacroExc;	// パラメータ処理表示
						PostMessage(hWnd, WM_CHAR, 'F', 0);
					}
					break;
				case 'D':
					nStat = AddFncParam(pLine, &FncParam, "S:D,N:0-2");
					if (nStat == TRUE) {
						++nMacroExc;	// パラメータ処理表示
						PostMessage(hWnd, WM_CHAR, 'D', 0);
					}
					break;
				default:
					// 1行内の文字をEditBoxへ送信
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
				// エラー通知方法を要検討
				sprintf(szBuf, "マクロの%d行目でエラーが発生しました。\n(%s)",
				        nRidx + 1, pLine);
				MsgBox(hMainWnd, szBuf, "Error", MB_OK);
			}
		}
		--nMacroExc;	// 停止
	}

	return nStat;
}

// 文字列を分解し、パラメータリストに追加
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

// マクロパラメータのチェック
int ChkMcrParam(char *pParam, char *pCheck)
{
/* Check書式 … type:value
   type      │value
   ─────┼───
   O(なし)   │
   N(数値)   │m-n
   C(1文字)  │A-Z
   S(文字列) │xxx
   P(先頭/末尾文字一致)
*/
	STRLIST ChkArg;
	int nStat = FALSE;
	int nMin, nMax, nNum, nCnt, nLen;
	char *pMin, *pMax;

	if (strlen(pCheck) == 1) {
		if (*pCheck == 'O') {
			nStat = TRUE;
		} else if (*pCheck == 'P') {
			// 先頭と末尾の1文字一致チェック
			nLen = strlen(pParam);
			if (*pParam == *(pParam + nLen - 1)) nStat = TRUE;
		}
	} else {
		if (*(pCheck + 1) == ':') {
			if (*pCheck == 'N') {
				// 数値チェック
				if (IsNumStr(pParam)) {
					StrSplit(pCheck, "-", SPLIT_NONE, &ChkArg);
					nMin = atoi(ChkArg.GetString(0));
					nMax = atoi(ChkArg.GetString(1));
					nNum = atoi(pParam);
					if (nMin <= nNum && nNum <= nMax) nStat = TRUE;
				}
			} else if (*pCheck == 'C') {
				// 1文字チェック
				if (strlen(pParam) == 1 && strlen(pCheck) == 3) {
					StrSplit(pCheck, "-", SPLIT_NONE, &ChkArg);
					pMin = ChkArg.GetString(0);
					pMax = ChkArg.GetString(1);
					if (*pMin <= *pParam && *pParam <= *pMax) nStat = TRUE;
				}
			} else if (*pCheck == 'S') {
				// 文字列チェック
				if (strcmp(pParam, pCheck + 2) == 0) nStat = TRUE;
			}
		}
	}

	return nStat;
}

//----------------------------------------------------------------------
//■共通処理
//----------------------------------------------------------------------
// ステータス表示
void DspStatus(char *pStr)
{
	HWND hStatus;
	char szBuf[MAX_BUF+1];

	sprintf(szBuf, "%s ", pStr);
	hStatus = GetDlgItem(hMainWnd, StatusBar);
	SendMessage(hStatus, SB_SETTEXT, 255 | 0, (WPARAM)szBuf);
	SendMessage(hStatus, WM_SIZE, 0, 0);
}

// EditBoxの文字列取得
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

// EditBoxへ文字列設定
void SetEditBox(char *pText)
{
	HWND hEdit;

	hEdit = GetDlgItem(hMainWnd, EditBox);
	SetWindowText(hEdit, pText);
	UpdateWindow(hEdit);
	SetFocus(hEdit);
}

// クリップボードからのテキストデータ取得
char *GetClipText(HWND hWnd)
{
	HANDLE hClipData = NULL;
	char *pText, *pCopy;
	int nFormat, bRet;
	char szBuf[MAX_BUF+1];

	// クリップボード内のテキスト確認
	bRet = IsClipboardFormatAvailable(CF_TEXT);
	if (bRet) {
		// クリップボードのオープン
		bRet = OpenClipboard(hWnd);
		if (bRet) {
			// クリップボードデータの取得
			hClipData = GetClipboardData(CF_TEXT);

			// クリップボードのテキストをコピー(データのハンドルは解放しない)
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

// クリップボードへテキストデータ設定
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

// 文字列置換
//   pOrg	:変換前テキスト
//   nFreFlg:変換前テキストのメモリ解放(0:解放しない、1:解放する)
//   pSrch	:検索データ
//   cStype	:検索データ型(a:全体、1:1文字形式、h:From-To形式、c:CSV形式)
//   pConv	:変換データ
//   cCtype	:変換データ型(a:全体、1:1文字形式、h:From-To形式、c:CSV形式)
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
#define MCH_NONE	0	// 一致なし
#define MCH_VALUE	1	// 一致(値)
#define MCH_FROMTO	2	// 一致(From-To)

	// 出力バッファ初期化
	pOutStr = OutBuf.AddEdit("", 0);

	// パラメータ設定
	OrigStr.SetData(pOrg, '1');
	SrchStr.SetData(pSrch, cStype);
	ConvStr.SetData(pConv, cCtype);

	// 設定する変換文字を格納するメモリ領域確保(コピーした値は上書きで使用)
	pConvBuf = strdup(ConvStr.GetCurPtr());

	// パラメータの参照位置取得
	pOrigPtr = OrigStr.GetCurPtr();
	pSrchPtr = SrchStr.GetCurPtr();
	nSrchLen = SrchStr.GetCurLen();
	while (*pOrigPtr) {
		nMchId = MCH_NONE;
		while (*pSrchPtr) {
			// 対象文字列と検索文字の一致を判定
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

			// 次の検索文字に更新
			SrchStr.MovNextPtr();
			pSrchPtr = SrchStr.GetCurPtr();
			nSrchLen = SrchStr.GetCurLen();
		}

		if (nMchId == MCH_VALUE) {
			// 検索文字と一致したら変換文字を出力
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
			// From-To形式の検索文字と一致したら変換文字を出力
			nIdx = SrchStr.GetCurIdx();
			ConvStr.GetRangeStr(nIdx, nOfset, pConvBuf);
			pOutStr = OutBuf.AddEdit(pConvBuf, 0);
			if (nSrchLen > 2) nSrchLen /= 2;
			OrigStr.UpdCurPtr(nSrchLen);
		} else {
			// 一致しなければ対象文字を出力
			nOrigLen = OrigStr.GetCurLen();
			pOutStr = OutBuf.AddEdit(pOrigPtr, nOrigLen);
			OrigStr.MovNextPtr();
		}
		// 検索文字を先頭に戻す
		SrchStr.MovTopPtr();

		pOrigPtr = OrigStr.GetCurPtr();
		pSrchPtr = SrchStr.GetCurPtr();
		nSrchLen = SrchStr.GetCurLen();
	}

	// メモリ解放
	free(pConvBuf);
	if (nFreFlg == 1) free(pOrg);

	return pOutStr;
}

// 余分なスペース/タブの削除
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

	// 変換する文字列の最後が改行で無い場合の対処
	if (nType == TRIM_HEAD) {
		pConv = Conv.AddEdit(pSave, 0);
		pSave = Save.Clear();
	}

	if (pSave != NULL) free(pSave);

	return pConv;
}

// インデントの調整
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

// 括りの調整
char *AdjQuote(char *pStr)
{
	char *pRet = NULL;
	char *pFind, *pConv, *pLast;
	int nIdx, nNext, nLen;
	char *HEAD_QUOTE = "\"(<[「【（＜“";
	char *TAIL_QUOTE = "\")>]」】）＞”";

	pFind = StrChr(HEAD_QUOTE, pStr);
	if (pFind) { // 行頭は括りの開始？
		// 行末の不要文字削除
		pConv = TrimLine(pStr, "\t\r\n", TRIM_TAIL);

		nIdx = (int)(pFind - HEAD_QUOTE);
		pLast = GetLastChar(pConv);
		nLen = strlen(pLast);
		if (!memcmp(pLast, TAIL_QUOTE + nIdx, nLen)) {	// 行末は括りの終了？
			// 行頭/行末の括り削除
			DelHeadTailChar(pConv);

			if (g_nCtrlStat == CTRL_DEL) { // 削除状態
				nNext = nIdx + (IsKanji(*pFind) ? 2 : 1);
				if (*(HEAD_QUOTE + nNext)) {
					// 次の文字で括る
					pRet = AddHeadTailChar(pConv, HEAD_QUOTE + nNext,
					                              TAIL_QUOTE + nNext);
					free(pConv);
				} else {
					pRet = pConv;
				}
			} else {
				pRet = pConv;
				// 削除状態設定
				g_nCtrlStat = CTRL_DEL;
			}
		} else {
			// ""で括る
			pRet = AddHeadTailChar(pConv, "\"", "\"");
			// 削除状態設定
			g_nCtrlStat = CTRL_DEL;

			free(pConv);
		}
	} else {
		// ""で括る
		pRet = AddHeadTailChar(pStr, "\"", "\"");
		// 削除状態設定
		g_nCtrlStat = CTRL_DEL;
	}

	return pRet;
}

// 行頭/行末に文字の付与
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

// 行頭/行末の文字削除
void DelHeadTailChar(char *pStr)
{
	int nLen;
	char *pLast;

	// 行頭削除
	nLen = (IsKanji(*pStr) ? 2 : 1); 
	strcpy(pStr, pStr + nLen);

	// 行末削除
	pLast = GetLastChar(pStr);
	*pLast = '\0';
}

// 最後の1文字取得
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

// ファイル名からツリー表示への変換
char *CnvTree(char *pStr)
{
	int nLayer, nMargin, nDepth;
	char *pFile, *pTree;
	TREEINFO Tree;

	// ツリー表示からファイル名へ変換
	pFile = Tree.Tree2File(pStr, &nLayer, &nMargin);

	// ファイル情報の登録
	Tree.RegFile(pFile);
	free(pFile);

	// ファイル情報からツリー表示への変換
	nDepth = -1 * nLayer;
	pTree = Tree.GetTree(nDepth, nMargin);

	return pTree;
}

// ファイルの実行
int ExecFile(char *pFile)
{
	HWND hEdit;
	int nStat, nLen, nLidx, nMax;
	char exec[MAX_PATH+1], *pParam, *pLine;
	STRLIST ClpLine;

	// カーソル位置の行取得
	hEdit = GetDlgItem(hMainWnd, EditBox);
	nLidx = GetCurLine(hEdit);
	StrSplit(pFile, "\r\n", SPLIT_NONE | SPLIT_SINGLE, &ClpLine);
	nMax = ClpLine.GetMaxCount();
	if (nLidx < nMax) {
		pLine = ClpLine.GetString(nLidx);
	} else {
		return 0;
	}

	// ファイル名をダブルクォートで括る
	nLen = strlen(pLine);
	pParam = (char *)malloc(nLen + 3);
	sprintf(pParam, "\"%s\"", pLine);

	/* ファイル属性取得 */
	nStat = GetFileAttributes(pLine);
	if (nStat != -1 && (nStat & FILE_ATTRIBUTE_DIRECTORY)) {
		/* ディレクトリの場合、Explorer起動 */
		nStat = (int)ShellExecute(NULL, "explore", pParam,
		                          NULL, NULL, SW_SHOWNORMAL);
	} else {
		/* 関連付けられた実行ファイルがあれば起動 */
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

// TexBox内カーソルの行位置取得
int GetCurLine(HWND hEdit)
{
	DWORD wParam, lParam;
	int nCurPos, nCsrPos = 0, nBytePos = 0, nLine = 0;
	char *pText, ch;

	// TexBox内カーソル位置取得
	SendMessage(hEdit, EM_GETSEL, (DWORD)&wParam, (DWORD)&lParam);
	nCurPos = wParam;

	pText = GetEditBox();
	if (pText) {
		ch = pText[nBytePos];
		while (ch != 0 && nBytePos < nCurPos) {
			// 参照文字位置更新
			if (IsKanji(ch)) {
				nBytePos += 2;
			} else {
				nBytePos++;
			}
			if (ch != 0x0d) nCsrPos++;

			// 行位置更新
			if (ch == 0x0a) ++nLine;

			ch = pText[nBytePos];
		}

		free(pText);
	}

	return nLine;
}

// 行単位のワイルドカード置換
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

	// 検索/置換文字の分解
	if (*pParam) {
		// デリミタ設定
		szDlmt[0] = *pParam;
		szDlmt[1] = '\0';
		// 制御文字変換
		pValue = Replace(pParam + 1, 0, CTRL_CHAR, 'c', CTRL_DATA, 'c');
		// デリミタで分解
		StrSplit(pValue, szDlmt, SPLIT_NONE | SPLIT_SINGLE, &InParam);
		nCnt = InParam.GetMaxCount();

		free(pValue);
		if (nCnt > 1) nStat = 0;
	}

	if (nStat < 0) {
		*lpStat = 0;
	} else {
		pOrg = pStr;
		// 入力された検索/置換の組合せ毎に変換
		for (Ridx = 0; (Ridx + 1) < nCnt; Ridx += 2) {
			// 改行で分解
			StrSplit(pOrg, "\r\n", SPLIT_SEP | SPLIT_MULTI, &OrgLine);

			// 検索/置換パラメータ取得
			pSrch = InParam.GetString(Ridx);
			pConv = InParam.GetString(Ridx + 1);

			// 検索パラメータの単独改行判定
			if (strcmp(pSrch, "\r\n") == 0) {
				bCrLf = TRUE;
			} else {
				bCrLf = FALSE;
			}

			// 行毎に置換
			for (Lidx = 0; Lidx < OrgLine.GetMaxCount(); ++Lidx) {
				pOrgLine = OrgLine.GetString(Lidx);

				pDstLine = WildReplace(pOrgLine, pSrch, pConv);
				if (bCrLf == TRUE) {
					// 単独改行の特殊処理
					pOrgLine = pDstLine;
					pDstLine = OnlyLfReplace(pOrgLine, pConv);
					free(pOrgLine);
				}
				DstLine.AddString(pDstLine);
				free(pDstLine);
			}

			// 行毎の置換をマージ
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

// パラメータファイルの取込み
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
	
	/* パラメータファイル名の取得 */
	nMax = CountChar(pExcFile, '\\');
	nIdx = SrchCharPos(pExcFile, '\\', nMax);
	if (*pExcFile == '"') {
		nStt = 1;
	} else {
		nStt = 0;
	}
	memcpy(ParamFile, pExcFile + nStt, nIdx - nStt + 1);
	strcpy(ParamFile + nIdx - nStt + 1, PARAM_FILE);
	/* パラメータファイル読込み */
	fp = fopen(ParamFile, "r");
	if (fp == NULL)	return;
	while (fgets(szBuf, 4*MAX_BUF, fp) != NULL) {
		/* 改行削除*/
		SubStr(szBuf, "\n", "", 0);

		if (nPid == ID_NONE) {
			/* カテゴリの判定 */
			if (!strcmp(szBuf, "[Window]")) {
				nPid = ID_WINDOW;
				nPosFlg = 1;
			} else if (!strcmp(szBuf, "[Option]")) {
				nPid = ID_OPTION;
			} else if (!strcmp(szBuf, "[Macros]")) {
				nPid = ID_MACROS;
			}
		} else if (nPid == ID_WINDOW) {
			/* ウィンドウ位置/サイズ取得 */
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
			/* オプション取得 */
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
			/* マクロ取得 */
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

	// ウィンドウ位置/サイズの変更
	if (nPosFlg == 1) {
		SetWindowPos(hMainWnd, HWND_TOP, sx, sy, cx, cy, 0);
	}

	return;
}

// パラメータ文字の判定
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

// パラメータファイルへの保存
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

// WindowsのOSバージョン取得
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

// バージョン情報の取得
void GetVerInfo(char *pFmt, char *pOutStr)
{
	sprintf(pOutStr, pFmt, VERSION, DATE);
}

// ヘルプの表示
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
		"【Keyコマンド】",
		"  a    : ClipBoard変更時の自動Activate切替",
		"  e    : ファイルとして実行",
		"  ^Z   : 編集を戻す",
		" -------------------------------------------------------------------",
		"  t    : 行頭の空白(全角含む)/タブ/'>'+行末の空白(全角含む)/タブ削除",
		"  Tb   : 行頭/行末の空白/タブ削除",
		"  Tz   : 行頭/行末の空白(全角含む)/タブ削除",
		"  Th   : 行頭の空白/タブ削除",
		"  T>   : 行頭の空白/タブ/'>'削除",
		"  Te   : 行末の空白/タブ削除",
		" -------------------------------------------------------------------",
		"  +    : 行頭の空白インデント増加",
		"  -    : 行頭の空白インデント減少",
		" -------------------------------------------------------------------",
		"  z    : 全角変換",
		"  H    : 半角変換(カナ含む)",
		"  h    : 半角変換(カナ除く)",
		"  u    : 大文字変換",
		"  l    : 小文字変換",
		" -------------------------------------------------------------------",
		"  j    : 行の結合",
		"  i    : 編集開始(ESCキーで編集終了)",
		"  \"    : 括り文字の変更 … \"\",(),<>,[],「」,【】,（）,＜＞,“”",
		"  d    : 日付文字の変更 … m/d, yyyy/m/d, m月d日, yyyy年m月d日, ...",
		"  y    : ファイル名リスト→ツリー表示の変更",
		"  Cr   : 改行コード変換(LF→CRLF)",
		"  Cl   : 改行コード変換(CRLF→LF)",
		" -------------------------------------------------------------------",
		"  Sa   : 昇順ソート(自然順)",
		"  Sd   : 降順ソート(自然順)",
		"  SA   : 昇順ソート(文字コード順)",
		"  SD   : 降順ソート(文字コード順)",
		" -------------------------------------------------------------------",
		"  R    : 置換(検索/置換文字入力)",
		"  r    : 置換(繰返し)",
		"  F    : 書式変換(変換種別/パラメータ入力)",
		"  f    : 書式変換(繰返し)",
		"  D    : 日時挿入(設定形式/年月日/時分選択)",
		"  M    : マクロ実行(新規/取込/編集/削除/実行)",
		"  m    : マクロ実行(繰返し)",
		" -------------------------------------------------------------------",
		"  Space: 1ページ下にスクロール",
		"  b    : 1ページ上にスクロール",
		"  >    : 1画面右にスクロール",
		"  <    : 1画面左にスクロール",
		NULL};

	if (nAct == 1 && g_nExcMode != MODE_HELP) {
		// ヘルプをEditBoxへ設定
		for (i = 0; pHelp[i]; ++i) {
			HelpStr.AddString(pHelp[i]);
		}
		pText = HelpStr.MergeAllStr("\r\n");
		SetEditBox(pText);
		free(pText);

		// Window位置調整
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

		// ヘルプ表示中表示On
		g_nExcMode = MODE_HELP;
	} else if (nAct == 0 && g_nExcMode == MODE_HELP) {
		// 操作履歴からEditBoxを元に戻す
		nMax = EditHist.GetMaxCount();
		if (nMax > 0) {
			pNow = EditHist.GetString(nMax - 1);
			SetEditBox(pNow);
		}

		// Window位置調整
		sx = OrgWin.left;
		sy = OrgWin.top;
		cx = OrgWin.right - OrgWin.left;
		cy = OrgWin.bottom - OrgWin.top;
		SetWindowPos(hMainWnd, HWND_TOP, sx, sy, cx, cy, 0);
		
		// ヘルプ表示中表示Off
		g_nExcMode = MODE_CMD;
	}
}
