#include "stdafx.h"
#include "string_attribute.h"
#include "attribute.h"
#include "Src\Resource\i18nres.h"
#include "Src\Resource\skinres.h"

using namespace UI;

namespace UI
{
AttributeBase*  CreateStringAttribute()
{
    return new StringAttribute();
}
}

StringAttribute::StringAttribute()
{
    m_pIStringAttribute = NULL;
    m_pBindValue = NULL;
    _this = NULL;
    _getter = NULL;
    _setter = NULL;
	m_bDefaultIsNULL = true;
	m_bI18n = false;
}

StringAttribute::~StringAttribute()
{
    SAFE_DELETE(m_pIStringAttribute);
}

IStringAttribute*  StringAttribute::GetIStringAttribute()
{
    if (!m_pIStringAttribute)
        m_pIStringAttribute = new IStringAttribute(this);

    return m_pIStringAttribute;
}

void  StringAttribute::SetBindValue(void* p)
{
    m_pBindValue = (String*)p;
	m_bDefaultIsNULL = false;
}

// 这种情况下，默认值为NULL
void  StringAttribute::SetBindFuction(void* _this, void* _setter, void* _getter)
{
    this->_this = _this;
    this->_setter = (pfnStringSetter)_setter;
    this->_getter = (pfnStringGetter)_getter;   
	m_bDefaultIsNULL = true;
}

LPCTSTR  StringAttribute::Get()
{
    if (m_pBindValue)
    {
        return m_pBindValue->c_str();
    }
    else if (_this && _getter)
    {
        LPCTSTR  szReturn = NULL;
		long  localVarThis = (long)_this;
		long  localVarGetter = (long)_getter;

		__asm
		{
			mov  ecx, localVarThis;  // 设置this
			call localVarGetter;    // 调用
			mov  szReturn, eax;        // 获取返回值
		}

        return szReturn;
    }
    else
    {
        return NULL;
    }
}

void  StringAttribute::Set(LPCTSTR szValue)
{
	if (m_bI18n)
	{
		if (szValue && szValue[0] && szValue[1])
		{
			if (szValue[0] == XML_I18N_PREFIX)
			{
				if (szValue[1] == XML_I18N_PREFIX)
				{
					// 两个##，取消转义
					szValue = szValue + 1;
				}
				else
				{
					// 后面的为需要解析的国际化字段
					szValue = szValue + 1;
					if (m_pSkinRes)
					{
						szValue = m_pSkinRes->GetI18nRes().I18nMap(szValue);
					}
				}
			}
		}

	}

    if (m_pBindValue)
    {
        if (szValue)
            m_pBindValue->assign(szValue);
        else
            m_pBindValue->clear();
    }
    else if (_this && _setter)
    {
		long  localVarThis = (long)_this;
		long  localVarSetter = (long)_setter;

		__asm
		{
			mov eax, dword ptr [szValue] // 压入参数
			push eax;

			mov  ecx, localVarThis;   // 设置this
			call localVarSetter;     // 调用
		}
    }
}

void  StringAttribute::Reset()
{
	Set(GetDefault());
}

bool  StringAttribute::IsDefaultValue()
{
    LPCTSTR szValue = Get();
	LPCTSTR szDefault = GetDefault();

    if (szValue == NULL && szDefault == NULL)
        return true;

	if (szValue && szDefault && 0 ==_tcscmp(szValue, szDefault))
		return true;

    return false;
}

StringAttribute*  StringAttribute::SetDefault(LPCTSTR szValue)
{
	if (szValue)
	{
		m_bDefaultIsNULL = false;
		m_strDefault = szValue;
	}
	else
	{
		m_bDefaultIsNULL = true;
		m_strDefault.clear();
	}

	return this;
}

LPCTSTR  StringAttribute::GetDefault()
{
	if (m_bDefaultIsNULL)
		return NULL;

	return m_strDefault.c_str();
}

void StringAttribute::Editor(AttributeEditorProxy* p, EditorAttributeFlag e)
{
    p->String2Editor(this, e);
}

void  StringAttribute::Internationalization()
{
	m_bI18n = true;
}