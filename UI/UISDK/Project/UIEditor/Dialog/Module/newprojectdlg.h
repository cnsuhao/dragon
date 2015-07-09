#pragma once
#include "Other\resource.h"

//
//	�½�����/Ƥ���Ի���
//
class CNewProjectDlg : public CDialogImpl<CNewProjectDlg>
{
public:
    enum {IDD = IDD_NEW_PROJECT };
    CNewProjectDlg(bool bCreateProject);
    ~CNewProjectDlg();

    BEGIN_MSG_MAP_EX(CNewProjectDlg)
        MSG_WM_INITDIALOG( OnInitDialog )
        MSG_WM_CLOSE( OnClose )
        COMMAND_ID_HANDLER_EX( IDOK, OnOk )
        COMMAND_ID_HANDLER_EX( IDCANCEL, OnCancel )
        COMMAND_ID_HANDLER_EX( IDC_BUTTON_BROWSE, OnBrowse )
        COMMAND_HANDLER_EX( IDC_EDIT_NEW_SKIN_NAME, EN_CHANGE, OnEnChange_NewSkinName )
        END_MSG_MAP()

public:
    void   OnClose();
    void   OnOk( UINT,int,HWND	);
    void   OnCancel( UINT,int,HWND	);
    BOOL   OnInitDialog(CWindow wndFocus, LPARAM lInitParam);

    void   OnBrowse( UINT, int, HWND );
    void   OnEnChange_NewSkinName(UINT, int, HWND );

public:
    String  m_strProjectFolder;    // ��������Ŀ¼ 
    String  m_strSkinFolder;       // Ƥ���ļ���·��
    String  m_strSkinPath;         // Ƥ��·��
    String  m_strSkinName;         // Ƥ������
    String  m_strUIProjPath;
    bool    m_bCreateProject;      // ��ǰ�Ǵ������̻��ǽ����һ��Ƥ��
};