/*
 * ���t�Ɋւ��鏈���̒�`
 */

#ifndef DATECTRL_INCLUDED
#define DATECTRL_INCLUDED

// �ő�l
#define MAX_PTN		20

// ���t�̏�ԂɊւ�����
typedef struct {
	int year;
	int month;
	int day;
	int wk;
	int nCurIdx;		// ���݂̕\���ʒu
	int nId[MAX_PTN];	// �\����
} DATESTAT;

// �֐��̒�`
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
