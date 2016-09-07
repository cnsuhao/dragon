#include "stdafx.h"
#include "Control\Button\button_desc.h"
#include "Control\Label\label_desc.h"
#include "Control\Menu\menu_desc.h"
#include "Control\Menu\menu.h"
#include "Control\Edit\edit_desc.h"
#include "Control\RadioButton\radiobutton_desc.h"
#include "Control\CheckButton\checkbutton_desc.h"

namespace UI
{
    extern "C"
    __declspec(dllexport) bool RegisterUIObject(IUIApplication* p)
    {
		p->RegisterUIObject(ButtonDescription::Get());
		p->RegisterUIObject(LabelDescription::Get());
		p->RegisterUIObject(PictureCtrlDescription::Get());
		p->RegisterUIObject(GifCtrlDescription::Get());
		p->RegisterUIObject(EditDescription::Get());
		p->RegisterUIObject(InstantEditDescription::Get());
		p->RegisterUIObject(PasswordEditDescription::Get());
		p->RegisterUIObject(RadioButtonDescription::Get());
		p->RegisterUIObject(CheckButtonDescription::Get());
		
		// p->RegisterUIObject(MenuDescription::Get());
		p->RegisterControlTagParseFunc(_T("Menu"), Menu::UIParseMenuTag);

        return true;
    }
}

extern "C"
bool UICTRL_RegisterUIObject(IUIApplication* p)
{
	return RegisterUIObject(p);
}

// 
// extern "C"
// bool UICreateTrayIcon(IUIApplication* pUIApplication, ITrayIcon** ppOut)
// {
// 	if (NULL == ppOut)
// 		return false;
// 
// 	TrayIcon* p = new TrayIcon(pUIApplication);
// 	*ppOut = static_cast<ITrayIcon*>(p);
// 	p->AddRef();
// 
// 	return true;
// }

// 检测一个按键是否按下，如CTRL,SHIFT,ALT
bool IsKeyDown(UINT vk)
{
	return ((GetKeyState(vk) & 0x8000) != 0);  // 或者 ( GetKeyState( VK_CONTROL ) < 0 ) ??
}
