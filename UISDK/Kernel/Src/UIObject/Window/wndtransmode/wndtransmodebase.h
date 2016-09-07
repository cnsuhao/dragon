#pragma once
#include "UISDK\Kernel\Inc\Interface\iwndtransmode.h"
#include "UISDK\Kernel\Src\Base\Message\message.h"

namespace UI
{
class CustomWindow;
interface ICustomWindow;

class WndTransModeBase : public Message, 
                         public IWindowTransparent
{
public:
    WndTransModeBase();
    virtual ~WndTransModeBase();

    virtual BOOL  ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0)
    {
        return FALSE;
    }
    virtual BOOL  ProcessMessage(UIMSG* pMsg, int nMsgMapID, bool bDoHook)
    {
        return FALSE;
    }

    virtual void  Init(ICustomWindow* pWnd);
    virtual void  Release()
    {
        delete this;
    }

    virtual bool  RequireAlphaChannel()
    {
        return false;
    }
    virtual void  Enable(bool b)
    {

    }
    virtual void  UpdateRgn() 
    {

    }
    virtual bool  Commit()
    {
        return false;
    }

public:
    HRGN  CreateRgnByVectorRect(vector<RECT>& vec);
    HRGN  CreateRgnByVectorPoint(vector<POINT>& vec);
    bool  GetWindowBitmapInfo(byte** ppScan0, int& nPitch);
	
	HWND  GetHWND();

public:
    CustomWindow*  m_pWindow;
};

}