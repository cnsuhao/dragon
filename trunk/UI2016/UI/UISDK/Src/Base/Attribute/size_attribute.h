#pragma once
#include "attributebase.h"

namespace UI
{

class SizeAttribute : public AttributeBase
{
public:
    SizeAttribute();
    ~SizeAttribute();

    virtual LPCTSTR  Get() override; 
    virtual void  Set(LPCTSTR) override;
	virtual void  Reset() override;
    virtual void  Editor(AttributeEditorProxy* p, EditorAttributeFlag e) override;
    virtual bool  IsDefaultValue() override;
    virtual void  SetBindValue(void*) override;
    virtual void  SetBindFuction(void* _this, void* _setter, void* _getter) override;

public:
	SizeAttribute*  SetDefault(LPSIZE);
    ISizeAttribute*  GetISizeAttribute();

private:
    void  get(LPSIZE);
    void  set(LPSIZE);

private:
    ISizeAttribute*  m_pISizeAttribute;

    SIZE*  m_pBindValue;
    SIZE   m_sizeDefault;

    void*   _this;
    pfnSizeSetter  _setter;
    pfnSizeGetter  _getter;
};

AttributeBase*  CreateSizeAttribute();

}