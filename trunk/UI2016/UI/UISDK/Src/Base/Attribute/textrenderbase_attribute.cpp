#include "stdafx.h"
#include "textrenderbase_attribute.h"
#include "attribute.h"
#include "..\Application\uiapplication.h"
#include "..\Object\object.h"

namespace UI
{
    AttributeBase*  CreateTextRenderBaseAttribute()
    {
        return new TextRenderBaseAttribute();
    }
}

TextRenderBaseAttribute::TextRenderBaseAttribute() 
{
    m_pITextRenderBaseAttribute = NULL;

    m_ppBindValue = NULL;
    m_pObject = NULL;
//     _this = NULL;
//     _setter = NULL;
//     _getter = NULL;
}

TextRenderBaseAttribute::~TextRenderBaseAttribute()
{
	SAFE_DELETE(m_pITextRenderBaseAttribute);
}

void  TextRenderBaseAttribute::SetBindValue(void* p)
{
    m_ppBindValue = (ITextRenderBase**)p;
}

void  TextRenderBaseAttribute::Set(LPCTSTR szType)
{
    if (!m_ppBindValue)
        return;

	if (!m_pUIApplication || !m_pObject)
    {
        UIASSERT(0);
        return;
    }

    SAFE_RELEASE(*m_ppBindValue);
    m_pUIApplication->GetIUIApplication()->CreateTextRenderBaseByName(
		szType, m_pObject->GetIObject(), m_ppBindValue);

	if (*m_ppBindValue)
	{
		IMapAttribute* pMapAttrib = NULL;
		m_pObject->GetMapAttribute(&pMapAttrib);

		SERIALIZEDATA data = { 0 };
		data.pUIApplication = m_pUIApplication->GetIUIApplication();
		data.pSkinRes = m_pSkinRes ? m_pSkinRes->GetISkinRes() : NULL;
		data.pMapAttrib = pMapAttrib;
		data.szParentKey = XML_TEXTRENDER_TYPE;
		data.szPrefix = m_strPrefix.c_str();
		data.nFlags = SERIALIZEFLAG_LOAD | SERIALIZEFLAG_LOAD_ERASEATTR;

		if (data.pUIApplication->IsDesignMode())
			data.SetErase(false);

		(*m_ppBindValue)->Serialize(&data);

		SAFE_RELEASE(pMapAttrib);
	}
}

void  TextRenderBaseAttribute::Reset()
{
	if (m_ppBindValue)
	{
		SAFE_RELEASE(*m_ppBindValue);
	}
	else
	{

	}
}

LPCTSTR  TextRenderBaseAttribute::Get()
{
    if (!m_ppBindValue)
        return NULL;

    //return (*m_ppBindValue)->GetName()
    return NULL;
}

bool  TextRenderBaseAttribute::IsDefaultValue()
{
	if (m_ppBindValue)
	{
		if (*m_ppBindValue == NULL)
			return true;
	}

	return false;
}


void  TextRenderBaseAttribute::Editor(AttributeEditorProxy* p, EditorAttributeFlag e)
{
	UIASSERT(0);
    // p->TextRenderBase2Editor(this, e);
}

ITextRenderBaseAttribute*  TextRenderBaseAttribute::GetITextRenderBaseAttribute()
{
    if (!m_pITextRenderBaseAttribute)
        m_pITextRenderBaseAttribute = new ITextRenderBaseAttribute(this);

    return m_pITextRenderBaseAttribute;
}
