#include <windows.h>
#include <stdio.h>
#include <ctype.h>
#include "StrData.h"
#include "OpeString.h"
#include "Formating.h"

#define MAX_BUF	1024

static int nDepth = 2;
static int nIndent = 0;
static char *pNoEndTag[] = {"AREA", "BASE", "BASEFONT", "BGSOUND", "BR", "COL", "EMBED", "FRAME", "HR", "IMG", "INPUT", "ISINDEX", "KEYGEN", "LINK", "META", "NEXTID", "PARAM", "PLANETEX", "SOURCE", "SPACER", "TRACK", "WBR", "!DOCTYPE", NULL};

static int FmtHtml(char *pSrc, StrEdit *pConv);
static int GetTag(char *pSrc, char *pTag);
static int ChkNoEndTag(char *pTag);
static void MakFmtStr(char *pSrc, int nStt, int nLen, StrEdit *pConv);
static char *GetToken(char *pOrg, char *pDlmt, int nBlk, int *pnLen);

#if 0
main(int argc, char **argv)
{
	char *pOrg, *pFmt, *pCnv;

	if (argc > 1) pOrg = argv[1];
	if (argc > 2) pFmt = argv[2];

	pCnv = CnvColDat(pOrg, pFmt, ",");
	printf("Org = /%s/\n", pOrg);
	printf("Cnv = /%s/\n", pCnv);
	free(pCnv);

	exit(0);
}
#endif

// HTML�̃t�H�[�}�b�g���`
char *AdjHtml(char *pStr)
{
	int nIdx = -1, nEnd;
	StrEdit Conv;
	char *pCnv;

	nIndent = 0;
	do {
		++nIdx;
		nEnd = FmtHtml(pStr + nIdx, &Conv);
		nIdx += nEnd;
	} while (pStr[nIdx]);

	pCnv = Conv.GetStr();
	if (pCnv){
		if (strcmp(pStr, pCnv)) {
			return strdup(pCnv);
		}
	}
	return NULL;
}

// 1�s���̃t�H�[�}�b�g���`
static int FmtHtml(char *pSrc, StrEdit *pConv)
{
	int nIdx, nHead;
	int nOfset = 0, nStat;
#define SPCSEQ_STT	0	// �󔒁E�^�u�J�n
#define SPCSEQ_CNT	1	// �󔒁E�^�u�p��
#define SPCSEQ_END	2	// �󔒁E�^�u�I��
	int nSpcSeq = SPCSEQ_STT;
	char szTag[MAX_BUF+1];

	nIdx = 0;
	nHead = nIdx;
	while (pSrc[nIdx] != '\0' && pSrc[nIdx] != '\n' && pSrc[nIdx] != '\r') {
		if (pSrc[nIdx] == '<') {
			// ���`�o��
			if (nHead < nIdx) {
				MakFmtStr(pSrc, nHead, nIdx - nHead, pConv);
			}

			// �^�O�擾
			nStat = GetTag(pSrc + nIdx + 1, szTag);
			if (nStat) {
				// �΂������^�O����
				nStat = ChkNoEndTag(szTag);
				if (nStat == 0) {
					if (pSrc[nIdx + 1] == '/') {
						// �I���^�O
						nIndent -= nDepth;
						nOfset = 0;
					} else {
						// �΂��L��^�O
						nOfset = nDepth;
					}
				} else {
					// �΂������^�O
					nOfset = 0;
				}
				nHead = nIdx;
			}
			nSpcSeq = SPCSEQ_END;
		} else if (pSrc[nIdx] == '>') {
			MakFmtStr(pSrc, nHead, nIdx - nHead + 1, pConv);

			nHead = nIdx + 1;
			nIndent += nOfset;
		} else {
			if (pSrc[nIdx] == ' ' || pSrc[nIdx] == '\t') {
				if (nSpcSeq == SPCSEQ_STT)
					nSpcSeq = SPCSEQ_CNT;
			} else {
				if (nSpcSeq != SPCSEQ_END)
					nHead = nIdx;
				nSpcSeq = SPCSEQ_END;
			}
		}

		// �Q�ƈʒu�X�V
		if (IsKanji(pSrc[nIdx])) {
			nIdx += 2;
		} else {
			++nIdx;
		}
	}

	// ���`�o��
	if (nHead < nIdx) {
		MakFmtStr(pSrc, nHead, nIdx - nHead, pConv);
	}

	return nIdx;
}

// HTML�̃^�O�擾
static int GetTag(char *pSrc, char *pTag)
{
	char *pRef; 
	char szBuf[MAX_BUF+1];
	int nWrt = 0, nStat = 0, i;

	while (*pSrc) {
		if (*pSrc == '>' || *pSrc == ' ') {
			szBuf[nWrt] = '\0';
			strcpy(pTag, szBuf);
			nStat = 1;
			break;
		} else {
			szBuf[nWrt++] = toupper(*pSrc);
		}
		++pSrc;
	}

	return nStat;
}

// �΂������^�O�̃`�F�b�N
static int ChkNoEndTag(char *pTag)
{
	int nStat = 0;

	for (int i = 0; pNoEndTag[i]; ++i) {
		if (!strcmp(pTag, pNoEndTag[i])) {
			nStat = 1;
			break;
		}
	}

	return nStat;
}

// ���`�o��
static void MakFmtStr(char *pSrc, int nStt, int nLen, StrEdit *pConv)
{
	int i, nCnt = 0;
	char *p;
	StrEdit Buf;

	for (i = 0; i < nIndent; ++i) Buf.AddEdit(" ", 0);
	p = Buf.AddEdit(pSrc + nStt, nLen);

	for (i = 0; p[i]; ++i) {
		if (p[i] != ' ' && p[i] != '\t') ++nCnt;
	}

	if (nCnt > 0) {
		pConv->AddEdit(p, 0);
		pConv->AddEdit("\r\n", 0);
	}
}

// �\�쐬
char *MakeTable(char *pText, char *pTokDlmt)
{
	char *pLinDlmt, *pLinTop, *pLinIdx, *pLinStr;
	char *pTokTop, *pTokIdx, *pTokStr;
	char *pStrFmt, *pNumFmt;
	char szBuf[MAX_BUF+1];
	int nLinLen, nLinCnt;
	int nTokLen, nTokCnt;
	int nMaxCol, nSavCnt;
	int nColIdx, i, j;
	int nPreLin, nPreCol;
	MngColFmt *pMngFmt;
	MngColDat *pMngDat;
	TABLEINFO ColFmt(sizeof(MngColFmt));
	TABLEINFO ColDat(sizeof(MngColDat));
	StrEdit TopFmt(TRUE), MidFmt(TRUE), BotFmt(TRUE);
	StrEdit OutDat;

	// ������
	pLinDlmt = "\r\n";
	nSavCnt = 0;
	nLinCnt = 0;
	nMaxCol = 0;

	// 1�s�擾
	pLinTop = pText;
	pLinIdx = GetToken(pLinTop, pLinDlmt, 1, &nLinLen);
	while (pLinIdx != NULL) {
		// 1�s�R�s�[
		pLinStr = (char *)malloc(nLinLen + 1);
		memcpy(pLinStr, pLinTop, nLinLen);
		pLinStr[nLinLen] = '\0';
		// 1�g�[�N���擾
		nTokCnt = 0;
		pTokTop = pLinStr;
		pTokIdx = GetToken(pTokTop, pTokDlmt, 0, &nTokLen);
		while (pTokIdx != NULL) {
			// 1�g�[�N�� �R�s�[
			pTokStr = (char *)malloc(nTokLen + 1);
			memcpy(pTokStr, pTokTop, nTokLen);
			pTokStr[nTokLen] = '\0';
			// �ő咷�X�V
			pMngFmt = (MngColFmt *)ColFmt.GetDataPtr(nTokCnt, TRUE);
			if (pMngFmt->nMax < nTokLen) pMngFmt->nMax = nTokLen;

			// �g�[�N�����ۑ�
			pMngDat = (MngColDat *)ColDat.GetDataPtr(nSavCnt, TRUE);
			pMngDat->pStr = pTokStr;
			pMngDat->nLin = nLinCnt;
			pMngDat->nCol = nTokCnt;
			++nSavCnt;

			// 1�g�[�N���擾
			++nTokCnt;
			if (nMaxCol < nTokCnt)	nMaxCol = nTokCnt; 
			pTokTop = pTokIdx;
			pTokIdx = GetToken(pTokTop, pTokDlmt, 0, &nTokLen);
		}

		// 1�s�擾
		free(pLinStr);
		++nLinCnt;
		pLinTop = pLinIdx;
		pLinIdx = GetToken(pLinTop, pLinDlmt, 1, &nLinLen);
	}

	// �r���t�H�[�}�b�g�̍쐬
	TopFmt.SetEdit("��");
	MidFmt.SetEdit("��");
	BotFmt.SetEdit("��");
	for (nColIdx = 0; nColIdx < ColFmt.Count(); ++nColIdx) {
		// �ő咷�̕␳(2�̔{��)
		pMngFmt = (MngColFmt *)ColFmt.GetDataPtr(nColIdx, FALSE);
		if (pMngFmt->nMax == 0)	pMngFmt->nMax = 2;
		if (pMngFmt->nMax % 2 == 1)	pMngFmt->nMax++;

		for (i = 0; i < (pMngFmt->nMax / 2); ++i) {
			TopFmt.AddEdit("��", 2);
			MidFmt.AddEdit("��", 2);
			BotFmt.AddEdit("��", 2);
		}

		if (nColIdx == (ColFmt.Count() - 1)) {
			TopFmt.AddEdit("��", 2);
			MidFmt.AddEdit("��", 2);
			BotFmt.AddEdit("��", 2);

			pStrFmt = "��%%-%d.%ds��";
			pNumFmt = "��%%%d.%ds��";
		} else {
			TopFmt.AddEdit("��", 2);
			MidFmt.AddEdit("��", 2);
			BotFmt.AddEdit("��", 2);

			pStrFmt = "��%%-%d.%ds";
			pNumFmt = "��%%%d.%ds";
		}
		sprintf(szBuf, pStrFmt, pMngFmt->nMax, pMngFmt->nMax);
		pMngFmt->pStrFmt = strdup(szBuf);
		sprintf(szBuf, pNumFmt, pMngFmt->nMax, pMngFmt->nMax);
		pMngFmt->pNumFmt = strdup(szBuf);
	}

	// �r���\�o��
	OutDat.AddEdit(TopFmt.GetStr(), 0);
	nPreLin = -1;
	nPreCol = nMaxCol;
	for (i = 0; i < ColDat.Count(); ++i) {
		pMngDat = (MngColDat *)ColDat.GetDataPtr(i, FALSE);
		if (pMngDat->nLin != nPreLin) {
			// �ő�J�������ɖ����Ȃ��f�[�^�̌㏈��
			if (nPreCol < (nMaxCol - 1)) {
				for (j = nPreCol + 1; j < nMaxCol; ++j) {
					pMngFmt = (MngColFmt *)ColFmt.GetDataPtr(j, FALSE);
					sprintf(szBuf, pMngFmt->pStrFmt, "");
					OutDat.AddEdit(szBuf, 0);
				}
			}
			// ���s����
			OutDat.AddEdit("\r\n", 0);
			if (pMngDat->nLin > 0) {
				OutDat.AddEdit(MidFmt.GetStr(), 0);
				OutDat.AddEdit("\r\n", 0);
			}
			nPreLin = pMngDat->nLin;
		}
		nPreCol = pMngDat->nCol;

		// �f�[�^�^�ɉ����������ŏo��
		pMngFmt = (MngColFmt *)ColFmt.GetDataPtr(pMngDat->nCol, FALSE);
		if (IsNumStr(pMngDat->pStr)) {
			sprintf(szBuf, pMngFmt->pNumFmt, pMngDat->pStr);
		} else {
			sprintf(szBuf, pMngFmt->pStrFmt, pMngDat->pStr);
		}
		OutDat.AddEdit(szBuf, 0);
	}
	// �ő�J�������ɖ����Ȃ��f�[�^�̌㏈��
	if (nPreCol < (nMaxCol - 1)) {
		for (j = nPreCol + 1; j < nMaxCol; ++j) {
			pMngFmt = (MngColFmt *)ColFmt.GetDataPtr(j, FALSE);
			sprintf(szBuf, pMngFmt->pStrFmt, "");
			OutDat.AddEdit(szBuf, 0);
		}
	}
	// �ŏI�s����
	OutDat.AddEdit("\r\n", 0);
	OutDat.AddEdit(BotFmt.GetStr(), 0);
	OutDat.AddEdit("\r\n", 0);

	// ���������
	for (int i = 0; i < ColDat.Count(); ++i) {
		pMngDat = (MngColDat *)ColDat.GetDataPtr(i, FALSE);
		free(pMngDat->pStr);
	}
	for (int i = 0; i < ColFmt.Count(); ++i) {
		pMngFmt = (MngColFmt *)ColFmt.GetDataPtr(i, FALSE);
		free(pMngFmt->pStrFmt);
		free(pMngFmt->pNumFmt);
	}

	return OutDat.GetStr();
}

// ��؂蕶���ɂ�镶����̎擾
char *GetToken(char *pOrg, char *pDlmt, int nBlk, int *pnLen)
{
	char *pIdx, *pTop, *pRet;

	pIdx = strpbrk(pOrg, pDlmt);
	if (pIdx != NULL) {
		pTop = pIdx;
		if (nBlk != 0) {
			// ��؂蕶���̘A�������e����ꍇ
			do {
				if (IsKanji(*pIdx)) {
					pIdx += 2;
				} else {
					++pIdx;
				}
			} while (*pIdx != '\0' && strchr(pDlmt, *pIdx));
		} else {
			++pIdx;
		}

		*pnLen = pTop - pOrg;
		pRet = pIdx;
	} else if(*pOrg != '\0') {
		*pnLen = strlen(pOrg);
		pRet = pOrg + *pnLen;
	} else {
		*pnLen = 0;
		pRet = NULL;
	}

	return pRet;
}

// �J�����f�[�^�������ϊ�
char *CnvColDat(char *pOrg, char *pFmt, char *pDlmt)
{
	STRLIST OrgLine, DstLine, ColData;
	char *pOrgLine, *pOrgStr, *pCnvStr, *pCol, *pTail;
	char szFmt[MAX_BUF+1], szSrch[MAX_BUF+1];
	int nRidx, nCidx, nRmax, nCmax;

	// �����f�[�^�̐��䕶���ϊ�
	strncpy(szFmt, pFmt, MAX_BUF);
	SubStr(szFmt, "\\t", "\t", 0);
	SubStr(szFmt, "\\r", "\r", 0);
	SubStr(szFmt, "\\n", "\r\n", 0);
	SubStr(szFmt, "\\f", "\f", 0);
	
	// ���̓f�[�^�����s�ŕ���
	StrSplit(pOrg, "\r\n", SPLIT_NONE | SPLIT_MULTI | SPLIT_EXCEL, &OrgLine);
	nRmax = OrgLine.GetMaxCount();

	// ���͍s���ɕϊ�
	for (nRidx = 0; nRidx < nRmax; ++nRidx) {
		// ���͍s���f���~�^�ŕ���
		pOrgLine = OrgLine.GetString(nRidx);
		StrSplit(pOrgLine , pDlmt, SPLIT_NONE | SPLIT_SINGLE | SPLIT_EXCEL, &ColData);
		nCmax = ColData.GetMaxCount();

		// �J�������ɕϊ�
		pOrgStr = strdup(szFmt);
		for (nCidx = nCmax; nCidx > 0; --nCidx) {
			// %1, %2, ... ���J�����f�[�^�Œu��
			pCol = ColData.GetString(nCidx - 1);
			sprintf(szSrch, "%%%d", nCidx);
			pCnvStr = WildReplace(pOrgStr, szSrch, pCol);

			free(pOrgStr);
			pOrgStr = pCnvStr;
		}

		// �o�͍s��ۑ�
		DstLine.AddString(pCnvStr);
		free(pCnvStr);
	}

	// �o�͍s���}�[�W���āA�Ō�̉��s���폜
	pCnvStr = DstLine.MergeAllStr("\r\n");
	pTail = TailStr(pCnvStr, 2);
	*pTail = '\0';

	return pCnvStr;
}

// ��������܂�/�܂܂Ȃ��s�̒��o
// Args:
// 1	char *	�����Ώە�����
// 2	char *	�������镶����
// 3	int 	TRUE:��������܂ލs�𒊏o�AFALSE:��������܂܂Ȃ��s�𒊏o
// Return:		���o�s
char *GrepLine(char *pOrg, char *pSrch, int bInc)
{
	STRLIST OrgLine, DstLine;
	int i, nCnt;
	char *pLine, *pPos, *pGrepStr, *pTail;

	// ���̓f�[�^�����s�ŕ���
	StrSplit(pOrg, "\r\n", SPLIT_NONE | SPLIT_SINGLE | SPLIT_EXCEL, &OrgLine);
	nCnt = OrgLine.GetMaxCount();

	// ���͍s���ɕ����񌟍�
	for (i = 0; i < nCnt ; ++i) {
		pLine = OrgLine.GetString(i);
		pPos = StrStr(pLine, pSrch);
		if (bInc) {
			// �܂ލs�𒊏o
			if (pPos) DstLine.AddString(pLine);
		} else {
			// �܂܂Ȃ��s�𒊏o
			if (pPos == NULL) DstLine.AddString(pLine);
		}
	}

	// �o�͍s���}�[�W���āA�Ō�̉��s���폜
	pGrepStr = DstLine.MergeAllStr("\r\n");
	pTail = TailStr(pGrepStr, 2);
	*pTail = '\0';

	return pGrepStr;
}
