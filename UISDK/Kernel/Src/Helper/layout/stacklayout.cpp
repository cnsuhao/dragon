#include "stdafx.h"
#include "stacklayout.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"
#include "UISDK\Kernel\Src\Base\Attribute\attribute.h"
#include "UISDK\Kernel\Src\Base\Attribute\enum_attribute.h"
#include "UISDK\Kernel\Src\Base\Attribute\flags_attribute.h"

namespace UI
{

StackLayout::StackLayout()
{
    m_nSpace = 0;
	this->m_eDirection = LAYOUT_STACK_LEFTTORIGHT;   // 默认堆栈是从左到右
}
StackLayout::~StackLayout()
{
}
void  StackLayout::Serialize(SERIALIZEDATA* pData)
{
    AttributeSerializer s(pData, TEXT("StackLayout"));
    s.AddLong(XML_LAYOUT_STACK_GAP,  m_nSpace);
    s.AddEnum(XML_LAYOUT_STACK_DIRECTION, *(long*)&m_eDirection)
        ->AddOption(LAYOUT_STACK_LEFTTORIGHT, XML_LAYOUT_STACK_LEFTTORIGHT)
        ->AddOption(LAYOUT_STACK_RIGHTTOLEFT, XML_LAYOUT_STACK_RIGHTTOLEFT)
        ->AddOption(LAYOUT_STACK_LEFTANDRIGHT, XML_LAYOUT_STACK_LEFTANDRIGHT)
        ->AddOption(LAYOUT_STACK_TOPTOBOTTOM, XML_LAYOUT_STACK_TOPTOBOTTOM)
        ->AddOption(LAYOUT_STACK_BOTTOMTOTOP, XML_LAYOUT_STACK_BOTTOMTOTOP)
        ->AddOption(LAYOUT_STACK_TOPANDBOTTOM, XML_LAYOUT_STACK_TOPANDBOTTOM);
}

void  StackLayout::SetLayoutDirection(LAYOUT_STACK_DIRECTION e)
{
    m_eDirection = e;
}
void  StackLayout::SetSpace(int n)
{
    m_nSpace = n;
}

SIZE  StackLayout::Measure()
{
	SIZE  size = {0,0};

	// 通过子控件来获取自己所需要的大小
	Object*  pChild = NULL;
	while (pChild = this->m_pPanel->EnumChildObject(pChild))
	{
        if (pChild->IsSelfCollapsed())
        {
            continue;
        }

        StackLayoutParam* pParam = s_GetObjectLayoutParam(pChild);
        if (!pParam)
        {
            continue;
        }

		SIZE  s = pChild->GetDesiredSize();

		if (IsHorz())
        {
			size.cx += s.cx + m_nSpace;
			if (size.cy < s.cy)
				size.cy = s.cy;
        }
        else
        {
			size.cy += s.cy + m_nSpace;
			if (size.cx < s.cx)
				size.cx = s.cx;
		}
	}

	return size;
}

bool  StackLayout::IsHorz()
{
    if (m_eDirection == LAYOUT_STACK_LEFTTORIGHT || 
        m_eDirection == LAYOUT_STACK_RIGHTTOLEFT ||
        m_eDirection == LAYOUT_STACK_LEFTANDRIGHT)
        return true;

    return false;
}
bool  StackLayout::IsVert()
{
    return !IsHorz();
}

void  StackLayout::Arrange(IObject* pIObjToArrage, bool bUpdate)
{
    Object* pObjToArrage = NULL;
    if (pIObjToArrage)
    {
        pObjToArrage = pIObjToArrage->GetImpl();
    }

	int  nConsume1 = 0; // 当前已消耗的高度或宽度（从left/top开始计数）
	int  nConsume2 = 0; // 当前已消耗的高度或宽度（从right/bottom开始计数）
	
	bool bStartToArrange = false;
	if (NULL == pObjToArrage)
    {
		bStartToArrange = true;
    }

	Object* pChild = NULL;
	while (pChild = m_pPanel->EnumChildObject(pChild))
	{
		if (pObjToArrage && pChild == pObjToArrage)
        {
			bStartToArrange = true;
        }

		// 放在IsSelfCollapsed之前。editor中也需要加载隐藏对象的布局属性
		StackLayoutParam* pParam = s_GetObjectLayoutParam(pChild);
		if (!pParam)
			continue;

		if (pChild->IsSelfCollapsed())
			continue;

		SIZE s;
		bool  bFill = pParam->GetConfigLayoutFlags() & LAYOUT_ITEM_ALIGN_FILL ? true:false;
		if (bStartToArrange || bFill)  // fill mode的子控件会受其它子控件大小改变影响，因此需要每次都判断
		{
            s = pChild->GetDesiredSize();
            if (bFill)
            {
                // 计算出剩下的对象的所需大小
                Object* pChildTemp = pChild;
                SIZE sNeedNext = {0, 0}; 
                while (pChildTemp = m_pPanel->EnumChildObject(pChildTemp))
                {
                    if (!pChildTemp->IsSelfVisible())
                    {
                        continue;
                    }

                    SIZE sThis = pChildTemp->GetDesiredSize();
                    if (IsHorz())
                    {
                        sNeedNext.cx += sThis.cx + m_nSpace;
                    }
                    else
                    {
                        sNeedNext.cy += sThis.cy + m_nSpace;
                    }
                }

                CRect rcClient;
                m_pPanel->GetClientRectInObject(&rcClient);
                int  nPanelWidth  = rcClient.Width();
                int  nPanelHeight = rcClient.Height();

                if (IsHorz())
                {
                    s.cx = nPanelWidth - sNeedNext.cx - nConsume2 - nConsume1;
                }
                else
                {
                    s.cy = nPanelHeight - sNeedNext.cy - nConsume2 - nConsume1;
                }
            }
		}
		else
		{
			s.cx = pChild->GetWidth() + pChild->GetMarginW();
			s.cy = pChild->GetHeight() + pChild->GetMarginH();
		}


		switch (this->m_eDirection)
		{    
		case LAYOUT_STACK_LEFTTORIGHT:
            ArrangeObject_Left(pChild, nConsume1, nConsume2, s);
			break;

		case LAYOUT_STACK_RIGHTTOLEFT:
            ArrangeObject_Right(pChild, nConsume1, nConsume2, s);
			break;

		case LAYOUT_STACK_LEFTANDRIGHT:
			{
				bool bDockLeft = true;   // 默认停靠在左侧

				if (pParam->GetConfigLayoutFlags() & LAYOUT_ITEM_ALIGN_LEFT)
					bDockLeft = true;
				else if (pParam->GetConfigLayoutFlags() & LAYOUT_ITEM_ALIGN_RIGHT)
					bDockLeft = false;
				else
					bDockLeft = true;

                // 计算位置
				if (bDockLeft)
                    ArrangeObject_Left(pChild, nConsume1, nConsume2, s);
				else
                    ArrangeObject_Right(pChild, nConsume1, nConsume2, s);

			}
			break;

			// 从上到下
		case LAYOUT_STACK_TOPTOBOTTOM:
			ArrangeObject_Top(pChild, nConsume1, nConsume2, s);
			break;

		case LAYOUT_STACK_BOTTOMTOTOP:
			ArrangeObject_Bottom(pChild, nConsume1, nConsume2, s);
			break;

		case LAYOUT_STACK_TOPANDBOTTOM:
			{
				bool bDockTop = true;   // 默认停靠在上侧

				if (pParam->GetConfigLayoutFlags() & LAYOUT_ITEM_ALIGN_TOP)
					bDockTop = true;
				else if (pParam->GetConfigLayoutFlags() & LAYOUT_ITEM_ALIGN_BOTTOM)
					bDockTop = false;
				else
					bDockTop = true;

                // 计算位置
				if (bDockTop)
					ArrangeObject_Top(pChild, nConsume1, nConsume2, s);
				else
					ArrangeObject_Bottom(pChild, nConsume1, nConsume2, s);
			}
			break;
		}
	}

    if (bUpdate)
    {
        m_pPanel->UpdateObject(true);
    }
}


void  StackLayout::ArrangeObject_Left(Object* pChildObj, int& nLeftCustom, int& nRightCustom, SIZE size)
{
    StackLayoutParam* pParam = s_GetObjectLayoutParam(pChildObj);
    if (!pParam)
        return;

    // 调用该函数时，自己的大小已经被求出来了
    CRect rcClient;
    m_pPanel->GetClientRectInObject(&rcClient);
    int  nPanelWidth  = rcClient.Width();  //m_pPanel->GetWidth();
    int  nPanelHeight = rcClient.Height(); //m_pPanel->GetHeight();

    int  nFlag = pParam->GetConfigLayoutFlags();
    int  y = 0;
    int  h = nPanelHeight;

    if (pParam->GetConfigWidth() < 0)
    {
        int nMaxWidthNow = nPanelWidth-nLeftCustom-nRightCustom;
        if (size.cx > nMaxWidthNow)
            size.cx = nMaxWidthNow;
    }

    if (nFlag & LAYOUT_ITEM_ALIGN_TOP)
    {
        y = 0;
        h = size.cy;
    }
    else if (nFlag & LAYOUT_ITEM_ALIGN_BOTTOM)
    {
        y = (nPanelHeight - size.cy);
        h = size.cy;
    }
    else if (nFlag & LAYOUT_ITEM_ALIGN_VCENTER)
    {
        y = (nPanelHeight - size.cy)/2;
        h = size.cy;
    }
  
    pChildObj->SetObjectPos(
        nLeftCustom + pChildObj->GetMarginL(), 
        y + pChildObj->GetMarginT(), 
        size.cx - pChildObj->GetMarginW(), 
        h - pChildObj->GetMarginH(),
        SWP_NOREDRAW|SWP_NOUPDATELAYOUTPOS|SWP_FORCESENDSIZEMSG);

    nLeftCustom += size.cx + m_nSpace;
}

void  StackLayout::ArrangeObject_Right(Object* pChildObj, int& nLeftCustom, int& nRightCustom, SIZE size)
{
    StackLayoutParam* pParam = s_GetObjectLayoutParam(pChildObj);
    if (!pParam)
        return;

    // 调用该函数时，自己的大小已经被求出来了
    CRect rcClient;
    m_pPanel->GetClientRectInObject(&rcClient);
    int  nPanelWidth  = rcClient.Width();  //m_pPanel->GetWidth();
    int  nPanelHeight = rcClient.Height(); //m_pPanel->GetHeight();

    int  nFlag = pParam->GetConfigLayoutFlags();
    int  y = 0;
    int  h = nPanelHeight;

    if (pParam->GetConfigWidth() < 0)
    {
        int nMaxWidthNow = nPanelWidth-nLeftCustom-nRightCustom;
        if (size.cx > nMaxWidthNow)
            size.cx = nMaxWidthNow;
    }

    if (nFlag & LAYOUT_ITEM_ALIGN_TOP)
    {
        y = 0;
        h = size.cy;
    }
    else if (nFlag & LAYOUT_ITEM_ALIGN_BOTTOM)
    {
        y = (nPanelHeight - size.cy);
        h = size.cy;
    }
    else if (nFlag & LAYOUT_ITEM_ALIGN_VCENTER)
    {
        y = (nPanelHeight - size.cy)/2;
        h = size.cy;
    }

    pChildObj->SetObjectPos(
        nPanelWidth - nRightCustom - size.cx + pChildObj->GetMarginL(), 
        0 + pChildObj->GetMarginT(), 
        size.cx - pChildObj->GetMarginW(), 
        h - pChildObj->GetMarginH(),
        SWP_NOREDRAW|SWP_NOUPDATELAYOUTPOS|SWP_FORCESENDSIZEMSG);

    nRightCustom += size.cx + m_nSpace;
}

void  StackLayout::ArrangeObject_Top(Object* pChildObj, int& nTopCustom, int& nBottomCustom, SIZE size)
{
    StackLayoutParam* pParam = s_GetObjectLayoutParam(pChildObj);
    if (!pParam)
        return;

    // 调用该函数时，自己的大小已经被求出来了
    CRect rcClient;
    m_pPanel->GetClientRectInObject(&rcClient);
    int  nPanelWidth  = rcClient.Width();  //m_pPanel->GetWidth();
    int  nPanelHeight = rcClient.Height(); //m_pPanel->GetHeight();

    int  nFlag = pParam->GetConfigLayoutFlags();
    int  x = 0;
    int  w = nPanelWidth;

    if (pParam->GetConfigHeight() < 0)
    {
        int nMaxHeigthNow = nPanelHeight-nTopCustom-nBottomCustom;
        if (size.cy > nMaxHeigthNow)
            size.cy = nMaxHeigthNow;
    }

    if (nFlag & LAYOUT_ITEM_ALIGN_LEFT)
    {
        x = 0;
        w = size.cx;
    }
    else if (nFlag & LAYOUT_ITEM_ALIGN_RIGHT)
    {
        x = (nPanelWidth - size.cx);
        w = size.cx;
    }
    else if (nFlag & LAYOUT_ITEM_ALIGN_CENTER)
    {
        x = (nPanelWidth - size.cx)/2;
        w = size.cx;
    }

    pChildObj->SetObjectPos(
        x + pChildObj->GetMarginL(), 
        nTopCustom + pChildObj->GetMarginT(), 
        w - pChildObj->GetMarginW(), 
        size.cy - pChildObj->GetMarginH(),
        SWP_NOREDRAW|SWP_NOUPDATELAYOUTPOS|SWP_FORCESENDSIZEMSG);

    nTopCustom += size.cy + m_nSpace;
}
void  StackLayout::ArrangeObject_Bottom(Object* pChildObj, int& nTopCustom, int& nBottomCustom, SIZE size)
{
    // 调用该函数时，自己的大小已经被求出来了
    StackLayoutParam* pParam = s_GetObjectLayoutParam(pChildObj);
    if (!pParam)
        return;

    CRect rcClient;
    m_pPanel->GetClientRectInObject(&rcClient);
    int  nPanelWidth  = rcClient.Width();  //m_pPanel->GetWidth();
    int  nPanelHeight = rcClient.Height(); //m_pPanel->GetHeight();

    int  nFlag = pParam->GetConfigLayoutFlags();
    int  x = 0;
    int  w = nPanelWidth;

    if (pParam->GetConfigHeight() < 0)
    {
        int nMaxHeigthNow = nPanelHeight-nTopCustom-nBottomCustom;
        if (size.cy > nMaxHeigthNow)
            size.cy = nMaxHeigthNow;
    }

    if (nFlag & LAYOUT_ITEM_ALIGN_LEFT)
    {
        x = 0;
        w = size.cx;
    }
    else if (nFlag & LAYOUT_ITEM_ALIGN_RIGHT)
    {
        x = (nPanelWidth - size.cx);
        w = size.cx;
    }
    else if (nFlag & LAYOUT_ITEM_ALIGN_CENTER)
    {
        x = (nPanelWidth - size.cx)/2;
        w = size.cx;
    }

    pChildObj->SetObjectPos( 
        x + pChildObj->GetMarginL(), 
        nPanelHeight - nBottomCustom - size.cy + pChildObj->GetMarginT(), 
        w - pChildObj->GetMarginW(), 
        size.cy - pChildObj->GetMarginH(),
        SWP_NOREDRAW|SWP_NOUPDATELAYOUTPOS|SWP_FORCESENDSIZEMSG);

    nBottomCustom += size.cy + m_nSpace;
}

//////////////////////////////////////////////////////////////////////////

StackLayoutParam::StackLayoutParam()
{
    m_nConfigWidth = AUTO;
    m_nConfigHeight = AUTO;
    m_nConfigLayoutFlags = 0;
}
StackLayoutParam::~StackLayoutParam()
{

}

void  StackLayoutParam::UpdateByRect()
{
    CRect  rcParent;
    m_pObj->GetParentRect(&rcParent);

    if (m_nConfigWidth >= 0)
    {
        m_nConfigWidth = rcParent.Width();
    }
    if (m_nConfigHeight >= 0)
    {
        m_nConfigHeight = rcParent.Height();
    }
}
void  StackLayoutParam::Serialize(SERIALIZEDATA* pData)
{
    AttributeSerializer s(pData, TEXT("StackLayoutParam"));
    s.AddLong(XML_WIDTH,  m_nConfigWidth) ->AddAlias(AUTO, XML_AUTO)->SetDefault(AUTO);
    s.AddLong(XML_HEIGHT, m_nConfigHeight)->AddAlias(AUTO, XML_AUTO)->SetDefault(AUTO);

    s.AddFlags(XML_LAYOUT_ITEM_ALIGN, m_nConfigLayoutFlags)
        ->AddFlag(LAYOUT_ITEM_ALIGN_LEFT,        XML_LAYOUT_ITEM_ALIGN_LEFT)
        ->AddFlag(LAYOUT_ITEM_ALIGN_RIGHT,       XML_LAYOUT_ITEM_ALIGN_RIGHT)
        ->AddFlag(LAYOUT_ITEM_ALIGN_TOP,         XML_LAYOUT_ITEM_ALIGN_TOP)
        ->AddFlag(LAYOUT_ITEM_ALIGN_BOTTOM,      XML_LAYOUT_ITEM_ALIGN_BOTTOM)
        ->AddFlag(LAYOUT_ITEM_ALIGN_CENTER,      XML_LAYOUT_ITEM_ALIGN_CENTER)
        ->AddFlag(LAYOUT_ITEM_ALIGN_VCENTER,     XML_LAYOUT_ITEM_ALIGN_VCENTER)
        ->AddFlag(LAYOUT_ITEM_ALIGN_FILL,        XML_LAYOUT_ITEM_ALIGN_FILL);
}

SIZE  StackLayoutParam::CalcDesiredSize()
{
    SIZE size = {0,0};

    bool bWidthNotConfiged = m_nConfigWidth == AUTO ? true:false;
    bool bHeightNotConfiged = m_nConfigHeight == AUTO ? true:false;;

    if (bWidthNotConfiged || bHeightNotConfiged)
    {
        // 获取子对象所需要的空间
        UISendMessage(m_pObj, UI_WM_GETDESIREDSIZE, (WPARAM)&size);

        // 如果有指定width、height的其中一个，那么忽略在上一步中得到的值
        if (this->m_nConfigWidth != AUTO)
            size.cx = this->m_nConfigWidth;
        if (this->m_nConfigHeight!= AUTO)
            size.cy = this->m_nConfigHeight;
    }
    else
    {
        size.cx = this->m_nConfigWidth;
        size.cy = this->m_nConfigHeight;
    }

    // 计算 margin 的大小
    size.cx += m_pObj->GetMarginW();
    size.cy += m_pObj->GetMarginH();

    return size;
}

bool  StackLayoutParam::IsSizedByContent()
{
    if (m_nConfigHeight != AUTO && m_nConfigWidth != AUTO)
        return false;

    return true;
}
}