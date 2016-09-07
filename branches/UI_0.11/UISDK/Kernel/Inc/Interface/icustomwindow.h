#ifndef ICUSTOMWINDOW_H_E0B1A6A7_425D_4218_A842_5BE5418F4E7D
#define ICUSTOMWINDOW_H_E0B1A6A7_425D_4218_A842_5BE5418F4E7D

#include "UISDK\Kernel\Inc\Interface\iwindow.h"

namespace UI
{
class CustomWindow;
interface IWindowTransparent;
interface UISDKAPI ICustomWindow : public IWindow
{
    UI_DECLARE_Ixxx_INTERFACE(ICustomWindow, CustomWindow)

    void  SetWindowResizeType(UINT nType);
    void  SetWindowTransparentType(WINDOW_TRANSPARENT_TYPE eMode);
    WINDOW_TRANSPARENT_TYPE  GetWindowTransparentType();
	IWindowTransparent*  GetWindowTransparent();
};

}

#endif // ICUSTOMWINDOW_H_E0B1A6A7_425D_4218_A842_5BE5418F4E7D