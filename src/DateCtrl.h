/*
 * 日付に関する処理の定義
 */

#ifndef DATECTRL_INCLUDED
#define DATECTRL_INCLUDED

// 最大値
#define MAX_PTN		20

// 日付の状態に関する情報
typedef struct {
	int year;
	int month;
	int day;
	int wk;
	int nCurIdx;		// 現在の表示位置
	int nId[MAX_PTN];	// 表示順
} DATESTAT;

// 関数の定義
char *AdjDate(char *pText);
void InitDateStat();
int CmpDateStat(DATESTAT *pStat1, DATESTAT *pStat2);
void CpyDateStat(DATESTAT *pStat1, DATESTAT *pStat2);
void NxtDateStat(DATESTAT *pStat);
int ChkDatePtn(char *pDate, int *pnMchStt, int *pnMchEnd);
void MakChkPtn(int nId, char *pPtn);
void AnlDate(int nId, char *pDate, DATESTAT *pDateStat);
int DelChar(char *pOrg, char *pDst, char cDel);
int SrchCnvDat(char **pCnv, char *pStr);
int SrchPtnId(int nId);
int AdjYearWeek(int *year, int *month, int *day, int *wk);
int GetWeekDay(int y, int m, int d);
void SetDspPtn(int nId, DATESTAT *pDateStat);
char *MakDspDate(DATESTAT *pDateStat);

#endif
