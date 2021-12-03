/*
 * DialogBoxÇ…ä÷Ç∑ÇÈèàóù
 */

#ifndef DIALOGBOX_INCLUDED
#define DIALOGBOX_INCLUDED

BOOL CALLBACK EventCall_MsgWnd(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
BOOL MsgWnd_Init(HWND hWnd);
BOOL MsgWnd_Command(HWND hWnd, WORD wNotifyCode, WORD wID, HWND hWndCtl);
BOOL CALLBACK EventCall_InputWnd(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
BOOL InputWnd_Init(HWND hWnd);
BOOL InputWnd_Command(HWND hWnd, WORD wNotifyCode, WORD wID, HWND hWndCtl);
BOOL CALLBACK EventCall_HotkeyWnd(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL HotkeyWnd_Init(HWND hDlg);
BOOL HotkeyWnd_Command(HWND hDlg, WORD wNotifyCode, WORD wID, HWND hWndCtl);
BOOL CALLBACK EventCall_FmtWnd(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
BOOL FmtWnd_Init(HWND hWnd);
BOOL FmtWnd_Command(HWND hWnd, WORD wNotifyCode, WORD wID, HWND hWndCtl);
void FmtWnd_SelChange(HWND hWnd);
BOOL CALLBACK EventCall_McrWnd(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
BOOL McrWnd_Init(HWND hWnd);
BOOL McrWnd_Command(HWND hWnd, WORD wNotifyCode, WORD wID, HWND hWndCtl);
void McrWnd_SetCtrl(HWND hWnd, int bNew, int bImp, int bEdt, int bDel, int bReg, int bExc);
void SetButton(HWND hWnd, int Id, bool bEnable, bool bDefBtn);
void SetEditBox(HWND hWnd, int Id, bool bEdit);
void SetComboBox(HWND hWnd, int Id, bool bEdit);
void SetMcrData(HWND hWnd, int nIdx);
int AddMcrInfo(char *pName, char *pMacro, int nCurIdx);
int GetMcrFromCb(char **pName, char **pMacro, int bNew);
BOOL CALLBACK EventCall_DateWnd(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL DateWnd_Init(HWND hWnd, LPARAM lParam);
BOOL DateWnd_Command(HWND hWnd, WORD wNotifyCode, WORD wID, HWND hWndCtl);
void DateWnd_SetCtrl(HWND hWnd);
void DateWnd_MoveCursor(HWND hWnd, int nWay);
int DateWnd_GetCsrPos(HWND hWnd);
void DateWnd_UpdDate(int nDiff);
void DateWnd_UpdTime(HWND hWnd, int nDiff);
int DateWnd_GetHalfHour(char *pTime);
int DateWnd_AddHalfHour(int nOrg, int nAdd);
void DateWnd_Half2Str(int nHalf, char *pTime);
LRESULT CALLBACK EventCall_DateDlg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK EventCall_StimeDlg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK EventCall_EtimeDlg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int MsgBox(HWND hWnd, char *pMsg, char *pTitle, int nType);
BOOL CALLBACK EventCall_MsgBox(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL MsgBox_Init(HWND hWnd);
int MsgBox_ChgWinRel(HWND hWnd, int dx, int dy, int dw, int dh);
HWND MsgBox_GetDispSize(HWND hWnd, int nId, int *pCurWd, int *pCurHt, int *pNedWd, int *pNedHt, int *pFntHt);
BOOL MsgBox_Command(HWND hWnd, WORD wNotifyCode, WORD wID, HWND hWndCtl);
void GetDispLoc(HWND hWnd, int *lpX, int *lpY);
void SortHist(char *pInput, STRLIST *pHist, int nHstMax);
char *GetDlgText(HWND hWnd, int nDlgId);
void ChgWinStyle(HWND hWnd, int nValue, int nMask);
void GetTime(char *pCurTime);

#endif
