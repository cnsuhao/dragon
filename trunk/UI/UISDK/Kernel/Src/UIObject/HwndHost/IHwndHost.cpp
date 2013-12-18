#include "stdafx.h"
#include "UISDK\Kernel\Inc\Interface\ihwndhost.h"
#include "UISDK\Kernel\Src\UIObject\HwndHost\HwndHost.h"

namespace UI
{

UI_IMPLEMENT_Ixxx_INTERFACE(IHwndHost, HwndHost, Object)

void  IHwndHost::Attach(HWND hWnd)
{
	m_pHwndHostImpl->Attach(hWnd);
}
HWND  IHwndHost::Detach()
{
	return m_pHwndHostImpl->Detach();
}
}
