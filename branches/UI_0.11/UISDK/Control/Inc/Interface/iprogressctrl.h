#ifndef IPROGRESSBAR_H_F31EC133_08A9_445f_8945_A669A163A5F0
#define IPROGRESSBAR_H_F31EC133_08A9_445f_8945_A669A163A5F0
#include "UISDK\Kernel\Inc\Interface\icontrol.h"

namespace UI
{

class ProgressBar;
interface UICTRLAPI IProgressBar : public IControl
{
    UI_DECLARE_Ixxx_INTERFACE(IProgressBar, ProgressBar)

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
};

}

#endif // IPROGRESSBAR_H_F31EC133_08A9_445f_8945_A669A163A5F0