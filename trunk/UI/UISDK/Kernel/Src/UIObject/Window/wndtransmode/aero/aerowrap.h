#pragma once
#include "../wndtransmodebase.h"
#include "UISDK\Kernel\Src\Util\dwm\dwmhelper.h"

namespace UI
{

class AeroWindowWrap: public WndTransModeBase
{
public:
    AeroWindowWrap();
    ~AeroWindowWrap();

    VIRTUAL_BEGIN_MSG_MAP(LayeredWindowWrap)
#if(_WIN32_WINNT >= 0x0600)
        MESSAGE_HANDLER(WM_DWMCOMPOSITIONCHANGED, _OnDwmCompositionChanged)
#endif
		MESSAGE_HANDLER(WM_NCCALCSIZE, _OnNcCalcSize)
        CHAIN_MSG_MAP_MEMBER_P(m_pAeroDisableMode);
    END_MSG_MAP()

    virtual BOOL  ProcessMessage(UIMSG* pMsg, int nMsgMapID, bool bDoHook)
    {
        return innerVirtualProcessMessage(pMsg, nMsgMapID, bDoHook);
    }
    UI_BEGIN_MSG_MAP
        UICHAIN_MSG_MAP_POINT_MEMBER(m_pAeroDisableMode)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
    UI_END_MSG_MAP


    LRESULT  _OnDwmCompositionChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT  _OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    void*  QueryInterface(const IID* pIID);

    virtual WINDOW_TRANSPARENT_MODE  GetModeValue();
    virtual bool  RequireAlphaChannel();
    virtual void  UpdateRgn();
    virtual void  Enable(bool b);
    virtual bool  Commit();
    virtual void  Init(ICustomWindow* pWnd);

    IAeroWindowWrap*  GetIAeroWndTransMode();
    AERO_MODE  GetAeroMode();
    void  GetBlurRegion(CRegion4* pregion);
	void  SetTransparentMargins(RECT* prcMargin);

public:

    AERO_MODE  m_eMode;
    union
    {
        struct TransparentMode
        {
            MARGINS  m_margins;
        }trans;

        struct BlurMode
        {
            RECT  m_regionBlur;  // 指定四周不进行blur的区域
            HRGN  m_hrgnBlurRgn;  // <-- 外部设置
        }blur;
    };    

    DwmHelper*  m_pDWM;
    WINDOW_TRANSPARENT_MODE  m_eAeroDisableMode;
    IWndTransMode*  m_pAeroDisableMode;

    IAeroWindowWrap*  m_pIAeroWndTransMode;
};


}