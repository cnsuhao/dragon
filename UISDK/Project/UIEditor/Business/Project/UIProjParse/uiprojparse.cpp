#include "stdafx.h"
#include "uiprojparse.h"
#include "Business/Project/DependsCtrlDll/dependsctrldll.h"
#include "../ProjectData/projectdata.h"

#define XML_DependsCtrlDll  _T("DependsCtrlDll")
#define XML_SKINLIST        _T("SkinList")
#define XML_SKINRES         _T("SkinRes")

CUIProjParse::CUIProjParse()
{
	m_bDirty = false;
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

const TCHAR*  CUIProjParse::GetUIProjPath()
{
	return m_strPath.c_str();
}

bool CUIProjParse::LoadUIProj(const TCHAR* szPath)
{
    if (NULL == szPath)
        return false;

	CProjectData* pProjectData = GetProjectData();
	UIASSERT(pProjectData);

    if (false == m_xml.Load(szPath))   return false;
    if (false == m_xml.FindElem())     return false;
    if (false == m_xml.IntoElem())     return false;

    m_strPath = szPath;

	TCHAR szDir[MAX_PATH] = {0};
	Util::GetPathDir(szPath, szDir);

    // ����Ƥ��
	if (m_xml.FindElem(XML_SKINLIST) && m_xml.IntoElem())
	{
		while (m_xml.FindElem(XML_SKINRES))
		{
			String strPath = m_xml.GetAttrib(XML_PATH);
			if (Util::IsFullPath(strPath.c_str()))
			{
				pProjectData->AddSkinResPath(strPath.c_str());
			}
			else
			{
				strPath.insert(0, szDir);
				pProjectData->AddSkinResPath(strPath.c_str());
			}
		}
		m_xml.OutOfElem();
	}

	// ����
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

void  CUIProjParse::SetDirty(bool b)
{
	m_bDirty = b;
}

void  CUIProjParse::Save()
{
	if (!m_bDirty)
		return;

    m_xml.ResetPos();
    if (false == m_xml.FindElem())     return ;
	if (false == m_xml.IntoElem())     return ;

	if (m_xml.FindElem(XML_SKINLIST))
	{
		m_xml.RemoveElem();
	}
	if (m_xml.FindElem(XML_DependsCtrlDll))
	{
		m_xml.RemoveElem();
	}

	if (m_xml.AddElem(XML_SKINLIST) && m_xml.IntoElem())
	{
		ISkinManager* pSkinMgr = GetEditSkinManager();
		uint nCount = pSkinMgr->GetSkinResCount();
		for (uint i = 0; i < nCount; i++)
		{
			ISkinRes* pSkinRes = pSkinMgr->GetSkinResByIndex(i);
			LPCTSTR szPath = pSkinRes->GetPath();

			TCHAR szRelativePath[MAX_PATH] = {0};
			Util::CalcRelativePathToFile(m_strPath.c_str(), szPath, szRelativePath);
			m_xml.AddElem(XML_SKINRES);
			m_xml.SetAttrib(XML_PATH, szRelativePath);
		}
		m_xml.OutOfElem();
	}

	if (m_xml.AddElem(XML_DependsCtrlDll) && m_xml.IntoElem())
	{
		for (UINT i = 0; i < GetProjDependsCtrlDllMgr()->m_vStrDlls.size(); i++)
		{
			m_xml.AddElem(XML_ITEM, GetProjDependsCtrlDllMgr()->m_vStrDlls[i]->strRelativePath);
		}
		m_xml.OutOfElem();
	}

	if (m_xml.Save(m_strPath.c_str()))
		m_bDirty = false;
}