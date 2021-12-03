/*
 * クリップボード編集ツール
 */

#include "TableInfo.h"

// 固定値 
#define MAX_EXCHHIST	10	// 置換条件履歴の最大数
#define MAX_FMTHIST		10	// 書式履歴の最大数

// Trimの実行タイプ
#define TRIM_HEAD	1	// 行頭
#define TRIM_TAIL	2	// 行末
#define TRIM_BOTH	3	// 行頭/行末

// モード
#define MODE_CMD	0	// コマンドモード
#define MODE_EDIT	1	// 編集モード
#define MODE_HELP	2	// ヘルプ表示モード

// 括り制御の状態
#define CTRL_NONE	0	// 未実行状態
#define CTRL_DEL	1	// 削除状態

// データ型定義
typedef struct {
	char *pSrch;
	char cStype;
	char *pConv;
	char cCtype;
} ConvParam;

typedef struct {
#define FMTID_ADJHTML	0	// HTMLコードの整形
#define FMTID_COLFMT	1	// カラムデータ → 書式変換
#define FMTID_MAKTBL	2	// カラムデータ → 表変換
#define FMTID_GETINC	3	// 文字列を含む行の抽出
#define FMTID_GETEXC	4	// 文字列を含まない行の抽出
	int nFmtId;			// フォーマットID
	STRLIST FmtHist;	// 書式変換パラメータの履歴
#define DLMTID_TAB	0	// Tab
#define DLMTID_CSV	1	// CSV
	int nDlmtId;		// デリミタID
} FormatParam;	// 書式変換パラメータ

typedef struct {
	STRLIST Name;	// マクロ名称
	STRLIST Macro;	// マクロ内容
} MacroParam;	// マクロ実行パラメータ

typedef struct {
	HWND hDate;			// 日付ダイアログのハンドル
	HWND hStime;		// 開始時間ダイアログのハンドル
	HWND hEtime;		// 終了時間ダイアログのハンドル
	HWND hCombo;		// 設定書式ダイアログのハンドル
#define DTFMT_FROMTO	0	// 年月日+開始時刻～終了時刻
#define DTFMT_DATETIME	1	// 年月日+時刻
#define DTFMT_DATE		2	// 年月日
	int bMacro;			// マクロ実行
	int nIdx;			// 設定書式ダイアログのカーソル位置
	char *pDateTime;	// 出力文字列
} DateWndParam;	// 日時設定パラメータ

// グローバル変数定義
extern HWND hMainWnd; 
extern STRLIST g_ExchHist;		// 置換条件履歴
extern WORD g_wHotKey;			// ホットキー
extern FormatParam g_FmtInfo;	// フォーマット情報
extern MacroParam g_McrInfo;	// マクロ情報
extern DateWndParam g_DateInfo;	// 日時設定情報

// 関数定義
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
