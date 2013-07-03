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

#ifndef _CONFIGH
#define _CONFIGH

#pragma once

#if defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0501
#include <windows.h>
#ifndef STRICT
#define STRICT
#endif
#define MEMORY_LEAK
#include <conio.h>
#include <direct.h>
#include <io.h>
#include <process.h>
#define PATH_SEPARATOR_STRING "\\"
#define PATH_SEPARATOR_CHAR '\\'
#else
#include <sys/stat.h>
#include <dirent.h>
#define _stricmp strcasecmp
#define _strnicmp strncasecmp
#define _vsnprintf vsnprintf
#define _access access
#define _mkdir(path) mkdir(path, 755)
#define PATH_SEPARATOR_STRING "/"
#define PATH_SEPARATOR_CHAR '/'
extern int _getch();
#endif

// enable memory leak detection
#ifdef MEMORY_LEAK
#include "memoryleak.h"
#else
#define NEW new
#endif

#ifndef MAX_PATH
#define MAX_PATH 256
#endif

#include <assert.h>
#include <errno.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <algorithm>
#include <deque>
#include <list>
#include <set>
#include <map>
#include <string>
#include <string.h>
#include <vector>
using namespace std;

// handy cleanup methods
#define SAFE_DELETE(p)		    { if(p)	{ delete (p); (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p)	{ if(p) { delete[] (p);	(p)=NULL; } }
#define SAFE_FREE(p)			{ if(p) { free(p); (p)=NULL; } }
#define SAFE_CLOSEHANDLE(p)		{ if(p) { CloseHandle(p); (p)=NULL; } }
#define SAFE_CLOSEFILE(p)		{ if(p) { fclose(p); (p)=NULL; } }
#define SAFE_RELEASEMUTEX(p)	{ if(p) { ReleaseMutex(p); (p)=NULL; } }
// safe print string
// count is the total length of the buffer, thus for 'char text[10]', count is 10
extern void SAFE_SPRINTF(char *buffer, size_t count, const char *format, ...);
// safe print string using variable amount of arguments
extern char* SAFE_VAPRINT(const char *format, ...);
// safe print to debug window
extern void SAFE_DEBUG(const char *format, ...);
// safe print string to file and immediately flush it
extern void SAFE_FLUSHPRINT(FILE *file, const char *format, ...);
// safe read line from file
extern bool SAFE_GETLINE(char *buffer, unsigned int buffersize, unsigned int &linelength, FILE *file);
// safe load file from disk
extern bool SAFE_LOADFILE(const char *fname, unsigned char *&data, unsigned int &length);
extern bool SAFE_LOADFILE(FILE *file, unsigned char *&data, unsigned int &length);

// extract extension of given filename
extern string GetFileExtension(const string& fname);

// get current time/date
// Note: if you need to save the date to disk, ensure to (always and consistently) convert it either
//       to a 32-bit or a 64-bit unsigned integer, otherwise files generated on a 64-bit machine will
//       not properly load on a 32-bit machine and vice versa
typedef time_t t_date;
extern t_date GetCurrentDate();
extern const char* GetDateAsString(t_date date);

// expand path when it starts with a tilde
#if !defined WIN32 && !defined _WIN32 && !defined WIN64 && !defined _WIN64
extern string TildeExpandPath(const string& path);
#endif

#endif