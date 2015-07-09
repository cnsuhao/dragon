#include "stdafx.h"
#include "canvaslayout.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"
#include "UISDK\Kernel\Src\Base\Attribute\attribute.h"
#include "UISDK\Kernel\Src\Base\Attribute\long_attribute.h"
#include "UISDK\Kernel\Src\Base\Attribute\flags_attribute.h"
#include "UISDK\Kernel\Src\Base\Application\uiapplication.h"

namespace UI
{

SIZE  CanvasLayout::Measure()
{
    SIZE size = {0,0};

    // ͨ���ӿؼ�����ȡ�Լ�����Ҫ�Ĵ�С
    Object*  pChild = NULL;
    while (pChild = this->m_pPanel->EnumChildObject(pChild))
    {
        CanvasLayoutParam* pParam = s_GetObjectLayoutParam(pChild);
        if (!pParam)
        {
            continue;
        }
        SIZE  s = pChild->GetDesiredSize();
        

        if (NDEF != pParam->GetConfigLeft())
        {
            s.cx += pParam->GetConfigLeft();
        }
        else if (NDEF != pParam->GetConfigRight())
        {
            s.cx += pParam->GetConfigRight();
        }
        if (NDEF != pParam->GetConfigTop())
        {
            s.cy += pParam->GetConfigTop();
        }
        else if (NDEF != pParam->GetConfigBottom())
        {
            s.cy += pParam->GetConfigBottom();
        }

        if (size.cx < s.cx)
        {
            size.cx = s.cx;
        }
        if (size.cy < s.cy)
        {
            size.cy = s.cy;
        }
    }
    return size;
}
void  CanvasLayout::Arrange(IObject* pIObjToArrage, bool bUpdate)
{
    Object* pObjToArrage = NULL;
    if (pIObjToArrage)
        pObjToArrage = pIObjToArrage->GetImpl();

    // ���øú���ʱ���Լ��Ĵ�С�Ѿ����������
    CRect rcClient;
    m_pPanel->GetClientRectInObject(&rcClient);
    int  nWidth  = rcClient.Width();  //m_pPanel->GetWidth();
    int  nHeight = rcClient.Height(); //m_pPanel->GetHeight();

    Object* pChild = NULL;
    while (pChild = m_pPanel->EnumChildObject(pChild))
    {
        if (pObjToArrage && pObjToArrage != pChild)
            continue;

        if (pChild->IsSelfCollapsed())
		{
			// ���صĿؼ����ڱ༭����Ҳ��Ҫ���ز������ԡ�
			if (m_pPanel->GetUIApplication()->IsDesignMode())
				s_GetObjectLayoutParam(pChild);

			continue;
		}

        this->ArrangeObject(pChild, nWidth, nHeight);

        if (pObjToArrage && pObjToArrage == pChild)
            break;
    }
    if (bUpdate)
    {
        m_pPanel->UpdateObject(true);
    }
}

void  CanvasLayout::ArrangeObject(Object*  pChild, const int& nWidth, const int& nHeight)
{
    int  x = 0, y = 0;                                  // pChild������parent�е�����

    CanvasLayoutParam* pParam = s_GetObjectLayoutParam(pChild);
    if (!pParam)
    {
        return;
    }

    int left = pParam->GetConfigLeft();
    int top  = pParam->GetConfigTop();
    int right  = pParam->GetConfigRight();
    int bottom = pParam->GetConfigBottom();
    // int nConfigWidth = pParam->GetConfigWidth();
    // int nConfigHeight = pParam->GetConfigHeight();
    int nConfigFlag = pParam->GetConfigLayoutFlags();

    //////////////////////////////////////////////////////////////////////////
    // ����� pChild �� rectP�Ŀ�͸�
    SIZE s = pParam->CalcDesiredSize();

    // ���ͬʱָ����left/right,�����width����
    if (left != NDEF && right != NDEF)
    {
        s.cx = nWidth - left - right;
    }
    // ���ͬʱָ����top/bottom�������height����
    if (top != NDEF && bottom != NDEF)
    {
        s.cy = nHeight - top - bottom;
    }
	if (s.cx < 0)
		s.cx = 0;
	if (s.cy < 0)
		s.cy = 0;

    CRect rcChildObj ;
    int nChildW = s.cx - pChild->GetMarginW();
    int nChildH = s.cy - pChild->GetMarginH();
    rcChildObj.SetRect(0, 0, nChildW, nChildH );

    // ��������꣬��left/right,top/bottom����һ��δָ���ģ���ôȡ0������DesktopLayout����ȡ���У�
    if (left != NDEF)
    {
        x = left;
        x += pChild->GetMarginL();
    }
    else
    {
        if (right != NDEF)
        {
            x = nWidth - right - nChildW;
            x -= pChild->GetMarginR();
        }
        else
        {
            if (nConfigFlag & LAYOUT_ITEM_ALIGN_CENTER)
            {
                x = ( nWidth - s.cx ) / 2;  // ����
            }
            else
            {
                x = 0;
                x += pChild->GetMarginL();
            }
        }
    }
    if (top != NDEF)
    {
        y = top;
        y += pChild->GetMarginT();
    }
    else
    {
        if (bottom != NDEF)
        {
            y = nHeight - bottom - nChildH;
            y -= pChild->GetMarginB();
        }
        else
        {
            if (nConfigFlag & LAYOUT_ITEM_ALIGN_CENTER)
            {
                y = ( nHeight - s.cy ) / 2;  // ����
            }
            else
            {
                y = 0;
                y += pChild->GetMarginT();
            }
        }
    }

    rcChildObj.OffsetRect(x, y);
    pChild->SetObjectPos(&rcChildObj, SWP_NOREDRAW|SWP_NOUPDATELAYOUTPOS|SWP_FORCESENDSIZEMSG);
}
//////////////////////////////////////////////////////////////////////////

CanvasLayoutParam::CanvasLayoutParam()
{
    m_nConfigWidth = m_nConfigHeight = AUTO;
    m_nConfigLeft = m_nConfigRight = m_nConfigTop = m_nConfigBottom = NDEF;
    m_nConfigLayoutFlags = 0;

}
CanvasLayoutParam::~CanvasLayoutParam()
{

}

SIZE  CanvasLayoutParam::CalcDesiredSize()
{
    SIZE size = {0,0};

	if (IsSizedByContent())
	{
        // ��ȡ�Ӷ�������Ҫ�Ŀռ�
        UISendMessage(m_pObj, UI_WM_GETDESIREDSIZE, (WPARAM)&size);

        // �����ָ��width��height������һ������ô��������һ���еõ���ֵ
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

    // ���� margin �Ĵ�С
    size.cx += m_pObj->GetMarginW();
    size.cy += m_pObj->GetMarginH();

    return size;
}

void  CanvasLayoutParam::UpdateByRect()
{
    Object* pParent = m_pObj->GetParentObject();
    if (!pParent)
        return;

    CRect  rcParent;
    CRect  rcPanel;
    m_pObj->GetParentRect(&rcParent);
    pParent->GetClientRectInObject(&rcPanel);

    bool  bSetX = false;
    if (m_nConfigLeft >= 0)
    {
        bSetX = true;
        m_nConfigLeft = rcParent.left;
    }
    if (m_nConfigRight >= 0)
    {
        bSetX = true;
        m_nConfigRight = rcPanel.Width() - rcParent.right;
    }
    if (!bSetX)
    {
        m_nConfigLeft = rcParent.left;   // ��left/right��û��ָ��ʱ��ǿ������һ��
    }


    bool  bSetY = false;
    if (m_nConfigTop >= 0)
    {
        bSetY = true;
        m_nConfigTop = rcParent.top;
    }
    if (m_nConfigBottom >= 0)
    {
        bSetY = true;
        m_nConfigBottom = rcPanel.Height() - rcParent.bottom;
    }
    if (!bSetY)
    {
        m_nConfigTop = rcParent.top;   // ��top/bottom��û��ָ��ʱ��ǿ������һ��
    }

    if (m_nConfigWidth >= 0)
    {
        m_nConfigWidth = rcParent.Width();
    }
    if (m_nConfigHeight >= 0)
    {
        m_nConfigHeight = rcParent.Height();
    }
}
void  CanvasLayoutParam::Serialize(SERIALIZEDATA* pData)
{
    AttributeSerializer s(pData, TEXT("CanvasLayoutParam"));

    s.AddLong(XML_WIDTH,  m_nConfigWidth) ->AddAlias(AUTO, XML_AUTO)->SetDefault(AUTO);
    s.AddLong(XML_HEIGHT, m_nConfigHeight)->AddAlias(AUTO, XML_AUTO)->SetDefault(AUTO);

    s.AddLong(XML_LAYOUT_ITEM_LEFT,   m_nConfigLeft)  ->SetDefault(NDEF);
    s.AddLong(XML_LAYOUT_ITEM_TOP,    m_nConfigTop)   ->SetDefault(NDEF);
    s.AddLong(XML_LAYOUT_ITEM_RIGHT,  m_nConfigRight) ->SetDefault(NDEF);
    s.AddLong(XML_LAYOUT_ITEM_BOTTOM, m_nConfigBottom)->SetDefault(NDEF);

    s.AddFlags(XML_LAYOUT_ITEM_ALIGN, m_nConfigLayoutFlags)
        ->AddFlag(LAYOUT_ITEM_ALIGN_LEFT,        XML_LAYOUT_ITEM_ALIGN_LEFT)
        ->AddFlag(LAYOUT_ITEM_ALIGN_RIGHT,       XML_LAYOUT_ITEM_ALIGN_RIGHT)
        ->AddFlag(LAYOUT_ITEM_ALIGN_TOP,         XML_LAYOUT_ITEM_ALIGN_TOP)
        ->AddFlag(LAYOUT_ITEM_ALIGN_BOTTOM,      XML_LAYOUT_ITEM_ALIGN_BOTTOM)
        ->AddFlag(LAYOUT_ITEM_ALIGN_CENTER,      XML_LAYOUT_ITEM_ALIGN_CENTER)
        ->AddFlag(LAYOUT_ITEM_ALIGN_VCENTER,     XML_LAYOUT_ITEM_ALIGN_VCENTER)
        ->AddFlag(LAYOUT_FLAG_FORCE_DESIREDSIZE, XML_LAYOUT_FLAG_FORCE_DESIREDSIZE);
}

int  CanvasLayoutParam::ParseAlignAttr(LPCTSTR szAttr)
{
    Util::ISplitStringEnum*  pEnum = NULL;
    int nCount = Util::SplitString(szAttr, XML_FLAG_SEPARATOR, &pEnum);

    int nRet = 0;
    for (int i = 0; i < nCount; i++)
    {
        LPCTSTR  szFlag = pEnum->GetText(i);
        if (0 == _tcscmp(szFlag, XML_LAYOUT_ITEM_ALIGN_LEFT))
            nRet |= LAYOUT_ITEM_ALIGN_LEFT;
        else if (0 == _tcscmp(szFlag, XML_LAYOUT_ITEM_ALIGN_RIGHT))
            nRet |= LAYOUT_ITEM_ALIGN_RIGHT;
        else if (0 == _tcscmp(szFlag, XML_LAYOUT_ITEM_ALIGN_TOP))
            nRet |= LAYOUT_ITEM_ALIGN_TOP;
        else if (0 == _tcscmp(szFlag, XML_LAYOUT_ITEM_ALIGN_BOTTOM))
            nRet |= LAYOUT_ITEM_ALIGN_BOTTOM;
        else if (0 == _tcscmp(szFlag, XML_LAYOUT_ITEM_ALIGN_CENTER))
            nRet |= LAYOUT_ITEM_ALIGN_CENTER;
        else if (0 == _tcscmp(szFlag, XML_LAYOUT_ITEM_ALIGN_VCENTER))
            nRet |= LAYOUT_ITEM_ALIGN_VCENTER;
		else if (0 == _tcscmp(szFlag, XML_LAYOUT_FLAG_FORCE_DESIREDSIZE))
			nRet |= LAYOUT_FLAG_FORCE_DESIREDSIZE;
    }
    SAFE_RELEASE(pEnum);

    return nRet;
}

bool  CanvasLayoutParam::IsSizedByContent()
{
    bool  bWidthNotConfiged = (m_nConfigLeft == NDEF || m_nConfigRight == NDEF) && m_nConfigWidth ==AUTO;
    bool  bHeightNotConfiged = (m_nConfigTop == NDEF || m_nConfigBottom == NDEF) && m_nConfigHeight == AUTO;

    if (bWidthNotConfiged || bHeightNotConfiged || 
        (m_nConfigLayoutFlags & LAYOUT_FLAG_FORCE_DESIREDSIZE))
        return true;

    return false;
}

}