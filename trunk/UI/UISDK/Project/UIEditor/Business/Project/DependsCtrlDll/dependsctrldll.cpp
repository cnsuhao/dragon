#include "stdafx.h"
#include "dependsctrldll.h"
#include "Business/Project/ProjectData/projectdata.h"

CDependsCtrlDllMgr::CDependsCtrlDllMgr()
{
    m_bDirty = false;
}

CDependsCtrlDllMgr::~CDependsCtrlDllMgr()
{
    vector<DependsCtrlDllItem*>::iterator iter = m_vStrDlls.begin();
    for (; iter != m_vStrDlls.end(); iter++)
    {
        DependsCtrlDllItem* p = *iter;
        SAFE_DELETE(p);
    }
    m_vStrDlls.clear();
}

bool CDependsCtrlDllMgr::AddCtrlDll(const TCHAR* szAbsolutePath)
{
    if (NULL == szAbsolutePath)
        return false;

    vector<DependsCtrlDllItem*>::iterator iter = m_vStrDlls.begin();
    for (; iter != m_vStrDlls.end(); iter++)
    {
        if (0 == _tcscmp((*iter)->strAbsolutePath.c_str(), szAbsolutePath))
            return false;
    }

    DependsCtrlDllItem* pItem = new DependsCtrlDllItem;
    pItem->strAbsolutePath = szAbsolutePath;

    TCHAR szRelativePath[MAX_PATH] = _T("");
    Util::CalcRelativePathToFile(GetProjectData()->m_uiprojParse.GetUIProjPath(),
        szAbsolutePath, szRelativePath);
    pItem->strRelativePath = szRelativePath;

    m_vStrDlls.push_back(pItem);
    
    m_bDirty = true;
    return true;
}

bool CDependsCtrlDllMgr::DelCtrlDll(const TCHAR* szPath)
{
    if (NULL == szPath)
        return false;

    vector<DependsCtrlDllItem*>::iterator iter = m_vStrDlls.begin();
    for (; iter != m_vStrDlls.end(); iter++)
    {
        if (0 == _tcscmp((*iter)->strAbsolutePath.c_str(), szPath))
        {
            DependsCtrlDllItem* pItem = *iter;
            SAFE_DELETE(pItem);

            m_vStrDlls.erase(iter);
            m_bDirty = true;
            return true;
        }
    }

    return false;
}


void  CDependsCtrlDllMgr::OnLoadItem(const TCHAR* szPath)
{
    if (!szPath)
        return;

    DependsCtrlDllItem* pNewItem = new DependsCtrlDllItem;
    if (Util::IsFullPath(szPath))
    {
        pNewItem->strAbsolutePath = szPath;
        pNewItem->strRelativePath = szPath;
    }
    else
    {
        TCHAR szUIProjDir[MAX_PATH] = _T("");
        TCHAR szAbsolutePath[MAX_PATH] = _T("");

        Util::CalcFullPathByRelative(GetProjectData()->m_uiprojParse.GetUIProjPath(), szPath, szAbsolutePath);

        pNewItem->strRelativePath = szPath;
        pNewItem->strAbsolutePath = szAbsolutePath;
    }
    if (!PathFileExists(pNewItem->strAbsolutePath.c_str()))
    {
        delete pNewItem;
        m_bDirty = true;
        return;
    }
    m_vStrDlls.push_back(pNewItem);
}