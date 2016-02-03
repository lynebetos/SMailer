////////////////////////////////////////////////////////////////////////////////
// Simple Mail Sender (Interface)
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

#ifndef _SMAILER_H_
#define _SMAILER_H_
//
#include "../MailString.h"
#include <vector>
#include <map>
#include <exception>
#include <winsock2.h>
//
namespace SMailer {

// class MimeContent(Abstract class)
/////////////////////////////////////

class MimeContent
{
public:

    MimeContent(const MailString content = _T(""));

    virtual MailString  getType() const = 0;
    virtual MailString  getDisposition() const;
    virtual MailString  getTransEncoding() const = 0;
    virtual MailString& getContent() = 0;

protected:

    MailString _content;
};

typedef std::vector<MimeContent*> MimeContents;

// class PlainTextContent
/////////////////////////////////////

class TextPlainContent : public MimeContent
{
public:

    TextPlainContent(const MailString content, 
                     const MailString charset = _T("gb2312"));

    virtual MailString  getType() const;
    virtual MailString  getTransEncoding() const;
    virtual MailString& getContent();

private:

    MailString _charset;
};

// class TextHtmlContent
/////////////////////////////////////

class TextHtmlContent : public MimeContent
{
public:

    TextHtmlContent(const MailString content, 
                    const MailString charset = _T("gb2312"));

    virtual MailString  getType() const;
    virtual MailString  getTransEncoding() const;
    virtual MailString& getContent();

private:

    MailString _charset;
};

// class AppOctStrmContent
/////////////////////////////////////

class AppOctStrmContent : public MimeContent
{
public:

    AppOctStrmContent(const MailString file_name);

    virtual MailString  getType() const;
    virtual MailString  getDisposition() const;
    virtual MailString  getTransEncoding() const;
    virtual MailString& getContent();

private:

    MailString _file_name;
    MailString _name;
};

// class Priority(Helper class)
/////////////////////////////////////

class Priority
{
public:

    static const MailString important;
    static const MailString normal;
    static const MailString trivial;
};

// class MailInfo
/////////////////////////////////////

typedef std::multimap<MailString, MailString> Receivers;

class MailInfo
{
public:

    MailInfo();

    void setSenderName(const MailString name);
    void setSenderAddress(const MailString address);
    MailString getSenderName() const;
    MailString getSenderAddress() const;

    void addReceiver(const MailString name, const MailString address);
    void setReceiver(const MailString name, const MailString address);
    const Receivers& getReceivers() const;

    void setPriority(MailString priority);
    MailString getPriority() const;

    void setSubject(const MailString subject);
    MailString getSubject() const;

    void addMimeContent(MimeContent* content);
    void clearMimeContents();
    const MimeContents& getMimeContents() const;

private:

    MailString  _sender_name;
    MailString  _sender_address;
    Receivers    _receivers;
    MailString  _priority;
    MailString  _subject;
    MimeContents _contents;
};

// class MailWrapper
/////////////////////////////////////

class MailWrapper
{
public:

    MailWrapper(MailInfo* mail_info);

    MailString getSenderAddress();
    MailString getHeader();
    MailString getEnd();

    void traverseReceiver();
    bool hasMoreReceiver();
    MailString nextReceiverAddress();

    void traverseContent();
    bool hasMoreContent();
    MailString& nextContent();

private:

    static const MailString _mailer_name;
    static const MailString _boundary;

    MailInfo* _mail_info;
    Receivers::const_iterator _rcv_itr;

    MailString _content;
    MimeContents::const_iterator _con_itr;

    MailString prepareFrom();
    MailString prepareTo();
    MailString prepareDate();
    MailString prepareName(const MailString raw_name);
};

// class MailSender
/////////////////////////////////////

class MailSender
{
public:

    MailSender(const MailString server_name, 
               const MailString user_name = _T(""), 
               const MailString user_pwd = _T(""));

    ~MailSender();

    void setMail(MailWrapper* mail);

    void sendMail();

private:

    enum {SERVICE_PORT = 25};

    enum Operaion
    {
        send_helo_cmd, 
        send_auth_cmd, 
        send_username, 
        send_password, 
        send_mail_cmd, 
        send_rcpt_cmd, 
        send_data_cmd, 
        send_header, 
        send_content, 
        send_end, 
        send_quit_cmd, 
    };

    MailString _server_name;
    MailString _user_name;
    MailString _user_pwd;
    SOCKET _socket;

    MailWrapper* _mail;

    void conn();
    void hello();
    void login();

    void sendHeader();
    void sendContent();
    void sendEnd();

    void quit();

    void sendRequest(Operaion operation, const MailString content);
    void rcvResponse(const MailString expected_response);

    friend class ErrorMessage;
};

// class ErrorMessage(Helper class)
/////////////////////////////////////

class ErrorMessage
{
public:

    static ErrorMessage& getInstance();

    MailString& request (MailSender::Operaion request_operation);
    MailString& response(const MailString expected_response);

private:

    std::map<MailSender::Operaion, MailString> _request_errmsg_map;
    std::map<MailString, MailString> _respons_errmsg_map;

    ErrorMessage();
};

// class MailException(Helper class)
/////////////////////////////////////

class MailException : public std::exception
{
public:

    MailException(const MailString message = _T(""))
     : _message(message)
    {
    }

    const char *what() const throw ()
    {
        return NULL;
    }

	const TCHAR *error() const throw()
	{
		return _message.c_str();
	}

private:

    MailString _message;
};

} // namespace SMailer

#endif // _SMAILER_H_
