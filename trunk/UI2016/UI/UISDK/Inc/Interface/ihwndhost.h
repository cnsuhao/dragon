#ifndef _UI_IHWNDHOST_H_
#define _UI_IHWNDHOST_H_
#include "Inc\Interface\icontrol.h"

namespace UI
{

class HwndHost;
interface __declspec(uuid("86783C0F-875E-4C01-A6D1-3C0A6D3DFDF5"))
UISDKAPI IHwndHost : public IControl
{
	void  Attach(HWND hWnd);
	HWND  Detach();

    UI_DECLARE_INTERFACE(HwndHost);
};

}

#endif // _UI_IHWNDHOST_H_