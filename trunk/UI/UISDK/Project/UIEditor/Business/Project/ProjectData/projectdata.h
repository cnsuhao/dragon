#pragma once
#include "Business/Project/UIEditor/IUIEditorImpl.h"
#include "Business/Project/UIProjParse/uiprojparse.h"


// 封装一个工程。这样就能方便的实现加载/卸载工程
class CProjectData
{
public:
    CProjectData();
    ~CProjectData();

    bool  OpenUIProj(const TCHAR* szPath);
    bool  ReRegistUIObject(bool bResotre);  

    const TCHAR*  GetProjName()
    {
        return m_strName.c_str();
    }

protected:
    bool  InitEditUIApplication();

public:
    String  m_strName;   // 从m_strPath中获取文件名，得到工程名

    CUIEditor         m_uieditor;
    IUIApplication*   m_pEditUIApp;  // 正在编辑的皮肤
    ISkinManager*     m_pEditSkinMgr;
    CUIProjParse      m_uiprojParse;
    CDependsCtrlDllMgr  m_mgrDependsCtrlDll;
};