#include "stdafx.h"
#include "richeditolemgr.h"
#include "Composite\Picture\pictureole.h"

using namespace UI;


IREOleBase::IREOleBase(REOleBase* p)
{
    m_pImplBase = p;
}
long  IREOleBase::GetOleType()
{
    return m_pImplBase->GetOleType();
}
void  IREOleBase::SetExternalData(IREOleExternalData* p)
{
    m_pImplBase->SetExternalData(p);
}
IREOleExternalData*  IREOleBase::GetExternalData()
{
    return m_pImplBase->GetExternalData();
}


IPictureREOle::IPictureREOle(PictureOle* p) : IREOleBase(p)
{
    m_pImpl = p;
}

LPCTSTR  IPictureREOle::GetPath()
{
    return m_pImpl->GetPath();
}
void  IPictureREOle::UpdateStatus(REOleLoadStatus e, LPCTSTR szPath)
{
    m_pImpl->UpdateStatus(e, szPath);
}