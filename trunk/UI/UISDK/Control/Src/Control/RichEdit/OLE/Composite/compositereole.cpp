#include "stdafx.h"
#include "../element/oleelement.h"
#include "compositereole.h"

using namespace UI;

CompositeREOle::CompositeREOle()
{
    m_pHoverElement = NULL;
    m_pPressElement = NULL;
}
CompositeREOle::~CompositeREOle()
{
    OLEELEMENTITER iter = m_elementList.begin();
    for (; iter != m_elementList.end(); iter++)
    {
        (*iter)->Release();
    }
    m_elementList.clear();
}

bool  CompositeREOle::AddElement(OleElement* p)
{
    if (!p)
        return false;
    if (IsOleElementExist(p))
        return false;

    m_elementList.push_back(p);
	p->SetCompositeREOle(this);

    // ��ЩԪ����Ӻ�ole��Сǰ��һ��������û�д���onsizechanged��Ϣ
    // ��������ʱ�Ͳ���һ�Ρ�
    p->UpdateLayout(m_rcDrawSave.Width(), m_rcDrawSave.Height());
    return true;
}

bool  CompositeREOle::IsOleElementExist(OleElement* p)
{
    OLEELEMENTITER iter;
    return FindOleElement(p, iter);
}

bool  CompositeREOle::FindOleElement(OleElement* p, OLEELEMENTITER& iterOut)
{
    OLEELEMENTITER iter = m_elementList.begin();
    for (; iter != m_elementList.end(); iter++)
    {
        if (*iter == p)
        {
            iterOut = iter;
            return true;
        }
    }

    iterOut = m_elementList.end();
    return false;
}

// ֻ�Ƴ���������
bool  CompositeREOle::RemoveElement(OleElement* p)
{
    OLEELEMENTITER iter;
    if (!FindOleElement(p, iter))
        return false;

	p->SetCompositeREOle(NULL);
    m_elementList.erase(iter);
    return true;
}
// �Ƴ���������
bool  CompositeREOle::DestroyElement(OleElement* p)
{
    OLEELEMENTITER iter;
    if (!FindOleElement(p, iter))
        return false;

    m_elementList.erase(iter);
    p->SetCompositeREOle(NULL);
    p->Release();
    
    return true;
}

void CompositeREOle::OnDraw(HDC hDC, RECT* prc)
{
    if (!prc)
        return;

    this->DrawBkg(hDC);

    OLEELEMENTITER iter = m_elementList.begin();
    for (; iter != m_elementList.end(); iter++)
    {
        OleElement* p = *iter;

        if (p->IsVisible())
        {
            p->Draw(hDC, prc->left, prc->top);
        }
    }

    this->DrawForegnd(hDC);

    return;
}

void  CompositeREOle::OnSizeChanged(int cx, int cy)
{
    OLEELEMENTITER iter = m_elementList.begin();
    for (; iter != m_elementList.end(); iter++)
    {
        OleElement* p = *iter;

        //if (p->IsVisible())   // �򵥴������ص�ҲҪ���֣�������ʾʱλ�ò�����
            p->UpdateLayout(cx, cy);
    }
}

LRESULT  CompositeREOle::ProcessMessage(UINT msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch (msg)
    {
    case WM_MOUSEMOVE:
        return OnMouseMove(wParam, lParam, bHandled);
        break;

    case WM_MOUSELEAVE:
        return OnMouseLeave(wParam, lParam, bHandled);
        break;

    case WM_LBUTTONDOWN:
        return OnLButtonDown(wParam, lParam, bHandled);
        break;

    case WM_LBUTTONDBLCLK:
        return OnLButtonDBClick(wParam, lParam, bHandled);
        break;

    case WM_LBUTTONUP:
        return OnLButtonUp(wParam, lParam, bHandled);
        break;

    case WM_SETCURSOR:
        return OnSetCursor(wParam, lParam, bHandled);
        break;
    }
	return __super::ProcessMessage(msg, wParam, lParam, bHandled);
}

void  CompositeREOle::ControlPoint2OlePoint(POINT* ptControl, POINT* ptOle)
{
    ptOle->x = ptControl->x - m_rcDrawSave.left;    
    ptOle->y = ptControl->y - m_rcDrawSave.top;
}

OleElement*  CompositeREOle::HitTest(POINT ptInOle)
{
    RECT  rcElem;

    OLEELEMENTITER iter = m_elementList.begin();
    for (; iter != m_elementList.end(); iter++)
    {
        OleElement* pElem = *iter;
        if (!pElem->IsEnable() || !pElem->IsVisible())
            continue;

        pElem->GetRectInOle(&rcElem);
        if (PtInRect(&rcElem, ptInOle))
            return pElem;
    }

    return NULL;
}

LRESULT  CompositeREOle::OnMouseMove(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
    ControlPoint2OlePoint(&pt, &pt);

    OleElement*  pNowHover = HitTest(pt);

    if (m_pHoverElement == pNowHover)
    {
        if (m_pHoverElement)
        {
            BOOL bHandled2 = FALSE;
            m_pHoverElement->ProcessMessage(WM_MOUSEMOVE, wParam, lParam, bHandled2);
            if (bHandled2)
                bHandled = TRUE;
        }
    }
    else
    {
        if (m_pHoverElement)
        {
            if (m_pPressElement && m_pPressElement == m_pHoverElement)
            {
                m_pPressElement = NULL;
            }

            BOOL bHandled2 = FALSE;
            m_pHoverElement->ProcessMessage(WM_MOUSELEAVE, wParam, lParam, bHandled2);
            if (bHandled2)
                bHandled = TRUE;
        }
        if (pNowHover)
        {
            BOOL bHandled2 = FALSE;
            pNowHover->ProcessMessage(WM_MOUSEMOVE, wParam, lParam, bHandled2);
            if (bHandled2)
                bHandled = TRUE;
        }
        m_pHoverElement = pNowHover;
    }

    return 0;
}

LRESULT  CompositeREOle::OnLButtonDown(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;

    if (m_pHoverElement)
    {
        m_pPressElement = m_pHoverElement;
        return m_pHoverElement->ProcessMessage(WM_LBUTTONDOWN, wParam, lParam, bHandled);
    }
    return 0;
}
LRESULT  CompositeREOle::OnLButtonDBClick(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;

    if (m_pHoverElement)
    {
        m_pPressElement = m_pHoverElement;
        return m_pHoverElement->ProcessMessage(WM_LBUTTONDBLCLK, wParam, lParam, bHandled);
    }
    return 0;
}
LRESULT  CompositeREOle::OnLButtonUp(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;
    if (m_pPressElement)
    {

        OleElement* pSave = m_pPressElement;
        m_pPressElement = NULL;

        return pSave->ProcessMessage(WM_LBUTTONUP, wParam, lParam, bHandled);
    }
    return 0;
}

LRESULT CompositeREOle::OnSetCursor(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;
    if (m_pHoverElement)
    {
        return m_pHoverElement->ProcessMessage(WM_SETCURSOR, wParam, lParam, bHandled);
    }
    return 0;
}

LRESULT  CompositeREOle::OnMouseLeave(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if (m_pPressElement)
    {
        m_pPressElement = NULL;
    }
    if (m_pHoverElement)
    {
        m_pHoverElement->ProcessMessage(WM_MOUSELEAVE, wParam, lParam, bHandled);
        m_pHoverElement = NULL;
    }
    bHandled = FALSE;
    return 0;
}