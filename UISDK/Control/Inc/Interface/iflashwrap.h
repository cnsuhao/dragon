#ifndef IFLASHWRAP_H_11AB807B_7A61_4003_85E8_7A424A7E9A93
#define IFLASHWRAP_H_11AB807B_7A61_4003_85E8_7A424A7E9A93
#include "UISDK\Kernel\Inc\Interface\icontrol.h"

namespace UI
{

class FlashWrap;
interface UICTRLAPI IFlashWrap : public IControl
{
    UI_DECLARE_Ixxx_INTERFACE(IFlashWrap, FlashWrap);

    void  Pause();
    void  Play();
    void  SetFlashUri(LPCTSTR szUri);
    HRESULT  CallFlashFunction(LPCTSTR szFuncName, LPCTSTR szArgs, BSTR* pbstrRet);
};

}

#endif // IFLASHWRAP_H_11AB807B_7A61_4003_85E8_7A424A7E9A93