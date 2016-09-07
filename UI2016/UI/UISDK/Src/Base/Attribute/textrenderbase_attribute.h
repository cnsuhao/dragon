#pragma once
#include "attributebase.h"

namespace UI
{

class TextRenderBaseAttribute : public AttributeBase
{
public:
    TextRenderBaseAttribute();
	~TextRenderBaseAttribute();

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
    void  SetObjectPtr(Object* p)
    {
        m_pObject = p; 
    }
    void  SetPrefix(LPCTSTR szPrefix)
    {
        if (szPrefix)
            m_strPrefix = szPrefix;
        else
            m_strPrefix.clear();
    }

    ITextRenderBaseAttribute*  GetITextRenderBaseAttribute();

private:
    ITextRenderBaseAttribute*  m_pITextRenderBaseAttribute;

    ITextRenderBase**  m_ppBindValue;
    Object*  m_pObject;

    String  m_strPrefix;
//     void*  _this;
//     pfnLongSetter  _setter;
//     pfnLongGetter  _getter;
};

AttributeBase*  CreateTextRenderBaseAttribute();

}
