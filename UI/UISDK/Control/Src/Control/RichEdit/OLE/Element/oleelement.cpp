#include "stdafx.h"
#include "oleelement.h"
#include "..\Composite\compositereole.h"

using namespace UI;

OleElement::OleElement()
{
    m_style.bEnable = true;
    m_style.bVisible = true;
	m_pREOle = NULL;
    m_lId = 0;
    ::memset(&m_layoutParam, NDEF, sizeof(m_layoutParam));
    ::SetRectEmpty(&m_rcInOle);
}
OleElement::~OleElement()
{

}

void  OleElement::Release()
{
    delete this;
}

void  OleElement::SetCompositeREOle(CompositeREOle* p)
{
	m_pREOle = p;
}

long  OleElement::GetId()
{
    return m_lId;
}
void  OleElement::SetId(long lId)
{
    m_lId = lId;
}

void  OleElement::SetVisible(bool b)
{
    m_style.bVisible = b;
}
void  OleElement::SetEnable(bool b)
{
    m_style.bEnable = b;
}

bool  OleElement::IsEnable()
{
    return m_style.bEnable;
}
bool  OleElement::IsVisible()
{
    return m_style.bVisible;
}       

LRESULT  OleElement::ProcessMessage(UINT msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;
    return 0;
}

void  OleElement::GetRectInOle(RECT* prc)
{
    CopyRect(prc, m_rcInOle);
}   

void  OleElement::Invalidate()
{
    if (!m_pREOle)
        return;

    CRect  rcElement(m_rcInOle);

    CRect  rcOle;
    m_pREOle->GetDrawRect(&rcOle);
    rcElement.OffsetRect(rcOle.left, rcOle.top);

    m_pREOle->InvalidateEx(&rcElement);
}

void  OleElement::UpdateLayout(int cxOle, int cyOle)
{

    SIZE s = {m_layoutParam.nWidth, m_layoutParam.nHeight};
    int x = 0, y = 0;

    // ���ͬʱָ����left/right,�����width����
    if (m_layoutParam.nLeft != NDEF && m_layoutParam.nRight != NDEF)
    {
        s.cx = cxOle - m_layoutParam.nLeft - m_layoutParam.nRight;
    }
    // ���ͬʱָ����m_layoutParam.nTop/m_layoutParam.nBottom�������height����
    if (m_layoutParam.nTop != NDEF && m_layoutParam.nBottom != NDEF)
    {
        s.cy = cyOle - m_layoutParam.nTop - m_layoutParam.nBottom;
    }

    // ���������
    if (m_layoutParam.nLeft != NDEF)
    {
        x = m_layoutParam.nLeft;
    }
    else
    {
        if (m_layoutParam.nRight != NDEF)
        {
            x = cxOle - m_layoutParam.nRight - s.cx;  // m_layoutParam.nRight��ָ�����Ҳ������Ļ�Ҳ�ľ���
        }
        else
        {
            // ����
            x = ( cxOle - s.cx) / 2;
        }
    }
    if (m_layoutParam.nTop != NDEF)
    {
        y = m_layoutParam.nTop;
    }
    else
    {
        if (m_layoutParam.nBottom != NDEF)
        {
            y = cyOle - m_layoutParam.nBottom - s.cy; // ͬm_layoutParam.nRight
        }
        else
        {
            // ����
            y = (cyOle - s.cy) / 2;
        }
    }

    m_rcInOle.SetRect(x, y, x+s.cx, y+s.cy);
}