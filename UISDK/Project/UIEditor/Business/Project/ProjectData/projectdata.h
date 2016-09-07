#pragma once
#include "Business/Project/UIEditor/IUIEditorImpl.h"
#include "Business/Project/UIProjParse/uiprojparse.h"


// ��װһ�����̡��������ܷ����ʵ�ּ���/ж�ع���
class CProjectData
{
public:
    CProjectData();
    ~CProjectData();

    bool  OpenUIProj(const TCHAR* szPath);
    bool  ReRegistUIObject(bool bResotre);  

    LPCTSTR  GetProjName();
	void  AddSkinResPath(LPCTSTR szPath);
// 	uint  GetSkinResPathCount();
// 	LPCTSTR  GetSkinResPathByIndex(uint);

protected:
    bool  InitEditUIApplication();

public:
    String  m_strName;   // ��m_strPath�л�ȡ�ļ������õ�������
//	vector<String>    m_vSkinResPath;  // Ƥ����Դ��

    CUIEditor         m_uieditor;
    IUIApplication*   m_pEditUIApp;  // ���ڱ༭��Ƥ��
    ISkinManager*     m_pEditSkinMgr;
    CUIProjParse      m_uiprojParse;
    CDependsCtrlDllMgr  m_mgrDependsCtrlDll;
};