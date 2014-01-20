#pragma once
#include "..\wndtransmodebase.h"

namespace UI
{

class AntiWindowWrap : public WndTransModeBase
{
public:
    AntiWindowWrap ();
    ~AntiWindowWrap ();

    virtual WINDOW_TRANSPARENT_MODE  GetModeValue()
    {
        return WINDOW_TRANSPARENT_MODE_ANTIALIASING;
    }
    virtual void  UpdateRgn();
    virtual void  Init(ICustomWindow* pWnd);
	virtual void  Enable(bool b);

private:
    C9Region  m_9region;
};

}