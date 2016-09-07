#include "stdafx.h"
#include "textrenderbase_attribute.h"
#include "attribute.h"
#if 0
using namespace UI;

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
SAFE_DELETE(m_pITextRenderBaseAttribute);
void  TextRenderBaseAttribute::SetBindValue(void* p)
{
    m_ppBindValue = (ITextRenderBase**)p;
}

void  TextRenderBaseAttribute::Set(LPCTSTR szType)
{
    if (!m_ppBindValue)
        return;

    if (!m_pUIApplication)
    {
        UIASSERT(0);
        return;
    }

    SAFE_RELEASE(*m_ppBindValue);
    m_pUIApplication->CreateTextRenderBaseByName(szType, m_pObject, m_ppBindValue);

//     if (*m_ppBindValue)
//     {
//          (*m_ppBindValue)->SetAttribute(pData->pMapAttrib, m_strPrefix.c_str(), pData->NeedErase());
//     }
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
    p->TextRenderBase2Editor(this, e);
}

ITextRenderBaseAttribute*  TextRenderBaseAttribute::GetITextRenderBaseAttribute()
{
    if (!m_pITextRenderBaseAttribute)
        m_pITextRenderBaseAttribute = new ITextRenderBaseAttribute(this);

    return m_pITextRenderBaseAttribute;
}
#endif