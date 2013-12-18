#include "stdafx.h"
#include "3dlayer.h"

namespace UI
{

Layer3d::Layer3d()
{
    m_pILayer3d = NULL;
    m_nRef = 0;
    m_bDirty = false;
}

Layer3d::~Layer3d()
{
#ifdef _DEBUG
    if (0 != m_nRef)
    {
        UI_LOG_WARN(_T("%s  m_nRef == %d"), FUNC_NAME, m_nRef);  
    }
#endif
}

void  Layer3d::OnSize(UINT nType, int cx, int cy)
{
    SetMsgHandled(FALSE);
    SetDirty(true);
}

long Layer3d::AddRef()
{
    return (++m_nRef);
}

long Layer3d::Release()
{
    return (--m_nRef);
}

void  Layer3d::SetDirty(bool b)
{
    m_bDirty = true;
}
bool  Layer3d::IsDirty()
{
    return m_bDirty;
}

void  Layer3d::Draw()
{
    if (!m_bDirty)
        return;

    Clear();
    ControlRenderLayer::Draw();
    m_bDirty = false;
}

void  Layer3d::Commit(HDC hDstDC)
{
    if (m_bDirty)
        this->Draw();

    ControlRenderLayer::Commit(hDstDC);
}

}