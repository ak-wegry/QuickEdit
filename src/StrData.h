/*
 * StrDataクラスの定義
 */

#ifndef STRDATA_INCLUDED
#define STRDATA_INCLUDED

#include <stdio.h>
#include <windows.h>

class StrData {
	char *m_pStr;	// 文字列データ
	char m_cType;	// データタイプ(a:全文字、1:1文字、h:From-To、c:CSV)
	int m_nCurPt;	// 文字列データの参照位置(現在)
	int m_nPrevPt;	// 文字列データの参照位置(更新前)
	int m_nCurLen;	// 参照位置のデータ長

public:
	StrData();	// コンストラクタ
	~StrData();	// デストラクタ
	int SetData(char *pStr, char cType);	// データ設定
	char GetType();		// データタイプ取得
	char *GetCurPtr();	// 現在位置にある値ポインタ取得
	int GetCurLen();	// 現在位置にある文字列長取得
	int GetCurIdx();	// 先頭位置から現在位置までのインデックス取得
	char *GetIdxPtr(int nIdx);		// インデックス位置にある値ポインタ取得
	int GetCurStr(char *pOutput);	// 現在位置にある文字列取得
	void MovNextPtr();	// 現在位置を次に進める
	void UpdCurPtr(int nOfset);		// 現在位置を更新する
	void MovTopPtr();	// 現在位置を先頭に戻す
	int SetCurLen();	// 現在位置にある値のByte長設定
	int ChkRange(char *pStr);		// 現在位置From-To指定の範囲内判定
	int GetRangeStr(int nIdx, int nOfset, char *pOutput);	// 現在位置From-To指定の文字列取得
};

class StrEdit {
	char *m_pStr;
	int m_nLen;
	int m_nMax;
	int m_bFreFlg;

public:
	StrEdit();	// コンストラクタ
	StrEdit(int bFlg);	// コンストラクタ
	~StrEdit();	// デストラクタ
	void AlocMem(int nLen);		// メモリ確保
	char *SetEdit(char *pStr);	// 文字列設定
	char *AddEdit(char *pStr, int nSize);	// 文字列追加
	char *Clear();	// 文字列クリア
	char *GetStr();	// 文字列取得
};

#endif
