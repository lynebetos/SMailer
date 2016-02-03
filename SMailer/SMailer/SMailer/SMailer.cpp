////////////////////////////////////////////////////////////////////////////////
// Simple Mail Sender (Implementation)
// Version 0.0
//
// Copyright (c) 2003 by Morning
// http://morningspace.51.net
// mailto:moyingzz@etang.com
//
// Permission to use, copy, modify, distribute and sell this program for any 
// purpose is hereby granted without fee, provided that the above copyright 
// notice appear in all copies and that both that copyright notice and this 
// permission notice appear in supporting documentation.
//
// It is provided "as is" without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include "SMailer.h"
#include "../MUtils/Base64Helper.h"
#include "../MUtils/FileHelper.h"
#include "../MUtils/WinSockHelper.h"
//
using namespace std;
namespace SMailer {

// constants defination
/////////////////////////////////////

enum {MAX_BUFFER_SIZE = 255};

const MailString Priority::important = _T("1");
const MailString Priority::normal    = _T("3");
const MailString Priority::trivial   = _T("5");

const MailString MailWrapper::_mailer_name = _T("SMailer");
const MailString MailWrapper::_boundary    = _T("#BOUNDARY#");

// Member functions of class MimeContent
/////////////////////////////////////

static MUtils::WinSockHelper s_winSockHelper;

MimeContent::MimeContent(const MailString content) : _content(content)
{
}

MailString MimeContent::getDisposition() const
{
    return _T("");
}

// Member functions of class PlainTextContent
/////////////////////////////////////

TextPlainContent::TextPlainContent(const MailString content, 
                                   const MailString charset)
    : MimeContent(content), _charset(charset)
{
}

MailString TextPlainContent::getType() const
{
    return _T("text/plain; charset=") + _charset;
}

MailString TextPlainContent::getTransEncoding() const
{
    return _T("8bit");
}

MailString& TextPlainContent::getContent()
{
    // you can add more codes here, such as wrapping lines 
    // or replacing '\n' with '\r\n', etc.
    return _content;
}

// Member functions of class TextHtmlContent
/////////////////////////////////////

TextHtmlContent::TextHtmlContent(const MailString content, 
                                 const MailString charset)
    : MimeContent(content), _charset(charset)
{
}

MailString TextHtmlContent::getType() const
{
    return _T("text/html; charset=") + _charset;
}

MailString TextHtmlContent::getTransEncoding() const
{
    return _T("8bit");
}

MailString& TextHtmlContent::getContent()
{
    // you can add more codes here, such as wrapping lines 
    // or replacing '\n' with '\r\n', etc.
    return _content;
}

// Member functions of class AppOctStrmContent
/////////////////////////////////////

AppOctStrmContent::AppOctStrmContent(const MailString file_name)
    : _file_name(file_name)
{
    TCHAR drive[_MAX_DRIVE];
    TCHAR direc[_MAX_DIR];
    TCHAR fname[_MAX_FNAME];
    TCHAR ext[_MAX_EXT];
#ifdef _UNICODE
	_wsplitpath(file_name.c_str(), drive, direc, fname, ext);
#else
    _splitpath(file_name.c_str(), drive, direc, fname, ext);
#endif
    _name  = fname;
    _name += ext;
}

MailString AppOctStrmContent::getType() const
{
    return _T("application/octet-stream; name=") + _name;
}

MailString AppOctStrmContent::getDisposition() const
{
    return _T("attachment; filename=") + _name;
}

MailString AppOctStrmContent::getTransEncoding() const
{
    return _T("base64");
}

MailString& AppOctStrmContent::getContent()
{
    // you can add more codes here, such as wrapping lines 
    // or replacing '\n' with '\r\n', etc.
    MUtils::FileHelper::open(_file_name, _content);
    _content = MUtils::Base64Helper::encode(_content);
    return _content;
}

// Member functions of class MailInfo
/////////////////////////////////////

MailInfo::MailInfo() : _priority(Priority::normal)
{
}

void MailInfo::setSenderName(const MailString name)
{
    _sender_name = name;
}

void MailInfo::setSenderAddress(const MailString address)
{
    _sender_address = address;
}

MailString MailInfo::getSenderName() const
{
    return _sender_name;
}

MailString MailInfo::getSenderAddress() const
{
    return _sender_address;
}

void MailInfo::addReceiver(const MailString name, const MailString address)
{
    _receivers.insert(Receivers::value_type(name, address));
}

void MailInfo::setReceiver(const MailString name, const MailString address)
{
    _receivers.clear();
    _receivers.insert(Receivers::value_type(name, address));
}

const Receivers& MailInfo::getReceivers() const
{
    return _receivers;
}

void MailInfo::setPriority(MailString priority)
{
    _priority = priority;
}

MailString MailInfo::getPriority() const
{
    return _priority;
}

void MailInfo::setSubject(const MailString subject)
{
    _subject = subject;
}

MailString MailInfo::getSubject() const
{
    return _subject;
}

void MailInfo::addMimeContent(MimeContent* content)
{
    _contents.push_back(content);
}

void MailInfo::clearMimeContents()
{
    _contents.clear();
}

const MimeContents& MailInfo::getMimeContents() const
{
    return _contents;
}

// Member functions of class MailWrapper
/////////////////////////////////////

MailWrapper::MailWrapper(MailInfo* mail_info) : _mail_info(mail_info)
{
}

MailString MailWrapper::getSenderAddress()
{
    MailString address;

    address  = _T("<");
    address += _mail_info->getSenderAddress();
    address += _T(">");

    return address;
}

MailString MailWrapper::getHeader()
{
    MailString header;

    header  = _T("From: ");
    header += prepareFrom() + _T("\r\n");

    header += _T("To: ");
    header += prepareTo() + _T("\r\n");

    header += _T("Date: ");
    header += prepareDate() + _T("\r\n");

    header += _T("Subject: ");
    header += _mail_info->getSubject() + _T("\r\n");

    header += _T("X-Mailer: ");
    header += _mailer_name + _T("\r\n");

    header += _T("X-Priority: ");
    header += _mail_info->getPriority() + _T("\r\n");

    header += _T("MIME-Version: 1.0\r\n");
    header += _T("Content-type: multipart/mixed; boundary=\"");
    header += _boundary + _T("\"\r\n");

    header += _T("\r\n");

    return header;
}

void MailWrapper::traverseReceiver()
{
    _rcv_itr = _mail_info->getReceivers().begin();
}

bool MailWrapper::hasMoreReceiver()
{
    return ( _rcv_itr != _mail_info->getReceivers().end() );
}

MailString MailWrapper::nextReceiverAddress()
{
    MailString address;

    address  = _T("<");
    address += (_rcv_itr++)->second;
    address += _T(">");

    return address;
}

void MailWrapper::traverseContent()
{
    _con_itr = _mail_info->getMimeContents().begin();
}

bool MailWrapper::hasMoreContent()
{
    return ( _con_itr != _mail_info->getMimeContents().end() );
}

MailString& MailWrapper::nextContent()
{
    _content  = _T("--") + _boundary + _T("\r\n");

    _content += _T("Content-Type: ");
    _content += (*_con_itr)->getType() + _T("\r\n");

    MailString disposition = (*_con_itr)->getDisposition();
    if ( !disposition.empty() )
    {
        _content += _T("Content-Disposition: ");
        _content += disposition + _T("\r\n");
    }

    _content += _T("Content-Transfer-Encoding: ");
    _content += (*_con_itr)->getTransEncoding() + _T("\r\n\r\n");

    _content += (*_con_itr)->getContent() + _T("\r\n\r\n");

    _con_itr++;

    return _content;
}

MailString MailWrapper::getEnd()
{
    MailString end;

    end += _T("\r\n--") + _boundary + _T("--\r\n");

    end += _T(".\r\n");

    return end;
}

MailString MailWrapper::prepareFrom()
{
    MailString from_string;

    from_string = prepareName(_mail_info->getSenderName());
    from_string += getSenderAddress();

    return from_string;
}

MailString MailWrapper::prepareTo()
{
    MailString to_string;

    traverseReceiver();
    while ( hasMoreReceiver() )
    {
        to_string += prepareName(_rcv_itr->first);
        to_string += nextReceiverAddress() + _T(", ");
    }

    return to_string.substr(0, to_string.length()-2);
}

MailString MailWrapper::prepareDate()
{
    char date_string[MAX_BUFFER_SIZE];

    time_t seconds;
    time(&seconds);
    strftime(date_string, MAX_BUFFER_SIZE, 
             "%a, %d %b %y %H:%M:%S +0800", 
             localtime(&seconds));          // +0800 maybe hard code

	int nDstLen = 0;
	TCHAR *pTime = M_A2T(date_string, sizeof(date_string), nDstLen);
	MailString strTime(pTime, nDstLen);
	delete[] pTime;
    return strTime;
}

MailString MailWrapper::prepareName(const MailString raw_name)
{
    MailString decorated_name;

    if (!raw_name.empty())
    {
        decorated_name  = _T("\"");
        decorated_name += raw_name;
        decorated_name += _T("\" ");
    }

    return decorated_name;
}

// Member functions of class MailSender
/////////////////////////////////////

MailSender::MailSender(const MailString server_name, 
                       const MailString user_name, 
                       const MailString user_pwd)
    : _server_name(server_name), _user_name(user_name), 
      _user_pwd(user_pwd), _mail(0)
{
    conn();

    hello();

    if ( !user_name.empty() )
        login();
}

MailSender::~MailSender()
{
	try
	{
		quit();
	}
	catch (MailException* e)
	{
		e->error();
	}
}

void MailSender::setMail(MailWrapper* mail)
{
    _mail = mail;
}

void MailSender::sendMail()
{
    if (!_mail)
        return;

    sendHeader();
    sendContent();
    sendEnd();
}

void MailSender::conn()
{
	int nDstLen = 0;
	char *pName = M_T2A(_server_name.c_str(), _server_name.size(), nDstLen);
    struct hostent* host = gethostbyname(pName);
	delete []pName;
    if (host == 0)
    {
        throw MailException(_T("Get server infomation error"));
    }

    _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = *(ULONG *)host->h_addr_list[0];
	//cout<<inet_ntoa(*((struct in_addr *)host->h_addr_list[0]));
	//server_addr.sin_addr.s_addr = htonl(inet_addr(inet_ntoa(*((struct in_addr *)host->h_addr_list[0]))));
    server_addr.sin_port = htons(SERVICE_PORT);

    int nRes = connect(_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if(nRes < 0)
	{
		int nErro = WSAGetLastError();
		cout<<nErro;
	}
    rcvResponse(_T("220"));
}

void MailSender::hello()
{
    char local_host[MAX_BUFFER_SIZE];
    if ( gethostname(local_host, MAX_BUFFER_SIZE) != 0 )
    {
        throw MailException(_T("Get local host name error"));
    }

	int nDstLen = 0;
	TCHAR *plocal = M_A2T(local_host, MAX_BUFFER_SIZE, nDstLen);

    MailString msg;

    msg  = _T("HELO ");
    msg += MailString(plocal) + _T("\r\n");
    sendRequest(send_helo_cmd, msg);
    rcvResponse(_T("250"));
	delete[] plocal;
}

void MailSender::login()
{
    MailString msg;

    msg = _T("AUTH LOGIN\r\n");
    sendRequest(send_auth_cmd, msg);
    rcvResponse(_T("334"));

    msg = MUtils::Base64Helper::encode(_user_name) + _T("\r\n");
    sendRequest(send_username, msg);
    rcvResponse(_T("334"));

    msg = MUtils::Base64Helper::encode(_user_pwd) + _T("\r\n");
    sendRequest(send_password, msg);
    rcvResponse(_T("235"));
}

void MailSender::sendHeader()
{
    MailString msg;

    msg  = _T("MAIL FROM: ");
    msg += _mail->getSenderAddress() + _T("\r\n");
    sendRequest(send_mail_cmd, msg);
    rcvResponse(_T("250"));

    _mail->traverseReceiver();
    while ( _mail->hasMoreReceiver() )
    {
        msg  = _T("RCPT TO: ");
        msg += _mail->nextReceiverAddress() + _T("\r\n");
        sendRequest(send_rcpt_cmd, msg);
        rcvResponse(_T("250"));
    }

    msg  = _T("DATA\r\n");
    sendRequest(send_data_cmd, msg);
    rcvResponse(_T("354"));

    sendRequest(send_header, _mail->getHeader());
}

void MailSender::sendContent()
{
    _mail->traverseContent();
    while ( _mail->hasMoreContent() )
    {
        sendRequest(send_content, _mail->nextContent());
    }
}

void MailSender::sendEnd()
{
    sendRequest(send_end, _mail->getEnd());
    rcvResponse(_T("250"));
}

void MailSender::quit()
{
	try
	{
		sendRequest(send_quit_cmd, _T("QUIT\r\n"));
		rcvResponse(_T("221"));
	}
	catch(MailException* e)
	{
		e->error();
	}

    closesocket(_socket);
}

void MailSender::rcvResponse(const MailString expected_response)
{
    int recv_bytes = 0;
    char response_buffer[MAX_BUFFER_SIZE];
    if ( (recv_bytes = recv(_socket, response_buffer, MAX_BUFFER_SIZE, 0)) < 0 )
    {
		DWORD dwErr = WSAGetLastError();
        throw MailException( 
            ErrorMessage::getInstance().response(expected_response)
        );
    }

	int nDstLen = 0;
	TCHAR *pRecv = M_A2T(response_buffer, recv_bytes, nDstLen);

    MailString response(pRecv, nDstLen);
	delete[] pRecv;
	MailCout << _T("[INFO]RECV(") << expected_response << _T("):" )
		<< response << std::endl;

    if ( response.substr(0, 3) != expected_response )
    {
        throw MailException( 
            ErrorMessage::getInstance().response(expected_response)
        );
    }
}

void MailSender::sendRequest(Operaion operation, 
                             const MailString content)
{
    MailCout << _T("[INFO]SEND:") << content << std::endl;
	int nDstLen = 0;
	int nSendLen = 0;
	char *pBuf = M_T2A(content.c_str(), content.size(), nDstLen);

	while(nDstLen >=  nSendLen)
	{
		int nNeedSend = 0;
		nDstLen > 4096 ? nNeedSend = 4096 : nNeedSend = nDstLen + 1;
		int nRetLen = 0;
		if ( (nRetLen = send(_socket, pBuf, nNeedSend, 0)) < 0 )
		{
			int err = WSAGetLastError();
			throw MailException(
				ErrorMessage::getInstance().request(operation)
				);
		}
		nSendLen += nNeedSend;
	}

	delete[] pBuf;
}

// Member functions of class ErrorMessage
/////////////////////////////////////

ErrorMessage& ErrorMessage::getInstance()
{
    static ErrorMessage _instance;
    return _instance;
}

ErrorMessage::ErrorMessage()
{
    _request_errmsg_map[MailSender::send_helo_cmd] = _T("Send HELO cmd error");
    _request_errmsg_map[MailSender::send_auth_cmd] = _T("Send AUTH cmd error");
    _request_errmsg_map[MailSender::send_username] = _T("Send user name error");
    _request_errmsg_map[MailSender::send_password] = _T("Send user password error");
    _request_errmsg_map[MailSender::send_mail_cmd] = _T("Send MAIL FROM cmd error");
    _request_errmsg_map[MailSender::send_rcpt_cmd] = _T("Send RCPT TO cmd error");
    _request_errmsg_map[MailSender::send_data_cmd] = _T("Send DATA cmd error");
    _request_errmsg_map[MailSender::send_header  ] = _T("Send mail header error");
    _request_errmsg_map[MailSender::send_content ] = _T("Send mail content error");
    _request_errmsg_map[MailSender::send_end     ] = _T("Send mail end error");
    _request_errmsg_map[MailSender::send_quit_cmd] = _T("Send QUIT cmd error");

    _respons_errmsg_map[_T("220")] = _T("Server connect error");
    _respons_errmsg_map[_T("250")] = _T("General server error");
    _respons_errmsg_map[_T("334")] = _T("Server authentication error");
    _respons_errmsg_map[_T("235")] = _T("Password error");
    _respons_errmsg_map[_T("354")] = _T("Server not ready for data");
    _respons_errmsg_map[_T("221")] = _T("Server didn't terminate session");
}

MailString& ErrorMessage::request(MailSender::Operaion request_operation)
{
    return _request_errmsg_map[request_operation];
}

MailString& ErrorMessage::response(const MailString expected_response)
{
    return _respons_errmsg_map[expected_response];
}

} // namespace SMailer
