#pragma once
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Control\Inc\Interface\iedit.h"
#include "UISDK\Control\Inc\Interface\icombobox.h"
namespace UI{ interface IButton; }

class CCreateWindowDlg : public IWindow
{
public:
    CCreateWindowDlg();
    ~CCreateWindowDlg();

    UI_BEGIN_MSG_MAP_Ixxx(CCreateWindowDlg)
        UIMSG_BN_CLICKED2(m_pBtnOk, OnOk)
        UIMSG_WM_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT(IWindow)


protected:
    void  OnInitialize();
    void  OnOk();

    bool  CreateNewWindowXml(const TCHAR* szPath);
    bool  InsertNewWindowNode2Xml(const TCHAR* szXmlPath, const TCHAR* szNodeName, const TCHAR* szId);
	bool  InsertNewWindowNode2Xml2(IUIDocument* pDoc, const TCHAR* szNodeName, const TCHAR* szId);
    bool  InsertInclude2SkinXml(ISkinRes*  pSkin, const TCHAR* szXmlPath);

private:
    IComboBox*  m_pComboSkinList;
    IComboBox*  m_pComboSkinFileList;
    IComboBox*  m_pComboClassList;
    IEdit*      m_pEditId;
    IButton*    m_pBtnOk;
};