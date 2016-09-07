#ifndef _UI_CONTROLAPI_H_
#define _UI_CONTROLAPI_H_

namespace UI
{
interface IUIApplication;
// interface ITrayIcon;
}

extern "C" UICTRLAPI 
bool  UICTRL_RegisterUIObject(UI::IUIApplication* p);
// extern "C" UICTRLAPI
// bool  UICreateTrayIcon(UI::IUIApplication* pUIApplication, UI::ITrayIcon** ppOut);

#endif  