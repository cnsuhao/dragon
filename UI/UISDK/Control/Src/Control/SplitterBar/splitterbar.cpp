#include "stdafx.h"
#include "splitterbar.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Kernel\Inc\Interface\iattribute.h"

using namespace UI;

SplitterBar::SplitterBar()
{
    m_pISplitterBar = NULL;
    m_pObjLeft = NULL;
    m_pObjRight = NULL;
    m_pObjParent = NULL; 

    m_ptLButtonDown.x = -1;
    m_ptLButtonDown.y = -1;
    m_ptLastMouseMove.x = -1;
    m_ptLastMouseMove.y = -1;
    m_nDeviation = 0;
    m_bButtonDown = false;

    m_nLeftMin = NDEF;
    m_nLeftMax = NDEF;
    m_nRightMin = NDEF;
    m_nRightMax = NDEF;
    m_rcAvailable.SetRectEmpty();
    m_nLeftOffset = 0;
    m_nRightOffset = 0;
    m_bEnableDrag = true;

	m_pObjLeft = NULL;
	m_pObjRight = NULL;

	m_eObjectsPos = SPLITTERBAR_LEFT_RIGHT;
	m_eAlignType = SPLITTERBAR_ALIGN_BOTTOM;
}
SplitterBar::~SplitterBar()
{

}

HRESULT  SplitterBar::OnFinalConstruct(IUIApplication* p)
{
	DO_PARENT_PROCESS(ISplitterBar, IPanel);

	ObjStyle s = {0};
	s.reject_self_mouse_msg = 1;
	m_pISplitterBar->ModifyObjectStyle(0, &s);
	return S_OK;
}

void  SplitterBar::OnFinalRelease()
{
	SetMsgHandled(FALSE);

	if (m_pObjParent)
		m_pObjParent->RemoveHook(m_pISplitterBar, 0, PARENT_MSG_ALT_ID);
}

void  SplitterBar::OnSerialize(SERIALIZEDATA* pData)
{
    DO_PARENT_PROCESS(ISplitterBar, IPanel);

	// 先获取一下自己的类型
	{
		AttributeSerializerWrap as(pData, TEXT("SplitterBar"));
		as.AddEnum(XML_SPLITTERBAR_OBJECTS_POS, this, 
			memfun_cast<pfnLongSetter>(&SplitterBar::SetObjectsPosType),
			memfun_cast<pfnLongGetter>(&SplitterBar::GetObjectsPosType))
			->AddOption(SPLITTERBAR_LEFT_RIGHT, XML_SPLITTERBAR_LEFT_RIGHT)
			->AddOption(SPLITTERBAR_TOP_BOTTOM, XML_SPLITTERBAR_UP_DOWN)
			->ReloadOnChanged();
	}

	if (IsTopBottom())
	{
		AttributeSerializerWrap as(pData, TEXT("SplitterBar"));

		as.AddEnum(XML_SPLITTERBAR_ALIGN, this, 
			memfun_cast<pfnLongSetter>(&SplitterBar::SetAlign),
			memfun_cast<pfnLongGetter>(&SplitterBar::GetAlign))
			->AddOption(SPLITTERBAR_ALIGN_TOP, XML_SPLITTERBAR_ALIGN_TOP)
			->AddOption(SPLITTERBAR_ALIGN_BOTTOM, XML_SPLITTERBAR_ALIGN_BOTTOM);

		as.AddString(XML_SPLITTERBAR_TOP_PREFIX XML_SPLITTERBAR_OBJ_ID, this,
			memfun_cast<pfnStringSetter>(&SplitterBar::set_top_obj_id),
			memfun_cast<pfnStringGetter>(&SplitterBar::get_top_obj_id));
		as.AddLong(XML_SPLITTERBAR_TOP_PREFIX XML_SPLITTERBAR_OBJ_MINSIZE, m_nTopMin);
		as.AddLong(XML_SPLITTERBAR_TOP_PREFIX XML_SPLITTERBAR_OBJ_MAXSIZE, m_nTopMax);
		
		as.AddString(XML_SPLITTERBAR_BOTTOM_PREFIX XML_SPLITTERBAR_OBJ_ID, this,
			memfun_cast<pfnStringSetter>(&SplitterBar::set_bottom_obj_id),
			memfun_cast<pfnStringGetter>(&SplitterBar::get_bottom_obj_id));
		as.AddLong(XML_SPLITTERBAR_BOTTOM_PREFIX XML_SPLITTERBAR_OBJ_MINSIZE, m_nBottomMin);
		as.AddLong(XML_SPLITTERBAR_BOTTOM_PREFIX XML_SPLITTERBAR_OBJ_MAXSIZE, m_nBottomMax);
	}
	else
	{
		AttributeSerializerWrap as(pData, TEXT("SplitterBar"));

		as.AddEnum(XML_SPLITTERBAR_ALIGN, this, 
			memfun_cast<pfnLongSetter>(&SplitterBar::SetAlign),
			memfun_cast<pfnLongGetter>(&SplitterBar::GetAlign))
			->AddOption(SPLITTERBAR_ALIGN_LEFT, XML_SPLITTERBAR_ALIGN_LEFT)
			->AddOption(SPLITTERBAR_ALIGN_RIGHT, XML_SPLITTERBAR_ALIGN_RIGHT);

		as.AddString(XML_SPLITTERBAR_LEFT_PREFIX XML_SPLITTERBAR_OBJ_ID, this,
			memfun_cast<pfnStringSetter>(&SplitterBar::set_left_obj_id),
			memfun_cast<pfnStringGetter>(&SplitterBar::get_left_obj_id));
		as.AddLong(XML_SPLITTERBAR_LEFT_PREFIX XML_SPLITTERBAR_OBJ_MINSIZE, m_nLeftMin);
		as.AddLong(XML_SPLITTERBAR_LEFT_PREFIX XML_SPLITTERBAR_OBJ_MAXSIZE, m_nLeftMax);

		as.AddString(XML_SPLITTERBAR_RIGHT_PREFIX XML_SPLITTERBAR_OBJ_ID, this,
			memfun_cast<pfnStringSetter>(&SplitterBar::set_right_obj_id),
			memfun_cast<pfnStringGetter>(&SplitterBar::get_right_obj_id));
		as.AddLong(XML_SPLITTERBAR_RIGHT_PREFIX XML_SPLITTERBAR_OBJ_MINSIZE, m_nRightMin);
		as.AddLong(XML_SPLITTERBAR_RIGHT_PREFIX XML_SPLITTERBAR_OBJ_MAXSIZE, m_nRightMax);
	}
}

void  SplitterBar::OnObjectLoaded()
{
	if (m_pObjParent)
		m_pObjParent->RemoveHook(m_pISplitterBar, 0, PARENT_MSG_ALT_ID);

    m_pObjParent = m_pISplitterBar->GetParentObject();
    if (!m_pObjParent)
        return;

	// 监听父控件大小改变事件
	m_pObjParent->AddHook(m_pISplitterBar, 0, PARENT_MSG_ALT_ID);

    if (IsTopBottom())
    {
		if (m_strTopObjId.length())
			m_pObjTop = m_pObjParent->FindChildObject(m_strTopObjId.c_str());

		if (m_strBottomObjId.length())
			m_pObjBottom = m_pObjParent->FindChildObject(m_strBottomObjId.c_str());
    }
    else
    {
        if (m_strLeftObjId.length())
            m_pObjLeft = m_pObjParent->FindChildObject(m_strLeftObjId.c_str());

        if (m_strRightObjId.length())
            m_pObjRight = m_pObjParent->FindChildObject(m_strRightObjId.c_str());
    }
}

void  SplitterBar::SetObjectsPosType(SPLITTED_OBJECTS_POS eDir)
{
    m_eObjectsPos = eDir;

    if (m_eObjectsPos == SPLITTERBAR_LEFT_RIGHT)
    {
        m_pISplitterBar->SetCursorId(XML_CURSOR_IDC_SPLITBAR_V);
    }
    else
    {
        m_pISplitterBar->SetCursorId(XML_CURSOR_IDC_SPLITBAR_H);        
    }
}
SPLITTED_OBJECTS_POS  SplitterBar::GetObjectsPosType()
{
	return m_eObjectsPos;
}

void  SplitterBar::SetAlign(SPLITTERBAR_ALIGN eAlign)
{
    m_eAlignType = eAlign;
}

SPLITTERBAR_ALIGN  SplitterBar::GetAlign()
{
	return m_eAlignType;
}

void  SplitterBar::set_left_obj_id(LPCTSTR szId)
{
	SETSTRING(m_strLeftObjId, szId);
}
void  SplitterBar::set_right_obj_id(LPCTSTR szId)
{
	SETSTRING(m_strRightObjId, szId);
}
void  SplitterBar::set_top_obj_id(LPCTSTR szId)
{
	SETSTRING(m_strTopObjId, szId);
}
void  SplitterBar::set_bottom_obj_id(LPCTSTR szId)
{
	SETSTRING(m_strBottomObjId, szId);
}
LPCTSTR  SplitterBar::get_left_obj_id()
{
	if (m_pObjLeft)
		return m_pObjLeft->GetId();
	else
		return m_strLeftObjId.c_str();
}
LPCTSTR  SplitterBar::get_right_obj_id()
{
	if (m_pObjRight)
		return m_pObjRight->GetId();
	else
		return m_strRightObjId.c_str();
}
LPCTSTR  SplitterBar::get_top_obj_id()
{
	if (m_pObjTop)
		return m_pObjTop->GetId();
	else
		return m_strTopObjId.c_str();
}
LPCTSTR  SplitterBar::get_bottom_obj_id()
{
	if (m_pObjBottom)
		return m_pObjBottom->GetId();
	else
		return m_strBottomObjId.c_str();
}

// 表示的是分隔条控件的方向
bool  SplitterBar::IsTopBottom()
{
    return m_eObjectsPos == SPLITTERBAR_TOP_BOTTOM ? true:false;
}
bool  SplitterBar::IsLeftRight()
{
    return m_eObjectsPos == SPLITTERBAR_LEFT_RIGHT ? true:false;
}

void  SplitterBar::OnSize(UINT nType, int cx, int cy)
{
    SetMsgHandled(FALSE);
    return;
}
void  SplitterBar::OnParentSize(UINT nType, int cx, int cy)
{
	OnParentSizeChanged();
	SetMsgHandled(FALSE);
	return;
}

void  SplitterBar::OnLButtonDown(UINT nFlags, POINT point)
{
    if (!m_bEnableDrag)
        return;

    m_ptLButtonDown = point;
    m_bButtonDown = true;

	REGION4 rMargin;
	m_pISplitterBar->GetMarginRegion(&rMargin);

    // 计算误差
    POINT pt;
    m_pISplitterBar->WindowPoint2ObjectPoint(&point, &pt, true);
    if (IsLeftRight())
    {
        m_nDeviation = rMargin.left + pt.x;
    }
    else
    {
        m_nDeviation = rMargin.top + pt.y;
    }

//     IWindowBase* pWindow = m_pISplitterBar->GetWindowObject();
//     if (pWindow)
//     {
//         pWindow->GetKeyboardMgr()->SetCapture(this, 0);
//     }
    m_ptLastMouseMove = point;  // 防止第一次响应MOUSEMOVE时就去调用ondraging
    CalcAvailableRegion();
}
void  SplitterBar::OnLButtonUp(UINT nFlags, POINT point)
{
    this->OnCancelMode();
}

void  SplitterBar::OnCancelMode()
{
    EndDrag();
}

void  SplitterBar::EndDrag()
{
    m_ptLButtonDown.x = m_ptLButtonDown.y = -1;
    m_ptLastMouseMove.x = m_ptLastMouseMove.y = -1;
    m_bButtonDown = false;
    m_nDeviation = 0;
    m_rcAvailable.SetRectEmpty();
}

void  SplitterBar::OnMouseMove(UINT nFlags, POINT point)
{
    if (!m_bButtonDown)
        return;

    // 防止外部其它因素产生的mousemove消息持续发送，导致CPU占用率高的问题
    // 也用于防止无意义的鼠标移动
    if (IsLeftRight() && point.x == m_ptLastMouseMove.x)
        return;
    if (IsTopBottom() && point.y == m_ptLastMouseMove.y)
        return;

    m_ptLastMouseMove = point;

    // 在这里传递给layout的坐标参数全部都转换为拖拽条的最左侧/最上侧位置，避免鼠标点击位置不同而产生的误差
    if (IsLeftRight())
        point.x -= m_nDeviation;
    else
        point.y -= m_nDeviation;

    // 将point转换成相对于自己的parent，因为自己的位置也是相对于parent的
    POINT ptParent;
    m_pObjParent->WindowPoint2ObjectPoint(&point, &ptParent, true);
    
    if (!this->OnDraging(ptParent))
    {
        EndDrag();
        SetCursor(LoadCursor(NULL, IDC_ARROW));
    }
}

bool  SplitterBar::OnDraging(POINT ptInParent)  // 相对于父对象的坐标
{
    POINT ptAvailable = {0,0};
    if (!this->TestAvailableDragPos(ptInParent, &ptAvailable))
        return true;

    {
        UIMSG  msg;
        msg.pMsgFrom = m_pISplitterBar;
        msg.wParam = (WPARAM)m_pISplitterBar;
        msg.message = UI_WM_NOTIFY;
        msg.nCode = SPLITTERBAR_NM_POSCHANGING;
        msg.wParam = (WPARAM)&ptInParent;
        msg.lParam = (LPARAM)&ptAvailable;
        SPLITTERBAR_POSCHANGING_RESULT eResult = 
            (SPLITTERBAR_POSCHANGING_RESULT)m_pISplitterBar->DoNotify(&msg);

        switch (eResult)
        {
        case SPLITTERBAR_NOTHANDLED:
            break;

        case SPLITTERBAR_HANDLED:
            return true;
            break;

        case SPLITTERBAR_HANDLED_AND_STOP_DRAG:
            return false;
            break;
        }
    }

    if (IsLeftRight())
    {
        UpdateLeftRightCtrlPos(ptAvailable.x);
    }
    else
    {
        UpdateUpDownCtrlPos(ptAvailable.y);
    }

    {
	    UIMSG  msg;
	    msg.pMsgFrom = m_pISplitterBar;
	    msg.wParam = (WPARAM)m_pISplitterBar;
	    msg.message = UI_WM_NOTIFY;
        msg.nCode = SPLITTERBAR_NM_POSCHANGED;
	    m_pISplitterBar->DoNotify(&msg);
    }

    return true;
}


void  SplitterBar::CalcAvailableRegion()
{
    m_rcAvailable.SetRectEmpty();

    if (NULL == m_pObjLeft || NULL == m_pObjRight)
        return;

    CRect  rcDragBarRect;
    CRect  rcLeftRect;
    CRect  rcRightRect;

    m_pISplitterBar->GetParentRect(&rcDragBarRect);
    m_pObjLeft->GetParentRect(&rcLeftRect);
    m_pObjRight->GetParentRect(&rcRightRect);

    do 
    {
        if (IsLeftRight())
        {
            // 计算拖拽条的宽度
            int  nDragbarSize = rcDragBarRect.Width();
            int  nFullLeft = rcLeftRect.left;
            int  nFullRight = rcRightRect.right;
            int  nFullTop = rcDragBarRect.top;
            int  nFullBottom = rcDragBarRect.bottom;

            long nWidth = nFullRight - nFullLeft;
            long nHeight = rcDragBarRect.Height();

            RECT rcAvai1 = {0,0,nWidth, nHeight};
            RECT rcAvai2 = {0,0,nWidth-nDragbarSize, nHeight};

            // 计算可拖拽范围
            if (m_nLeftMin > 0)
            {
                rcAvai1.left = m_nLeftMin;
            }
            if (m_nLeftMax > 0)
            {
                rcAvai1.right = m_nLeftMax;
            }

            if (m_nRightMin > 0)
            {
                rcAvai2.right = nWidth - m_nRightMin - nDragbarSize;
            }
            if (m_nRightMax > 0)
            {
                rcAvai2.left = nWidth - m_nRightMax - nDragbarSize;
            }

            if (::IntersectRect(&m_rcAvailable, &rcAvai1, &rcAvai2))
            {
                ::OffsetRect(&m_rcAvailable, nFullLeft, nFullTop); // 转成窗口坐标
            }
            else
            {
                // 配置与当前窗口大小不匹配
                m_rcAvailable.SetRectEmpty();
                break;
            }
        }
        else
        {
            // 计算拖拽条的高度
            int  nDragbarSize = rcDragBarRect.Height();
            int  nFullLeft = rcDragBarRect.left;
            int  nFullRight = rcDragBarRect.right;
            int  nFullTop = rcLeftRect.top;
            int  nFullBottom = rcRightRect.bottom;

            long nHeight = nFullBottom - nFullTop;
            long nWidth = rcDragBarRect.Width();

            RECT rcAvai1 = {0,0,nWidth, nHeight};
            RECT rcAvai2 = {0,0,nWidth, nHeight-nDragbarSize};

            // 计算可拖拽范围
            if (m_nLeftMin > 0)
            {
                rcAvai1.top = m_nLeftMin;
            }
            if (m_nLeftMax > 0)
            {
                rcAvai1.bottom = m_nLeftMax;
            }

            if (m_nRightMin > 0)
            {
                rcAvai2.bottom = nHeight - m_nRightMin - nDragbarSize;
            }
            if (m_nRightMax > 0)
            {
                rcAvai2.top = nHeight - m_nRightMax - nDragbarSize;
            }

            if (::IntersectRect(&m_rcAvailable, &rcAvai1, &rcAvai2))
            {
                ::OffsetRect(&m_rcAvailable, nFullLeft, nFullTop); // 转成窗口坐标
            }
            else
            {
                // 配置与当前窗口大小不匹配
                m_rcAvailable.SetRectEmpty();
                break;
            }
        }
    }while (0);
}

// 根据鼠标拖拽位置(窗口坐标)，计算最终拖拽条的位置
bool  SplitterBar::TestAvailableDragPos(POINT point, POINT* ptAvailable)  
{
    // 获取需要布局的子对象指针（只布局两个子对象）
    if (m_rcAvailable.IsRectEmpty())
        return false;

    *ptAvailable = point;
    if (IsLeftRight())
    {
        if (point.x > m_rcAvailable.right)
        {
            ptAvailable->x = m_rcAvailable.right;
        }
        else if (point.x < m_rcAvailable.left)
        {
            ptAvailable->x = m_rcAvailable.left;
        }
    }
    else
    {
        if (point.y > m_rcAvailable.bottom)
        {
            ptAvailable->y = m_rcAvailable.bottom;
        }
        else if (point.y < m_rcAvailable.top)
        {
            ptAvailable->y = m_rcAvailable.top;
        }
    }

    return true;
}



void  SplitterBar::UpdateLeftRightCtrlPos(int nPos)
{
    CRect  rc;
    m_pISplitterBar->GetParentRect(&rc);

    LONG nPrevLeft = rc.left;
    if (nPrevLeft == nPos)
        return;

    LONG  nWidth = rc.Width();

    long  left = nPos;
    long  right = left + nWidth;

    rc.left = left;
    rc.right = right;
    m_pISplitterBar->SetObjectPos(&rc, SWP_NOREDRAW|SWP_NOSENDCHANGING);

    if (m_pObjLeft)
    {
        CRect rcLeftCtrl;
        m_pObjLeft->GetParentRect(&rcLeftCtrl);
        rcLeftCtrl.right = left - m_nLeftOffset;
        m_pObjLeft->SetObjectPos(&rcLeftCtrl, SWP_NOREDRAW);
    }
    if (m_pObjRight)
    {
        CRect  rcRightCtrl;
        m_pObjRight->GetParentRect(&rcRightCtrl);
        rcRightCtrl.left = right + m_nRightOffset;
        m_pObjRight->SetObjectPos(&rcRightCtrl, SWP_NOREDRAW);
    }

    m_pObjParent->UpdateObject(true);
}

// nPos为top或者left
void  SplitterBar::UpdateUpDownCtrlPos(int nPos)
{
    CRect  rc;
    m_pISplitterBar->GetParentRect(&rc);

    LONG  nPrevTop = rc.top;
    if (nPrevTop == nPos)
        return;

    LONG  nHeight = rc.Height();

    long  top = nPos;
    long  bottom = top + nHeight;
    rc.top = top;
    rc.bottom = bottom;
    m_pISplitterBar->SetObjectPos(&rc, SWP_NOREDRAW|SWP_NOSENDCHANGING);

    if (m_pObjTop)
    {
        CRect  rcUpCtrl;
        m_pObjTop->GetParentRect(&rcUpCtrl);
        rcUpCtrl.bottom = top - m_nTopOffset;
        m_pObjTop->SetObjectPos(&rcUpCtrl, SWP_NOREDRAW);
    }
    if (m_pObjBottom)
    {
        CRect  rcBottomCtrl;
        m_pObjBottom->GetParentRect(&rcBottomCtrl);
        rcBottomCtrl.top = bottom + m_nBottomOffset;
        m_pObjBottom->SetObjectPos(&rcBottomCtrl, SWP_NOREDRAW);
    }

    m_pObjParent->UpdateObject(true);
}

// 在窗口大小改变时，要重新调整两侧控件大小，保证二者都可见
void  SplitterBar::OnParentSizeChanged()
{
    if (NULL == m_pObjLeft || NULL == m_pObjRight)
        return;
    
    if (!IsDragable())
        return;

    CRect rcDragBar;
    m_pISplitterBar->GetParentRect(&rcDragBar);

    POINT ptLast = {-1, -1};

    if (IsTopBottom())
    {
        CRect rcTop, rcBottom;
        m_pObjTop->GetParentRect(&rcTop);
        m_pObjBottom->GetParentRect(&rcBottom);

        bool bHandled = false;
        if (m_eAlignType == SPLITTERBAR_ALIGN_BOTTOM)
        {
            do 
            {
                if (m_nBottomMax != NDEF && rcBottom.Height() > m_nBottomMax)
                {
                    ptLast.y = rcBottom.bottom - m_nBottomMax - rcDragBar.Height();
                    bHandled = true;
                    break;
                }

                if (m_nBottomMin != NDEF && rcBottom.Height() < m_nBottomMin)
                {
                    ptLast.y = rcBottom.bottom - m_nBottomMin - rcDragBar.Height();
                    bHandled = true;
                    break;
                }

                if (m_nTopMin != NDEF && rcTop.Height() < m_nTopMin)
                {
                    ptLast.y = rcTop.top + m_nTopMin;

					//  优化保证底侧的最小值
					int nMaxLast = rcBottom.bottom - m_nBottomMin - rcDragBar.Height();
					if (ptLast.y > nMaxLast)
						ptLast.y = nMaxLast;

                    bHandled = true;
                    break;
                }

                if (m_nTopMax != NDEF && rcTop.Height() > m_nTopMax)
                {
                    ptLast.y = rcTop.top + m_nTopMax;
                    bHandled = true;
                    break;
                }
            }while(0);
        }
        else if (m_eAlignType == SPLITTERBAR_ALIGN_TOP)
        {
            do 
            {
                if (m_nTopMin != NDEF && rcTop.Height() < m_nTopMin)
                {
                    ptLast.y = rcTop.top + m_nTopMin;
                    bHandled = true;
                    break;
                }

                if (m_nTopMax != NDEF && rcTop.Height() > m_nTopMax)
                {
                    ptLast.y = rcTop.top + m_nTopMax;
                    bHandled = true;
                    break;
                }

                if (m_nBottomMax != NDEF && rcBottom.Height() > m_nBottomMax)
                {
                    ptLast.y = rcBottom.bottom - m_nBottomMax - rcDragBar.Height();
                    bHandled = true;
                    break;
                }

                if (m_nBottomMin != NDEF && rcBottom.Height() < m_nBottomMin)
                {
                    ptLast.y = rcBottom.bottom - m_nBottomMin - rcDragBar.Height();

					//  优化保证上侧的最小值
					int nMinLast = rcTop.top + m_nTopMin;
					if (ptLast.y < nMinLast)
						ptLast.y = nMinLast;
                    bHandled = true;
                    break;
                }

            }while(0);
        }

        if (bHandled)
        {
            UpdateUpDownCtrlPos(ptLast.y);
        }
    }
    else
    {
        CRect rcLeft, rcRight;
        m_pObjLeft->GetParentRect(&rcLeft);
        m_pObjRight->GetParentRect(&rcRight);

         bool bHandled = false;
        if (SPLITTERBAR_ALIGN_RIGHT == m_eAlignType)
        {
            do
            {
                if (m_nRightMax != NDEF && rcRight.Width() > m_nRightMax)
                {
                    ptLast.x = rcRight.right - m_nRightMax - rcDragBar.Width();
                    bHandled = true;
                    break;
                }

                if (m_nRightMin != NDEF && rcRight.Width() < m_nRightMin)
                {
                    ptLast.x = rcRight.right - m_nRightMin - rcDragBar.Width();
                    bHandled = true;
                    break;
                }

                if (m_nLeftMin != NDEF && rcLeft.Width() < m_nLeftMin)
                {
                    ptLast.x = rcLeft.left + m_nLeftMin;

					int lMaxLast = rcRight.right - m_nRightMin - rcDragBar.Width(); // 保证右侧最小大小值
					if (ptLast.x > lMaxLast)
						ptLast.x = lMaxLast;
                    bHandled = true;
                    break;
                }

                if (m_nLeftMax != NDEF && rcLeft.Width() > m_nLeftMax)
                {
                    ptLast.x = rcLeft.left + m_nLeftMax;
                    bHandled = true;
                    break;
                }
            }while(0);
        }
        else
        {
            do
            {
                if (m_nLeftMin != NDEF && rcLeft.Width() < m_nLeftMin)
                {
                    ptLast.x = rcLeft.left + m_nLeftMin;
                    bHandled = true;
                    break;
                }

                if (m_nLeftMax != NDEF && rcLeft.Width() > m_nLeftMax)
                {
                    ptLast.x = rcLeft.left + m_nLeftMax;
                    bHandled = true;
                    break;
                }

                if (m_nRightMax != NDEF && rcRight.Width() > m_nRightMax)
                {
                    ptLast.x = rcRight.right - m_nRightMax - rcDragBar.Width();
                    bHandled = true;
                    break;
                }

                if (m_nRightMin != NDEF && rcRight.Width() < m_nRightMin)
                {
                    ptLast.x = rcRight.right - m_nRightMin - rcDragBar.Width();

					int lMinLast = rcLeft.left + m_nLeftMin;  // 保证左侧的最小大小
					if (ptLast.x < lMinLast)
						ptLast.x = lMinLast;
                    bHandled = true;
                    break;
                }

            }while(0);
        }
        if (bHandled)
        {
            UpdateLeftRightCtrlPos(ptLast.x);
        }
    }
}

void  SplitterBar::EnableDrag(bool b)
{
    m_bEnableDrag = b;
}
bool  SplitterBar::IsDragable()
{
    return m_bEnableDrag;
}


// 外部设置了splitterbar控件的位置后，更新两侧的控件位置
void  SplitterBar::UpdateControlPosBySplitterBarCurPos(bool bRedraw)
{
    CRect  rc;
    m_pISplitterBar->GetWindowRect(&rc);

    if (IsLeftRight())
    {
        long  left = rc.left;
        long  right = left + rc.Width();

        if (m_pObjLeft)
        {
            CRect rcLeftCtrl;
            m_pObjLeft->GetParentRect(&rcLeftCtrl);
            rcLeftCtrl.right = left - m_nLeftOffset;
            m_pObjLeft->SetObjectPos(&rcLeftCtrl, SWP_NOREDRAW);
        }
        if (m_pObjRight)
        {
            CRect  rcRightCtrl;
            m_pObjRight->GetParentRect(&rcRightCtrl);
            rcRightCtrl.left = right + m_nRightOffset;
            m_pObjRight->SetObjectPos(&rcRightCtrl, SWP_NOREDRAW);
        }
    }
    else
    {
        long  top = rc.top;
        long  bottom = top + rc.Height();

        if (m_pObjTop)
        {
            CRect  rcUpCtrl;
            m_pObjTop->GetParentRect(&rcUpCtrl);
            rcUpCtrl.bottom = top - m_nTopOffset;
            m_pObjTop->SetObjectPos(&rcUpCtrl, SWP_NOREDRAW);
        }
        if (m_pObjBottom)
        {
            CRect  rcBottomCtrl;
            m_pObjBottom->GetParentRect(&rcBottomCtrl);
            rcBottomCtrl.top = bottom + m_nBottomOffset;
            m_pObjBottom->SetObjectPos(&rcBottomCtrl, SWP_NOREDRAW);
        }
    }

    if (bRedraw && m_pObjParent)
    {
        m_pObjParent->UpdateObject();
    }
}


int  SplitterBar::GetLeftMin()
{
    return m_nLeftMin;
}
int  SplitterBar::GetLeftMax()
{
    return m_nLeftMax;
}
int  SplitterBar::GetRightMin()
{
    return m_nRightMin;
}
int  SplitterBar::GetRightMax()
{
    return m_nRightMax;
}
int  SplitterBar::GetUpMin()
{
    return m_nTopMin;
}
int  SplitterBar::GetUpMax()
{
    return m_nTopMax;
}
int  SplitterBar::GetDownMin()
{
    return m_nBottomMin;
}
int  SplitterBar::GetDownMax()
{
    return m_nBottomMax;
}