/*
 * パス名からツリー表示を生成する処理の定義
 */

#ifndef TREEINFO_INCLUDED
#define TREEINFO_INCLUDED

#include <windows.h>
#include "OpeString.h"

typedef struct s_TreeMng {
	int nLayer;					// 階層
	char *pName;				// 項目名
	struct s_TreeMng *pNext;	// 次のデータ
	struct s_TreeMng *pChild;	// 一つ下の階層データ
	struct s_TreeMng *pParent;	// 一つ上の階層データ
} TREEMNG;

class TREEINFO {
	TREEMNG *m_pTop;	// データの先頭
	int m_nMaxDepth;	// 階層の最大数

public:
	TREEINFO();					// コンストラクタ
	~TREEINFO();				// デストラクタ
	void FreeData(TREEMNG *);	// 管理データの解放
	int RegFile(char *);		// ファイル情報の登録
	int MaxDepth();				// 最大階層数
	TREEMNG *RegData(TREEMNG *, TREEMNG *, char *, int);	// 管理データの登録
	char *GetTree(int, int);								// ツリー文字列の取得
	char *MakTree(TREEMNG *, char *, char *, int, int);		// ツリー表示作成
	char *TREEINFO::MakPath(TREEMNG *);						// 上位のパス名作成
	char *CatStr(char *, ...);// 引数の文字列結合(p1…p2以降の引数の解放指定(0:解放なし/1:解放あり)、文字列長=p2以降の引数の数)
	int SrchName(char *);					// ツリー表示からフォルダ/ファイル名の位置を検索
	char *Name2Path(STRLIST *, int);		// フォルダ/ファイル名からパス名を生成
	char *Tree2File(char *, int *, int *);	// ツリー表示→ファイル名変換
};

#endif
