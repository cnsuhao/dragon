#pragma once
#include "attributebase.h"

namespace UI
{
class RenderBaseAttribute : public AttributeBase
{
public:
    RenderBaseAttribute();
	~RenderBaseAttribute();

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

    IRenderBaseAttribute*  GetIRenderBaseAttribute();

private:
    IRenderBaseAttribute*  m_pIRenderBaseAttribute;

    IRenderBase**  m_ppBindValue;
    Object*  m_pObject;

    String  m_strPrefix;

//     void*  _this;
//     pfnStringSetter  _setter;
//     pfnStringGetter  _getter;
};

AttributeBase*  CreateRenderBaseAttribute();
}