////////////////////////////////////////////////////////////////////////////////
// General utilities : File I/O helper class
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

#ifndef _FILE_HELPER_H_
#define _FILE_HELPER_H_
//
#include "../MailString.h"
#include <vector>
#include <fstream>
#include <stdio.h>
//
namespace MUtils {

class FileHelper
{
public:

    // used to open binary file
    static bool open(const MailString filename, MailString& content)
    {
		int nDstLen = 0;
		char *pfile = M_T2A(filename.c_str(), filename.size(), nDstLen);
        FILE *file = fopen(pfile, "rb");
		delete[] pfile;

        if (file == NULL)
            return false;

        fseek(file, 0, SEEK_END);
        int len = ftell(file);
		rewind(file);

        char *buffer = new char[len];
        fread(buffer, sizeof(char), len, file);

		TCHAR *pBuf = M_A2T(buffer, len, nDstLen);

        content.assign(pBuf, nDstLen);
        delete []buffer;
		delete []pBuf;

        fclose(file);
        return true;
    }

    // used to open text file
    static bool open(const MailString file_name, std::vector<MailString>& lines)
    {
        MailFile file(file_name.c_str(), std::ios::in);
        if (!file) 
        {
            return false;
        }

        lines.clear();
        TCHAR buffer[buffer_size];

        while (file.getline(buffer, buffer_size, _T('\n')))
        {
            lines.push_back(buffer);
        }
        
        return true;
    }

private:

    enum { buffer_size = 3000 };
};

} // namespace MUtils

#endif // _FILE_HELPER_H_