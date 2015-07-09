#pragma once
#include "attributebase.h"
#include "attribute_alias.h"

namespace UI
{

class LongAttribute : public AttributeBase
{
public:
    LongAttribute();
    ~LongAttribute();
    ILongAttribute*  GetILongAttribute();

    virtual LPCTSTR  Get() override; 
    virtual void  Set(LPCTSTR) override;
	virtual void  Reset() override;
    virtual void  Editor(AttributeEditorProxy*, EditorAttributeFlag e) override;
    virtual bool  IsDefaultValue() override;
    virtual void  SetBindValue(void*) override;
    virtual void  SetBindFuction(void* _this, void* _setter, void* _getter) override;

public:
	LongAttribute*  SetDefault(long l);
    LongAttribute*  AddAlias(long, LPCTSTR);

    long  GetLong();
    void  SetLong(long);

	uint  GetAliasCount();
	long  EnumAlias(pfnEnumAliasCallback, WPARAM, LPARAM);

protected:
    long*  m_pBindValue;
    long   m_lDefault;

    void*   _this;
    pfnLongSetter  _setter;
    pfnLongGetter  _getter;

    LongAttributeAlias  m_mapAlias;

private:
    ILongAttribute*  m_pILongAttribute;
};

AttributeBase*  CreateLongAttribute();

}