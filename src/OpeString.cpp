/*
 * �����񑀍�Ɋւ��鏈��
 */

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "OpeString.h"

/* SJIS�̊������� */
int IsKanji(unsigned char c)
{
	if((c >= 0x80 && c < 0xA0) || c >= 0xE0) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/* ���l������̔��� */
int IsNumStr(char *p)
{
	while (*p) {
		if (*p < '0' || '9' < *p)	return FALSE;
		++p;
	}
	return TRUE;
}

/* �����񂩂當���S�����������ʒu�����߂� */
char *StrPbrk(char *s1, char *s2)
{
	char *ret;

	while (*s1) {
		ret = StrChr(s2, s1);
		if (ret != NULL) {
			ret = s1;
			break;
		}

		if (IsKanji(*s1)) {
			s1 += 2;
		} else {
			++s1;
		}
	}

	return ret;
}

/* �����̌��� */
char *StrChr(char *s, char *c)
{
	char *ret = NULL;

	while (*s) {
		if (IsKanji(*s)) {
			if (!memcmp(s, c, 2)) {
				ret = s;
				break;
			} else { 
				s += 2;
			}
		} else {
			if (*s == *c) {
				ret = s;
				break;
			} else {
				++s;
			}
		}
	}

	return ret;
}

// �����񂩂當����̈ʒu����
// Args:
// 1	char *	�����Ώە�����
// 2	char *	�������镶����
// Return:		����:�|�C���^�A������:NULL
char *StrStr(char *pOrg, char *pSrch)
{
	int nLen;	// ����������̒���
	char *pFnd;	// ���������ʒu

	nLen = strlen(pSrch);
	pFnd = NULL;
	if (*pSrch) {
		while (*pOrg) {
			if (!memcmp(pOrg, pSrch, nLen)) {
				// ���������ꍇ�͈ʒu��߂�l�֐ݒ�
				pFnd = pOrg;
				break;
			} else {
				// �������̏ꍇ�͎Q�ƈʒu�X�V
				if (IsKanji(*pOrg)) {
					pOrg += 2;
				} else {
					pOrg += 1;
				}
			}
		}
	} else {
		// �������镶����NULL������̏ꍇ�́A
		// �����Ώە�����NULL������݈̂�v�Ɣ���
		if (*pOrg == '\0') pFnd = pOrg;
	}

	return pFnd;
}

/* CommandLine�t�@�C�����̕��� */
int GetElement(char *pOrg, int nIdx, char **pArg)
{
	static char *pSave = NULL;
	static int nPos[4096];
	static int nMax = 0;
	int i, stt_f;
	char cSrch;

	if (pOrg != NULL) {
		if (pSave != NULL) {
			delete pSave;
			nMax = 0;
		}
		pSave = strdup(pOrg);

		i = 0;
		stt_f = 0;
		while (pSave[i]) {
			if (stt_f == 0) {
				if(pSave[i] == '"') {
					++i;
					nPos[++nMax] = i;
					cSrch = '"';
					stt_f = 1;
				} else if(pSave[i] == ' ') {
					++i;
				} else {
					nPos[++nMax] = i;
					cSrch = ' ';
					stt_f = 1;
				}
			} else {
				if (pSave[i] == cSrch) {
					pSave[i] = '\0';
					++i;
					stt_f = 0;
				} else {
					if (IsKanji(pSave[i])) {
						i += 2;
					} else {
						++i;
					}
				}
			}
		}
		return nMax;
	} else {
		if (nIdx <= nMax) {
			*pArg = pSave + nPos[nIdx];
			return 1;
		} else {
			*pArg = NULL;
			return 0;
		}
	}
}

/* ���蕶���̃J�E���g */
int CountChar(char *p, char c)
{
	int n = 0;

	while (*p) {
		if (*p == c) ++n;
		if (IsKanji(*p)) {
			p += 2;
		} else {
			++p;
		}
	}

	return n;
}

/* n�Ԗڂ̓��蕶���ʒu�̌��� */
int SrchCharPos(char *p, char c, int idx)
{
	int i = 0, n = 0, find_f = 0;

	while (p[i] && find_f == 0) {
		if (p[i] == c) {
			++n;
			if (idx <= n) {
				find_f = 1;
				break;
			}
		}

		if (IsKanji(p[i])) {
			i += 2;
		} else {
			++i;
		}
	}

	if (find_f) {
		return i;
	} else {
		return -1;
	}
}

/* ������̒u������ */
int SubStr(char *pStr, char *pSrch, char *pRep, int nCnt)
{
	int	nIdx = 0, nDone = 0;
	int nSrchLen, nRepLen;
	char buf[4096+1];

	nSrchLen = strlen(pSrch);
	nRepLen = strlen(pRep);
	while (pStr[nIdx]) {
		/* �u�����������̌��� */
		if(!strncmp(pStr + nIdx, pSrch, nSrchLen)) {
			/* �����̒u���� */
			strcpy(buf, pStr + nIdx + nSrchLen);
			strcpy(pStr + nIdx, pRep);
			strcpy(pStr + nIdx + nRepLen, buf);
			++nDone;
			/* �u�����񐔂̃`�F�b�N */
			if (nDone == nCnt)	break;
			/* �����ʒu�̍X�V */
			nIdx += (nRepLen + 1);
		} else {
			/* �����ʒu�̍X�V */
			if (IsKanji(pStr[nIdx])) {
				nIdx += 2;
			} else {
				++nIdx;
			}
		}
	}
	return(nDone);
}

/* ��납��N�����ڂ̃|�C���^ */
char *TailStr(char *pStr, int nCnt)
{
	int nLen;
	char *pTail;

	nLen = strlen(pStr);
	if (nCnt <= nLen) {
		pTail = pStr + nLen - nCnt;
	} else {
		pTail = pStr;
	}

	return pTail;
}

/* �������r(��/��������ʂȂ�) */
int CmpStr(unsigned char *s1, unsigned char *s2)
{
	int nStat;
	unsigned char bLow1, bLow2;
	char bKanji = 0;

	while (*s1 && *s2) {
		if (isalpha(*s1) && isalpha(*s2) && bKanji == 0) {
			bLow1 = *s1 & 0x20;
			bLow2 = *s2 & 0x20;
			nStat = (int)(bLow1 - bLow2);
		} else {
			nStat = (int)(*s1 - *s2);
		}
		if (nStat != 0) break;

		if (IsKanji(*s1) && IsKanji(*s2)) {
			bKanji = 1;
		} else {
			bKanji = 0;
		}
		++s1;
		++s2;
	}

	return nStat;
}

/* ���C���h�J�[�h�����񔻒�֐� */
int WildSrch(char *pOrgStr, char *pWldStr, int *lpStart, int *lpLength)
{
#define INC_POS(x)	IsKanji(*x) ? x+=2 : x++

#define NOMATCH (-1)
#define SEARCH	0
#define BEGIN	1
#define MATCH	2
	int		nStat,			// ��ԕ\�� (0:������ 1:��v�� 2:��v -1:�s��v)
			nMchPos = 0,	// ��v�J�n�ʒu
			bMchFlg,		// ��v���
			bSpcFlg,		// ���ꕶ���\��
			nSubTop,		// �ċA�̎��̊J�n�ʒu
			nSubLen;		// �ċA�̎��̕�����̒���
	char	*pOrgTop,		// ����������̐擪�ʒu
			*pWldTop;		// ���C���h�J�[�h�擪�ʒu

	// ������Ԑݒ�
	if (*pOrgStr) {
		nStat = SEARCH;
	} else {
		nStat = NOMATCH;
	}
	// ����������C���C���h�J�[�h�̐擪�ʒu�ۑ�
	pOrgTop = pOrgStr;
	pWldTop = pWldStr;

	while (nStat == SEARCH || nStat == BEGIN) { // ����I���܂�
		// ���ꕶ���\���N���A
		bSpcFlg = 0;
		switch (*pWldStr) { // ���C���h�J�[�h����
			case '*':
				// ���C���h�J�[�h�������P�i�߂�
				++pWldStr;
				if (*pWldStr == '\0') { // '*'�͍ŏI�������H
					// ����������̈�v�J�n�ʒu��ݒ�
					*lpStart = nMchPos;
					*lpLength = strlen(pOrgTop + nMchPos);
					// ��ԁ���v
					nStat = MATCH;
				} else {
					if (WildSrch(pOrgStr, pWldStr, &nSubTop, &nSubLen)==TRUE) {
						// ����������̈�v�J�n�ʒu��ݒ�
						*lpStart = nMchPos;
						*lpLength = (pOrgStr - pOrgTop) - nMchPos 
						             + nSubTop + nSubLen;
						// ��ԁ���v
						nStat = MATCH;
					}
				}
				break;

			case '?':
				if (*pOrgStr == '\0') { // ����������͏I�����H
					// ��ԕ\�����s��v
					nStat = NOMATCH;
				} else {
					// ��ԕ\������v��
					nStat = BEGIN;
				}
				// ���������ƃ��C���h�J�[�h�������P�i�߂�
				INC_POS(pOrgStr);
				++pWldStr;
				break;

			case '\0': // Null
				if (nStat == SEARCH) { // ��Ԃ͌��������H
					// ��ԁ��s��v
					nStat = NOMATCH;
				} else {
					// ����������̈�v�J�n�ʒu��ݒ�
					*lpStart = nMchPos;
					*lpLength = (pOrgStr - pOrgTop) - nMchPos;
					// ��ԁ���v
					nStat = MATCH;
				}
				break;

			case '\\':
				// ���ꕶ���\����ݒ肷��
				bSpcFlg = 1;
				; // No BREAK

			default: // �ʏ핶��
				// ������v����
				bMchFlg = FALSE;
				if (IsKanji(*pOrgStr)) { // �������H
					if (*pOrgStr == *(pWldStr + bSpcFlg) &&
						*(pOrgStr + 1) == *(pWldStr + bSpcFlg + 1)) {
						bMchFlg = TRUE;
					}
				} else {
					if (*pOrgStr == *(pWldStr + bSpcFlg)) bMchFlg = TRUE;
				}

				if (bMchFlg) { // ������v���H
					// ��Ԃ��������ł���΁A��ԁ���v��
					if (nStat == SEARCH) nStat = BEGIN;
					// ���������ƃ��C���h�J�[�h�������P�i�߂�
					INC_POS(pOrgStr);
					pWldStr += bSpcFlg;
					INC_POS(pWldStr);
				} else {
					if (nStat == BEGIN) { // ��Ԃ͈�v�����H
						// ��ԁ�������
						nStat = SEARCH;
						// ������������v�J�n�ʒu�ɖ߂�
						pOrgStr = pOrgTop + nMchPos;
						// ���C���h�J�[�h������擪�ɖ߂�
						pWldStr = pWldTop;
					}
					// �����������P�i�߂�
					INC_POS(pOrgStr);
				}
				break;
		}

		if (nStat == SEARCH) { // ��Ԃ͌��������H
			// ��v�J�n�ʒu�������ʒu�ɂ���
			nMchPos = pOrgStr - pOrgTop;
			if (*pOrgStr == '\0') { // ���������I�����H
				// ��ԁ��s��v
				nStat = NOMATCH;
			}
		}
	}

	if (nStat == MATCH) { // ��v���H
		return TRUE; // ��v
	} else {
		return FALSE; // �s��v
	}
}

/* ������̕��� */
int StrSplit(char *pStr, char *pDlmt, int nFmt, STRLIST *pList)
{
#define PID_SRCH_DLMT	0		// �f���~�^������
#define PID_SRCH_CHAR	1		// �f���~�^�ȊO������
#define PID_SRCH_QUOTE	2		// �_�u���N�H�[�g������
	int nIdx = 0,				// �������镶����̎Q�ƈʒu
		nTop,					// ��������e�J�����̐擪�ʒu
		nLen,					// �J����������̒���
		nCnt = 0,				// ���������f���~�^�ȊO�̃f�[�^��
		nDLen;					// �f���~�^�̒���
	int nPid = PID_SRCH_DLMT,	// �������
		nRetPid,				// �_�u���N�H�[�g�������̏������
		nAdjUpd = 0;			// �Q�ƈʒu�X�V�̕␳�o�C�g��
	int nAddCnt = 0,			// ���X�g�ւ̏o�̓f�[�^��
		bEndFlg = FALSE;		// ���������̏I���\��
	char *pRet, *pBuf, *pOrg, *pFnd, szNull[2];

	pList->Clear();

	if ((nFmt & SPLIT_MULTI) == 0) { // 1�ł܂�̃f���~�^�ŕ���
		nDLen = strlen(pDlmt);
	} else {
		nDLen = 1;
	}

	nTop = nIdx;
	while (bEndFlg == FALSE) {
		if ((nFmt & SPLIT_MULTI) == 0) { // 1�ł܂�̃f���~�^�ŕ���
			pRet = NULL;
			if (pStr[nIdx] == '\0' ||
			    strncmp(pStr + nIdx, pDlmt, nDLen) == 0) pRet = pDlmt;
		} else {
			pRet = strchr(pDlmt, pStr[nIdx]);
		}

		if (nPid == PID_SRCH_DLMT) { // �f���~�^�������H
			if (pStr[nIdx] == '\"' && (nFmt & SPLIT_EXCEL)) { // EXCEL�`���Ń_�u���N�H�[�g�H
				// ������ʕۑ�
				nRetPid = nPid;
				// �_�u���N�H�[�g����������
				nPid = PID_SRCH_QUOTE;
			} else if (pRet) { // �f���~�^�H
				if (pStr[nIdx] == '\0') { // ������̍Ō�H
					nAddCnt = 1;
					++nCnt;
				} else if ((nFmt & SPLIT_NONE) || (nFmt & SPLIT_SEP)) { // ������̂� or ������ƃf���~�^�ɕ���
					nAddCnt = 1;
					nAdjUpd = nDLen - 1;
				} else { // �f���~�^�t��������ɕ���
					nAdjUpd = nDLen - 1;
				}

				// �f���~�^�ȊO����������
				nPid = PID_SRCH_CHAR;
			}
		} else if (nPid == PID_SRCH_CHAR) { // �f���~�^�ȊO������
			if (pRet == NULL) { // �f���~�^�ȊO���H
				if (nFmt & SPLIT_NONE) { // ������݂̂ɕ����H
					nTop = nIdx;
				} else {
					nAddCnt = 1;
				}

				// �f���~�^����������
				nPid = PID_SRCH_DLMT;
				++nCnt;

				// EXCEL�`���Ń_�u���N�H�[�g�̏���
				if (pStr[nIdx] == '\"' && (nFmt & SPLIT_EXCEL)) {
					// ������ʕۑ�
					nRetPid = nPid;
					// �_�u���N�H�[�g����������
					nPid = PID_SRCH_QUOTE;
				}
			} else {
				if ((nFmt & SPLIT_MULTI) == 0) { // 1�ł܂�̃f���~�^�ŕ���
					if (nFmt & SPLIT_NONE) {
						nTop = nIdx;
						nAddCnt = 1;
					} else if (nFmt & SPLIT_SEP) { // ������ƃf���~�^�ɕ���
						nAddCnt = 2;
					} else {
						nAddCnt = 1;
					}
					nAdjUpd = nDLen - 1;
					++nCnt;
				}
			}
		} else if (nPid == PID_SRCH_QUOTE) { // �_�u���N�H�[�g������
			if (pStr[nIdx] == '\"') { // �_�u���N�H�[�g�H
				if (pStr[nIdx+1] == '\"') { // ���̕����̓_�u���N�H�[�g�H
					// �Q�ƈʒu��1�o�C�g�]���ɍX�V
					nAdjUpd = 1;
				} else {
					// ������ʂ����ɖ߂�
					nPid = nRetPid;
				}
			}
		}

		// �s�P�ʂ̕������o�^
		if (nAddCnt) {
			nLen = nIdx - nTop;
			pBuf = (char *)malloc(nLen + 1);
			memcpy(pBuf, pStr + nTop, nLen);
			pBuf[nLen] = '\0';

			if (nFmt & SPLIT_EXCEL) { // EXCEL�`���H
				// �O��̃_�u���N�H�[�g�폜
				if (*pBuf == '\"' && pBuf[nLen-1] == '\"') {
					pBuf[nLen-1] = '\0';
					strcpy(pBuf, pBuf + 1);;

					// �_�u���N�H�[�g2��1�֕ϊ�
					pOrg = pBuf;
					pFnd = strstr(pOrg, "\"\"");
					while (pFnd) {
						strcpy(pFnd, pFnd + 1);
						pOrg = pFnd + 1;
						pFnd = strstr(pOrg, "\"\"");
					}
				}
			}

			pList->AddString(pBuf);
			free(pBuf);

			nTop = nIdx;
			if (nAddCnt == 2) {
				szNull[0] = '\0';
				pList->AddString(szNull);
			}
			nAddCnt = 0;
		}

		if (pStr[nIdx]) {
			// �Q�ƈʒu�X�V
			if (IsKanji(pStr[nIdx])) {
				nIdx += 2;
			} else {
				nIdx = nIdx + 1 + nAdjUpd;
				nAdjUpd = 0;
			}
		} else {
			bEndFlg = TRUE;
		}
	}

	return nCnt;
}

/* ���C���h�J�[�h�����ɂ�镶����̒u������ */
char *WildReplace(char *pStr, char *pSrch, char *pRep)
{
	int	nIdx = 0, nStat;
	int nStart, nMchLen;
	STRLIST ConvList;
	char *pBuf;

	nStat = WildSrch(pStr + nIdx, pSrch, &nStart, &nMchLen);
	while (nStat) {
		ConvList.AddString(pStr + nIdx, nStart);
		ConvList.AddString(pRep);
//printf("nIdx = %d, nStart = %d, nMchLen = %d\n", nIdx, nStart, nMchLen);
		nIdx += (nStart + nMchLen);

		nStat = WildSrch(pStr + nIdx, pSrch, &nStart, &nMchLen);
	}
	ConvList.AddString(pStr + nIdx);
//printf("rest = /%s/\n", pStr + nIdx);

	return ConvList.MergeAllStr("");
}

/* �P�Ɖ��s(���O��0x0d�łȂ�0x0a)�̒u������ */
char *OnlyLfReplace(char *pStr, char *pRep)
{
	int	nIdx = 0, nStat;
	int nStart, nMchLen;
	BOOL bMchFlg;
	STRLIST ConvList;
	char *pBuf, *pSrch;

	pSrch = "\n";
	nStat = WildSrch(pStr + nIdx, pSrch, &nStart, &nMchLen);
	while (nStat) {
		// �P�Ɖ��s����
		bMchFlg = TRUE;
		if ((nIdx + nStart) > 0) {
			if (*(pStr + nIdx + nStart - 1) == '\r') {
				bMchFlg = FALSE;
			}
		}

		// �P�Ɖ��s�̒u����
		ConvList.AddString(pStr + nIdx, nStart);
		if (bMchFlg == TRUE) {
			ConvList.AddString(pRep);
		} else {
			ConvList.AddString(pSrch);
		}
		nIdx += (nStart + nMchLen);

		nStat = WildSrch(pStr + nIdx, pSrch, &nStart, &nMchLen);
	}
	ConvList.AddString(pStr + nIdx);

	return ConvList.MergeAllStr("");
}


#if DEBUG
main(int argc, char **argv)
{
	char *id, *p1, *p2, *p3, *p;
	int stt, len, ret;
	STRLIST List;

	id = (argc > 1) ? argv[1] : "";
	p1 = (argc > 2) ? argv[2] : "";
	p2 = (argc > 3) ? argv[3] : "";
	p3 = (argc > 4) ? argv[4] : "";

	if (!strcmp(id, "srch")) {
		ret = WildSrch(p1, p2, &stt, &len);
		printf("ret = %d, start = %d, len = %d\n", ret, stt, len);
	} else if (!strcmp(id, "replace")) {
		p = WildReplace(p1, p2, p3);
		printf("result = /%s/\n", p);
	} else if (!strcmp(id, "split")) {
		ret = StrSplit(p1, p2, atoi(p3), &List);
		printf("ret = %d\n", ret);
		for (int i = 0; i < List.GetMaxCount(); ++i) {
			printf("%d : /%s/\n", i, List.GetString(i));
		}
	}
}
#endif
