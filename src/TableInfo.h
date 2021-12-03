/*
 * TABLEINFOクラスの定義
 */

#ifndef TABLEINFO_INCLUDED
#define TABLEINFO_INCLUDED

/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
*/

/* ソートキー */
enum SORT_ID {
	SORT_ASC_KEYPTR1,
	SORT_DSC_KEYPTR1,
	SORT_ASC_KEYVAL2,
	SORT_DSC_KEYVAL2
};

class TABLEINFO {
#define TRUE	1
#define FALSE	0
#define NULL	0
	int m_nNextPos;	// 未使用データ領域の開始位置
	int m_nMaxCnt;	// 確保データ領域の数
	int m_nSize;	// 1データのサイズ
	char *m_pData;	// データ領域の先頭ポインタ
	int m_nSortId;	// ソートキー

	int QuickSort(char *pData, int cbSz, int nTop, int nBottom);
	int CompData(char *pData1, char *pData2);

public:
	TABLEINFO();			// コンストラクタ
	TABLEINFO(int cbSz);	// コンストラクタ
	~TABLEINFO();			// デストラクタ
	void *GetDataPtr(int nPos, int bExpand);	//テーブルのポインタ取得
	int Count();			// データ数の取得
	void ClearCount(); //データ数のクリア
	void Sort(enum SORT_ID Id);	// テーブルのソート
	void DelLastData();		// 最終データの削除
	int MoveData(int nOrgIdx, int nDstIdx);		// データの移動
	void debug();
};

class STRLIST {
	int nCurIdx;
	int nMaxCnt;
	TABLEINFO *pStrMng;

public:
	STRLIST();
	~STRLIST();
	void Clear();
	void AddString(char *pStr);
	void AddString(char *pString, int nLen);
	int InsString(int nIdx, char *pString);
	int DelString(int nIdx);
	char *GetString(int nIdx);
	int SrchString(char *pSrch);
	int STRLIST::MoveString(int nOrgIdx, int nDstIdx);
	char *MergeAllStr(char *pSep);
	char *GetCurrentString();
	int GetCurrentIndex();
	int GetMaxCount();
	int MoveNextIndex();
	int MovePrevIndex();
	void DelLastString();
	void Sort(enum SORT_ID Id);
	char *debug();
};

#endif
