#include "stdafx.h"
#include "combobox.h"
#include "UISDK\Control\Inc\Interface\ibutton.h"
#include "UISDK\Control\Src\Renderbase\renderbasedefine.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Kernel\Inc\Interface\icolorrender.h"
#include "UISDK\Control\Src\Control\ListBase\PopupControlWindow\popupcontrolwindow.h"
#include "UISDK\Control\Src\Control\ListBox\listbox.h"
#include "UISDK\Kernel\Inc\Interface\iattribute.h"
#include "UISDK\Control\Src\Control\Edit\edit.h"

#define DEFAULT_COMBOBOX_DROPWND_ID  TEXT("DefaultComboBoxDropWnd")
using namespace UI;

ComboBoxBase::ComboBoxBase()
{
    m_pIComboBoxBase = NULL;
	m_pEdit = NULL;
	m_pButton = NULL;
	
    m_pPopupWnd = NULL;
	m_bEditDropList = true;
	m_strDropWndId = DEFAULT_COMBOBOX_DROPWND_ID;
}

ComboBoxBase::~ComboBoxBase()
{
	m_pEdit = NULL;     // �ɸ�Obj�ͷ�
	m_pButton = NULL;   // �ɸ�Obj�ͷ�

    destroyDropWnd();
}

HRESULT  ComboBoxBase::FinalConstruct(IUIApplication* p)
{
	HRESULT hr = DO_PARENT_PROCESS3(IComboBoxBase, IControl);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

void  ComboBoxBase::OnIntialize()
{
    DO_PARENT_PROCESS(IComboBoxBase, IControl);

    if (!m_pEdit)
    {
        m_pEdit = (IEdit*)m_pIComboBoxBase->FindChildObject(
            COMBOBOX_EDIT_ID);
    }
    if (!m_pButton)
    {
        m_pButton = (IButton*)m_pIComboBoxBase->FindChildObject(
            COMBOBOX_BUTTON_ID);
    }
    
    IUIApplication* pUIApp = m_pIComboBoxBase->GetUIApplication();
	ISkinRes* pSkinRes = m_pIComboBoxBase->GetSkinRes();
    if (!m_pEdit)
    {
        IEdit::CreateInstance(pSkinRes, &m_pEdit);
        m_pEdit->SetId(COMBOBOX_EDIT_ID);
        m_pIComboBoxBase->AddChild(m_pEdit);

        ObjStyle s = {0};
        s.transparent = 1;
        m_pEdit->ModifyObjectStyle(&s, 0);
    }

    if (!m_pButton)
    {
        IButton::CreateInstance(pSkinRes, &m_pButton);
        m_pButton->SetId(COMBOBOX_BUTTON_ID);
        m_pIComboBoxBase->AddChild(m_pButton);

        m_pButton->SetDrawFocusType(
            BUTTON_RENDER_DRAW_FOCUS_TYPE_NONE);
        m_pButton->SetAutoSizeType(BUTTON_AUTOSIZE_TYPE_BKIMAGE);
    }


    // �޸�һЩĬ������
    m_pEdit->GetImpl()->SetEditType(EDIT_STYLE_COMBOBOX);
    {
        ObjStyle s = {0};
        s.transparent = 1;
        m_pEdit->ModifyObjectStyle(&s, 0);
    }

    m_pButton->SetTabstop(false);

    m_pButton->ClearNotify();
    m_pButton->AddHook(m_pIComboBoxBase, 0, 
            COMBOBOX_BUTTON_MSG_HOOK_MSG_ID);

    m_pEdit->ClearNotify();
    m_pEdit->AddHook(m_pIComboBoxBase, 0, 
            COMBOBOX_EDIT_MSG_HOOK_MSG_ID);
    m_pEdit->SetNotify(m_pIComboBoxBase, 
            COMBOBOX_EDIT_MSG_HOOK_MSG_ID);

	OnSize(0, m_pIComboBoxBase->GetWidth(), m_pIComboBoxBase->GetHeight());
}

void  ComboBoxBase::FinalRelease()
{
    SetMsgHandled(FALSE);

    if (m_pEdit)
    {
        m_pEdit->RemoveHook(m_pIComboBoxBase);
    }
    if (m_pButton)
    {
        m_pButton->RemoveHook(m_pIComboBoxBase);
    }
}

void  ComboBoxBase::OnCreateByEditor(CREATEBYEDITORDATA* pData)
{
    SetMsgHandled(FALSE);
    pData->rcInitPos.right = pData->rcInitPos.left + 100;
    pData->rcInitPos.bottom = pData->rcInitPos.top + 25;

    EditorAddObjectResData addResData = {0};
    addResData.pUIApp = pData->pUIApp;
    addResData.pParentObj = m_pIComboBoxBase;
    addResData.pParentXml = pData->pXml;

    addResData.objiid = uiiidof(IEdit);
    addResData.ppCreateObj = (void**)&m_pEdit;
	addResData.szId = COMBOBOX_EDIT_ID;
    pData->pEditor->AddObjectRes(&addResData);

    addResData.objiid = uiiidof(IButton);
    addResData.ppCreateObj = (void**)&m_pButton;
	addResData.szId = COMBOBOX_BUTTON_ID;
    pData->pEditor->AddObjectRes(&addResData);
}

void  ComboBoxBase::OnSerialize(SERIALIZEDATA* pData)
{
//    bool bBorderConfiged = pMapAttrib->HasAttrib(XML_BORDER);
	DO_PARENT_PROCESS(IComboBoxBase, IControl);

    {
    AttributeSerializerWrap as(pData, TEXT("ComboBoxBase"));
    as.AddString(XML_COMBOBOX_DROPWNDID, this,
        memfun_cast<pfnStringSetter>(&ComboBoxBase::SetDropWndId),
        memfun_cast<pfnStringGetter>(&ComboBoxBase::GetDropWndId));
    }

#if 0
    IUIApplication* pUIApplication = m_pIComboBoxBase->GetUIApplication();
    if (m_pEdit)
        m_pEdit->SetAttributeByPrefix(XML_COMBOBOX_EDIT_PRIFIX, pMapAttrib, bReload, true);
    if (m_pButton)
        m_pButton->SetAttributeByPrefix(XML_COMBOBOX_BUTTON_PRIFIX, pMapAttrib, bReload, true);

    ITextRenderBase*  pTextRenderBase = NULL;
    pMapAttrib->GetAttr_TextRenderBase(NULL, XML_TEXTRENDER_TYPE, true, pUIApplication, m_pIComboBoxBase, &pTextRenderBase);
    if (pTextRenderBase)
    {
        m_pIComboBoxBase->SetTextRender(pTextRenderBase);
        SAFE_RELEASE(pTextRenderBase);
    }

    if (NULL == m_pIComboBoxBase->GetTextRender())
    {
        pUIApplication->CreateTextRenderBase(
                TEXTRENDER_TYPE_SIMPLE, 
                m_pIComboBoxBase, 
                &pTextRenderBase);

        if (pTextRenderBase)
        {

            SERIALIZEDATA data = {0};
            data.pUIApplication = pUIApplication;
            data.pMapAttrib = pMapAttrib;
            data.szPrefix = NULL;
            data.nFlags = SERIALIZEFLAG_LOAD|
                    SERIALIZEFLAG_LOAD_ERASEATTR;
            pTextRenderBase->Serialize(&data);

            m_pIComboBoxBase->SetTextRender(pTextRenderBase);
            pTextRenderBase->Release();
        }
    }

	// �������� 
	if (NULL == m_pIComboBoxBase->GetBkRender())
	{
        IRenderBase*  pRenderBase = NULL;
        pUIApplication->CreateRenderBase(
                RENDER_TYPE_THEME_COMBOBOX_BKGND, 
                m_pIComboBoxBase, 
                &pRenderBase);

        if (pRenderBase)
        {
            m_pIComboBoxBase->SetBkgndRender(pRenderBase);
			ObjStyle s = {0};
			s.transparent = 1;
		    m_pIComboBoxBase->ModifyObjectStyle(&s, 0);
            SAFE_RELEASE(pRenderBase);

            if (!bBorderConfiged)
            {
                CRegion4 r (2,0,0,0);  // ΪEdit����������
                m_pIComboBoxBase->SetBorderRegion(&r);
            }
        }
	}

	if (NULL == m_pButton->GetBkRender()) 
	{
        IRenderBase*  pRenderBase = NULL;
        pUIApplication->CreateRenderBase(
                RENDER_TYPE_THEME_COMBOBOX_BUTTON, 
                m_pButton, &pRenderBase);
        if (pRenderBase)
        {
            m_pButton->SetBkgndRender(pRenderBase);
			ObjStyle s = {0};
			s.transparent = 1;
            m_pButton->ModifyObjectStyle(&s, 0);
            SAFE_RELEASE(pRenderBase);
        }
	}
#endif
}

void  ComboBoxBase::SetDropWndId(LPCTSTR szId)
{
    if (!szId || 0 == szId[0])
        m_strDropWndId = DEFAULT_COMBOBOX_DROPWND_ID;
    else
        SETSTRING(m_strDropWndId, szId);
}
LPCTSTR  ComboBoxBase::GetDropWndId()
{
    return m_strDropWndId.c_str();
}

void  ComboBoxBase::GetAutoSize(SIZE* pSize)
{
    SIZE s1 = m_pEdit->GetDesiredSize();
	SIZE s2 = m_pButton->GetDesiredSize();

	pSize->cx = s1.cx + s2.cx;
    pSize->cy = max(s1.cy,s2.cy);
}

void ComboBoxBase::OnSize(UINT nType, int cx, int cy)
{
    SetMsgHandled(FALSE);

	CRect  rcClient;
	m_pIComboBoxBase->GetObjectClientRect(&rcClient);

	int nBtnW = 0;
    CRegion4 rcMarginBtn(0, 0, 0, 0);

    if (m_pButton)
    {
        nBtnW = m_pButton->GetWidth();
        if (0 == nBtnW)
        {
            SIZE s = m_pButton->GetDesiredSize();
            nBtnW = s.cx;
        }

        m_pButton->GetMarginRegion(&rcMarginBtn);
        m_pButton->SetObjectPos(
            rcClient.right - nBtnW - rcMarginBtn.right,
            rcClient.top + rcMarginBtn.top, 
            nBtnW,
            rcClient.Height() - rcMarginBtn.Height(),
            SWP_NOREDRAW);
    }

    if (m_pEdit)
    {
	    CRegion4 rcMarginEdit;
	    m_pEdit->GetMarginRegion(&rcMarginEdit);
	    m_pEdit->SetObjectPos(
		    rcClient.left + rcMarginEdit.left,
		    rcClient.top + rcMarginEdit.top, 
		    rcClient.Width()
                - nBtnW 
                - rcMarginEdit.left 
                - rcMarginEdit.right 
                - rcMarginBtn.left 
                - rcMarginBtn.right,
		    rcClient.Height() 
                - rcMarginEdit.top 
                - rcMarginEdit.bottom, 
		    SWP_NOREDRAW);
    }
}

bool  ComboBoxBase::IsDropDown()
{
	if (!m_pPopupWnd)
		return false;

	return m_pPopupWnd->IsVisible() ? true:false;
}

void ComboBoxBase::OnEraseBkgnd(IRenderTarget* pRenderTarget)
{
    IRenderBase* pRenderBase = m_pIComboBoxBase->GetBkRender();
    bool  bReadOnly = !m_bEditDropList;

	if (pRenderBase)
	{
		CRect rc(0,0, m_pIComboBoxBase->GetWidth(), m_pIComboBoxBase->GetHeight());

		if (!m_pIComboBoxBase->IsEnable())
		{
			pRenderBase->DrawState(pRenderTarget, &rc, bReadOnly?COMBOBOX_BKGND_RENDER_STATE_READONLY_DISABLE:COMBOBOX_BKGND_RENDER_STATE_DISABLE);
		}
		else if (m_pIComboBoxBase->IsPress() || IsDropDown() /*|| m_pButton->IsPress() || m_pEdit->IsPress()*/)
		{
			pRenderBase->DrawState(pRenderTarget, &rc, bReadOnly?COMBOBOX_BKGND_RENDER_STATE_READONLY_PRESS:COMBOBOX_BKGND_RENDER_STATE_PRESS);
		}
		else if (m_pIComboBoxBase->IsHover() || m_pEdit->IsFocus() /*|| m_pButton->IsHover() || m_pEdit->IsHover()*/)
		{
			pRenderBase->DrawState(pRenderTarget, &rc, bReadOnly?COMBOBOX_BKGND_RENDER_STATE_READONLY_HOVER:COMBOBOX_BKGND_RENDER_STATE_HOVER);
		}
		else
		{
			pRenderBase->DrawState(pRenderTarget, &rc, bReadOnly?COMBOBOX_BKGND_RENDER_STATE_READONLY_NORMAL:COMBOBOX_BKGND_RENDER_STATE_NORMAL);
		}
	}
}

// ��������COMBOBOX�ƶ���EDIT/BUTTON��ʱˢ��Ϊnormal״̬
void ComboBoxBase::OnStateChanged(UINT nMask)
{
//	this->UpdateObject();

	UIMSG msg;
	msg.message = UI_WM_REDRAWOBJECT;
	msg.pMsgTo = m_pIComboBoxBase;
	UIPostMessage(m_pIComboBoxBase->GetUIApplication(), &msg);
}

// ʹ��
void ComboBoxBase::OnChildObjStateChanged(UINT nMask)
{
	SetMsgHandled(FALSE);  // ����ΪTRUE�ᵼ���ӿؼ�ˢ�²���������EDIT����ʾ���
	
    IObject* pObj = static_cast<IObject*>(GetCurMsg()->pMsgTo);
    if (nMask & (OSB_HOVER | OSB_FORCEHOVER | OSB_PRESS | OSB_FORCEPRESS | OSB_FOCUS))
    {
        m_pIComboBoxBase->SetForceHover(pObj->IsHover()||pObj->IsPress() || pObj->IsFocus(), false);

        // ��Ϊ�ӿؼ����ڱ�HOOK��ûˢ���ˣ���Combobox��ˢ��
        UIMSG msg;
        msg.pMsgFrom = m_pIComboBoxBase;
        msg.pMsgTo = m_pIComboBoxBase;
        msg.message = UI_WM_STATECHANGED2;
        msg.wParam = OSB_FORCEHOVER;
        UIPostMessage(m_pIComboBoxBase->GetUIApplication(), &msg);  // �п����Ǵ��Ӷ������Ƶ�COMBOBOX�����Ҫ�ӳ�ˢ��

        SetMsgHandled(TRUE);
    }
}

LRESULT  ComboBoxBase::OnEditKeyDown(WPARAM wParam, LPARAM)
{
	if (!m_bEditDropList)
		return UI::NOT_HANDLED;

	UIMSG  msg;
	msg.message = UI_WM_NOTIFY;
	msg.nCode = UI_EN_KEYDOWN;
	msg.wParam = wParam;
	msg.pMsgFrom = m_pIComboBoxBase;
	return m_pIComboBoxBase->DoNotify(&msg);
}


LPCTSTR  ComboBoxBase::GetText()
{
    if (m_pEdit)
	    return m_pEdit->GetText();

	return NULL;
}

// �ⲿ�����޸���ʾ�ı�ʱ��Ҳ��Ҫͬ����ǰ�б�ѡ��
void  ComboBoxBase::SetText(LPCTSTR szText, bool bUpdate)
{
	if (m_pEdit)
		m_pEdit->SetTextNoFilter(szText, false);

	// ͬ����ǰѡ����
	UISendMessage(m_pIComboBoxBase, COMBOBOX_MSG_SETTEXT, (WPARAM)szText);

    if (bUpdate)
    {
        m_pIComboBoxBase->UpdateObject();
    }
}

void  ComboBoxBase::SetReadOnly(bool b)
{
    m_bEditDropList = !b;

    if (m_pEdit)
    {
        m_pEdit->SetVisible(m_bEditDropList,false);
        m_pEdit->SetReadOnly(b, false);
    }
}

bool  ComboBoxBase::IsReadOnly()
{
    return !m_bEditDropList;
}

void ComboBoxBase::OnRedrawObject()
{
	m_pIComboBoxBase->UpdateObject();
}

void ComboBoxBase::OnLButtonDown(UINT nFlags, POINT point)
{
	if (!m_bEditDropList)
	{
		if (m_pIComboBoxBase->IsForcePress())
			return;

        DropDown();
	}
}
void ComboBoxBase::OnBtnLButtonDown(UINT nFlags, POINT point)
{
	if (m_pButton->IsForcePress())
		return;
    
	DropDown();
}

void ComboBoxBase::OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
	SetMsgHandled(FALSE);
}

// PopupListBoxWindow��ʾ/����ʱ�����͹�������Ϣ
void ComboBoxBase::OnInitPopupControlWindow()
{
	SetMsgHandled(FALSE);
}
void ComboBoxBase::OnUnInitPopupControlWindow()
{
    SetMsgHandled(FALSE);

	if (m_bEditDropList)
	{
        m_pButton->SetForcePress(false);  		
	}
	else
	{
        m_pIComboBoxBase->SetForcePress(false);   
        m_pIComboBoxBase->UpdateObject();
	}
}


PopupListBoxWindow*  ComboBoxBase::GetDropWnd()
{
	if (m_pPopupWnd)
		return m_pPopupWnd;

	if (m_strDropWndId.empty())
		return NULL;

	do 
	{
		PopupListBoxWindow::CreateInstance(m_pIComboBoxBase->GetSkinRes(), &m_pPopupWnd);

		// ָ��һ�������ڣ������ǲ����ö��ķ�ʽ��������EDIT��������ʱ��listbox�ڵ������뷨����
		if (!m_pPopupWnd->Create(m_strDropWndId.c_str(), m_pIComboBoxBase->GetHWND()))
			break;

		m_pPopupWnd->SetBindObject(m_pIComboBoxBase);
		m_pPopupWnd->SetContentObject(NULL);

		return m_pPopupWnd;
	} while (0);

	destroyDropWnd();
	return NULL;
}

void  ComboBoxBase::destroyDropWnd()
{
	if (m_pPopupWnd)
	{
		m_pPopupWnd->Destroy();
		SAFE_DELETE_Ixxx(m_pPopupWnd);
	}	
}

IObject*  ComboBoxBase::GetContentObject(LPCTSTR szText)
{
	if (!szText)
		return NULL;

	PopupListBoxWindow* pDropWnd = GetDropWnd();
	if (!pDropWnd)
		return NULL;

	IObject* pObj = pDropWnd->FindChildObject(szText);
	pDropWnd->SetContentObject(pObj);

	return pObj;
}

void ComboBoxBase::CloseUp()
{
    UIMSG  msg;
    msg.message = UI_WM_NOTIFY;
    msg.nCode = UI_CBN_CLOSEUP;
    msg.pMsgTo = m_pIComboBoxBase;
    msg.pMsgFrom = m_pIComboBoxBase;

    BOOL bHandle = 0;
    bHandle = UISendMessage(&msg);

	if (!bHandle)
	{
		UIMSG  msg;
		msg.message = UI_WM_NOTIFY;
		msg.nCode = UI_CBN_CLOSEUP;
		msg.pMsgFrom = m_pIComboBoxBase;
		bHandle = m_pIComboBoxBase->DoNotify(&msg);
	}

	if (!bHandle)
	{
		PopupListBoxWindow* pDropWnd = GetDropWnd();
		if (pDropWnd)
			pDropWnd->Hide();
	}
}


void  ComboBoxBase::DropDown()
{
	UIMSG  msg;
	msg.message = UI_WM_NOTIFY;
	msg.nCode = UI_CBN_DROPDOWN;
	msg.pMsgTo = m_pIComboBoxBase;
	msg.pMsgFrom = m_pIComboBoxBase;

	BOOL bHandle = 0;
	bHandle = (BOOL)UISendMessage(&msg);

	// ������û�д����򽻸��ⲿ����
	if (!bHandle)
	{
		msg.pMsgTo = NULL;
		msg.bHandled = FALSE;
		bHandle = (BOOL)m_pIComboBoxBase->DoNotify(&msg);
	}
	if (!bHandle)
	{
		bHandle = defaultDropDown()?TRUE:FALSE;
	}

	if (bHandle)
	{
		if (m_bEditDropList)
		{
			m_pButton->SetForcePress(true);
		}
		if (m_bEditDropList && m_pEdit) // �ɱ༭�����������ø��༭��
		{
			m_pEdit->SetFocusInWindow();
		}
	}
}

bool ComboBoxBase::defaultDropDown()
{
	PopupListBoxWindow*  pDropWnd = GetDropWnd();
	if (!pDropWnd)
		return false;

	HWND hWnd = m_pIComboBoxBase->GetHWND();
	CRect rcWindow;
	m_pIComboBoxBase->GetWindowRect(&rcWindow);

	POINT pt = {rcWindow.left, rcWindow.bottom};
	::MapWindowPoints(hWnd, NULL, (LPPOINT)&pt, 1);

	SIZE size = {rcWindow.Width(), 20};
	SIZE sDesired = pDropWnd->GetDesiredSize(); 
	if (sDesired.cy <= 0)
		return false;
	
// 	if (sDesired.cx > size.cx)
// 		size.cx = sDesired.cx;

	size.cy = sDesired.cy;

	// 	if (m_pButton && m_pButton->IsSelfVisible())
	// 	{
	// 		CRect rcClickFrom;
	// 		m_pButton->GetWindowRect(&rcClickFrom);
	// 		m_pPopupWrapWnd->SetPopupFromInfo(hWnd, &rcClickFrom);
	// 	}
	// 	else
	{
		pDropWnd->SetPopupFromInfo(hWnd, &rcWindow);
	}


	pDropWnd->Show(pt, size, FALSE, SW_SHOWNOACTIVATE);  // <-- ���ﲻҪ��ģ̬�ģ�����ֱ�ӵ�����ڹرհ�ťʱ�ᵼ�±���

	return true;
}
//////////////////////////////////////////////////////////////////////////

ComboBox::ComboBox()
{
    m_pIComboBox = NULL; 
    m_pListCtrl = NULL; 
}

void  ComboBox::FinalRelease()
{
    SetMsgHandled(FALSE);
}

HRESULT  ComboBox::OnDropDown(WPARAM, LPARAM)
{
    if (!m_pListCtrl)
        return HANDLED;

    if (!m_pListCtrl->GetFirstItem())
        return HANDLED;

    SetMsgHandled(FALSE);
    return NOT_HANDLED;
}

LRESULT  ComboBox::OnSetText(UINT, WPARAM wParam, LPARAM)
{
	IListBox*  pListBox = GetListBox();
	if (!pListBox)
		return 0;

	LPCTSTR szText = (LPCTSTR)wParam;
	if (!szText)
	{
		pListBox->ClearSelectItem(true);
		return 0;
	}

	IListItemBase* pMatchItem = pListBox->FindItemByText(szText);
	if (pMatchItem)
	{
		// ���ﲻ��notify����Ϊ��������û����۵��õ�settext����notify sel changed
		// �ͻ�����߼��ظ�
		pListBox->SelectItem(pMatchItem, false, false);
	}
	else
	{
		pListBox->ClearSelectItem(true);
	}
	return 0;
}

bool  ComboBox::AddString(LPCTSTR szText, int nAddItemFlags)
{ 
    if (this->AddStringEx(szText, nAddItemFlags))
        return true;

    return false;
}
IListBoxItem* ComboBox::AddStringEx(LPCTSTR szText, int nAddItemFlags)
{
	IListBox* pListBox = GetListBox();
	if (!pListBox)
		return NULL;

    IListBoxItem* p = pListBox->AddString(szText, nAddItemFlags);
    return static_cast<IListBoxItem*>(p);
}

bool  ComboBox::SetCurSel(int nIndex)
{
	IListBox* pListBox = GetListBox();
	if (!pListBox)
		return false;

    return pListBox->SetSel(nIndex);
}

void  ComboBox::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
	DO_PARENT_PROCESS(IComboBox, IComboBoxBase);

    if (!bReload)
    {
        // ���Ҫ����������EDIT/BUTTON���棬����EDIT��readonly�����ֻᱻ��ʾ����
        bool bReadonly = false;
        if (MAPATTR_RET_NOT_EXIST != pMapAttrib->GetAttr_bool(XML_READONLY, true, &bReadonly))
        {
            SetReadOnly(bReadonly);
        }
    }
}

void  ComboBox::OnPaint(IRenderTarget* pRenderTarget)
{
    if (m_bEditDropList)
		return;

	ITextRenderBase*  pTextRender = m_pIComboBox->GetTextRender();
	if (m_pEdit && pTextRender)
	{
		CRect rc;
		m_pIComboBox->GetObjectClientRect(&rc);
		if (m_pButton && m_pButton->IsSelfVisible())
		{
			RECT rcBtn;
			m_pButton->GetParentRect(&rcBtn);
			rc.right = rcBtn.left;
		}
		pTextRender->DrawState(pRenderTarget, &rc, 0, m_pEdit->GetText());
	}
}


void ComboBox::OnLCNSelChanged(IMessage* pObjMsgFrom, IListItemBase* pOldSelItem, IListItemBase* pSelItem)
{
    if (pSelItem)
    {
		// ����edit������������editֻ����������ʱ��Ҳ������������settext���������ı�
        m_pEdit->SetTextNoFilter(pSelItem->GetText(), false);
        m_pEdit->SetSel(0,-1);   // ȫѡ
    }

    UIMSG   msg;
    msg.message = UI_WM_NOTIFY;
    msg.nCode    = UI_CBN_SELCHANGED;
    msg.wParam  = (WPARAM)pOldSelItem;
    msg.lParam  = (LPARAM)pSelItem;
    msg.pMsgFrom = m_pIComboBox;
    m_pIComboBox->DoNotify( &msg );
}

LRESULT  ComboBox::OnListCtrlClick(WPARAM, LPARAM)
{
    this->CloseUp();
    return 0;
}

LRESULT  ComboBox::OnListCtrlKeyDown(WPARAM vKey, LPARAM)
{
	IListBox* pListBox = GetListBox();
	if (!pListBox)
		return NOT_HANDLED;

	if (vKey == VK_RETURN)
	{
		IListItemBase* pItemHover = pListBox->GetHoverItem();
		IListItemBase* pItemSelect = pListBox->GetFirstSelectItem();

		if (pItemHover && pItemHover!=pItemSelect)
		{
			pListBox->SelectItem(pItemHover, false);
		}
		
		this->CloseUp();
		return HANDLED;
	}
	return NOT_HANDLED;
}

void  ComboBox::ResetContent()
{
	IListBox* pListBox = GetListBox();
	if (pListBox)
		pListBox->RemoveAllItem(LF_NONE);
}
void ComboBox::OnInitPopupControlWindow()
{
    SetMsgHandled(FALSE);

	IListBox* pListBox = GetListBox();
	if (!pListBox)
		return;

    // ������ǰ��ɼ�
	pListBox->SetHoverItem(NULL);  // ɾ����һ�δ�ʱ��hover�ֱ�Ӹ�����ǰѡ����
	IListItemBase* pSel = pListBox->GetFirstSelectItem();
	if (pSel)
	{
		pListBox->MakeItemVisible(pSel, NULL);
	}
}


void ComboBox::OnUnInitPopupControlWindow()
{
	SetMsgHandled(FALSE);
	IListBox* pListBox = GetListBox();
	if (!pListBox)
		return;

	pListBox->SetHoverItem(NULL);  // ��ձ�ѡ�е�item���Է��´��ٴε���ʱ��״̬δ���£���Ȼ��ʾ��һ�ε�hover item��������������listbox��ȡ,listbox��combobox��ȡ,combobox��ʵ���������ڵĴ��ڻ�ȡ��
}

IListBox*  ComboBox::GetListBox() 
{
	if (m_pListCtrl)
		return m_pListCtrl;

#define DefaultListBoxId  TEXT("")
	IObject* pObj = this->GetContentObject(DefaultListBoxId);
	if (!pObj)
		return NULL;

	m_pListCtrl = (IListBox*)pObj->QueryInterface(uiiidof(IListBox));
	if (!m_pListCtrl)
		return NULL;

	ListCtrlStyle s = {0};
	s.popuplistbox = 1;
	m_pListCtrl->ModifyListCtrlStyle(&s, 0);
	m_pListCtrl->SetBindObject(m_pIComboBox);
	m_pListCtrl->AddHook(m_pIComboBox, 0, COMBOBOX_LISTBOX_MSG_HOOK_MSG_ID);
	m_pListCtrl->SetNotify(m_pIComboBox, COMBOBOX_LISTBOX_MSG_HOOK_MSG_ID);

	// ����һ�����߶�
	if (NDEF == m_pListCtrl->GetMaxHeight())
		m_pListCtrl->SetMaxHeight(200); 

	return m_pListCtrl;
}


LRESULT  ComboBox::OnEditChanged(WPARAM wParam, LPARAM)
{
	SetMsgHandled(FALSE);

	if (!m_bEditDropList)
		return 0;

	bool bSetText = wParam?true:false;
	if (bSetText)
		return 0;

	// �û��ֶ��������ݣ��������б���������
	IListBox*  pListBox = GetListBox();
	if (pListBox)
	{
		pListBox->ClearSelectItem(false);
	}
	return 0;
}
