#pragma once
#include "attributebase.h"

namespace UI
{

class ColorAttribute : public AttributeBase
{
public:
    ColorAttribute();
	~ColorAttribute();

    virtual LPCTSTR  Get() override; 
    virtual void  Set(LPCTSTR) override;
	virtual void  Reset() override;
    virtual void  Editor(AttributeEditorProxy* p, EditorAttributeFlag e) override;
    virtual bool  IsDefaultValue() override;
    virtual void  SetBindValue(void*) override;
    virtual void  SetBindFuction(void* _this, void* _setter, void* _getter) override
    {
        // TODO:
        UIASSERT(0);
        return;
    }

public:
    IColorAttribute*  GetIColorAttribute();

private:
    IColorAttribute*  m_pIColorAttribute;

    Color**  m_ppBindValue;

//     void*  _this;
//     pfnLongSetter  _setter;
//     pfnLongGetter  _getter;
};

AttributeBase*  CreateColorAttribute();

}
