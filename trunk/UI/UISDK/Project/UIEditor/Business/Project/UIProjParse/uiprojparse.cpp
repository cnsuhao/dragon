#include "stdafx.h"
#include "uiprojparse.h"
#include "Business/Project/DependsCtrlDll/dependsctrldll.h"
#define XML_DependsCtrlDll  _T("DependsCtrlDll")


CUIProjParse::CUIProjParse()
{
}

CUIProjParse::~CUIProjParse()
{
    Unload();
}

void CUIProjParse::Unload()
{
}

bool CUIProjParse::Create(const TCHAR* szDir, const TCHAR* szName)
{
    return false;
}

bool CUIProjParse::LoadUIProj(const TCHAR* szPath)
{
    if (NULL == szPath)
        return false;

    if (false == m_xml.Load(szPath))   return false;
    if (false == m_xml.FindElem())     return false;
    if (false == m_xml.IntoElem())     return false;

    m_strPath = szPath;

    // ½âÎöÎÄ¼þ
    m_xml.SavePos();

    if (m_xml.FindElem(XML_DependsCtrlDll) && m_xml.IntoElem())
    {
        while (m_xml.FindElem(XML_ITEM))
        {
            String strPath = m_xml.GetData();
            GetProjDependsCtrlDllMgr()->OnLoadItem(strPath.c_str());
        }
    }

    return true;
}

void  CUIProjParse::Save()
{
    m_xml.ResetPos();
    if (false == m_xml.FindElem())     return ;
    if (false == m_xml.IntoElem())     return ;

    m_xml.SavePos();
    bool bNeedSave = false;

    if (GetProjDependsCtrlDllMgr()->m_bDirty)
    {
        if (m_xml.FindElem(XML_DependsCtrlDll))
        {
            m_xml.RemoveElem();
        }
       
        m_xml.AddElem(XML_DependsCtrlDll);
        m_xml.IntoElem();

        for (UINT i = 0; i < GetProjDependsCtrlDllMgr()->m_vStrDlls.size(); i++)
        {
            m_xml.AddElem(XML_ITEM, GetProjDependsCtrlDllMgr()->m_vStrDlls[i]->strRelativePath);
        }
        GetProjDependsCtrlDllMgr()->m_bDirty = false;
        bNeedSave = true;
    }

    if (bNeedSave)
        m_xml.Save(m_strPath.c_str());
}