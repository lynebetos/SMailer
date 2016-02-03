// test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "../SMailer/MailSend.h"

int _tmain(int argc, _TCHAR* argv[])
{
	CMailSend _MailSend;

	//_MailSend.SetMailSender(_T("smtp.chinatransinfo.com"), _T("wangxulin@chinatransinfo.com"), _T("qwer1234"));
	_MailSend.SetMailSender(_T("smtp.163.com"), _T("linlin-ring@163.com"), _T("1985221213"));
	_MailSend.AddMailReceiver(_T("wangxulin@chinatransinfo.com"));
	_MailSend.SetInfo("1234567", "C:\\Users\\Administrator\\Desktop\\171log\\BusStopTerminal.log");
	_MailSend.SendMail();

	return 0;
}

