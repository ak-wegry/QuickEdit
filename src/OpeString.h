/*
 * �����񑀍�Ɋւ��鏈���̒�`
 */

#ifndef OPESTRING_INCLUDED
#define OPESTRING_INCLUDED

#include "TableInfo.h"

#define SPLIT_SINGLE	0	// [�f���~�^]1�ł܂�̃f���~�^�ŕ���
#define SPLIT_MULTI		1	// [�f���~�^]�A�������f���~�^�ŕ���
#define SPLIT_NONE		2	// [������@]������݂̂ɕ���
#define SPLIT_WITH		4	// [������@]�f���~�^�t��������ɕ���
#define SPLIT_SEP		8	// [������@]������ƃf���~�^�ɕ���
#define SPLIT_EXCEL		16	// [EXCEL�`��]"�`"�ň͂܂ꂽ�f�[�^�̗L����

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
