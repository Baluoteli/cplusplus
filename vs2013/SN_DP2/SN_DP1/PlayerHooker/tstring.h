#ifndef _TSTRING_H
#define _TSTRING_H 

#include <tchar.h>
#include <string>
#include <iostream>   
#include <string>   
#include <sstream> 
#include <ostream>

using namespace std;

#ifndef _TSTRING_DEF_
	#define _TSTRING_DEF_
	
	#ifdef _UNICODE   
		#define tstring std::wstring   
	#else
		#define tstring std::string   
	#endif  
#endif

#ifdef _UNICODE   
	#define tostringstream wostringstream
	#define tistringstream wistringstream
#else
	#define tostringstream ostringstream
	#define tistringstream istringstream
#endif  

static void ToTCHAR(TCHAR* pcDest, wchar_t* pcSource, int nLen)
{
#ifdef _UNICODE
	memcpy(pcDest, pcSource, 2 * nLen);
#else
	char* const pcByteSource = (TCHAR*)pcSource;
	for (int i = 0; i < nLen; i++)
	{
		pcDest[i] = pcByteSource[2 * i + 1];
	}
#endif
}

static void ToTCHAR(TCHAR * pcDest, char* pcSource, int nLen)
{
#ifdef _UNICODE
	for (int i = 0; i < nLen; i++)
	{
		pcDest[i] = pcSource[2 * i + 1];
	}
#else
	memcpy(pcDest, pcSource, nLen);
#endif
}
#endif