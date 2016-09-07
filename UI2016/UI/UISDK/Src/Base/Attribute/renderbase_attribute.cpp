#include "stdafx.h"
#include "renderbase_attribute.h"
#include "attribute.h"
#include "..\Application\uiapplication.h"
#include "..\Object\object.h"

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
    m_ppBindValue = NULL;
    m_pObject = NULL;
//     _this = NULL;
//     _setter = NULL;
//     _getter = NULL;
}

RenderBaseAttribute::~RenderBaseAttribute()
{
	SAFE_DELETE(m_pIRenderBaseAttribute);
}

void  RenderBaseAttribute::SetBindValue(void* p)
{
    m_ppBindValue = (IRenderBase**)p;
}

void  RenderBaseAttribute::Set(LPCTSTR szType)
{
    if (!m_ppBindValue)
        return;

	if (!m_pUIApplication || !m_pObject)
    {
        UIASSERT(0);
        return;
    }

    SAFE_RELEASE(*m_ppBindValue);
    m_pUIApplication->GetIUIApplication()->CreateRenderBaseByName(
		szType, m_pObject->GetIObject(), m_ppBindValue);

	if (*m_ppBindValue)
    {
		IMapAttribute* pMapAttrib = NULL;
		m_pObject->GetMapAttribute(&pMapAttrib);

		SERIALIZEDATA data = { 0 };
		data.pUIApplication = m_pUIApplication->GetIUIApplication();
		data.pSkinRes = m_pSkinRes ? m_pSkinRes->GetISkinRes() : NULL;
		data.pMapAttrib = pMapAttrib;
		data.szPrefix = m_strPrefix.c_str();
		data.nFlags = SERIALIZEFLAG_LOAD | SERIALIZEFLAG_LOAD_ERASEATTR;
		(*m_ppBindValue)->Serialize(&data);

		if (data.pUIApplication->IsDesignMode())
			data.SetErase(false);

		SAFE_RELEASE(pMapAttrib);
    }
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

	UIASSERT(0);
    //return (*m_ppBindValue)->GetName();
	return NULL;
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
	UIASSERT(0);
    //p->RenderBase2Editor(this, e);
}

IRenderBaseAttribute*  RenderBaseAttribute::GetIRenderBaseAttribute()
{
    if (!m_pIRenderBaseAttribute)
        m_pIRenderBaseAttribute = new IRenderBaseAttribute(this);

    return m_pIRenderBaseAttribute;
}
