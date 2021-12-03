/*
 * 文字列操作に関する処理の定義
 */

#ifndef OPESTRING_INCLUDED
#define OPESTRING_INCLUDED

#include "TableInfo.h"

#define SPLIT_SINGLE	0	// [デリミタ]1固まりのデリミタで分解
#define SPLIT_MULTI		1	// [デリミタ]連続したデリミタで分解
#define SPLIT_NONE		2	// [分解方法]文字列のみに分解
#define SPLIT_WITH		4	// [分解方法]デリミタ付き文字列に分解
#define SPLIT_SEP		8	// [分解方法]文字列とデリミタに分解
#define SPLIT_EXCEL		16	// [EXCEL形式]"～"で囲まれたデータの有効化

int IsKanji(unsigned char c);
int IsNumStr(char *p);
char *StrPbrk(char *s1, char *s2);
char *StrChr(char *s, char *c);
char *StrStr(char *pOrg, char *pSrch);
int GetElement(char *pOrg, int nIdx, char **pArg);
int CountChar(char *p, char c);
int SrchCharPos(char *p, char c, int idx);
int SubStr(char *pStr, char *pSrch, char *pRep, int nCnt);
char *TailStr(char *pStr, int nCnt);
int CmpStr(unsigned char *s1, unsigned char *s2);
int WildSrch(char *pOrgStr, char *pWldStr, int *lpStart, int *lpLength);
int StrSplit(char *pStr, char *pDlmt, int nFmt, STRLIST *pList);
char *WildReplace(char *pStr, char *pSrch, char *pRep);
char *OnlyLfReplace(char *pStr, char *pRep);

#endif
