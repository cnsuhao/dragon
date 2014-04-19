#include "stdafx.h"
#include "object.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Src\RenderLayer2\layer\renderlayer2.h"
#include "UISDK\Kernel\Inc\Interface\ilayout.h"
#include "UISDK\Kernel\Src\Helper\layout\canvaslayout.h"
#include "UISDK\Kernel\Src\UIObject\HwndHost\HwndHost.h"
#include "UISDK\Kernel\Src\RenderLayer2\layer\windowrender.h"

namespace UI
{
	

void  Object::ParentClientPoint2ChildPoint(Object* pObjChild, const POINT* pt, POINT* pOut)
{
	pOut->x = pt->x - pObjChild->m_rcParent.left;
	pOut->y = pt->y - pObjChild->m_rcParent.top;
}
void  Object::ObjectPoint2ObjectClientPoint(Object* pObj, const POINT* pt,  POINT* pOut)
{
	pOut->x = pt->x - pObj->m_rcNonClient.left;
	pOut->y = pt->y - pObj->m_rcNonClient.top;

	int xOffset = 0, yOffset = 0;
	if (pObj->GetScrollOffset(&xOffset, &yOffset))
	{
		pOut->x += xOffset;
		pOut->y += yOffset;
	}
}
void  Object::ObjectPoint2ObjectNonClientPoint(Object* pObj, const POINT* pt, POINT* pOut)
{
    pOut->x = pt->x - pObj->m_rcBorder.left;
    pOut->y = pt->y - pObj->m_rcBorder.left;
}
void  Object::ObjectRect2ObjectClientRect(Object* pObj, const RECT* prc, RECT* pOut)
{
	POINT pt = {prc->left, prc->top};
	ObjectPoint2ObjectClientPoint(pObj, &pt, &pt);

	SetRect(pOut, 
		pt.x, 
		pt.y, 
		pt.x+(prc->right-prc->left), 
		pt.y+(prc->bottom-prc->top));
}

void  Object::ParentClientPoint2ChildClientPoint(Object* pObjChild, const POINT* pt, POINT* pOut)
{
	ParentClientPoint2ChildPoint(pObjChild, pt, pOut);
	if (!pObjChild->IsNcObject())
	{
		ObjectPoint2ObjectClientPoint(pObjChild, pOut, pOut);
	}
}
void  Object::ParentClientRect2ChildClientRect(Object* pObjChild, const RECT* prc, RECT* pOut)
{
	POINT pt = {prc->left, prc->top};
	ParentClientPoint2ChildClientPoint(pObjChild, &pt, &pt);

	SetRect(pOut, 
		pt.x, 
		pt.y, 
		pt.x+(prc->right-prc->left), 
		pt.y+(prc->bottom-prc->top));
}
void  Object::ParentPoint2ChildPoint(Object* pObjChild, const POINT* pt, POINT* pOut)
{
	POINT ptTemp;
    if (pObjChild->IsNcObject())
    {
        ObjectPoint2ObjectNonClientPoint(pObjChild->m_pParent, pt, &ptTemp);
    }
    else
    {
	    ObjectPoint2ObjectClientPoint(pObjChild->m_pParent, pt, &ptTemp);
    }
	ParentClientPoint2ChildPoint(pObjChild, &ptTemp, pOut);
}

void  Object::ParentRect2ChildRect(Object* pObjChild, const RECT* prc, RECT* pOut)
{
	POINT pt = {prc->left, prc->top};
	ParentPoint2ChildPoint(pObjChild, &pt, &pt);

	SetRect(pOut, 
		pt.x, 
		pt.y, 
		pt.x+(prc->right-prc->left), 
		pt.y+(prc->bottom-prc->top));
}

void  Object::ChildPoint2ParentClientPoint(Object* pObjChild, const POINT* ptChild, POINT*  ptOut)
{
	ptOut->x = ptChild->x + pObjChild->m_rcParent.left;
	ptOut->y = ptChild->y + pObjChild->m_rcParent.top;
}

void  Object::ChildRect2ParentClientRect(Object* pObjChild, const RECT* prc, RECT*  pOut)
{
	POINT pt = {prc->left, prc->top};
	ChildPoint2ParentClientPoint(pObjChild, &pt, &pt);

	SetRect(pOut, 
		pt.x, 
		pt.y, 
		pt.x+(prc->right-prc->left), 
		pt.y+(prc->bottom-prc->top));
}

void  Object::ObjectClientPoint2ObjectPoint(Object*  pObj, const POINT* ptChild, POINT*  ptOut)
{
	ptOut->x = ptChild->x + pObj->m_rcNonClient.left;
	ptOut->y = ptChild->y + pObj->m_rcNonClient.top;

	int xScroll = 0, yScroll = 0;
	if (pObj->GetScrollOffset(&xScroll, &yScroll))
	{
		ptOut->x -= xScroll;
		ptOut->y -= yScroll;
	}
}

void  Object::ObjectClientRect2ObjectRect(Object*  pObj, const RECT* prc, RECT*  pOut)
{
	POINT pt = {prc->left, prc->top};
	ObjectClientPoint2ObjectPoint(pObj, &pt, &pt);

	SetRect(pOut, 
		pt.x, 
		pt.y, 
		pt.x+(prc->right-prc->left), 
		pt.y+(prc->bottom-prc->top));
}

void  Object::ChildPoint2ParentPoint(Object* pObjChild, const POINT* ptChild, POINT*  ptOut)
{
	ChildPoint2ParentClientPoint(pObjChild, ptChild, ptOut);
	if (!pObjChild->IsNcObject())
	{
		ObjectClientPoint2ObjectPoint(pObjChild->m_pParent, ptOut, ptOut);
	}
}

void  Object::ChildRect2ParentRect(Object* pObjChild, const RECT* prc, RECT*  pOut)
{
	POINT pt = {prc->left, prc->top};
	ChildPoint2ParentPoint(pObjChild, &pt, &pt);

	SetRect(pOut, 
		pt.x, 
		pt.y, 
		pt.x+(prc->right-prc->left), 
		pt.y+(prc->bottom-prc->top));
}


// 要绘制该对象之前，获取该对象在窗口中的实际位置，用于设置偏移量和裁剪区
POINT Object::GetWindowPoint()
{
	POINT pt = {0, 0};

	Object* pObjParent = NULL;
	Object* pObjChild = this;
	while (pObjParent = this->EnumParentObject(pObjParent))
	{
		ChildPoint2ParentPoint(pObjChild, &pt, &pt);
		pObjChild = pObjParent;
	}

	return pt;
}

void Object::GetWindowRect(CRect* lprc)
{
	UIASSERT(lprc);

	POINT pt = this->GetWindowPoint();
	lprc->SetRect(pt.x, pt.y, pt.x+GetWidth(), pt.y+GetHeight());
}


//
// 获取该对象的偏移量
//
// ReturnQ
//		返回false表示该对象无滚动数据
//
bool Object::GetScrollOffset(int* pxOffset, int* pyOffset)
{
	if (NULL == pxOffset || NULL == pyOffset)
		return false;

	*pxOffset = 0; *pyOffset = 0;

	bool bHScroll = this->TestStyle(OBJECT_STYLE_HSCROLL);
	bool bVScroll = this->TestStyle(OBJECT_STYLE_VSCROLL);
	if (bHScroll || bVScroll)
	{
		::UISendMessage(this, UI_WM_GETSCROLLOFFSET, (WPARAM)pxOffset, (LPARAM)pyOffset);
		return true;
	}

	return false;
}

bool Object::GetScrollRange(int* pxRange, int* pyRange)
{
	if (NULL == pxRange || NULL == pyRange)
		return false;

	*pxRange = 0; *pyRange = 0;

	bool bHScroll = this->TestStyle(OBJECT_STYLE_HSCROLL);
	bool bVScroll = this->TestStyle(OBJECT_STYLE_VSCROLL);
	if (bHScroll || bVScroll)
	{
		::UISendMessage(this, UI_WM_GETSCROLLRANGE, (WPARAM)pxRange, (LPARAM)pyRange);
		return true;
	}

	return false;
}

// 2014.3.28将窗口坐标转换成对象内部坐标，在有旋转变换等情况下，需要进行坐标变换
void Object::WindowPoint2ObjectPoint(const POINT* ptWindow, POINT* ptObj, bool bCalcTransform)
{
	if (NULL == ptObj || NULL == ptWindow)
		return;

    if (false == bCalcTransform)
    {
	    POINT pt = this->GetWindowPoint();
	    ptObj->x = ptWindow->x - pt.x;
	    ptObj->y = ptWindow->y - pt.y;
    }
    else
    {
        POINT pt = *ptWindow;

        Object* pObjParent = NULL;
        Object* pObjChild = this;

        vector<Object*>  vAncestor;
        while (pObjParent = this->REnumParentObject(pObjParent))
        {
            vAncestor.push_back(pObjParent);
        }
        vAncestor.push_back(this);  // 最后还要在自己的坐标范围内转换一次

        unsigned int nSize = vAncestor.size();
        for (unsigned int i = 0; i < nSize; i++)
        {
            Object* p = vAncestor[i];
            if (p->m_pRenderLayer && p->m_pRenderLayer->IsTransformed())
            {
                p->m_pRenderLayer->TransformPoint(&pt, &pt);
            }

            if (p != this)
            {
                ParentPoint2ChildPoint(vAncestor[i+1], &pt, &pt);
            }
        }

        *ptObj = pt;
    }
}

void Object::WindowPoint2ObjectClientPoint(const POINT* ptWindow, POINT* ptClient, bool bCalcTransform)
{
	if (NULL == ptClient || NULL == ptWindow)
		return;

	POINT pt = {0};
	WindowPoint2ObjectPoint(ptWindow, &pt, bCalcTransform);
	ObjectPoint2ObjectClientPoint(this, &pt, ptClient);
}


void  Object::WindowRect2ObjectClientRect(const RECT* rcWindow, RECT* rcObj)
{
	POINT ptWindow = {rcWindow->left, rcWindow->top};
	POINT ptClient = {0};

	this->WindowPoint2ObjectClientPoint(&ptWindow, &ptClient, false);
	rcObj->left = ptClient.x;
	rcObj->top = ptClient.y;
	rcObj->right = rcObj->left + (rcWindow->right-rcWindow->left);
	rcObj->bottom = rcObj->top + (rcWindow->bottom-rcWindow->top);
}
void  Object::WindowRect2ObjectRect(const RECT* rcWindow, RECT* rcObj)
{
	POINT ptWindow = {rcWindow->left, rcWindow->top};
	POINT ptObj = {0};

	this->WindowPoint2ObjectPoint(&ptWindow, &ptObj, false);
	rcObj->left = ptObj.x;
	rcObj->top = ptObj.y;
	rcObj->right = rcObj->left + (rcWindow->right-rcWindow->left);
	rcObj->bottom = rcObj->top + (rcWindow->bottom-rcWindow->top);
}

//////////////////////////////////////////////////////////////////////////


// 获取一个对象在窗口上的可视区域。例如用于绘制该对象时的裁剪

bool  Object::GetVisibleRectInWindow(RECT* prc)
{
	return CalcVisibleRectInAncestor(GetWindowObject(), NULL, prc);
}

// 计算对象在层中的位置（不是层缓存中的位置，缓存也可能有偏移）
bool  Object::GetVisibleRectInLayer(RECT* prc)
{
	RenderLayer2*  pRenderLayer = GetRenderLayer2();
	if (!pRenderLayer)
		return GetVisibleRectInWindow(prc);

	if (!CalcVisibleRectInAncestor(pRenderLayer->GetCreateObject(), NULL, prc))
        return false;

    return true;
}

bool  Object::GetVisibleClientRectInLayer(RECT* prc)
{
    RenderLayer2*  pRenderLayer = GetRenderLayer2();
    if (!pRenderLayer)
        return GetVisibleRectInWindow(prc);

    CRect rcClient;
    this->GetClientRect(&rcClient);
    if (!CalcVisibleRectInAncestor(pRenderLayer->GetCreateObject(), &rcClient, prc))
        return false;

    return true;
}

//
// prcObjPart
//   [in] 默认是CRect  rcClip(0, 0, GetWidth(), GetHeight());
//        但为了支持只求Object上的某一部分区域在祖先中的可见区域，增加该参数
//     
bool  Object::CalcVisibleRectInAncestor(__in Object*  pObjAncestor, __in const RECT* prcObjPart, __out RECT* prcOut)
{
	// 从下往上遍历
	if (!pObjAncestor)
		return false;

    // 初始化rcObjPart，有可能给的参数范围就超出了自己的区域
    if (prcOut)
        SetRectEmpty(prcOut);

    RECT  rcObj = {0, 0, GetWidth(), GetHeight()};
    RECT  rcObjPart = {0};   // 规范过的合理区域

    if (NULL == prcObjPart)
    {
        CopyRect(&rcObjPart, &rcObj);
    }
    else
    {
        if (!::IntersectRect(&rcObjPart, prcObjPart, &rcObj))
            return false;
    }
    
    // 自己
	if (pObjAncestor == this)
	{
		if (prcOut)
		    CopyRect(prcOut, &rcObjPart);
		return true;
	}

    //  向上遍历
	CRect    rcClip(&rcObjPart);
	Object*  pParent = NULL;
	Object*  pChild = this;
	while (pParent = this->EnumParentObject(pParent))
	{
        // 转换成在父中的区域
		Object::ChildRect2ParentRect(pChild, &rcClip, &rcClip);

        // 与父对象进行求交
		CRect  rcParent(0, 0, pParent->GetWidth(), pParent->GetHeight());
		if (!::IntersectRect(rcClip, &rcClip, &rcParent))
			return false;

		if (pParent == pObjAncestor)
			break;

		pChild = pParent;
	}
	
	::CopyRect(prcOut, &rcClip);
	return true;
}


// 测试窗口上的区域与自己的交集
bool  Object::IntersectWindowRect(const RECT* prcWindow, RECT* prcIntersectWnd, RECT* prcIntersectObj)
{
	UIASSERT(prcWindow);

	// wnd
	RECT  rcWndMe, rcIntersectWnd;
	GetVisibleRectInWindow(&rcWndMe);

	if (!::IntersectRect(&rcIntersectWnd, &rcWndMe, prcWindow))
		return false;

	if (prcIntersectWnd)
	{
		CopyRect(prcIntersectWnd, &rcIntersectWnd);
	}

	// obj
	if (prcIntersectObj)
	{
		//         RECT  rcObj = {0};
		//         WindowRect2ObjectRect(prcWindow, &rcObj);
		// 
		//         RECT  rcMy = {0, 0, GetWidth(), GetHeight()};
		//         RECT  rcIntersect = {0};
		//         IntersectRect(&rcIntersect, &rcMy, &rcObj);
		//         ::CopyRect(prcIntersectObj, &rcIntersect);

		RECT rcMe = {0, 0, GetWidth(), GetHeight()};
		rcMe.left += rcIntersectWnd.left - rcWndMe.left;
		rcMe.right -= rcWndMe.right - rcIntersectWnd.right;
		rcMe.top += rcIntersectWnd.top - rcWndMe.top;
		rcMe.bottom += rcWndMe.bottom - rcIntersectWnd.bottom;

		CopyRect(prcIntersectObj, &rcMe);

	}
	return true;
}


// 注：这里的clientrect 并不是0,0起点的，而是相对于Object左上角而言
void Object::GetClientRect(CRect* prc)
{
	if (NULL == prc)
		return;

	prc->SetRect(
		m_rcNonClient.left, 
		m_rcNonClient.top, 
		this->GetWidth() - m_rcNonClient.right, 
		this->GetHeight()- m_rcNonClient.bottom); 
}

// 注：这里的clientrect 是0,0起点的

void Object::GetClientRectAsWin32( CRect* prc )
{
	if (NULL == prc)
		return;

	prc->SetRect(
		0, 
		0, 
		this->GetWidth() - m_rcNonClient.left - m_rcNonClient.right, 
		this->GetHeight()- m_rcNonClient.top - m_rcNonClient.bottom); 
}

// clientrect在窗口中的坐标
void Object::GetClientRectInWindow( CRect* prc )
{
	if (NULL == prc)
		return;

	this->GetWindowRect(prc);
	prc->left += m_rcNonClient.left;
	prc->top  += m_rcNonClient.top;
	prc->right  -= m_rcNonClient.right;
	prc->bottom -= m_rcNonClient.bottom;
}



// Panel/Object需要响应WM_SIZE，但有可能控件拦截了WM_SIZE导致内部无法处理到
// 因此再增加一个虚函数，专门让内部处理size改变事件
void  Object::notify_WM_SIZE(UINT nType, UINT nWidth, UINT nHeight)
{
    this->virtualOnSize(nType, nWidth, nHeight);
    UISendMessage(m_pIObject, WM_SIZE, 0, MAKELPARAM(nWidth, nHeight));
}
void  Object::virtualOnSize(UINT nType, UINT nWidth, UINT nHeight)
{
    if (m_pRenderLayer)
        m_pRenderLayer->OnSize(nWidth, nHeight);
}

//
//	当对象显示/隐藏，或者大小发生改变时，重新刷新自己所在layout的布局
//
//	1. 获取自己的所在Layout
//	2. 测量Layout的大小是否发生改变，如果这个Layout大小改变，再获取上一层Layout
//
void Object::UpdateLayout(bool bUpdate)
{
    if (this->GetObjectType() == OBJ_WINDOW)
    {
        ILayout* pLayout = static_cast<WindowBase*>(this)->GetLayout();
        if (pLayout)
        {
            pLayout->Arrange(NULL);

            if (bUpdate)
                this->UpdateObject();
        }
        return;
    }

    Object* pParent = this->GetParentObject();
    Object* pObjectToUpdate = this;

    while (pParent)
    {
        int nObjType = pParent->GetObjectType();
        if (OBJ_PANEL != nObjType && OBJ_WINDOW != nObjType)  // 例如listview中的headerctrl，它的父对象不是panel
        {
            pParent->notify_WM_SIZE(0, pParent->GetWidth(), pParent->GetHeight());
            return;
        }

        ILayout* pLayout = ((Panel*)pParent)->GetLayout();
        if (NULL == pLayout)
        {
            UI_LOG_WARN(_T("%s  GetLayout failed. id=%s"), FUNC_NAME, pParent->m_strId.c_str());
            break;
        }

        ILayoutParam*  pLayoutParam = pParent->GetLayoutParam();
        if (pLayoutParam && !pLayoutParam->IsSizedByContent())
        {
            pLayout->Arrange(pObjectToUpdate->GetIObject(), bUpdate);
            break;
        }

        SIZE sizeOld = {pParent->GetWidth(), pParent->GetHeight()};
        SIZE size = pParent->GetDesiredSize();

        if (sizeOld.cx == size.cx || sizeOld.cy == size.cy)
        {
            pLayout->Arrange(pObjectToUpdate->GetIObject(), bUpdate);
            break;
        }
        // pParent的大小发生了变化，继续往上
        if (pParent->GetParentObject())
        {
            pObjectToUpdate = pParent;
            pParent = pParent->GetParentObject();  // TODO: 有点乱... 当是窗口的时候size为window rect，sizeOld为client rect，因此肯定不一样
            continue;                              // 所以在这里如果发现是window( parent==null )则继续往下处理
        }
        else if (pParent->GetObjectType() == OBJ_WINDOW)
        {
            HWND hWnd = GetHWND();

            CRect rcWndNow;
            ::GetWindowRect(hWnd, rcWndNow);
            if (rcWndNow.Width() == size.cx && rcWndNow.Height() == size.cy)  // 当大小没有改变时，不会触发WM_SIZE，也就不会更新了
            {
                pLayout->Arrange(pObjectToUpdate->GetIObject(), bUpdate);
            }
            else
            {
                
                UINT nFlag = SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE;
                if (!bUpdate) 
                {
                    WindowRender*  pRender = static_cast<WindowBase*>(pParent)->GetWindowRender();
                    pRender->SetCanCommit(false);
                    SetWindowPos(GetHWND(), 0, 0,0, size.cx, size.cy, nFlag);
                    pRender->SetCanCommit(true);
                }
                else
                {
                    SetWindowPos(GetHWND(), 0, 0,0, size.cx, size.cy, nFlag);
                }
            }
            break;
        }
    }
}

// UpdateLayout更新自己在窗口上的布局，一般不修改其自己的内部布局
// UpdateMyLayout只修改自己的内部布局
void  Object::UpdateMyLayout(bool bUpdate)
{
    IPanel*  pPanel = (IPanel*)m_pIObject->QueryInterface(uiiidof(IPanel));
    if (pPanel)
    {
        ILayout*  pLayout = pPanel->GetLayout();
        if (pLayout)
        {
            pLayout->Arrange(NULL, bUpdate);
        }
    }
}

//
//	获取对象自己期望的大小
//
SIZE Object::GetDesiredSize()
{
    if (!m_pLayoutParam)
    {
        CreateLayoutParam();

        if (!m_pLayoutParam) // 还是创建不成功，例如Combobox中的Button，它没有父Panel-Layout
        {
            m_pLayoutParam = CanvasLayout::s_CreateLayoutParam(m_pIObject);
        }
    }

    return m_pLayoutParam->CalcDesiredSize();
}


void Object::SetObjectPos(int x, int y, int cx, int cy, int nFlag)
{
    bool bMove = (nFlag&SWP_NOMOVE)?false:true;
    bool bSize = (nFlag&SWP_NOSIZE)?false:true;

    //  TODO: 加上这一段代码会导致SetVisible中的布局出现问题，如果pp -> p -> o，o发生变化
    //  让p去重新布局，然后p再检测自己的desiredsize变化了，再向pp请求重新布局，结果pp的大
    //  小没有发生变化，导致p不会去重新布局，o刷新失败。
    //     if (bMove && 
    //         x == m_rcParent.left &&
    //         y == m_rcParent.top && 
    //         bSize && 
    //         cx == m_rcParent.Width() && 
    //         cy == m_rcParent.Height())
    //         return;

    nFlag |= SWP_NOZORDER;   // 该函数不提供修改ZORDER的功能
    WINDOWPOS wndpos =  {NULL, NULL, x, y, cx, cy, nFlag};
    UISendMessage(this, WM_WINDOWPOSCHANGING, 0, (LPARAM)&wndpos);
    x = wndpos.x;
    y = wndpos.y;
    cx = wndpos.cx;
    cy = wndpos.cy;
    nFlag = wndpos.flags;

    if (x == m_rcParent.left && y == m_rcParent.top)
        bMove = false;
    if (cx == m_rcParent.Width() && cy == m_rcParent.Height())
        bSize = false;

    if (bMove&&bSize)
    {

    }
    else if (bMove)
    {
        cx = this->GetWidth();
        cy = this->GetHeight();
    }
    else if (bSize)
    {
        x = m_rcParent.left;
        y = m_rcParent.top;
    }
    else
    {
        if (nFlag & SWP_FORCESENDSIZEMSG)
        {
            notify_WM_SIZE(0, m_rcParent.Width(),m_rcParent.Height());
        }
        return;  // DONOTHING
    }

    RECT rcOldVisibleRect = {0};
    if (bMove || bSize) 
    {
        // 刷新移动前的区域位置
        if (!(nFlag & SWP_NOREDRAW))
        {
            this->GetVisibleRectInWindow(&rcOldVisibleRect);  // 获取下当前会刷新的区域范围，放在后面进行提交
            this->UpdateObjectBkgnd(false);  // 这里不能立即刷新，否则后面再调用UpdateObject可能造成闪烁（移动后的位置有重合的情况下）
        }
    }

    int nObjType = this->GetObjectType();
    if (nObjType == OBJ_WINDOW)
    {
        // 对于窗口来说，这里设置的是非客户区的大小
        WindowBase* pThis = (WindowBase*)this;
        ::SetWindowPos(pThis->GetHWND(), NULL, x, y, cx, cy, SWP_NOZORDER|SWP_NOACTIVATE);
        ::GetClientRect(pThis->GetHWND(), &m_rcParent);
        return;
    }
    else if (nObjType == OBJ_HWNDHOST)
    {
        m_rcParent.SetRect(x, y, x+cx, y+cy);

        // 转化为窗口坐标，而不是父对象坐标
        HwndHost* pThis = (HwndHost*)this;
        if (pThis->m_hWnd)
        {
            CRect rcWindow;
            pThis->GetWindowRect(&rcWindow);
            ::SetWindowPos(pThis->m_hWnd, NULL, 
                rcWindow.left, rcWindow.top, rcWindow.Width(), rcWindow.Height(), 
                SWP_NOZORDER|SWP_NOACTIVATE);
        }

        if (!(nFlag&SWP_NOUPDATELAYOUTPOS))
        {
            UpdateLayoutPos();
        }
        return;
    }
    else
    {
        ::SetRect(&m_rcParent, x,y,x+cx,y+cy);
    }
    if (!(nFlag&SWP_NOUPDATELAYOUTPOS))
    {
        UpdateLayoutPos();
    }

    // MSDN: MoveWindow sends the WM_WINDOWPOSCHANGING, WM_WINDOWPOSCHANGED, WM_MOVE, WM_SIZE, and WM_NCCALCSIZE messages to the window. 
    // 在这里我们暂时只先发送WM_MOVE/WM_SIZE消息
    if (!(nFlag & SWP_NOSENDCHANGING))
    {
        if (bMove)
        {
            UISendMessage(this, WM_MOVE, 0, MAKELPARAM(m_rcParent.left,m_rcParent.top));
        }
        if (bSize)
        {
            notify_WM_SIZE(0, m_rcParent.Width(),m_rcParent.Height());
        }

        WINDOWPOS wndpos2 =  {NULL, NULL, x, y, cx, cy, nFlag};
        UISendMessage(this, WM_WINDOWPOSCHANGED, NULL, (LPARAM)&wndpos2);
    }

    if (bMove || bSize)
    {
        // 刷新移动后的区域位置
        if (!(nFlag & SWP_NOREDRAW))
        {
            this->UpdateObject();

            // 将移动之前的区域数据提交到窗口上，避免闪烁
            this->GetWindowObject()->CommitDoubleBuffet2Window(NULL, &rcOldVisibleRect);
        }
        else
        {
            GetRenderLayer2()->SetDirty(true);
        }
    }
}


void Object::SetObjectPos(CRect* prc, int nFlag)
{
    if (NULL == prc)
        return;

    this->SetObjectPos(prc->left, prc->top, prc->Width(), prc->Height(), nFlag);
}

// 根据m_rcParent更新 m_nConfigLeft/m_nConfigRight/m_nConfigTop/m_nConfigBottom/m_nConfigLayoutFlags
void Object::UpdateLayoutPos()
{
    if (m_pLayoutParam)
    {
        m_pLayoutParam->UpdateByRect();
    }
}

int Object::GetWidth()
{
	return m_rcParent.Width();
}


int Object::GetHeight()
{
	return m_rcParent.Height();
}


int Object::GetWidthWithMargins()
{
	return m_rcParent.Width() + m_rcMargin.left + m_rcMargin.right;
}


int Object::GetHeightWithMargins()
{
	return m_rcParent.Height() + m_rcMargin.top + m_rcMargin.bottom;
}


//
// 遍历自己的nc object来更新自己的non client region
//
void Object::UpdateObjectNonClientRegion()
{
	CRegion4  rcNonClient(0,0,0,0);

    UIMSG msg;
	msg.message = UI_WM_CALC_PARENT_NONCLIENTRECT;
	msg.wParam = (WPARAM)&rcNonClient;
	msg.pMsgFrom = this->GetIMessage();

	Object* pNcChild = NULL;
	while (pNcChild = this->EnumNcChildObject(pNcChild))
	{
		msg.pMsgTo = pNcChild->GetIMessage();
		UISendMessage(&msg);
	}

	this->SetNonClientRegionExcludePaddingBorder(&rcNonClient);
}


void Object::SetNonClientRegionExcludePaddingBorder( CRegion4* prc )
{
	m_rcNonClient.SetRect(
		prc->left   + m_rcPadding.left   + m_rcBorder.left,
		prc->top    + m_rcPadding.top    + m_rcBorder.top,
		prc->right  + m_rcPadding.right  + m_rcBorder.right,
		prc->bottom + m_rcPadding.bottom + m_rcBorder.bottom);
}


void Object::SetNonClientRegionExcludePaddingBorderL( int n )
{
	m_rcNonClient.left = n + m_rcPadding.left + m_rcBorder.left;
}

void Object::SetNonClientRegionExcludePaddingBorderT( int n )
{
	m_rcNonClient.top = n + m_rcPadding.top + m_rcBorder.top;
}

void Object::SetNonClientRegionExcludePaddingBorderB( int n )
{
	m_rcNonClient.bottom = n + m_rcPadding.bottom + m_rcBorder.bottom;
}

void Object::SetNonClientRegionExcludePaddingBorderR( int n )
{
	m_rcNonClient.right = n + m_rcPadding.right + m_rcBorder.right;
}


void Object::GetParentRect( CRect* prc )
{
	if (NULL == prc)
		return;

	prc->CopyRect(&m_rcParent);
}

ILayoutParam*  Object::GetLayoutParam()
{
    return m_pLayoutParam;
}
void  Object::CreateLayoutParam()
{
    SAFE_RELEASE(m_pLayoutParam);

    if (!m_pParent)
        return;

    ILayout* pLayout = (ILayout*)UISendMessage(m_pParent, UI_WM_GETLAYOUT);
    if (!pLayout)
        return;

    m_pLayoutParam = pLayout->CreateLayoutParam(m_pIObject);
}

void  Object::SetLayoutParam(ILayoutParam* p)
{
    SAFE_RELEASE(m_pLayoutParam);
    m_pLayoutParam = p;
}
int   Object::GetConfigWidth()
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() != LAYOUT_TYPE_CANVAS)
            return NDEF;
        else
            return static_cast<CanvasLayoutParam*>(m_pLayoutParam)->GetConfigWidth();
    }
    else
    {
        int nRet = NDEF;
        m_pIMapAttributeRemain->GetAttr_int(XML_WIDTH, false, &nRet);
        return nRet;
    }
}
int   Object::GetConfigHeight()
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() != LAYOUT_TYPE_CANVAS)
            return NDEF;
        else
            return static_cast<CanvasLayoutParam*>(m_pLayoutParam)->GetConfigHeight();
    }
    else
    {
        int nRet = NDEF;
        m_pIMapAttributeRemain->GetAttr_int(XML_HEIGHT, false, &nRet);
        return nRet;
    }
}
int   Object::GetConfigLayoutFlags()
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() != LAYOUT_TYPE_CANVAS)
            return 0;
        else
            return static_cast<CanvasLayoutParam*>(m_pLayoutParam)->GetConfigLayoutFlags();
    }
    else
    {
        const TCHAR* szText = m_pIMapAttributeRemain->GetAttr(XML_LAYOUT_ITEM_ALIGN, false);
        if (!szText)
            return 0;

        return CanvasLayoutParam::ParseAlignAttr(szText);
    }
}
int   Object::GetConfigLeft()
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() != LAYOUT_TYPE_CANVAS)
            return NDEF;
        else
            return static_cast<CanvasLayoutParam*>(m_pLayoutParam)->GetConfigLeft();
    }
    else
    {
        int nRet = NDEF;
        m_pIMapAttributeRemain->GetAttr_int(XML_LAYOUT_ITEM_LEFT, false, &nRet);
        return nRet;
    }
}
int   Object::GetConfigRight()
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() != LAYOUT_TYPE_CANVAS)
            return NDEF;
        else
            return static_cast<CanvasLayoutParam*>(m_pLayoutParam)->GetConfigRight();
    }
    else
    {
        int nRet = NDEF;
        m_pIMapAttributeRemain->GetAttr_int(XML_LAYOUT_ITEM_RIGHT, false, &nRet);
        return nRet;
    }
}
int   Object::GetConfigTop()
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() != LAYOUT_TYPE_CANVAS)
            return NDEF;
        else
            return static_cast<CanvasLayoutParam*>(m_pLayoutParam)->GetConfigTop();
    }
    else
    {
        int nRet = NDEF;
        m_pIMapAttributeRemain->GetAttr_int(XML_LAYOUT_ITEM_TOP, false, &nRet);
        return nRet;
    }
}
int   Object::GetConfigBottom()
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() != LAYOUT_TYPE_CANVAS)
            return NDEF;
        else
            return static_cast<CanvasLayoutParam*>(m_pLayoutParam)->GetConfigBottom();
    }
    else
    {
        int nRet = NDEF;
        m_pIMapAttributeRemain->GetAttr_int(XML_LAYOUT_ITEM_BOTTOM, false, &nRet);
        return nRet;
    }
}

void  Object::SetConfigWidth(int n)
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
            static_cast<CanvasLayoutParam*>(m_pLayoutParam)->SetConfigWidth(n);
    }
    else
    {
        CanvasLayout::s_GetObjectLayoutParam(this)->SetConfigWidth(n);
    }
}
void  Object::SetConfigHeight(int n)
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
            static_cast<CanvasLayoutParam*>(m_pLayoutParam)->SetConfigHeight(n);
    }
    else
    {
        CanvasLayout::s_GetObjectLayoutParam(this)->SetConfigHeight(n);
    }
}
void  Object::SetConfigLayoutFlags(int n)
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
            static_cast<CanvasLayoutParam*>(m_pLayoutParam)->SetConfigLayoutFlags(n);
    }
    else
    {
        CanvasLayout::s_GetObjectLayoutParam(this)->SetConfigLayoutFlags(n);
    }
}
void  Object::SetConfigLeft(int n)
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
            static_cast<CanvasLayoutParam*>(m_pLayoutParam)->SetConfigLeft(n);
    }
    else
    {
        CanvasLayout::s_GetObjectLayoutParam(this)->SetConfigLeft(n);
    }
}
void  Object::SetConfigRight(int n)
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
            static_cast<CanvasLayoutParam*>(m_pLayoutParam)->SetConfigRight(n);
    }
    else
    {
        CanvasLayout::s_GetObjectLayoutParam(this)->SetConfigRight(n);
    }
}
void  Object::SetConfigTop(int n)
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
            static_cast<CanvasLayoutParam*>(m_pLayoutParam)->SetConfigTop(n);
    }
    else
    {
        CanvasLayout::s_GetObjectLayoutParam(this)->SetConfigTop(n);
    }
}
void  Object::SetConfigBottom(int n)
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
            static_cast<CanvasLayoutParam*>(m_pLayoutParam)->SetConfigBottom(n);
    }
    else
    {
        CanvasLayout::s_GetObjectLayoutParam(this)->SetConfigBottom(n);
    }
}

}