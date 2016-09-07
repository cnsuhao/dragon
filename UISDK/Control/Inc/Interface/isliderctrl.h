#ifndef ISLIDERCTRL_H_955F5E3E_3091_4ca4_96CD_52929B078315
#define ISLIDERCTRL_H_955F5E3E_3091_4ca4_96CD_52929B078315
#include "UISDK\Kernel\Inc\Interface\icontrol.h"

namespace UI
{

//
//	滑动条位置改变通知
//
//		message:UI_WM_NOTIFY
//		code:   UI_TRBN_POSCHANGED
//		wparam: new Pos
//		lParam: scroll type, SB_LEFT SB_RIGHT SB_THUMBTRACK SB_ENDSCROLL ...
//		pMsgFrom: this
//
#define UI_TRBN_POSCHANGED  130172216

// void OnTrbnPosChanged( int nPos, int nScrollType );
#define UIMSG_TRBN_POSCHANGED_ID(OBJID, func)         \
    if (uMsg == UI_WM_NOTIFY  &&                      \
        code == UI_TRBN_POSCHANGED &&                 \
        NULL != pMsgFrom &&                           \
        0 == _tcscmp(((IObject*)pMsgFrom)->GetId(), OBJID)) \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func ((int)wParam, (int)lParam);              \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void OnTrbnPosChanged( int nPos, int nScrollType );
#define UIMSG_TRBN_POSCHANGED( p, func )              \
    if (uMsg == UI_WM_NOTIFY  &&                      \
        code == UI_TRBN_POSCHANGED &&                 \
        NULL != pMsgFrom &&                           \
        (pMsgFrom) == (UI::IMessage*)p)               \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func( (int)wParam, (int)lParam );             \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }


class SliderCtrl;
interface UICTRLAPI ISliderCtrl : public IControl
{
    UI_DECLARE_Ixxx_INTERFACE(ISliderCtrl, SliderCtrl);

    int   SetPos(int nPos, bool bUpdate=true);
    int   SetPage(int nPage, bool bUpdate=true);
    void  SetRange(int nLower, int nUpper, bool bUpdate=true);
    void  SetScrollInfo(LPCSCROLLINFO lpsi, bool bUpdate=true);

    void  SetLine(int nLine);
    int   GetPos();
    int   GetPage();
    int   GetLine();
    void  GetRange(int& nLower, int& nUpper);
    int   GetRange();

    void  SetDirectionType(PROGRESSBAR_SCROLL_DIRECTION_TYPE eType);
    PROGRESSBAR_SCROLL_DIRECTION_TYPE  GetDirectionType();
    int   SetTickFreq(int nFreq);
};

}

#endif // ISLIDERCTRL_H_955F5E3E_3091_4ca4_96CD_52929B078315