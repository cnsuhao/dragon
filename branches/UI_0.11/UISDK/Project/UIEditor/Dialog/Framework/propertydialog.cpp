#include "StdAfx.h"
#include "PropertyDialog.h"
#include "UISDK\Kernel\Inc\Interface\ixmlwrap.h"
#include "statusbar.h"
#include "Business\uieditorfunction.h"
#include "UISDK\Control\Inc\Interface\irichedit.h"
#include "UISDK\Control\Inc\Interface\iedit.h"


CPropertyDialog::CPropertyDialog(void)
{
    m_pPropertyHandler = NULL;
	m_pPropertyCtrl = NULL;
    m_pCommandEdit = NULL;
	g_pGlobalData->m_pPropertyDialog = this;
    m_pRichEditDescript = NULL;
}

CPropertyDialog::~CPropertyDialog(void)
{
	if (g_pGlobalData)
	{
		g_pGlobalData->m_pPropertyDialog = NULL;
	}
}

BOOL  CPropertyDialog::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	m_pPropertyCtrl = (IPropertyCtrl*)this->FindChildObject(_T("propertyctrl"));
    m_pCommandEdit = (IEdit*)this->FindChildObject(_T("edit_command"));
    m_pRichEditDescript = (IRichEdit*)this->FindChildObject(_T("property_desc"));

    if (m_pCommandEdit)
    {
        SetFocusObject(m_pCommandEdit);
        m_pCommandEdit->ModifyStyleEx(EDIT_STYLE_WANTTAB, 0, false);
    }
    if (m_pRichEditDescript)
    {
        m_pRichEditDescript->SetReadOnly(true);
    }

	return TRUE;
}

void  CPropertyDialog::OnClose()
{
	::ShowWindow(GetHWND(), SW_HIDE);
}
void  CPropertyDialog::OnDestroy()
{
    if (m_pPropertyHandler)
    {
        m_pPropertyHandler->OnUnload();
        m_pPropertyHandler = NULL;
    }
	m_pPropertyCtrl = NULL;
}

// 获取指定key字段的属性项回调函数
bool ObjXmlPropertyCtrlListItemEnumProc(IListItemBase* pItem, WPARAM w, LPARAM l)
{
    LPCTSTR szKey = (LPCTSTR)w;
    LPCTSTR szItemKey = (LPCTSTR)UISendMessage(pItem, UI_PROPERTYCTRLITEM_MSG_GETKEYSTRING);
    if (!szItemKey)
        return true;

    if (0 == _tcscmp(szKey, szItemKey))
        return false;

    return true;
}
IListItemBase*  CPropertyDialog::FindPropertyItemByKey(LPCTSTR szKey)
{
    IListItemBase* pItem = m_pPropertyCtrl->EnumItemByProc(ObjXmlPropertyCtrlListItemEnumProc, NULL, (WPARAM)szKey,NULL);    
    return pItem;
}

void  CPropertyDialog::ShowStatusText(LPCTSTR  szText)
{
    g_pGlobalData->m_pStatusBar->SetStatusText2(szText);
}

LRESULT  CPropertyDialog::OnPropertyCtrlSelChange(WPARAM w, LPARAM l)
{
	IListItemBase*  pItem = m_pPropertyCtrl->GetFirstSelectItem();
	if (!pItem)
	{
		SetDecsript(NULL,NULL);
		return 0;
	}

    String strDesc;
    if (pItem->GetToolTip())
        strDesc.append(pItem->GetToolTip());

    // 显示内容，可方便复制
    LPCTSTR szValue = (LPCTSTR)UISendMessage(
            pItem, UI_PROPERTYCTRLITEM_MSG_GETVALUESTRING);
    if (szValue)
        strDesc.append(szValue);
	SetDecsript(pItem->GetText(), strDesc.c_str());
	return 0;
}

void   CPropertyDialog::SetDecsript(LPCTSTR szKey, LPCTSTR  szDesc)
{
	m_pRichEditDescript->SetText(_T(""));
	if (!szKey)
		return ;

	m_pRichEditDescript->AppendText(szKey, -1);

	// 粗体字
	CHARFORMAT2 cf;
	memset(&cf, 0, sizeof(cf));
	cf.cbSize = sizeof(cf);
	cf.dwMask = CFM_BOLD;
	cf.dwEffects |= CFE_BOLD;
	m_pRichEditDescript->SetSel(0, _tcslen(szKey));
	m_pRichEditDescript->SetCharFormatEx(SCF_SELECTION, &cf);
	m_pRichEditDescript->SetSel(0, 0);

	// 描述内容
	m_pRichEditDescript->AppendText(_T("\r\n"), -1);
	if (szDesc)
	{
		m_pRichEditDescript->AppendText(szDesc);	
	}
}

IPropertyHandler*  CPropertyDialog::GetHandler()
{
	return m_pPropertyHandler;
}

bool  CPropertyDialog::AttachHandler(IPropertyHandler* p)
{
	if (m_pPropertyHandler == p)
		return true;

    if (m_pPropertyHandler)
    {
        DetachHandler(m_pPropertyHandler);
    }
    m_pPropertyHandler = p;
    if (m_pPropertyHandler)
    {
        m_pPropertyHandler->OnLoad(this);
    }

    return true;
}

bool  CPropertyDialog::DetachHandler(IPropertyHandler* p)
{
	if (!p || m_pPropertyHandler != p)
		return false;

	m_pPropertyHandler->OnUnload();
	m_pPropertyHandler = NULL;

	return true;
}