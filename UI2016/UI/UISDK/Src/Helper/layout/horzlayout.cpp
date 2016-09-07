#include "stdafx.h"
#include "horzlayout.h"
#include "Src\Base\Object\object.h"
#include "Src\Base\Attribute\attribute.h"
#include "Src\Base\Attribute\enum_attribute.h"
#include "Src\Base\Attribute\flags_attribute.h"
#include "Src\Util\DPI\dpihelper.h"

namespace UI
{

HorzLayout::HorzLayout()
{
    m_nSpace = 0;
//	this->m_eDirection = LAYOUT_Horz_LEFTTORIGHT;   // 默认堆栈是从左到右
}
HorzLayout::~HorzLayout()
{
}
void  HorzLayout::Serialize(SERIALIZEDATA* pData)
{
    AttributeSerializer s(pData, TEXT("HorzLayout"));

//     s.AddLong(XML_LAYOUT_HORZ_GAP, this,
//         memfun_cast<pfnLongSetter>(&HorzLayout::LoadGap),
//         memfun_cast<pfnLongGetter>(&HorzLayout::SaveGap));
// 
//     s.AddEnum(XML_LAYOUT_Horz_DIRECTION, *(long*)&m_eDirection)
//         ->AddOption(LAYOUT_Horz_LEFTTORIGHT, XML_LAYOUT_Horz_LEFTTORIGHT)
//         ->AddOption(LAYOUT_Horz_RIGHTTOLEFT, XML_LAYOUT_Horz_RIGHTTOLEFT)
//         ->AddOption(LAYOUT_Horz_LEFTANDRIGHT, XML_LAYOUT_Horz_LEFTANDRIGHT)
//         ->AddOption(LAYOUT_Horz_TOPTOBOTTOM, XML_LAYOUT_Horz_TOPTOBOTTOM)
//         ->AddOption(LAYOUT_Horz_BOTTOMTOTOP, XML_LAYOUT_Horz_BOTTOMTOTOP)
//         ->AddOption(LAYOUT_Horz_TOPANDBOTTOM, XML_LAYOUT_Horz_TOPANDBOTTOM);
}

void  HorzLayout::LoadGap(long n)
{
    m_nSpace = ScaleByDpi_if_gt0(n);
}
long  HorzLayout::SaveGap()
{
    return RestoreByDpi_if_gt0(m_nSpace);
}

void  HorzLayout::SetSpace(int n)
{
    m_nSpace = n;
}

SIZE  HorzLayout::Measure()
{
	SIZE s = { 0 };

	// 父控件内间距
	RECT rcPadding = { 0 };
	m_pPanel->GetPaddingRegion(&rcPadding);
	s.cx += rcPadding.left + rcPadding.right;
	s.cy += rcPadding.top + rcPadding.bottom;

	int nMaxHeight = 0;

	Object* pChild = NULL;
	while (pChild = m_pPanel->EnumChildObject(pChild))
	{
		// 放在IsSelfCollapsed之前。editor中也需要加载隐藏对象的布局属性
		HorzLayoutParam* pParam = s_GetObjectLayoutParam(pChild);
		if (!pParam)
			continue;

		if (pChild->IsSelfCollapsed())
			continue;

		switch (pParam->m_eWidthValueType)
		{
		case LAYOUT_VALUE_SET:
		{
			s.cx += pParam->m_nConfigWidth;
		}
		break;
		case LAYOUT_VALUE_AUTO:
		{
			s.cx += pChild->GetDesiredSize().cx;
		}
		break;
		}

		RECT rMargin = { 0 };
		pChild->GetMarginRegion(&rMargin);

		s.cx += rMargin.left + rMargin.right;

		int nHeight = rMargin.top + rMargin.bottom;
		switch (pParam->m_eHeightValueType)
		{
		case LAYOUT_VALUE_SET:
		{
			nHeight += pParam->m_nConfigHeight;
		}
		break;
		case LAYOUT_VALUE_AUTO:
		{
			nHeight += pChild->GetDesiredSize().cy;
		}
		break;
		}
		nMaxHeight = max(nMaxHeight, nHeight);
	}
	s.cy = nMaxHeight;

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

        HorzLayoutParam* pParam = s_GetObjectLayoutParam(pChild);
        if (!pParam)
        {
            continue;
        }

		SIZE  s = pChild->GetDesiredSize();

		size.cx += s.cx + m_nSpace;
		if (size.cy < s.cy)
			size.cy = s.cy;
	}

	return size;
#endif
}

// 布局顺序：
// 1. 计算出所有已知和期望的大小
// 2. 计算出剩余宽度
// 3. 计算出布局顺序

struct ObjLayoutInfo
{
	Object*  pObj;
	int width;
	int height;
};

void  HorzLayout::DoArrage(IObject* pIObjToArrage)
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

	// 计算每个子控件需要的宽度及其布局顺序
	vector<ObjLayoutInfo>  vecInfo(nChildCount);
	memset(&vecInfo[0], 0, sizeof(ObjLayoutInfo)*nChildCount);

	int nLeftCursor = 0;  // 用于指定这个控件的布局顺序，居右对齐的得反着排序
	int nRightCursor = nChildCount-1;

	int nNeedWidth = 0;
	int nAvgCount = 0;
	
	Object* pChild = NULL;
	while (pChild = m_pPanel->EnumChildObject(pChild))
	{
		// 放在IsSelfCollapsed之前。editor中也需要加载隐藏对象的布局属性
		HorzLayoutParam* pParam = s_GetObjectLayoutParam(pChild);
		if (!pParam)
			continue;

		if (pChild->IsSelfCollapsed())
			continue;


		//  计算布局顺序
		int nIndex = nLeftCursor;
		if (pParam->m_nConfigLayoutFlags & LAYOUT_ITEM_ALIGN_RIGHT)
		{
			nIndex = nRightCursor;
			nRightCursor--;
		}
		else
		{
			nIndex = nLeftCursor;
			nLeftCursor++;
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

		if (pParam->m_eWidthValueType == LAYOUT_VALUE_SET)
		{
			nObjWidth = pParam->m_nConfigWidth;
		}
		else if (pParam->m_eWidthValueType == LAYOUT_VALUE_AVG)
		{
			// 本次循环结束后再计算
			nAvgCount++;
			nObjWidth = 0;
		}

		if (pParam->m_eHeightValueType == LAYOUT_VALUE_SET)
		{
			nObjHeight = pParam->m_nConfigHeight;
		}

		vecInfo[nIndex].pObj = pChild;
		vecInfo[nIndex].width = nObjWidth;
		vecInfo[nIndex].height = nObjHeight;

		nNeedWidth += nObjWidth + pChild->GetMarginW();
	}

	// 计算平均宽度
	int nAvgWidth = 0;
	int nAvgDiff = 0;  // 误差补足
	if (nAvgCount > 0)
	{
		int nTotal = (rcParent.Width() - nNeedWidth);
		nAvgWidth = (nTotal / nAvgCount);
		nAvgDiff = nTotal - (nAvgWidth * nAvgCount);
	}

	// 开始布局
	int nLeftConsume = 0;
	int nRightConsume = 0;
	for (int i = 0; i < nChildCount; i++)
	{
		ObjLayoutInfo& info = vecInfo[i];
		pChild = info.pObj;
		if (!pChild)
			continue;

		// 放在IsSelfCollapsed之前。editor中也需要加载隐藏对象的布局属性
		HorzLayoutParam* pParam = s_GetObjectLayoutParam(pChild);
		if (!pParam)
			continue;

		if (pChild->IsSelfCollapsed())
			continue;

		RECT rMargin = { 0 };
		pChild->GetMarginRegion(&rMargin);

		CRect rcObj;

		// 如果是平均宽度，为其宽度赋值
		if (pParam->m_eWidthValueType == LAYOUT_VALUE_AVG)
		{
			info.width = nAvgWidth + nAvgDiff;
			nAvgDiff = 0;
		}

		// 计算x坐标
		
		if (pParam->m_nConfigLayoutFlags & LAYOUT_ITEM_ALIGN_RIGHT)
		{
			nRightConsume += rMargin.right;

			rcObj.right = rcParent.right - nRightConsume;
			rcObj.left = rcObj.right - info.width;

			nRightConsume += info.width + rMargin.left;
		}
		else // if (pParam->m_nConfigLayoutFlags & LAYOUT_ITEM_ALIGN_LEFT)
		{
			nLeftConsume += rMargin.left;

			rcObj.left = rcParent.left + nLeftConsume;
			rcObj.right = rcObj.left + info.width;

			nLeftConsume += info.width + rMargin.right;
		}

		// 计算y坐标
		if ((pParam->m_nConfigLayoutFlags & LAYOUT_ITEM_ALIGN_TOP) &&
			(pParam->m_nConfigLayoutFlags & LAYOUT_ITEM_ALIGN_BOTTOM))
		{
			rcObj.top = rcParent.top + rMargin.top;
			rcObj.bottom = rcParent.bottom - rMargin.bottom;
		}
		else if (pParam->m_nConfigLayoutFlags & LAYOUT_ITEM_ALIGN_BOTTOM)
		{
			rcObj.bottom = rcParent.bottom - rMargin.bottom;
			rcObj.top = rcObj.bottom - info.height;
		}
		else if (pParam->m_nConfigLayoutFlags & LAYOUT_ITEM_ALIGN_VCENTER)
		{
			rcObj.top = rcParent.top + (rcParent.Height() - info.height) / 2;
			rcObj.top = rcObj.top + rMargin.top - rMargin.bottom;
			rcObj.bottom = rcObj.top + info.height;
		}
		else // LAYOUT_ITEM_ALIGN_TOP:
		{
			rcObj.top = rcParent.top + rMargin.top;
			rcObj.bottom = rcObj.top + info.height;
		}

		pChild->SetObjectPos(
			rcObj,
			SWP_NOREDRAW | SWP_NOUPDATELAYOUTPOS | SWP_FORCESENDSIZEMSG);
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
		HorzLayoutParam* pParam = s_GetObjectLayoutParam(pChild);
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
					sNeedNext.cx += sThis.cx + m_nSpace;
                }

                CRect rcClient;
                m_pPanel->GetClientRectInObject(&rcClient);
                int  nPanelWidth  = rcClient.Width();
                int  nPanelHeight = rcClient.Height();

				s.cx = nPanelWidth - sNeedNext.cx - nConsume2 - nConsume1;
            }
		}
		else
		{
			s.cx = pChild->GetWidth() + pChild->GetMarginW();
			s.cy = pChild->GetHeight() + pChild->GetMarginH();
		}

		ArrangeObject_Left(pChild, nConsume1, nConsume2, s);
	}
#endif
}

void  HorzLayout::OnChildObjectVisibleChanged(IObject* pObj)
{
    UIASSERT (pObj);
	UIASSERT(pObj->GetParentObject());
	UIASSERT(pObj->GetParentObject()->GetImpl() == m_pPanel);

    SetDirty(true);
    m_pPanel->Invalidate();
}

HorzLayoutParam::HorzLayoutParam()
{
    m_nConfigWidth = AUTO;
    m_nConfigHeight = AUTO;
    m_nConfigLayoutFlags = 0;

	m_eWidthValueType = LAYOUT_VALUE_SET;
	m_eHeightValueType = LAYOUT_VALUE_SET;
}
HorzLayoutParam::~HorzLayoutParam()
{

}

void  HorzLayoutParam::UpdateByRect()
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
void  HorzLayoutParam::Serialize(SERIALIZEDATA* pData)
{
    AttributeSerializer s(pData, TEXT("HorzLayoutParam"));
    
    s.AddLong(
        XML_WIDTH,  this,
        memfun_cast<pfnLongSetter>(&HorzLayoutParam::LoadConfigWidth), 
        memfun_cast<pfnLongGetter>(&HorzLayoutParam::SaveConfigWidth)
        )->AddAlias(AUTO, XML_AUTO)->SetDefault(AUTO)
		->SetCompatibleKey(XML_LAYOUT_PREFIX XML_WIDTH);;

    s.AddLong(
        XML_HEIGHT,  this,
        memfun_cast<pfnLongSetter>(&HorzLayoutParam::LoadConfigHeight), 
        memfun_cast<pfnLongGetter>(&HorzLayoutParam::SaveConfigHeight)
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

long  HorzLayoutParam::GetConfigWidth()
{
    return m_nConfigWidth;
}
void  HorzLayoutParam::SetConfigWidth(long n)
{
    m_nConfigWidth = n;
}
void  HorzLayoutParam::LoadConfigWidth(long n)
{
    m_nConfigWidth = ScaleByDpi(n);
}
long  HorzLayoutParam::SaveConfigWidth()
{
    return RestoreByDpi(m_nConfigWidth);
}

long  HorzLayoutParam::GetConfigHeight()
{
    return m_nConfigHeight;
}
void  HorzLayoutParam::SetConfigHeight(long n)
{
    m_nConfigHeight = n;
}
void  HorzLayoutParam::LoadConfigHeight(long n)
{
    m_nConfigHeight = ScaleByDpi(n);
}
long  HorzLayoutParam::SaveConfigHeight()
{
    return RestoreByDpi(m_nConfigHeight);
}

void  HorzLayoutParam::SetConfigLayoutFlags(long n)
{
    m_nConfigLayoutFlags = n;
}
long  HorzLayoutParam::GetConfigLayoutFlags()
{
    return m_nConfigLayoutFlags;
}

SIZE  HorzLayoutParam::CalcDesiredSize()
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

bool  HorzLayoutParam::IsSizedByContent()
{
    if (m_nConfigHeight != AUTO && m_nConfigWidth != AUTO)
        return false;

    return true;
}
}