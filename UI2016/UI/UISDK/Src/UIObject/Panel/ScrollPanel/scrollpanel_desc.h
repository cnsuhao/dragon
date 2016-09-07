#pragma once
#include "Inc\Interface\iobjectdescription.h"
#if 0
#include "Inc\Interface\iscrollpanel.h"

class ScrollPanelDescription : public IObjectDescription
{
public:
	static IObjectDescription*  Get()
	{
		static ScrollPanelDescription s;
		return &s;
	}

    virtual void  CreateInstance(UI::ISkinRes* p, void** pp) override
    {
        *pp = IScrollPanel::CreateInstance(p);
    }

	virtual OBJ_TYPE  GetMajorType() override
	{
		return OBJ_PANEL;
	}
	virtual long  GetMinorType() override
	{
		return PANEL_SIMPLE;
	}
	virtual const wchar_t*  GetCategory() override
	{
		return CATEGORY_CONTAINER; 
	}
	virtual const wchar_t*  GetTagName() override
	{
		return L"ScrollPanel";
	}
	virtual const GUID&  GetGUID() override
	{
		return IID_UI_IScrollPanel;
	}

    virtual void  GetDefaultSize(long& w, long h) override
    {
        w = 100;
        h = 100;
    }
    virtual HBITMAP  LoadIcon() override 
    {
        return NULL;
    }
};
#endif