#include "stdafx.h"
#include "color_attribute.h"
#include "attribute.h"
#include "..\Application\uiapplication.h"
#include "..\Object\object.h"
#include "Src\Resource\skinres.h"


namespace UI
{
    AttributeBase*  CreateColorAttribute()
    {
        return new ColorAttribute();
    }
}

ColorAttribute::ColorAttribute() 
{
    m_pIColorAttribute = NULL;

    m_ppBindValue = NULL;
//     _this = NULL;
//     _setter = NULL;
//     _getter = NULL;
}

ColorAttribute::~ColorAttribute()
{
	SAFE_DELETE(m_pIColorAttribute);
}

void  ColorAttribute::SetBindValue(void* p)
{
    m_ppBindValue = (Color**)p;
}

void  ColorAttribute::Set(LPCTSTR szColorId)
{
    if (!m_ppBindValue)
        return;

	if (!m_pSkinRes)
    {
        UIASSERT(0);
        return;
    }

	if (m_ppBindValue)
	{
		SAFE_RELEASE(*m_ppBindValue);
		if (!szColorId)
			return;
		
		ColorRes& colorRes = m_pSkinRes->GetColorRes();
		colorRes.GetColor(szColorId, m_ppBindValue);

#ifdef EDITOR_MODE
        if (*m_ppBindValue)
        {
            (*m_ppBindValue)->SetResId(szColorId);
        }
#endif
	}
}

void  ColorAttribute::Reset()
{
	if (m_ppBindValue)
	{
		SAFE_RELEASE(*m_ppBindValue);
	}
	else
	{
	}
}

LPCTSTR  ColorAttribute::Get()
{
    if (!m_ppBindValue || !*m_ppBindValue)
        return NULL;
    
#ifdef EDITOR_MODE
    LPCTSTR szText = (*m_ppBindValue)->GetResId();
    if (szText)
        return szText;
#endif

    LPTSTR szBuffer = GetTempBuffer();
    (*m_ppBindValue)->ToWebString(szBuffer);
    return szBuffer;
}

bool  ColorAttribute::IsDefaultValue()
{
	if (m_ppBindValue)
	{
		if (*m_ppBindValue == NULL)
			return true;
	}

	return false;
}

void  ColorAttribute::Editor(SERIALIZEDATA* pData, AttributeEditorProxy* p, EditorAttributeFlag e)
{
	p->Color2Editor(this, e);
}

IColorAttribute*  ColorAttribute::GetIColorAttribute()
{
    if (!m_pIColorAttribute)
        m_pIColorAttribute = new IColorAttribute(this);

    return m_pIColorAttribute;
}
