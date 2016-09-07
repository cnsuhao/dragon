#include "stdafx.h"
#include "long_attribute.h"
#include "attribute.h"

using namespace UI;

namespace UI
{
    AttributeBase*  CreateLongAttribute()
    {
        return new LongAttribute();
    }
}

LongAttribute::LongAttribute() 
{
    m_pILongAttribute = NULL;
    m_pBindValue = NULL;
    m_lDefault = 0;

    _this = NULL;
    _setter = NULL;
    _getter = NULL;
}

LongAttribute::~LongAttribute()
{
    SAFE_DELETE(m_pILongAttribute);
}

ILongAttribute*  LongAttribute::GetILongAttribute()
{
    if (!m_pILongAttribute)    
        m_pILongAttribute = new ILongAttribute(this);

    return m_pILongAttribute;
}

void  LongAttribute::SetBindValue(void* p)
{
    m_pBindValue = (long*)p;
}

void  LongAttribute::SetBindFuction(void* _this, void* _setter, void* _getter)
{
    this->_this = _this;
    this->_setter = (pfnLongSetter)_setter;
    this->_getter = (pfnLongGetter)_getter;
}

LongAttribute*  LongAttribute::SetDefault(long l)
{
	m_lDefault = l;

	return this;
}

//
// 注：不能直接写：b = _getter();
//     这样的结果是将LongAttribute的this又mov到ecx了
//
long  LongAttribute::GetLong()
{
    if (m_pBindValue)
    {
        return (*m_pBindValue);
    }
    else if (_getter && _this)
    {
        long lReturn = m_lDefault;
        long localVarThis = (long)_this;
        long localVarGetter = (long)_getter;

        __asm
        {
            mov ecx, localVarThis;  // 设置this
            call localVarGetter;    // 调用
            mov lReturn, eax;       // 获取返回值
        }

        return lReturn;
    }
    else
    {
        return m_lDefault;
    }
}
void  LongAttribute::SetLong(long l)
{
    if (m_pBindValue)
    {
        (*m_pBindValue) = l;
    }
    else if (_setter && _this)
    {
        long localVarThis = (long)_this;
        long localVarSetter = (long)_setter;

        __asm
        {
            mov eax, dword ptr [l]   // 压入参数
            push eax;

            mov ecx, localVarThis;   // 设置this
            call localVarSetter;     // 调用
        }
    }
}
LPCTSTR  LongAttribute::Get()
{
    long lValue = GetLong();
    
    LPCTSTR szAlias = m_mapAlias.GetAlias(lValue);
    if (szAlias)
    {
        return szAlias;
    }
    else
    {
		TCHAR* szText = GetTempBuffer(64);
        _stprintf(szText, TEXT("%d"), lValue);
        return szText;
    }
}

void  LongAttribute::Set(LPCTSTR szValue)
{
    long lValue = m_lDefault;

    if (m_mapAlias.GetAlias(szValue, &lValue))
    {
        SetLong(lValue);
    }
    else
    {
        lValue = _ttoi(szValue);
        SetLong(lValue);
    }
}

void  LongAttribute::Reset()
{
	SetLong(m_lDefault);
}

bool  LongAttribute::IsDefaultValue()
{
    return GetLong() == m_lDefault;
}

LongAttribute*  LongAttribute::AddAlias(long l, LPCTSTR sz)
{
    m_mapAlias.AddAlias(l, sz);
    return this;
}

void  LongAttribute::Editor(AttributeEditorProxy* p, EditorAttributeFlag e)
{
    p->Long2Editor(this, e);
}

long  LongAttribute::EnumAlias(pfnEnumAliasCallback c, WPARAM w, LPARAM l)
{
	return m_mapAlias.EnumAlias(c, w, l);
}
uint  LongAttribute::GetAliasCount()
{
	return m_mapAlias.GetCount();
}