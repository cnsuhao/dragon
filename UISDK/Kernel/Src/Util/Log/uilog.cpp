#include "stdafx.h"
#include "UISDK\Kernel\Src\Base\Application\uiapplication.h"
using namespace UI;

void  _cdecl UILog(LOG_LEVEL lLevel, LPCTSTR szFile, LPCTSTR szFunction, long lLine, LPCTSTR szFormat, ...)
{
	ILog* pLog = UIApplication::s_pLog;
	if (!pLog)
		return;

	VARIANT_BOOL b = VARIANT_FALSE;
	pLog->TestCanLog(UIApplication::s_lUiLogCookie, lLevel, &b);
	if (VARIANT_FALSE == b)
		return;

	va_list argList;                           
	va_start(argList, szFormat);           

	int nLength = _vsctprintf( szFormat, argList ) + 1;
	TCHAR* pszFormantStack = (TCHAR*)_malloca(nLength*sizeof(TCHAR));
	_vstprintf_s(pszFormantStack, nLength, szFormat, argList);

	LOGDATA data = {0};
	data.lCookie = UIApplication::s_lUiLogCookie;
	data.level = lLevel;
	data.szContent = (OLECHAR*)pszFormantStack;
	data.szFile = (OLECHAR*)szFile;
	data.szFunction = (OLECHAR*)szFunction;
	data.lLine = lLine;
	pLog->Log(&data);   

	_freea(pszFormantStack);
	va_end(argList); 
}

