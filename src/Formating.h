/*
 * 書式の整形に関する処理の定義
 */

#ifndef FORMATING_INCLUDED
#define FORMATING_INCLUDED

// データ型定義
typedef struct {
	int nMax;		// カラム最大長
	char *pStrFmt;	// 文字列用カラム表示フォーマット
	char *pNumFmt;	// 数値用カラム表示フォーマット
} MngColFmt;

typedef struct {
	char *pStr;	// カラム文字列
	int nLin;	// 行番号
	int nCol;	// カラム番号
} MngColDat;

// 関数の定義
char *AdjHtml(char *pStr);
char *MakeTable(char *pText, char *pTokDlmt);
char *CnvColDat(char *pOrg, char *pFmt, char *pDlmt);
char *GrepLine(char *pOrg, char *pSrch, int bInc);

#endif
