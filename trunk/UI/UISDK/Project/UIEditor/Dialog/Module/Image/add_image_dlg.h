#pragma once
#include "Other\Resource.h"


//
//	����һ��image�Ի���
//
class CAddImageDlg : public CDialogImpl<CAddImageDlg>
{
public:
    enum {IDD = IDD_NEW_IMAGE };
    CAddImageDlg(ISkinRes* hSkin, bool bModifyMode=false );
    ~CAddImageDlg();

    BEGIN_MSG_MAP_EX(CAddImageDlg)
        MSG_WM_INITDIALOG( OnInitDialog )
        COMMAND_ID_HANDLER_EX( IDOK, OnOk )
        COMMAND_ID_HANDLER_EX( IDCANCEL, OnCancel )
        COMMAND_ID_HANDLER_EX( IDC_BUTTON_BROWSE, OnBrowse )
        MSG_WM_PAINT( OnPaint )
        END_MSG_MAP()

        void   OnOk( UINT,int,HWND	);
    void   OnCancel( UINT,int,HWND	);
    BOOL   OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
    void   OnBrowse( UINT, int, HWND );
    void   OnPaint(HDC);

    void   SetImageInfo(IImageResItem* pItem);

public:
    CEdit     m_editID;
    CEdit     m_editPath;
    CComboBox  m_comboImageListLayout;
    CEdit     m_editImageListItemCount;

    ISkinRes* m_pSkin;

    String    m_strDataSourceFolder;    // image.xml����·�������ڿ���ͼƬ����Ŀ¼��
    String    m_strId;
    String    m_strPath;
    IImageResItem*  m_pImageResItem;

private:
    bool      m_bModifyMode;  //  ��ǰ���������ڴ��������Ǳ������޸�
    bool      m_bFirstTime;   // 
};
