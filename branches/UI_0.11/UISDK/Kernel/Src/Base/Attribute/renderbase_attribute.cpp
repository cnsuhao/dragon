#include "stdafx.h"
#include "renderbase_attribute.h"
#include "attribute.h"

using namespace UI;
#if 0
namespace UI
{
    AttributeBase*  CreateRenderBaseAttribute()
    {
        return new RenderBaseAttribute();
    }
}

RenderBaseAttribute::RenderBaseAttribute() 
{
    m_pIRenderBaseAttribute = NULL;
//     m_ppBindValue = NULL;
//     m_pObject = NULL;
//     _this = NULL;
//     _setter = NULL;
//     _getter = NULL;
}
SAFE_DELETE(m_pIRenderBaseAttribute);
void  RenderBaseAttribute::SetBindValue(void* p)
{
    m_ppBindValue = (IRenderBase**)p;
}

void  RenderBaseAttribute::Set(LPCTSTR szType)
{
    if (!m_ppBindValue)
        return;

    if (!m_pUIApplication)
    {
        UIASSERT(0);
        return;
    }

    SAFE_RELEASE(*m_ppBindValue);
    m_pUIApplication->CreateRenderBaseByName(szType, m_pObject, m_ppBindValue);

//     if (*m_ppBindValue)
//     {
//          (*m_ppBindValue)->SetAttribute(pData->pMapAttrib, m_strPrefix.c_str(), pData->NeedErase());
//     }
}

void  RenderBaseAttribute::Reset()
{
	if (m_ppBindValue)
	{
		SAFE_RELEASE(*m_ppBindValue);
	}
	else
	{

	}
}

LPCTSTR  RenderBaseAttribute::Get()
{
    if (!m_ppBindValue)
        return NULL;

    return (*m_ppBindValue)->GetName();
}

bool  RenderBaseAttribute::IsDefaultValue()
{
	if (m_ppBindValue)
	{
		if (*m_ppBindValue == NULL)
			return true;
	}

	return false;
}

void  RenderBaseAttribute::Editor(AttributeEditorProxy* p, EditorAttributeFlag e)
{
    p->RenderBase2Editor(this, e);
}

IRenderBaseAttribute*  RenderBaseAttribute::GetIRenderBaseAttribute()
{
    if (!m_pIRenderBaseAttribute)
        m_pIRenderBaseAttribute = new IRenderBaseAttribute(this);

    return m_pIRenderBaseAttribute;
}
#endif