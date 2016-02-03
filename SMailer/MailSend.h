#pragma once
#include "SMailer/SMailer/SMailer.h"
#include <shellapi.h> 
#include <string>
#include <vector>
using namespace std;

using namespace SMailer;

#ifndef MdString
#ifdef _UNICODE
typedef  wstring MdString;
#else
typedef  string MdString;
#endif
#endif

class CMailSend
{
public:
	CMailSend();
	~CMailSend();

public:
	void SendMail();
	void SetMailSender(const TCHAR *pMailServer, 
		const TCHAR *pUserName, const TCHAR *pPassWord)
	{
		ATLASSERT(pMailServer || pUserName || pPassWord);
		m_strMailServer.assign(pMailServer);
		m_strMailUser.assign(pUserName);
		m_strMailPwd.assign(pPassWord);
	}

	void AddMailReceiver(const TCHAR *pMailReceiver)
	{
		ATLASSERT(pMailReceiver);
		m_vecMailReceiver.push_back(pMailReceiver);
	}

	void SetInfo(const TCHAR *pszInfo, const TCHAR *pszFilePath)
	{
		ATLASSERT(pszInfo || pszFilePath);
		m_strInfo.assign(pszInfo);
		m_strFilePath.assign(pszFilePath);
	}
private:
	MdString m_strMailServer;
	MdString m_strMailUser;
	MdString m_strMailPwd;
	MdString m_strInfo;
	MdString m_strFilePath;

	vector<MdString> m_vecMailReceiver;
};


