#ifndef IHWNDHOST_H_291D8CA9_A0C4_42d3_8CC1_855A0DC8A14F
#define IHWNDHOST_H_291D8CA9_A0C4_42d3_8CC1_855A0DC8A14F
#include "UISDK\Kernel\Inc\Interface\icontrol.h"

namespace UI
{

class HwndHost;
interface UISDKAPI IHwndHost : public IControl
{
    UI_DECLARE_Ixxx_INTERFACE(IHwndHost, HwndHost);

	void  Attach(HWND hWnd);
	HWND  Detach();
};

}

#endif // IHWNDHOST_H_291D8CA9_A0C4_42d3_8CC1_855A0DC8A14F