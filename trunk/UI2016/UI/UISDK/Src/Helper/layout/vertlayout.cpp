#include "stdafx.h"
#include "vertlayout.h"
#include "Src\Base\Object\object.h"
#include "Src\Base\Attribute\attribute.h"
#include "Src\Base\Attribute\enum_attribute.h"
#include "Src\Base\Attribute\flags_attribute.h"
#include "Src\Util\DPI\dpihelper.h"

namespace UI
{

VertLayout::VertLayout()
{
    m_nSpace = 0;
//	this->m_eDirection = LAYOUT_Vert_LEFTTORIGHT;   // 默认堆栈是从左到右
}
VertLayout::~VertLayout()
{
}
void  VertLayout::Serialize(SERIALIZEDATA* pData)
{
    AttributeSerializer s(pData, TEXT("VertLayout"));

//     s.AddLong(XML_LAYOUT_VERT_GAP, this,
//         memfun_cast<pfnLongSetter>(&VertLayout::LoadGap),
//         memfun_cast<pfnLongGetter>(&VertLayout::SaveGap));
// 
//     s.AddEnum(XML_LAYOUT_Vert_DIRECTION, *(long*)&m_eDirection)
//         ->AddOption(LAYOUT_Vert_LEFTTORIGHT, XML_LAYOUT_Vert_LEFTTORIGHT)
//         ->AddOption(LAYOUT_Vert_RIGHTTOLEFT, XML_LAYOUT_Vert_RIGHTTOLEFT)
//         ->AddOption(LAYOUT_Vert_LEFTANDRIGHT, XML_LAYOUT_Vert_LEFTANDRIGHT)
//         ->AddOption(LAYOUT_Vert_TOPTOBOTTOM, XML_LAYOUT_Vert_TOPTOBOTTOM)
//         ->AddOption(LAYOUT_Vert_BOTTOMTOTOP, XML_LAYOUT_Vert_BOTTOMTOTOP)
//         ->AddOption(LAYOUT_Vert_TOPANDBOTTOM, XML_LAYOUT_Vert_TOPANDBOTTOM);
}

void  VertLayout::LoadGap(long n)
{
    m_nSpace = ScaleByDpi_if_gt0(n);
}
long  VertLayout::SaveGap()
{
    return RestoreByDpi_if_gt0(m_nSpace);
}

void  VertLayout::SetSpace(int n)
{
    m_nSpace = n;
}

SIZE  VertLayout::Measure()
{
	SIZE s = { 0 };

	// 父控件内间距
	RECT rcPadding = { 0 };
	m_pPanel->GetPaddingRegion(&rcPadding);
	s.cx += rcPadding.left + rcPadding.right;
	s.cy += rcPadding.top + rcPadding.bottom;

	int nMaxWidth = 0;

	Object* pChild = NULL;
	while (pChild = m_pPanel->EnumChildObject(pChild))
	{
		// 放在IsSelfCollapsed之前。editor中也需要加载隐藏对象的布局属性
		VertLayoutParam* pParam = s_GetObjectLayoutParam(pChild);
		if (!pParam)
			continue;

		if (pChild->IsSelfCollapsed())
			continue;

		switch (pParam->m_eHeightValueType)
		{
		case LAYOUT_VALUE_SET:
		{
			s.cy += pParam->m_nConfigHeight;
		}
		break;
		case LAYOUT_VALUE_AUTO:
		{
			s.cy += pChild->GetDesiredSize().cy;
		}
		break;
		}

		RECT rMargin = { 0 };
		pChild->GetMarginRegion(&rMargin);

		s.cy += rMargin.top + rMargin.bottom;

		int nWidth = rMargin.left + rMargin.right;
		switch (pParam->m_eWidthValueType)
		{
		case LAYOUT_VALUE_SET:
		{
			nWidth += pParam->m_nConfigWidth;
		}
		break;
		case LAYOUT_VALUE_AUTO:
		{
			nWidth += pChild->GetDesiredSize().cx;
		}
		break;
		}
		nMaxWidth = max(nMaxWidth, nWidth);
	}
	s.cx = nMaxWidth;

	return s;

#if 0
	SIZE  size = {0,0};

	// 通过子控件来获取自己所需要的大小
	Object*  pChild = NULL;
	while (pChild = this->m_pPanel->EnumChildObject(pChild))
	{
        if (pChild->IsSelfCollapsed())
        {
            continue;
        }

        VertLayoutParam* pParam = s_GetObjectLayoutParam(pChild);
        if (!pParam)
        {
            continue;
        }

		SIZE  s = pChild->GetDesiredSize();

		size.cy += s.cy + m_nSpace;
		if (size.cx < s.cx)
			size.cx = s.cx;
	}

	return size;
#endif
}

struct ObjLayoutInfo
{
	Object*  pObj;
	int width;
	int height;
};

void  VertLayout::DoArrage(IObject* /*pIObjToArrage*/)
{
	// 计算每个子控件需要的宽度及其布局顺序
	int nChildCount = m_pPanel->GetChildCount();
	if (nChildCount <= 0)
		return;

	// 父控件内间距
	RECT rcPadding = { 0 };
	m_pPanel->GetPaddingRegion(&rcPadding);

	CRect rcParent;
	m_pPanel->GetObjectClientRect(&rcParent);

	vector<ObjLayoutInfo>  vecInfo(nChildCount);
	memset(&vecInfo[0], 0, sizeof(ObjLayoutInfo)*nChildCount);

	int nTopCursor = 0;  // 用于指定这个控件的布局顺序，居右对齐的得反着排序
	int nBottomCursor = nChildCount-1;

	int nNeedHeight = 0;
	int nAvgCount = 0;

	Object* pChild = NULL;
	while (pChild = m_pPanel->EnumChildObject(pChild))
	{
		// 放在IsSelfCollapsed之前。editor中也需要加载隐藏对象的布局属性
		VertLayoutParam* pParam = s_GetObjectLayoutParam(pChild);
		if (!pParam)
			continue;

		if (pChild->IsSelfCollapsed())
			continue;

		//  计算布局顺序
		int nIndex = nTopCursor;
		if (pParam->m_nConfigLayoutFlags & LAYOUT_ITEM_ALIGN_BOTTOM)
		{
			nIndex = nBottomCursor;
			nBottomCursor--;
		}
		else
		{
			nIndex = nTopCursor;
			nTopCursor++;
		}

		// 计算所需要的宽高
		int nObjWidth = 0;
		int nObjHeight = 0;

		if (pParam->IsSizedByContent())
		{
			SIZE s = pChild->GetDesiredSize();
			nObjWidth = s.cx;
			nObjHeight = s.cy;
		}

		if (pParam->m_eHeightValueType == LAYOUT_VALUE_SET)
		{
			nObjHeight = pParam->m_nConfigHeight;
		}
		else if (pParam->m_eHeightValueType == LAYOUT_VALUE_AVG)
		{
			// 本次循环结束后再计算
			nAvgCount++;
			nObjHeight = 0;
		}

		if (pParam->m_eWidthValueType == LAYOUT_VALUE_SET)
		{
			nObjWidth = pParam->m_nConfigWidth;
		}

		ObjLayoutInfo& info = vecInfo[nIndex];
		info.pObj = pChild;
		info.width = nObjWidth;
		info.height = nObjHeight;

		nNeedHeight += nObjHeight + pChild->GetMarginH();
	}

	// 计算平均宽度
	int nAvgHeight = 0;
	int nAvgDiff = 0;  // 误差补足
	if (nAvgCount > 0)
	{
		int nTotal = (rcParent.Height()- nNeedHeight);
		nAvgHeight = ((rcParent.Height()-nNeedHeight) / nAvgCount);
		nAvgDiff = nTotal - (nAvgHeight * nAvgCount);
	}

	// 开始布局
	int nTopConsume = 0;
	int nBottomConsume = 0;

	for (int i = 0; i < nChildCount; i++)
	{
		ObjLayoutInfo& info = vecInfo[i];
		pChild = info.pObj;
		if (!pChild)
			continue;

		// 放在IsSelfCollapsed之前。editor中也需要加载隐藏对象的布局属性
		VertLayoutParam* pParam = s_GetObjectLayoutParam(pChild);
		if (!pParam)
			continue;

		if (pChild->IsSelfCollapsed())
			continue;

		RECT rMargin = { 0 };
		pChild->GetMarginRegion(&rMargin);

		CRect rcObj;

		// 如果是平均宽度，为其宽度赋值
		if (pParam->m_eHeightValueType == LAYOUT_VALUE_AVG)
		{
			info.height = nAvgHeight;
			info.height += nAvgDiff;
			nAvgDiff = 0;
		}

		// 计算y坐标
		if (pParam->m_nConfigLayoutFlags & LAYOUT_ITEM_ALIGN_BOTTOM)
		{
			nBottomConsume += rMargin.bottom;

			rcObj.bottom = rcParent.bottom - nBottomConsume;
			rcObj.top = rcObj.bottom - info.height;

			nBottomConsume += info.height + rMargin.top;
		}
		else // if (pParam->m_nConfigLayoutFlags & LAYOUT_ITEM_ALIGN_TOP)
		{
			nTopConsume += rMargin.top;

			rcObj.top = rcParent.top + nTopConsume;
			rcObj.bottom = rcObj.top + info.height;

			nTopConsume += info.height + rMargin.bottom;
		}

		// 计算x坐标
		if ((pParam->m_nConfigLayoutFlags & LAYOUT_ITEM_ALIGN_LEFT) &&
			(pParam->m_nConfigLayoutFlags & LAYOUT_ITEM_ALIGN_RIGHT))
		{
			rcObj.left = rcParent.left + rMargin.left;
			rcObj.right = rcParent.right - rMargin.right;
		}
		else if (pParam->m_nConfigLayoutFlags & LAYOUT_ITEM_ALIGN_RIGHT)
		{
			rcObj.right = rcParent.right - rMargin.right;
			rcObj.left = rcObj.right - info.width;
		}
		else if (pParam->m_nConfigLayoutFlags & LAYOUT_ITEM_ALIGN_CENTER)
		{
			rcObj.left = rcParent.left + (rcParent.Width() - info.width) / 2;
			rcObj.left = rcObj.left + rMargin.left - rMargin.right;
			rcObj.right = rcObj.left + info.width;
		}
		else // if (pParam->m_nConfigLayoutFlags & LAYOUT_ITEM_ALIGN_LEFT)
		{
			rcObj.left = rcParent.left + rMargin.left;
			rcObj.right = rcObj.left + info.width;
		}

		pChild->SetObjectPos(
			rcObj,
			SWP_NOREDRAW|SWP_NOUPDATELAYOUTPOS|SWP_FORCESENDSIZEMSG);
	}

#if 0
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
		VertLayoutParam* pParam = s_GetObjectLayoutParam(pChild);
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
                    sNeedNext.cy += sThis.cy + m_nSpace;
                }

                CRect rcClient;
                m_pPanel->GetClientRectInObject(&rcClient);
                int  nPanelWidth  = rcClient.Width();
                int  nPanelHeight = rcClient.Height();

                s.cy = nPanelHeight - sNeedNext.cy - nConsume2 - nConsume1;
            }
		}
		else
		{
			s.cx = pChild->GetWidth() + pChild->GetMarginW();
			s.cy = pChild->GetHeight() + pChild->GetMarginH();
		}

		if (pParam->m_nConfigLayoutFlags & LAYOUT_ITEM_ALIGN_BOTTOM)
			ArrangeObject_Bottom(pChild, nConsume1, nConsume2, s);
		else
			ArrangeObject_Top(pChild, nConsume1, nConsume2, s);
	}
#endif
}

void  VertLayout::OnChildObjectVisibleChanged(IObject* pObj)
{
    UIASSERT (pObj);
	UIASSERT(pObj->GetParentObject());
	UIASSERT(pObj->GetParentObject()->GetImpl() == m_pPanel);

    SetDirty(true);
    m_pPanel->Invalidate();
}

//////////////////////////////////////////////////////////////////////////

VertLayoutParam::VertLayoutParam()
{
    m_nConfigWidth = AUTO;
    m_nConfigHeight = AUTO;
    m_nConfigLayoutFlags = 0;
	m_eWidthValueType = LAYOUT_VALUE_SET;
	m_eHeightValueType = LAYOUT_VALUE_SET;
}
VertLayoutParam::~VertLayoutParam()
{

}

void  VertLayoutParam::UpdateByRect()
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

void  VertLayoutParam::Serialize(SERIALIZEDATA* pData)
{
    AttributeSerializer s(pData, TEXT("VertLayoutParam"));
    
    s.AddLong(
        XML_WIDTH,  this,
        memfun_cast<pfnLongSetter>(&VertLayoutParam::LoadConfigWidth), 
        memfun_cast<pfnLongGetter>(&VertLayoutParam::SaveConfigWidth)
        )->AddAlias(AUTO, XML_AUTO)->SetDefault(AUTO)
		->SetCompatibleKey(XML_LAYOUT_PREFIX XML_WIDTH);;

    s.AddLong(
        XML_HEIGHT,  this,
        memfun_cast<pfnLongSetter>(&VertLayoutParam::LoadConfigHeight), 
        memfun_cast<pfnLongGetter>(&VertLayoutParam::SaveConfigHeight)
        )->AddAlias(AUTO, XML_AUTO)->SetDefault(AUTO)
		->SetCompatibleKey(XML_LAYOUT_PREFIX XML_HEIGHT);;

    s.AddFlags(XML_LAYOUT_ITEM_ALIGN, m_nConfigLayoutFlags)
        ->AddFlag(LAYOUT_ITEM_ALIGN_LEFT,        XML_LAYOUT_ITEM_ALIGN_LEFT)
        ->AddFlag(LAYOUT_ITEM_ALIGN_RIGHT,       XML_LAYOUT_ITEM_ALIGN_RIGHT)
        ->AddFlag(LAYOUT_ITEM_ALIGN_TOP,         XML_LAYOUT_ITEM_ALIGN_TOP)
        ->AddFlag(LAYOUT_ITEM_ALIGN_BOTTOM,      XML_LAYOUT_ITEM_ALIGN_BOTTOM)
        ->AddFlag(LAYOUT_ITEM_ALIGN_CENTER,      XML_LAYOUT_ITEM_ALIGN_CENTER)
        ->AddFlag(LAYOUT_ITEM_ALIGN_VCENTER,     XML_LAYOUT_ITEM_ALIGN_VCENTER)
        ->AddFlag(LAYOUT_ITEM_ALIGN_FILL,        XML_LAYOUT_ITEM_ALIGN_FILL);

	s.AddEnum(XML_LAYOUT_WIDTH_TYPE, *(long*)&m_eWidthValueType)
		->AddOption(LAYOUT_VALUE_SET,     XML_LAYOUT_VALUE_TYPE_SET)
		->AddOption(LAYOUT_VALUE_AUTO,    XML_LAYOUT_VALUE_TYPE_AUTO)
		->AddOption(LAYOUT_VALUE_AVG,     XML_LAYOUT_VALUE_TYPE_AVG)
		->AddOption(LAYOUT_VALUE_PERCENT, XML_LAYOUT_VALUE_TYPE_PERCENT)
		->SetCompatibleKey(XML_LAYOUT_PREFIX XML_LAYOUT_WIDTH_TYPE);

	s.AddEnum(XML_LAYOUT_HEIGHT_TYPE, *(long*)&m_eHeightValueType)
		->AddOption(LAYOUT_VALUE_SET,     XML_LAYOUT_VALUE_TYPE_SET)
		->AddOption(LAYOUT_VALUE_AUTO,    XML_LAYOUT_VALUE_TYPE_AUTO)
		->AddOption(LAYOUT_VALUE_AVG,     XML_LAYOUT_VALUE_TYPE_AVG)
		->AddOption(LAYOUT_VALUE_PERCENT, XML_LAYOUT_VALUE_TYPE_PERCENT)
		->SetCompatibleKey(XML_LAYOUT_PREFIX XML_LAYOUT_HEIGHT_TYPE);
}

long  VertLayoutParam::GetConfigWidth()
{
    return m_nConfigWidth;
}
void  VertLayoutParam::SetConfigWidth(long n)
{
    m_nConfigWidth = n;
}
void  VertLayoutParam::LoadConfigWidth(long n)
{
    m_nConfigWidth = ScaleByDpi(n);
}
long  VertLayoutParam::SaveConfigWidth()
{
    return RestoreByDpi(m_nConfigWidth);
}

long  VertLayoutParam::GetConfigHeight()
{
    return m_nConfigHeight;
}
void  VertLayoutParam::SetConfigHeight(long n)
{
    m_nConfigHeight = n;
}
void  VertLayoutParam::LoadConfigHeight(long n)
{
    m_nConfigHeight = ScaleByDpi(n);
}
long  VertLayoutParam::SaveConfigHeight()
{
    return RestoreByDpi(m_nConfigHeight);
}

void  VertLayoutParam::SetConfigLayoutFlags(long n)
{
   // m_nConfigLayoutFlags = n;
}
long  VertLayoutParam::GetConfigLayoutFlags()
{
   // return m_nConfigLayoutFlags;
	return 0;
}

SIZE  VertLayoutParam::CalcDesiredSize()
{
    SIZE size = {0,0};

    bool bWidthNotConfiged = m_nConfigWidth == AUTO ? true:false;
    bool bHeightNotConfiged = m_nConfigHeight == AUTO ? true:false;;

    if (bWidthNotConfiged || bHeightNotConfiged)
    {
        // 获取子对象所需要的空间
        UISendMessage(m_pObj, UI_MSG_GETDESIREDSIZE, (WPARAM)&size);

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

bool  VertLayoutParam::IsSizedByContent()
{
    if (m_nConfigHeight != AUTO && m_nConfigWidth != AUTO)
        return false;

    return true;
}
}