#include "stdafx.h"

#include "UISDK\Control\Src\Control\Button\button.h"
#include "UISDK\Control\Src\Control\RadioButton\radiobutton.h"
#include "UISDK\Control\Src\Renderbase\themerender.h"
#include "UISDK\Control\Src\Renderbase\focusrender.h"
#include "UISDK\Control\Src\Control\CompoundButton\compoundbutton.h"
#include "UISDK\Control\Src\Control\RadioButton\radiobutton.h"
#include "UISDK\Control\Src\Control\CheckButton\checkbutton.h"
#include "UISDK\Control\Src\Control\Edit\edit.h"
#include "UISDK\Control\Src\Control\Edit\instantedit.h"
#include "UISDK\Control\Src\Control\Groupbox\groupbox.h"
#include "UISDK\Control\Src\Control\ProgressCtrl\progressctrl.h"
#include "UISDK\Control\Src\Control\SliderCtrl\sliderctrl.h"
#include "UISDK\Control\Src\Control\Label\label.h"
#include "UISDK\Control\Src\Control\LedCtrl\ledctrl.h"
#include "UISDK\Control\Src\Control\LedCtrl\ledexctrl.h"
#include "UISDK\Control\Src\Control\HeaderCtrl\headerlistctrl.h"
#include "UISDK\Control\Src\Control\FlashWrap\flashwrap.h"
#include "UISDK\Control\Src\Control\IE\iewrap.h"
#include "UISDK\Control\Src\Control\TrayIcon\trayicon.h"
#include "UISDK\Control\Src\Control\ListBox\listbox.h"
#include "UISDK\Control\Src\Control\ComboBox\combobox.h"
#include "UISDK\Control\Src\Control\Menu\menu.h"
#include "UISDK\Control\Src\Control\RichEdit\richedit.h"
#include "UISDK\Control\Src\Control\HyperLink\hyperlink.h"
#include "UISDK\Control\Src\Control\TabCtrl\tabctrl.h"
#include "UISDK\Control\Src\Control\TabCtrl\tabctrlbutton.h"
#include "UISDK\Control\Src\Control\ListView\listview.h"
#include "UISDK\Control\Src\Control\TreeView\treeview.h"
#include "UISDK\Control\Src\Control\ScrollBar\systemscrollbar.h"
#include "UISDK\Control\Src\Control\ListTreeView\listtreeview.h"
#include "UISDK\Control\Src\Control\SplitterBar\splitterbar.h"
#include "UISDK\Control\Src\Control\Win32Edit\win32edit.h"
#include "UISDK\Control\Src\Control\RichText\richtext.h"


namespace UI
{
    extern "C"
    __declspec(dllexport) bool RegisterUIObject(IUIApplication* p)
    {
		LPCTSTR  CATEGORY_CONTROL   = _T("UICtrl/Control"); 
		LPCTSTR  CATEGORY_CONTAINER = _T("UICtrl/Container");
		LPCTSTR  CATEGORY_3RD       = _T("UICtrl/3rd");
		LPCTSTR  CATEGORY_LISTCTRL  = _T("UICtrl/ListControl");

        REGISTER_UI_OBJECT(p, CATEGORY_CONTROL, Button);
        REGISTER_UI_OBJECT(p, CATEGORY_CONTROL, CompoundButton);
        REGISTER_UI_OBJECT(p, CATEGORY_CONTROL, RadioButton);
        REGISTER_UI_OBJECT(p, CATEGORY_CONTROL, CheckButton);
        REGISTER_UI_OBJECT(p, CATEGORY_CONTROL, HyperLink);
        REGISTER_UI_OBJECT(p, CATEGORY_CONTROL, Edit);
        REGISTER_UI_OBJECT(p, CATEGORY_CONTROL, InstantEdit);
		//REGISTER_UI_OBJECT(p, CATEGORY_CONTROL, Win32Edit);
        REGISTER_UI_OBJECT(p, CATEGORY_CONTAINER, GroupBox);
        REGISTER_UI_OBJECT(p, CATEGORY_CONTROL, ProgressBar);
        REGISTER_UI_OBJECT(p, CATEGORY_CONTROL, Label);
        REGISTER_UI_OBJECT(p, CATEGORY_CONTROL, PictureCtrl);
        REGISTER_UI_OBJECT(p, CATEGORY_CONTROL, LEDCtrl);
		REGISTER_UI_OBJECT(p, CATEGORY_CONTROL, LEDExCtrl);
		REGISTER_UI_OBJECT(p, CATEGORY_CONTROL, RichText);
		REGISTER_UI_OBJECT(p, CATEGORY_CONTROL, RichEdit);
		REGISTER_UI_OBJECT(p, CATEGORY_CONTROL, GifCtrl);
		REGISTER_UI_OBJECT(p, CATEGORY_CONTROL, SystemHScrollBar);
		REGISTER_UI_OBJECT(p, CATEGORY_CONTROL, SystemVScrollBar);
		REGISTER_UI_OBJECT(p, CATEGORY_CONTROL, SplitterBar);

        REGISTER_UI_OBJECT(p, CATEGORY_3RD, FlashWrap);
        REGISTER_UI_OBJECT(p, CATEGORY_3RD, IEWrap);

        REGISTER_UI_OBJECT(p, CATEGORY_LISTCTRL, ListBox);
        REGISTER_UI_OBJECT(p, CATEGORY_LISTCTRL, ComboBox);
        REGISTER_UI_OBJECT(p, CATEGORY_LISTCTRL, ListView);
        REGISTER_UI_OBJECT(p, CATEGORY_LISTCTRL, HeaderListCtrl);
        REGISTER_UI_OBJECT(p, CATEGORY_LISTCTRL, TreeView);
        REGISTER_UI_OBJECT(p, CATEGORY_LISTCTRL, ListTreeView);
     
        p->RegisterControlTagParseFunc(_T("Menu"), Menu::UIParseMenuTag);  // REGISTER_UI_OBJECT(p, Menu);
        p->RegisterControlTagParseFunc(_T("TabCtrl"), TabCtrl::UIParseTabCtrlTag);  // REGISTER_UI_OBJECT(p, TabCtrl);
		REGISTER_UI_OBJECT(p, CATEGORY_CONTROL, SliderCtrl);
		//p->RegisterControlTagParseFunc(_T("SliderCtrl"), SliderCtrl::UIParseMenuTag); // 
        REGISTER_UI_OBJECT(p, CATEGORY_CONTROL, TabCtrlButton);

        REGISTER_UI_RENDERBASE(p, ButtonBkThemeRender);
        REGISTER_UI_RENDERBASE(p, RadioButtonThemeRender);
        REGISTER_UI_RENDERBASE(p, CheckButtonThemeRender);
        REGISTER_UI_RENDERBASE(p, EditBkThemeRender);
        REGISTER_UI_RENDERBASE(p, GroupBoxBkThemeRender);
        REGISTER_UI_RENDERBASE(p, ProgressBarBkgndThemeRender);
        REGISTER_UI_RENDERBASE(p, ProgressBarForegndThemeRender);
        REGISTER_UI_RENDERBASE(p, SliderCtrlBkgndThemeRender);
        REGISTER_UI_RENDERBASE(p, SliderTrackButtonThemeRender);
        REGISTER_UI_RENDERBASE(p, HeaderCtrlItemBkgndThemeRender);
        REGISTER_UI_RENDERBASE(p, HeaderCtrlRightBlankThemeRender);
        REGISTER_UI_RENDERBASE(p, HScrollBarBkgndThemeRender);
        REGISTER_UI_RENDERBASE(p, VScrollBarBkgndThemeRender);
        REGISTER_UI_RENDERBASE(p, ScrollLineUpButtonBkThemeRender);
        REGISTER_UI_RENDERBASE(p, ScrollLineDownButtonBkThemeRender);
        REGISTER_UI_RENDERBASE(p, ScrollLineLeftButtonBkThemeRender);
        REGISTER_UI_RENDERBASE(p, ScrollLineRightButtonBkThemeRender);
        REGISTER_UI_RENDERBASE(p, HScrollThumbButtonThemeRender);
        REGISTER_UI_RENDERBASE(p, VScrollThumbButtonThemeRender);
        REGISTER_UI_RENDERBASE(p, ListboxBkThemeRender);
        REGISTER_UI_RENDERBASE(p, ComboboxBkThemeRender);
        REGISTER_UI_RENDERBASE(p, ComboboxButtonBkThemeRender);
        REGISTER_UI_RENDERBASE(p, MenuBkThemeRender);
        REGISTER_UI_RENDERBASE(p, MenuSeperatorThemeRender);
        REGISTER_UI_RENDERBASE(p, MenuStringItemRender);
        REGISTER_UI_RENDERBASE(p, MenuPopupTriangleRender);
        REGISTER_UI_RENDERBASE(p, MenuCheckedIconThemeRender);
        REGISTER_UI_RENDERBASE(p, MenuRadioIconThemeRender);
        REGISTER_UI_RENDERBASE(p, MenuRadioCheckIconBkThemeRender);
        REGISTER_UI_RENDERBASE(p, TabCtrlButtonBkThemeRender);
        REGISTER_UI_RENDERBASE(p, ListViewCtrlForeThemeRender);
        REGISTER_UI_RENDERBASE(p, TreeViewCtrlExpandCollapseIconThemeRender);
        REGISTER_UI_RENDERBASE(p, ListViewFocusRender);
        
        REGISTER_UI_TEXTRENDERBASE(p, MenuTextThemeRender);
        REGISTER_UI_TEXTRENDERBASE(p, ListViewThemeTextRender);

        return true;
    }

   
}

// 可以直接由IM_UI工程调用的导出接口
extern "C"
bool UICtrl_RegisterUIObject(IUIApplication* p)
{
	return RegisterUIObject(p);
}

extern "C"
bool UICreateTrayIcon(IUIApplication* pUIApplication, ITrayIcon** ppOut)
{
	if (NULL == ppOut)
		return false;

	TrayIcon* p = new TrayIcon(pUIApplication);
	*ppOut = static_cast<ITrayIcon*>(p);
	p->AddRef();

	return true;
}