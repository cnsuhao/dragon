#include "stdafx.h"
#include "versionhelper.h"
#include "UISDK\Kernel\Inc\Interface\iuiapplication.h"
#include "UISDK\Control\Inc\Base\inc.h"

using namespace UI;


int  UISDKVersion::GetMajor()
{
	return UISDK_VERSION_MAJOR;
}
int  UISDKVersion::GetMinor()
{
	return UISDK_VERSION_MINOR;
}
int  UISDKVersion::GetPatch()
{
	return UISDK_VERSION_PATCH;
}
int  UISDKVersion::GetVersionText(TCHAR* szText, int nTextSize)
{
	TCHAR szTemp[32] = {0};
	_stprintf(szTemp, TEXT("%d.%d.%d"), GetMajor(), GetMinor(), GetPatch());

	int nDesiredLen = _tcslen(szTemp)+1;
	if (nTextSize < nDesiredLen)
		return nDesiredLen;

	_tcscpy(szText, szTemp);
	return nDesiredLen;
}