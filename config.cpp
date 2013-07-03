/*	TOP-SURF: a visual words toolkit
	Copyright (C) 2010 LIACS Media Lab, Leiden University,
	                   Bart Thomee (bthomee@liacs.nl),
					   Erwin M. Bakker (erwin@liacs.nl)	and
					   Michael S. Lew (mlew@liacs.nl).

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    See http://www.gnu.org/licenses/gpl.html for the full license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

	In addition, this work is covered under the Creative Commons
	Attribution license version 3.
    See http://creativecommons.org/licenses/by/3.0/ for the full license.
*/

#include "config.h"

#include <stdarg.h>

// safe print string
void SAFE_SPRINTF(char *buffer, size_t count, const char *format, ...)
{
	if (buffer && count > 0)
	{
		va_list	argptr;
		va_start(argptr, format);
		int chars = _vsnprintf(buffer, count, format, argptr);
		va_end(argptr);
		if (chars == count)
		{
			// no null-terminator was appended, we need to do that ourselves
			buffer[count-1] = '\0';
		}
		else if (chars == -1)
		{
			// length of formatted string exceeded length of buffer, and no
			// null-terminator was appended
			buffer[count-1] = '\0';
		}
	}
}

// print string using variable amount of arguments
char* SAFE_VAPRINT(const char *format, ...)
{
	va_list	argptr;
	static char	string[2][32000];	// in case va is called by nested functions
	static int index = 0;
	char *buf;

	buf = string[index & 1];
	index++;

	va_start(argptr, format);
	_vsnprintf(buf, 31999, format, argptr);
	va_end(argptr);

	return buf;
}

// easy print to debug window
void SAFE_DEBUG(const char *format, ...)
{
	va_list	argptr;
	static char	string[2][32000];	// in case va is called by nested functions
	static int index = 0;
	char *buf;

	buf = string[index & 1];
	index++;

	va_start(argptr, format);
	_vsnprintf(buf, 31999, format, argptr);
	va_end(argptr);

#if defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64
	OutputDebugString(buf);
#else
	fprintf(stdout, "%s", buf);
#endif
}

// safe print string to file and immediately flush it
void SAFE_FLUSHPRINT(FILE *file, const char *format, ...)
{
	va_list	argptr;
	va_start(argptr, format);
	vfprintf(file, format, argptr);
	va_end(argptr);
	fflush(file);
}

// safe read line from file
bool SAFE_GETLINE(char *buffer, unsigned int buffersize, unsigned int &linelength, FILE *file)
{
	if (buffer == NULL || buffersize == 0 || file == NULL)
		return false;
	// read line from file
	if (fgets(buffer, buffersize, file) == NULL)
		return false;
	// trim newline character that fgets normally reads as well
	linelength = (unsigned int)strlen(buffer);
	if (linelength > 0 && buffer[linelength-1] == '\n')
		buffer[--linelength] = '\0';
	return true;
}

// quick load file from disk
bool SAFE_LOADFILE(const char *fname, unsigned char *&data, unsigned int &length)
{
	if (fname == NULL || &data == NULL)
		return false;
	FILE *file = fopen(fname, "rb");
	if (file == NULL)
		return false;
	// load the file
	bool success = SAFE_LOADFILE(file, data, length);
	fclose(file);
	return success;
}

bool SAFE_LOADFILE(FILE *file, unsigned char *&data, unsigned int &length)
{
	// determine size of file
	if (fseek(file, 0L, SEEK_END) != 0)
		return false;
	long pos = ftell(file);
	if (pos < 0)
		return false;
	if (pos == 0)
	{
		length = 0;
		*data = NULL;
		return true;
	}
	rewind(file);
	// read all data
	data = new unsigned char[pos];
	if (fread(data, pos, 1, file) != 1)
	{
		delete[] data;
		return false;
	}
	length = pos;
	return true;
}

string GetFileExtension(const string& fname)
{
    if (fname.find_last_of(".") != std::string::npos)
        return fname.substr(fname.find_last_of(".")+1);
    return "";
}

// get current date
t_date GetCurrentDate()
{
	t_date date;
	localtime(&date);
	return date;
}

const char* GetDateAsString(t_date date)
{
	// the return value string of ctime contains exactly 26 characters and
	// has the form: Wed Jan 02 02:03:55 1980\n\0
	// Note: we don't like the \n so we should remove it
	char *t = ctime(&date);
	t[24] = '\0';
	return t;
}

#if !defined WIN32 && !defined _WIN32 && !defined WIN64 && !defined _WIN64
#include <pwd.h>
string TildeExpandPath(const string& path)
{
	if (path.empty())
		return "";
	// check if the path starts with a tilde
	if (path.at(0) != '~')
		return path;
	// get the home directory
	char *home = getenv("HOME");
	if (home == NULL) {
		// look up the directory in the user database
		struct passwd *entry = (struct passwd *)getpwuid(getuid());
		if (entry == NULL)
			return "";
		home = entry->pw_dir;
	}
	string expand = home;
	expand += path.substr(1);
	return expand;
}
#endif

#if !defined WIN32 && !defined _WIN32 && !defined WIN64 && !defined _WIN64
#include <termios.h>
#include <unistd.h>
// get a single character from the console
int _getch()
{
	struct termios oldt, newt;
	int ch;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return ch;
}
#endif