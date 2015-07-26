#pragma once
#include "Business/Project/DependsCtrlDll/dependsctrldll.h"
#include "3rd/markup/markup.h"

class CUIProjParse
{
public:
    CUIProjParse();
    ~CUIProjParse();

    bool  Create(const TCHAR* szDir, const TCHAR* szName);
    bool  LoadUIProj(const TCHAR* szPath);
    void  Unload();
    void  Save();

	void  SetDirty(bool b);
    LPCTSTR  GetUIProjPath();

private:
    String   m_strPath;
    CMarkup  m_xml;
    
	bool  m_bDirty;
};