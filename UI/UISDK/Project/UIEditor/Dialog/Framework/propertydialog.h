#pragma once
#include "Other\Resource.h"
#include "ipropertyhandler.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Project\UIEditorCtrl\Inc\ipropertyctrl.h"

namespace UI
{
    interface IPropertyCtrl;
    interface IEdit;
	interface IRadioButton;
    interface IRichEdit;
}

class CPropertyDialog : public IWindow
{
public:
	enum {IDD = IDD_DIALOG_PROPERTY};
	CPropertyDialog(void);
	~CPropertyDialog(void);

 	UI_BEGIN_MSG_MAP_Ixxx(CPropertyDialog)
		UIMSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		UIMSG_WM_DESTROY(OnDestroy)
        UIMSG_WM_NOTIFY_CODE_FROM(UI_LCN_SELCHANGED, m_pPropertyCtrl, OnPropertyCtrlSelChange)

        if (m_pPropertyHandler)
        {
            if (m_pPropertyHandler->ProcessMessage(pMsg, nMsgMapID, false))
                return TRUE;
        }
 	UI_END_MSG_MAP_CHAIN_PARENT(IWindow)


public:
    IListItemBase*  FindPropertyItemByKey(LPCTSTR szKey);
    bool  AttachHandler(IPropertyHandler* p);
	bool  DetachHandler(IPropertyHandler* p);
	IPropertyHandler*  GetHandler();

	IPropertyCtrl*  GetPropertyCtrl(){
		return m_pPropertyCtrl;
	}
	IRichEdit*  GetDescriptRichEdit(){
		return m_pRichEditDescript;
	}
	IEdit*  GetCommandEdit(){
		return m_pCommandEdit;
	}

private:
    void   SetDecsript(LPCTSTR szKey, LPCTSTR  szDesc);
    void   ShowStatusText(LPCTSTR  szText);

private:
	BOOL   OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void   OnClose();
	void   OnDestroy();
    LRESULT  OnPropertyCtrlSelChange(WPARAM w, LPARAM l);

public:
    IPropertyHandler*  m_pPropertyHandler;
	IPropertyCtrl*  m_pPropertyCtrl;
    IRichEdit*  m_pRichEditDescript;
    IEdit*  m_pCommandEdit;
};
