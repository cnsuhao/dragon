#ifndef CONTROLAPI_H_AC9FAB02_B6D5_4a6b_97CE_9C7A03586855
#define CONTROLAPI_H_AC9FAB02_B6D5_4a6b_97CE_9C7A03586855

namespace UI
{
interface IUIApplication;
interface ITrayIcon;
}

extern "C" UICTRLAPI 
bool  UICtrl_RegisterUIObject(UI::IUIApplication* p);
extern "C" UICTRLAPI
bool  UICreateTrayIcon(UI::IUIApplication* pUIApplication, UI::ITrayIcon** ppOut);

#endif  // CONTROLAPI_H_AC9FAB02_B6D5_4a6b_97CE_9C7A03586855