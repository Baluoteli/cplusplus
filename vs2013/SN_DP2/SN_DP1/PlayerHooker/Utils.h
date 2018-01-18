#ifndef _UTILS_H_
#define _UTILS_H_

#include <windows.h>
#include <stdio.h>
#include <wtypes.h>
#include <vector>

#include "tstring.h" 


#ifndef __MAX
#   define __MAX(a, b)   (((a) > (b)) ? (a) : (b))
#endif
#ifndef __MIN
#   define __MIN(a, b)   (((a) < (b)) ? (a) : (b))
#endif

#define FREENULL(a) do {if (a != NULL) {free( a ); a = NULL;} } while(0)

#define DELETENULL(a) do {if (a != NULL) {delete( a ); a = NULL;} } while(0)

#define SAFE_DELETE(p)			{ if(NULL != p) { delete (p);     (p)=NULL; } }

#define SAFE_DESTROY(p)			{ if(NULL != p) { p->Destroy();   (p)=NULL; } }

#define SAFE_DELETE_ARRAY(p)	{ if(NULL != p) { delete[] (p);   (p)=NULL; } }

static int Round(double dVal)
{
	if (dVal > 0.0)
	{
		return (int)(dVal + 0.5);
	}
	else
	{
		return (int)(dVal - 0.5);
	}
}

static void ShowDebugInfo(const TCHAR* format,...)
{
#ifdef _DEBUG 

	#define MAX_DBG_MSG_LEN (1024)
	TCHAR buf[MAX_DBG_MSG_LEN];
	va_list ap;

	va_start(ap, format);

	_vsntprintf_s(buf, sizeof(buf), format, ap);

	OutputDebugString(buf);

	va_end(ap);

#endif
}

#ifdef _DEBUG
	#define __dbg_printf ShowDebugInfo
#else
	static void __dbg_printf (const TCHAR* format,...) {}
	#define DBG  1?((void)(NULL)):__dbg_printf
#endif

#define OUTPUT_DEBUG_INFO __dbg_printf

#define _FLP_ "%s:%d: "
#define _FL_ __FUNC__, __LINE__

#define _FFLP_ "%s:" _FLP_
#define _FFL_ __FILE__, _FL_


static int GetBasicAudioVolume(int nVolume)
{
	return Round(pow(10.0, double(nVolume / 2500.0)) * 10001 - 1);
}

tstring ExtractFileName(const tstring& strFilePath);

BOOL StartupProcess(const TCHAR* pProcessName);
BOOL IsProcessRunning(const TCHAR* pProcessName);
BOOL KillProcess(LPTSTR pProcessName);

#endif
