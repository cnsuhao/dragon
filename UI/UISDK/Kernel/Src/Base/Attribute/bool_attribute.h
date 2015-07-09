#pragma once
#include "attributebase.h"

namespace UI
{

class BoolAttribute : public AttributeBase
{
public:
    BoolAttribute();
    ~BoolAttribute();
    IBoolAttribute*  GetIBoolAttribute();

    virtual LPCTSTR  Get() override; 
    virtual void  Set(LPCTSTR) override;
	virtual void  Reset() override;
    virtual void  Editor(AttributeEditorProxy* p, EditorAttributeFlag e) override;
    virtual bool  IsDefaultValue() override;
    virtual void  SetBindValue(void*) override;
    virtual void  SetBindFuction(void* _this, void* _setter, void* _getter) override;

public:
	BoolAttribute*  SetDefault(bool b);

    bool  GetBool();
    void  SetBool(bool);
    bool  GetDefaultBool() { return m_bDefault; }

private:
    IBoolAttribute*  m_pIBoolAttribute;

    bool*  m_pBindValue;
    bool   m_bDefault;

    void*   _this;
    pfnBoolSetter  _setter;
    pfnBoolGetter  _getter;
};

AttributeBase*  CreateBoolAttribute();

}