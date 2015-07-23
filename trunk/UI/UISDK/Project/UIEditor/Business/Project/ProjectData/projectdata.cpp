#include "stdafx.h"
#include "projectdata.h"

CProjectData::CProjectData()
{
    m_pEditUIApp = NULL;
    m_pEditSkinMgr = NULL;
}
CProjectData::~CProjectData()
{
    m_uieditor.Destroy();  // ����UIApp�ͷ�֮ǰ���ͷ�object UIElement
    SAFE_RELEASE(m_pEditUIApp);
}


bool  CProjectData::OpenUIProj(const TCHAR* szPath)
{
    if (!::PathFileExists(szPath))
        return false;

    TCHAR  szDir[MAX_PATH] = _T("");
    TCHAR  szExt[MAX_PATH] = _T("");

    Util::GetPathFileExt(szPath, szExt);
    if (0 != _tcscmp(XML_PROJECT_EXT, szExt))
        return false;

    // ���ļ�����ȡ������
    TCHAR szName[MAX_PATH] = _T("");
    Util::GetPathFileName(szPath, szName);
    TCHAR* p = _tcsstr(szName, _T("."));
    if (p)
        *p = _T('\0');
    m_strName = szName;

    // ����Application
    if (!InitEditUIApplication())
        return false;

	if (!m_uiprojParse.LoadUIProj(szPath))
		return false;

    // ע��������
    this->ReRegistUIObject(false);

    return true;
}

bool  CProjectData::InitEditUIApplication()
{
    CreateUIApplicationInstance(&m_pEditUIApp);   // �༭��Ƥ��

    // ����Ƥ���༭ģʽ
    m_pEditUIApp->SetDesignMode(true);
	m_pEditUIApp->SetUIEditorPtr(&m_uieditor);
    m_pEditSkinMgr = m_pEditUIApp->GetSkinMgr();

    m_pEditUIApp->LogUI(g_pGlobalData->m_pLog);
    return true;
}

// bRestore: ����һ�μ��ع���ʱ����Ҫrestore
// ���޸������ؼ���ʱ����Ҫ�Ȼ�ԭע������
bool  CProjectData::ReRegistUIObject(bool bResotre)
{
    if (bResotre)
    {
        m_pEditUIApp->RestoreRegisterUIObject();
    }

    for (UINT i = 0; i < m_mgrDependsCtrlDll.m_vStrDlls.size(); i++)
    {
        TCHAR szFileName[MAX_PATH] = _T("");
        const TCHAR* szText = m_mgrDependsCtrlDll.m_vStrDlls[i]->strAbsolutePath.c_str();
        
        Util::GetPathFileName(szText, szFileName);

        HMODULE hModule = NULL;
        hModule = ::GetModuleHandle(szFileName);
        if (!hModule)
            hModule = ::LoadLibrary(szText);

        if (hModule)
        {
            funRegisterUIObjectPtr p = (funRegisterUIObjectPtr)GetProcAddress(hModule, "RegisterUIObject");
            if (p)
            {
                p(m_pEditUIApp);
            }
        }
    }
    return true;
}