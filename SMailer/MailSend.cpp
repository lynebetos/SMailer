#include "StdAfx.h"
#include "MailSend.h"

CMailSend::CMailSend()
{

}

CMailSend::~CMailSend()
{

}

void CMailSend::SendMail()
{
	char szHost[128] = {0};
	gethostname(szHost, sizeof(szHost));
	hostent *phost = gethostbyname(szHost);

	int nDestLen = 0;
	TCHAR *pszHost = M_A2T(szHost, sizeof(szHost), nDestLen);
	TCHAR *pszIP   = M_A2T(inet_ntoa(*(IN_ADDR*)phost->h_addr_list[0]), 32, nDestLen);

	TCHAR szInfo[512] = {0};
	M_Format(szInfo, _T("From:%s IP:%s\r\n%s"), pszHost, pszIP, m_strInfo.c_str());

	delete[] pszIP;
	delete[] pszHost;

	SMailer::TextPlainContent  textContent(szInfo);
	SMailer::AppOctStrmContent AppContent(m_strFilePath);

	SMailer::MailInfo info;
	info.setSenderName(m_strMailUser);
	info.setSenderAddress(m_strMailUser);
	info.setPriority(SMailer::Priority::normal);
	info.setSubject(_T("Crash Bug"));
	info.addMimeContent(&textContent);
	info.addMimeContent(&AppContent);

	for(unsigned int i=0; i<m_vecMailReceiver.size(); i++)
		info.addReceiver(m_vecMailReceiver.at(i), m_vecMailReceiver.at(i));

	try
	{
		SMailer::MailSender sender(m_strMailServer, m_strMailUser, m_strMailPwd);
		sender.setMail(&SMailer::MailWrapper(&info));
		sender.sendMail();
	}
	catch (SMailer::MailException& e)
	{
		e.error();
	}
	catch (...)
	{
		throw _T("Unkown error");
	}
}