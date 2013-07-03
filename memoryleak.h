#ifndef _MEMORYLEAKH
#define _MEMORYLEAKH

#pragma once

#if defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64

/////////////////////////////////////////////////////////////////////////
// for debugging memory leaks type "{,,msvcr80d.dll}_crtBreakAlloc" in
// the watch window to set the allocation number at which to break
// dynamically or insert the statement "_CrtSetBreakAlloc([alloc#]);"
// at the beginning of the main() function.
/////////////////////////////////////////////////////////////////////////
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#ifdef _DEBUG
#define NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define NEW new
#endif


// memory leak finder
class FindMemoryLeaks
{
#ifdef _DEBUG
public:
	FindMemoryLeaks() {}
	FindMemoryLeaks(long alloc) { _CrtSetBreakAlloc(alloc); }
	~FindMemoryLeaks() { _CrtDumpMemoryLeaks(); }
#endif
};

class FindMemoryLeaksExt
{
#ifdef _DEBUG
public:
	FindMemoryLeaksExt()
	{
		_CrtMemCheckpoint(&m_checkpoint);
	};
	~FindMemoryLeaksExt()
	{
		_CrtMemState checkpoint;
		_CrtMemCheckpoint(&checkpoint);
		_CrtMemState diff;
		if (_CrtMemDifference(&diff, &m_checkpoint, &checkpoint))
			_CrtMemDumpStatistics(&diff);
		//_CrtMemDumpAllObjectsSince(&diff);
	};
private:
	_CrtMemState m_checkpoint;
#endif
};

#endif

#endif
