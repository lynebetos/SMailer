#ifndef __MAIL_STRING_H__
#define __MAIL_STRING_H__

// add by lix 20123.8.13

#include <string>
#include <fstream>
#include <iostream>
#include <tchar.h>
using namespace std;

#ifdef _UNICODE
typedef wstring		MailString;
typedef wifstream	MailFile;
#define MailCout	wcout		
#else
typedef string		MailString;
typedef ifstream	MailFile;
#define MailCout	cout
#endif

inline TCHAR* M_A2T(const char *pSrc, int nSrcLen, int &nDstLen)
{
#ifdef _UNICODE
	nDstLen = MultiByteToWideChar(CP_ACP, 0, pSrc, nSrcLen, NULL, 0);

	TCHAR *pDst = new TCHAR[nDstLen+1];
	memset(pDst, 0, (nDstLen+1)*2);
	MultiByteToWideChar(CP_ACP, 0, pSrc, nSrcLen, pDst, nDstLen);
	nDstLen += 1;
	return pDst;
#else
	TCHAR *pDst = new TCHAR[nSrcLen+1];
	memset(pDst, 0, (nSrcLen+1));
	memcpy(pDst, pSrc, nSrcLen);
	nDstLen = nSrcLen + 1;
	return pDst;
#endif

	return NULL;
}

inline char *M_T2A(const TCHAR *pSrc, int nSrcLen, int &nDstLen)
{
#ifdef _UNICODE
	nDstLen = WideCharToMultiByte(CP_ACP, 0, pSrc, nSrcLen, NULL, 0, NULL, NULL);

	char *pDst = new char[nDstLen+1];
	memset(pDst, 0, (nDstLen+1));
	WideCharToMultiByte(CP_ACP, 0, pSrc, nSrcLen, pDst, nDstLen, NULL, NULL);
	nDstLen += 1;
	return pDst;
#else
	char *pDst = new char[nSrcLen+1];
	memset(pDst, 0, (nSrcLen+1));
	memcpy(pDst, pSrc, nSrcLen);
	nDstLen = nSrcLen+1;
	return pDst;
#endif

	return NULL;
}

inline void M_Format(TCHAR *pszString, const TCHAR *pszFormat,...)
{
	va_list   arglist;
	va_start(arglist, pszFormat);

#ifdef _UNICODE
	wvsprintf(pszString, pszFormat, arglist);
#else
	vsprintf(pszString, pszFormat, arglist);
#endif

	va_end(arglist);
}

#endif