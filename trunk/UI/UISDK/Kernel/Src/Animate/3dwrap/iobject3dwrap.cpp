#include "stdafx.h"
#include "UISDK\Kernel\Inc\Interface\iobject3dwrap.h"
#include "ui3dwrap.h"

namespace UI
{
IObject3DWrap::IObject3DWrap(Object3DWrap* p)
{
    m_pImpl = p;
}
Object3DWrap*  IObject3DWrap::GetImpl()
{
    return m_pImpl;
}

void  IObject3DWrap::Begin()
{
    m_pImpl->Begin();
}
void  IObject3DWrap::End()
{
    m_pImpl->End();
}

void  IObject3DWrap::SetOriginPoint(POINT pt)
{
    m_pImpl->SetOriginPoint(pt);
}
void  IObject3DWrap::Rotate(int x, int y, int z)
{
    m_pImpl->Rotate(x, y, z);
}
void  IObject3DWrap::Move(int x, int y, int z)
{
    m_pImpl->Move(x, y, z);
}

}