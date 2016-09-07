#include "stdafx.h"
#include "button.h"
#include <atltypes.h>

#include "UISDK\Control\Src\Renderbase\renderbasedefine.h"
#include "UISDK\Kernel\Inc\Interface\iimagerender.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Kernel\Inc\Interface\iattribute.h"

using namespace UI;


ButtonBase::ButtonBase()
{
    m_pIButtonBase = NULL;
	m_nCheckState = BST_UNCHECKED;
	m_bSpaceKeyDown = false;
	m_lButtonType = BUTTON_STYLE_PUSHBUTTON;
}


//	��ť������
long  ButtonBase::GetButtonType()
{
	return m_lButtonType;
}
void ButtonBase::SetButtonType(long n)
{
	m_lButtonType = n;
}

void ButtonBase::OnEraseBkgnd(IRenderTarget* pRenderTarget)
{
    IRenderBase* pBkgndRender = m_pIButtonBase->GetBkRender();
	if (pBkgndRender)
	{
		bool  bDisable = !m_pIButtonBase->IsEnable();
		bool  bHover   = m_pIButtonBase->IsHover();
		bool  bPress   = m_pIButtonBase->IsPress();
		bool  bForePress = m_pIButtonBase->IsForcePress();
		bool  bChecked = IsChecked();
        bool  bDefault = m_pIButtonBase->IsDefault();

		CRect rc(0,0, m_pIButtonBase->GetWidth(), m_pIButtonBase->GetHeight());

		if (bDisable)
		{
			pBkgndRender->DrawState(pRenderTarget, &rc, bChecked?BUTTON_BKGND_RENDER_STATE_SELECTED_DISABLE:BUTTON_BKGND_RENDER_STATE_DISABLE);
		}
		else if (bForePress || (bPress && bHover))
		{
			pBkgndRender->DrawState(pRenderTarget, &rc, bChecked?BUTTON_BKGND_RENDER_STATE_SELECTED_PRESS:BUTTON_BKGND_RENDER_STATE_PRESS);
		}
		else if (bHover || bPress)
		{
			pBkgndRender->DrawState(pRenderTarget, &rc, bChecked?BUTTON_BKGND_RENDER_STATE_SELECTED_HOVER:BUTTON_BKGND_RENDER_STATE_HOVER);
		}
        else if (bDefault)
        {
            pBkgndRender->DrawState(pRenderTarget, &rc, BUTTON_BKGND_RENDER_STATE_DEFAULT);
        }
		else 
		{
			pBkgndRender->DrawState(pRenderTarget, &rc, bChecked?BUTTON_BKGND_RENDER_STATE_SELECTED_NORMAL:BUTTON_BKGND_RENDER_STATE_NORMAL);
		}
	}

	if (m_pIButtonBase->IsFocus())  // ��focus����������Ϊ�˱���������paddingʱ,��onpaint�л���focus rectҲ���ܵ�Ӱ��
	{
        LRESULT lRet = ::SendMessage(m_pIButtonBase->GetHWND(), WM_QUERYUISTATE, 0, 0);
        if (!(lRet & UISF_HIDEFOCUS))
        {
    		this->DrawFocus(pRenderTarget);
        }
	}	
}


void ButtonBase::OnStateChanged2(UINT nMask)
{
	m_pIButtonBase->UpdateObject();
}

// void ButtonBase::OnMouseMove(UINT nFlags, POINT point)
// {
// 	// �ж����ʱ�����������������ڰ���״̬��������ڶ������棬���ʱ��Ҫ��ʾhover״̬
// 	bool bOut = false;
// 	bOut = ( HTNOWHERE == this->OnHitTest( &point ) );
// 	if( bOut )
// 	{
// 		if( IsHover() )
// 		{
// 			this->SetHover( false );
// 			this->UpdateObject( );
// 		}
// 	}
// 	else
// 	{
// 		if( !IsHover() )
// 		{
// 			this->SetHover( true );
// 			this->UpdateObject( );
// 		}
// 	}
// 	
// }
// void ButtonBase::OnMouseLeave()
// {
// 	this->SetHover( false );
// 	this->SetPress( false );
// 	this->UpdateObject( );
// }
// void ButtonBase::OnLButtonDown(UINT nFlags, POINT point)
// {
// 	this->SetPress( true );
// 	this->UpdateObject( );
// }
void ButtonBase::OnLButtonDblClk(UINT nFlags, POINT point)
{
	// Ϊ����Hook���ص���һ�εĵ���¼�����˫���������ϵİ�ť
	UIMSG  msg;
	msg.pMsgTo = m_pIButtonBase;
	msg.message = WM_LBUTTONDOWN;
	msg.wParam  = (WPARAM)nFlags;
	msg.lParam  = MAKELPARAM(point.x,point.y);
	::UISendMessage( &msg );  
}

void  ButtonBase::OnLButtonDown(UINT nFlags, POINT point)
{
    if (m_pIButtonBase->TestStyleEx(BUTTON_STYLE_CLICK_ONDOWN))
    {
        this->OnClicked(&point);  
    }   
}
void ButtonBase::OnLButtonUp(UINT nFlags, POINT point)
{
    if (m_pIButtonBase->TestStyleEx(BUTTON_STYLE_CLICK_ONDOWN))
        return;

    if (m_pIButtonBase->IsHover())
    {
        this->OnClicked(&point);
    }

//  IObject* pParent = m_pIButtonBase->GetParentObject();
//  if (NULL == pParent)
//      return;
// 
// 	POINT ptObj = {0,0};
// 	pParent->WindowPoint2ObjectClientPoint(&point, &ptObj, true);
// 
//     // UI_WM_HITTEST���ж����Լ���m_rcParent�Ľ��������Ҫ����������ת����parent��client����
//     UINT nHitTest = UISendMessage(m_pIButtonBase, UI_WM_HITTEST, (WPARAM)&ptObj);
//     if (HTNOWHERE != nHitTest)
// 	{
// 		this->OnClicked(&point);   // ��ע������DoNotify���ܵ��µ�ǰpress hover�������ı䣬ʹ�ñ��ؼ���ʧˢ��
// 	}
}
void  ButtonBase::OnRButtonUp(UINT nFlags, POINT point)
{
	UIMSG   msg;
	msg.message = UI_WM_NOTIFY;
	msg.nCode   = UI_BN_RCLICK;
	msg.wParam  = NULL; //(WPARAM)pt;
	msg.lParam  = NULL;
	msg.pMsgFrom = this->m_pIButtonBase;

	m_pIButtonBase->DoNotify(&msg);
}

void ButtonBase::OnClicked(POINT* pt)
{
	// ֪ͨ��Ϣ
	UIMSG   msg;
	msg.message = UI_WM_NOTIFY;
	msg.nCode   = UI_BN_CLICKED;
	msg.wParam  = NULL; //(WPARAM)pt;
	msg.lParam  = NULL;
	msg.pMsgFrom = this->m_pIButtonBase;
    
    msg.pMsgTo = m_pIButtonBase;
    BOOL bHandled = FALSE;
    UISendMessage(&msg, 0, &bHandled); // ���Լ�����������д���Ҳ��������ȥ֪ͨ���ڣ�����һ��UI_BN_CLICKED��Ϣ����ͳһ����
}

void  ButtonBase::DrawFocus(IRenderTarget* p)
{
    UIMSG msg;
    msg.pMsgTo = m_pIButtonBase;
    msg.wParam = (WPARAM)p;
    msg.nCode = UI_BN_DRAWFOCUS;
    msg.message = UI_WM_NOTIFY;

    UISendMessage(&msg);
}

void ButtonBase::OnSetFocus(IObject*)
{
}
void ButtonBase::OnKillFocus(IObject*)
{
    if (Util::IsKeyDown(VK_SPACE))
    {
        bool bIsForePress = m_pIButtonBase->IsForcePress();
        m_pIButtonBase->clearStateBit(OSB_FORCEPRESS);
        if (bIsForePress)
        {
            m_pIButtonBase->UpdateObject();
        }
    }
}
void ButtonBase::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if (VK_SPACE == nChar && m_pIButtonBase->IsEnable())
	{
        // alt+space�ǵ������ڵ�sysmenu
        if (Util::IsKeyDown(VK_SHIFT))
            return;
        if (Util::IsKeyDown(VK_CONTROL))
            return;
        if (Util::IsKeyDown(VK_MENU))
            return;

        // windows�ؼ���SPACE����ʱ����ȥΪButton����xxxBNSetCapture��
        // �������е������Ϣ��ֻ�ܵ���������
		m_pIButtonBase->setStateBit(OSB_FORCEPRESS);
		m_pIButtonBase->UpdateObject();

        m_pIButtonBase->SetMouseCapture(BUTTON_VK_SPACE_MOUSECAPTURE_NOTIFY_ID);
	}
}
void ButtonBase::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if (VK_SPACE == nChar && m_pIButtonBase->IsEnable() && m_pIButtonBase->IsForcePress())
	{
		m_pIButtonBase->clearStateBit(OSB_FORCEPRESS);
        m_pIButtonBase->ReleaseMouseCapture();

		if (!Util::IsKeyDown(VK_LBUTTON))
		{
			// ����������չ
            POINT pt = {0,0};
            UIMSG* pCurMsg = m_pIButtonBase->GetCurMsg();
            if (pCurMsg)
                pt = pCurMsg->pt;

			this->OnClicked(&pt);

		}
		m_pIButtonBase->UpdateObject();
	}
}

// �ڰ��¿ո����������϶�����״̬�������ղ���keyup��Ϣ��
LRESULT  ButtonBase::OnEnterSizeMove(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SetMsgHandled(FALSE);

	if (m_pIButtonBase->IsEnable())
	{
		m_pIButtonBase->clearStateBit(OSB_FORCEPRESS);
		m_pIButtonBase->ReleaseMouseCapture();

		m_pIButtonBase->UpdateObject();
	}
	return 0;
}

void ButtonBase::SetCheck(int nCheckState)
{
	if(nCheckState == BST_CHECKED)
		this->SetChecked();
	else
		this->SetUnChecked();
}

void  ButtonBase::SetCheck(bool bCheck, bool bUpdate)
{
    if (bCheck)
        this->SetChecked();
    else
        this->SetUnChecked();

    if (bUpdate)
    {
        m_pIButtonBase->UpdateObject();
    }
}

void ButtonBase::SetChecked()
{
	if (m_nCheckState & BST_CHECKED)
	{
	}
	else
	{
		m_nCheckState |= BST_CHECKED;
	}
}

void ButtonBase::SetUnChecked()
{
	if ( m_nCheckState & BST_CHECKED )
	{
		m_nCheckState &= ~BST_CHECKED;
	}
	else
	{

	}
}
int  ButtonBase::GetCheck()
{
	return m_nCheckState;
}
bool ButtonBase::IsChecked()
{
	return (m_nCheckState & BST_CHECKED) ? true:false;
}


//////////////////////////////////////////////////////////////////////////
Button::Button()
{
    m_pIButton = NULL;
    m_eAutoSizeType = BUTTON_AUTOSIZE_TYPE_CONTENT;
    m_eIconAlignFlag = ALIGN_LEFT;
    m_lIconMarginText = 0;
    m_eDrawFocusType = BUTTON_RENDER_DRAW_FOCUS_TYPE_NONE;
}
Button::~Button()
{
}

void  Button::OnInitialize()
{
    SetMsgHandled(FALSE);

    // ����Լ��Ƿ���DefaultPushButton
	if (m_pIButtonBase->TestStyleEx(BUTTON_STYLE_DEFPUSH))
	{
		IWindowBase* pWindow = m_pIButton->GetWindowObject();
		if (pWindow)
		{
			UISendMessage(pWindow, 
				UI_DM_SETDEFID, (WPARAM)m_pIButton);
		}
	}
}

void  Button::SetIconFromFile(LPCTSTR szIconPath)
{
    m_pIButton->SetForegndRender(NULL);
    if (NULL == szIconPath)
        return;

    IUIApplication*  pUIApplication = m_pIButton->GetUIApplication();
    if (NULL == pUIApplication)
    {
        UIASSERT(0);
        return;
    }

    IRenderBase*   pForeRender = NULL;
    pUIApplication->CreateRenderBase(RENDER_TYPE_IMAGE, m_pIButton, &pForeRender);
    IImageRender*  pImageForeRender = (IImageRender*)pForeRender->QueryInterface(uiiidof(IImageRender));

    UI::IRenderBitmap*  pRenderBitmap = NULL;
    UI::UICreateRenderBitmap(pUIApplication, m_pIButton->GetGraphicsRenderLibraryType(), IMAGE_ITEM_TYPE_IMAGE, &pRenderBitmap);
    pRenderBitmap->LoadFromFile(szIconPath, true);
    pImageForeRender->SetRenderBitmap(pRenderBitmap);
    pImageForeRender->SetImageDrawType(UI::DRAW_BITMAP_CENTER);
    SAFE_RELEASE(pRenderBitmap);

    m_pIButton->SetForegndRender(pForeRender);
    SAFE_RELEASE(pForeRender);
}

void  Button::SetIconFromImageId(LPCTSTR szImageId)
{
    m_pIButton->SetForegndRender(NULL);
    if (NULL == szImageId)
        return;

    IUIApplication*  pUIApplication = m_pIButton->GetUIApplication();
    if (NULL == pUIApplication)
    {
        UIASSERT(0);
        return;
    }

    UI::IRenderBitmap*  pRenderBitmap = NULL;
    pUIApplication->GetActiveSkinImageRes()->GetBitmap(
        szImageId, 
        m_pIButton->GetGraphicsRenderLibraryType(), 
        &pRenderBitmap);
    if (!pRenderBitmap)
        return;

    IRenderBase*   pForeRender = NULL;
    pUIApplication->CreateRenderBase(RENDER_TYPE_IMAGE, m_pIButton, &pForeRender);
    IImageRender*  pImageForeRender = (IImageRender*)pForeRender->QueryInterface(uiiidof(IImageRender));

    pImageForeRender->SetRenderBitmap(pRenderBitmap);
    pImageForeRender->SetImageDrawType(UI::DRAW_BITMAP_CENTER);
    SAFE_RELEASE(pRenderBitmap);

    m_pIButton->SetForegndRender(pForeRender);
    SAFE_RELEASE(pForeRender);
}

// ��HBITMAP���ᱻRenderBitmap������
void  Button::SetIconFromHBITMAP(HBITMAP hBitmap)
{
    m_pIButton->SetForegndRender(NULL);
    if (NULL == hBitmap)
        return;

    IUIApplication*  pUIApplication = m_pIButton->GetUIApplication();
    if (NULL == pUIApplication)
    {
        UIASSERT(0);
        return;
    }

    IRenderBase*   pForeRender = NULL;
    pUIApplication->CreateRenderBase(RENDER_TYPE_IMAGE, m_pIButton, &pForeRender);
    IImageRender*  pImageForeRender = (IImageRender*)pForeRender->QueryInterface(uiiidof(IImageRender));

    UI::IRenderBitmap*  pRenderBitmap = NULL;
    UI::UICreateRenderBitmap(pUIApplication, UI::GRAPHICS_RENDER_LIBRARY_TYPE_GDI, IMAGE_ITEM_TYPE_IMAGE, &pRenderBitmap);
    pRenderBitmap->Attach(hBitmap, true);
    pImageForeRender->SetRenderBitmap(pRenderBitmap);
    pImageForeRender->SetImageDrawType(UI::DRAW_BITMAP_CENTER);
    SAFE_RELEASE(pRenderBitmap);

    m_pIButton->SetForegndRender(pForeRender);
    SAFE_RELEASE(pForeRender);
}

LPCTSTR  Button::GetText() 
{ 
    return m_strText.c_str(); 
}

LPCTSTR  Button::SaveText()
{
	if (m_strText.empty()) // �գ�������xml����
		return NULL;
	return m_strText.c_str();
}

void  Button::SetText(LPCTSTR  szText) 
{
    SETSTRING(m_strText, szText);
}

void  Button::SetTextAndUpdate(LPCTSTR  szText)
{
    SetText(szText);
    m_pIButton->UpdateObject();
}

void  Button::GetDesiredSize(SIZE* pSize)
{
    pSize->cx = pSize->cy = 0;

    // ��ť��auto size���԰�����ͼ���С���㣬Ҳ���԰����ݵĴ�С����
    switch (m_eAutoSizeType)
    {
    case BUTTON_AUTOSIZE_TYPE_BKIMAGE:
        {
            if (m_pIButton->GetBkRender())
            {
                *pSize = m_pIButton->GetBkRender()->GetDesiredSize( );
            }
        }
        break;

    case BUTTON_AUTOSIZE_TYPE_CONTENT:
        {
            REGION4 rcNonClient;
            m_pIButton->GetNonClientRegion(&rcNonClient);

            pSize->cx = rcNonClient.left+rcNonClient.right;
            pSize->cy = rcNonClient.top+rcNonClient.bottom;

            SIZE sIconText = {0,0};
            {
                SIZE sizeIcon = {0,0};
                SIZE sizeText = {0,0};

                if (m_pIButton->GetForeRender())
                {
                    sizeIcon = m_pIButton->GetForeRender()->GetDesiredSize();
                }
                if (GetTextRender())
                {
                    sizeText = GetTextRender()->GetDesiredSize(m_strText.c_str());
                }

                switch (m_eIconAlignFlag)
                {
                case ALIGN_LEFT:
                case ALIGN_RIGHT:
                    {
                        sIconText.cx = sizeIcon.cx + sizeText.cx + m_lIconMarginText;
                        sIconText.cy = max( sizeIcon.cy, sizeText.cy );
                    }
                    break;
                case ALIGN_TOP:
                case ALIGN_BOTTOM:
                    {
                        sIconText.cx = max( sizeIcon.cx, sizeText.cx );
                        sIconText.cy = sizeIcon.cy + sizeText.cy + m_lIconMarginText;
                    }
                    break;
                }
            }
            pSize->cx += sIconText.cx;
            pSize->cy += sIconText.cy;
        }
        break;

    case BUTTON_AUTOSIZE_TYPE_BKIMAGEHEIGHT_CONTENTWIDTH:
        {
            SIZE sHeight = {0,0};
            if (m_pIButton->GetBkRender())
                sHeight = m_pIButton->GetBkRender()->GetDesiredSize( );

            SIZE sWidth;
            REGION4 rcNonClient;
            m_pIButton->GetNonClientRegion(&rcNonClient);

            sWidth.cx = rcNonClient.left+rcNonClient.right;
            sWidth.cy = rcNonClient.top+rcNonClient.bottom;

            SIZE sIconText = {0,0};
            {
                SIZE sizeIcon = {0,0};
                SIZE sizeText = {0,0};

                if (m_pIButton->GetForeRender())
                {
                    sizeIcon = m_pIButton->GetForeRender()->GetDesiredSize(  );
                }
                if (GetTextRender())
                {
                    sizeText = GetTextRender()->GetDesiredSize(m_strText.c_str());
                }

                switch (m_eIconAlignFlag)
                {
                case ALIGN_LEFT:
                case ALIGN_RIGHT:
                    {
                        sIconText.cx = sizeIcon.cx + sizeText.cx + m_lIconMarginText;
                        sIconText.cy = max( sizeIcon.cy, sizeText.cy );
                    }
                    break;
                case ALIGN_TOP:
                case ALIGN_BOTTOM:
                    {
                        sIconText.cx = max( sizeIcon.cx, sizeText.cx );
                        sIconText.cy = sizeIcon.cy + sizeText.cy + m_lIconMarginText;
                    }
                    break;
                }

                sWidth.cx += sIconText.cx;
                sWidth.cy += sIconText.cy;
            }

            pSize->cx = sWidth.cx;
            pSize->cy = sHeight.cy;
        }

    default:
        UI_LOG_WARN( _T("ButtonBase::GetAutoSize  Unknown autosize type: %d"), m_eAutoSizeType);
        break;
    }
}

UINT  Button::OnGetDlgCode(LPMSG lpMsg)
{   
    UINT nRet = DLGC_BUTTON;
    if (m_pIButton->IsDefault())
    {
        nRet |= DLGC_DEFPUSHBUTTON;
    }
    else
    {
        if (m_pIButton->CanTabstop() && HasDefaultPushButtonStyle())
        {
            nRet |= DLGC_UNDEFPUSHBUTTON;  // ��ʾ���ڲ���Ĭ�ϰ�ť�����Լ����Գ�ΪĬ�ϰ�ť
        }
    }

    return nRet;
}

void Button::OnSerialize(SERIALIZEDATA* pData)
{
    DO_PARENT_PROCESS(IButton, IButtonBase);

    AttributeSerializerWrap s(pData, TEXT("Button"));

    s.AddString(XML_TEXT, this, 
        memfun_cast<pfnStringSetter>(&Button::SetText),
        memfun_cast<pfnStringGetter>(&Button::SaveText))
        ->AsData();

    s.AddLong(XML_BUTTON_ICON_TEXT_SPACE, m_lIconMarginText);

    s.AddBool(XML_BUTTON_DEFPUSHBUTTON, this, 
        memfun_cast<pfnBoolSetter>(&Button::SetDefaultPushButtonStyle),
        memfun_cast<pfnBoolGetter>(&Button::HasDefaultPushButtonStyle))
        ->SetDefault(false);

    s.AddEnum(XML_BUTTON_AUTOSIZE_TYPE, *(long*)&m_eAutoSizeType)
        ->AddOption(BUTTON_AUTOSIZE_TYPE_CONTENT,  XML_BUTTON_AUTOSIZE_TYPE_CONTENT)
        ->AddOption(BUTTON_AUTOSIZE_TYPE_BKIMAGE,  XML_BUTTON_AUTOSIZE_TYPE_BKIMAGE)
        ->AddOption(BUTTON_AUTOSIZE_TYPE_BKIMAGEHEIGHT_CONTENTWIDTH, 
                    XML_BUTTON_AUTOSIZE_TYPE_BKIMAGEHEIGHT_CONTENTWIDTH)
        ->SetDefault(BUTTON_AUTOSIZE_TYPE_CONTENT);

    s.AddEnum(XML_BUTTON_ICON_ALIGN, *(long*)&m_eIconAlignFlag)
        ->AddOption(ALIGN_LEFT,   XML_ALIGN_LEFT)
        ->AddOption(ALIGN_RIGHT,  XML_ALIGN_RIGHT)
        ->AddOption(ALIGN_TOP,    XML_ALIGN_TOP)
        ->AddOption(ALIGN_BOTTOM, XML_ALIGN_BOTTOM)
        ->AddOption(ALIGN_CENTER, XML_ALIGN_CENTER)
        ->SetDefault(ALIGN_LEFT);

    s.AddEnum(XML_BUTTON_DRAW_FOCUS_TYPE, *(long*)&m_eDrawFocusType)
        ->AddOption(BUTTON_RENDER_DRAW_FOCUS_TYPE_DOT,  XML_BUTTON_DRAW_FOCUS_TYPE_DOT)
        ->AddOption(BUTTON_RENDER_DRAW_FOCUS_TYPE_NONE, XML_BUTTON_DRAW_FOCUS_TYPE_NONE)
        ->SetDefault(BUTTON_RENDER_DRAW_FOCUS_TYPE_NONE);
}

// bool  Button::useWinTheme()
// {
// 	m_pIButton->SetBkgndRender(NULL);
// 
// 	if (this->GetButtonType() != BUTTON_STYLE_PUSHBUTTON)
// 		return false;
// 
// 	IRenderBase* p = NULL;
// 	m_pIButton->GetUIApplication()->CreateRenderBase(
// 		RENDER_TYPE_THEME_BUTTON_BKGND, 
// 		this->m_pIButton, &p);
// 
// 	if (!p)
// 		return false;
// 	
// 	IMapAttribute* pMapAttrib = NULL;
// 	m_pIButton->GetMapAttribute(&pMapAttrib);
// 	p->SetAttribute(pMapAttrib, XML_BACKGND_RENDER_PREFIX, true);
// 	pMapAttrib->Release();
// 
// 	m_pIButton->SetBkgndRender(p);
// 	p->Release();
// 
// 	ObjStyle s = {0};
// 	s.transparent = 1;
// 	m_pIButton->ModifyObjectStyle(&s, 0);
// 
// 	return true;
// }

void  Button::OnCreateByEditor(CREATEBYEDITORDATA* pData)
{
	pData->rcInitPos.right = pData->rcInitPos.left + 100;
	pData->rcInitPos.bottom = pData->rcInitPos.top + 24;
}

void  Button::SetDrawFocusType(BUTTON_RENDER_DRAW_FOCUS_TYPE eType)
{
    m_eDrawFocusType = eType;
}
void  Button::SetAutoSizeType(BUTTON_AUTO_SIZE_TYPE eType)
{
    m_eAutoSizeType = eType;
}

void  Button::SetDefaultPushButtonStyle(bool b)
{
    if (b)
        m_pIButton->ModifyStyleEx(BUTTON_STYLE_DEFPUSH, 0, 0);
    else
        m_pIButton->ModifyStyleEx(0, BUTTON_STYLE_DEFPUSH, 0);
}
bool  Button::HasDefaultPushButtonStyle()
{
    return m_pIButton->TestStyleEx(BUTTON_STYLE_DEFPUSH);
}

//
// ���ݲ��������ã������ͼ����ı��Ļ�������
//
//	Remark:
//	   �����λ��û��ȥ�������λ��
//
void Button::analyzeIconTextRect(IRenderTarget* pRenderTarget, CRect& rcIcon, CRect& rcText)
{
    m_pIButton->GetObjectClientRect(&rcIcon);
    rcText.CopyRect(rcIcon);

    SIZE s = {0,0};
    if (m_pIButton->GetForeRender())
    {
        s = m_pIButton->GetForeRender()->GetDesiredSize();
    }

    switch (m_eIconAlignFlag)
    {
    case ALIGN_LEFT:
        {
            rcIcon.right = rcIcon.left + s.cx;
            rcText.left  = rcIcon.right + m_lIconMarginText;
        }
        break;
    case ALIGN_TOP:
        {
            rcIcon.bottom = rcIcon.top + s.cy;
            rcText.top = rcIcon.bottom + m_lIconMarginText;
        }
        break;
    case ALIGN_RIGHT:
        {
            rcIcon.left = rcIcon.right - s.cx;
            rcText.right = rcIcon.left - m_lIconMarginText;
        }
        break;
    case ALIGN_BOTTOM:
        {
            rcIcon.top = rcIcon.bottom - s.cy;
            rcText.bottom = rcIcon.top - m_lIconMarginText;
        }
        break;

    case ALIGN_CENTER:
        {
            rcIcon.left = rcIcon.left + (rcIcon.Width() - s.cx)/2;
            rcIcon.right = rcIcon.left + s.cx;
            rcIcon.top = rcIcon.top + (rcIcon.Height() - s.cy)/2;
            rcIcon.bottom = rcIcon.top + s.cy;
        }
        break;
    }
}

// �ӳٴ���textrender
ITextRenderBase*  Button::GetTextRender()
{
    ITextRenderBase* p = m_pIButton->GetTextRender();
    if (!p && m_strText.length() > 0)
        p = m_pIButton->CreateDefaultTextRender();

    return p;
}

void Button::OnPaint(IRenderTarget* pRenderTarget)
{
    CRect rcIcon, rcText;

    this->analyzeIconTextRect(pRenderTarget, rcIcon, rcText);

    bool  bDisable = !m_pIButton->IsEnable();
    bool  bHover   = m_pIButton->IsHover();
    bool  bPress   = m_pIButton->IsPress();
    bool  bChecked = IsChecked() || m_pIButton->IsSelected();
    bool  bForePress = m_pIButton->IsForcePress();

    int  nState = 0;
    if (bDisable)
    {
        nState = bChecked?BUTTON_ICON_RENDER_STATE_SELECTED_DISABLE:BUTTON_ICON_RENDER_STATE_DISABLE;
    }
    else if (bForePress || (bPress && bHover))
    {
        nState = bChecked?BUTTON_ICON_RENDER_STATE_SELECTED_PRESS:BUTTON_ICON_RENDER_STATE_PRESS;
    }
    else if (bHover || bPress)
    {
        nState = bChecked?BUTTON_ICON_RENDER_STATE_SELECTED_HOVER:BUTTON_ICON_RENDER_STATE_HOVER;
    }
    else 
    {
        nState = bChecked?BUTTON_ICON_RENDER_STATE_SELECTED_NORMAL:BUTTON_ICON_RENDER_STATE_NORMAL;
    }

    if (m_pIButton->GetForeRender())
        m_pIButton->GetForeRender()->DrawState(pRenderTarget, &rcIcon, nState);

    if (m_strText.length() > 0)
    {
        ITextRenderBase* pTextRender = GetTextRender();
        if (pTextRender)
            pTextRender->DrawState(pRenderTarget, &rcText, nState, m_strText.c_str());
    }
    return;
}

LRESULT  Button::DrawFocus(WPARAM w, LPARAM)
{
    IRenderTarget* pRenderTarget = (IRenderTarget*)w;
    switch(m_eDrawFocusType)
    {
    case BUTTON_RENDER_DRAW_FOCUS_TYPE_NONE:
        break;
    case BUTTON_RENDER_DRAW_FOCUS_TYPE_DOT:
        {
            CRect  rc;
            switch (this->GetButtonType())
            {
            case BUTTON_STYLE_RADIOBUTTON:
            case BUTTON_STYLE_CHECKBUTTON:
                {
                    CRect rcIcon, rcText;
                    this->analyzeIconTextRect(pRenderTarget, rcIcon, rcText );  // ���ص������ǻ���client��

                    this->m_pIButton->ClientRect2ObjectRect(&rcText, &rc);   
                }
                break;

            case BUTTON_STYLE_HYPERLINK:
                {
                    this->m_pIButton->GetClientRectInObject(&rc);
                }
                break;

                //case BUTTON_STYLE_PUSHBUTTON:
            default:
                {
                    rc.SetRect(0,0, m_pIButton->GetWidth(), m_pIButton->GetHeight());
                    ::InflateRect( &rc, -3, -3 );
                }
                break;
            }

            pRenderTarget->DrawFocusRect(&rc);
        }
        break;
    }

    return 0;
}

void  Button::OnClicked()
{
    UIMSG   msg;
    msg.message = UI_WM_NOTIFY;
    msg.nCode   = UI_BN_CLICKED;
    msg.wParam  = NULL; //(WPARAM)&pt;
    msg.lParam  = NULL;
    msg.pMsgFrom = m_pIButton;
    LRESULT lRet = m_pIButton->DoNotify(&msg);
}


