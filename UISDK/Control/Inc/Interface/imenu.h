#ifndef IMENU_H_31481C5E_BC0B_4872_9C54_FF8D06736CFB
#define IMENU_H_31481C5E_BC0B_4872_9C54_FF8D06736CFB
#include "UISDK\Kernel\Inc\Interface\ilistctrlbase.h"
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"

namespace UI
{

    //	点击菜单项
//		message: UI_WM_NOTIFY
//		code:    UI_MENU_CLICK 
//      wparam:  
//      lparam: 
//
#define UI_MENU_CLICK 132331849
    
// void OnMenuClick(UINT nMenuID);
#define UIMSG_MENU_CLICK(func)                        \
    if (uMsg == UI_WM_NOTIFY  &&                      \
        code == UI_MENU_CLICK )                       \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((UINT)wParam);                           \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

#define MENU_ITEM_SEPARATOR_ID  (-1)

// 获取菜单图标区域的宽度
// Return: long lWidth
#define UI_WM_MENU_GETICONGUTTERWIDTH     136111802

// 获取菜单右侧弹出箭头的宽度
// Return: long lWidth
#define UI_WM_MENU_GETPOPUPTRIANGLEWIDTH  136111803

#define UI_LISTITEM_TYPE_MENU_STRING      136111859
#define UI_LISTITEM_TYPE_MENU_SEPARATOR   136111900
#define UI_LISTITEM_TYPE_MENU_POPUP       136111901

enum TRACK_POPUP_MENU_FLAG
{
    UI_TPM_RETURNCMD = TPM_RETURNCMD,
    UI_TPM_CENTERALIGN = TPM_CENTERALIGN,
    UI_TPM_RIGHTALIGN = TPM_RIGHTALIGN,
    UI_TPM_VCENTERALIGN = TPM_VCENTERALIGN,
    UI_TPM_BOTTOMALIGN = TPM_BOTTOMALIGN,

    UI_TPM_SHOWACTIVATE = 0x80000000,  // 抢焦点，用于支持跨进程的CEF右击弹出菜单 
}; 

class MenuItemBase;
class MenuItem;
class Menu;
interface IMenu;
interface IWindow;

//////////////////////////////////////////////////////////////////////////
class MenuSeparatorItemShareData;
interface IMenuSeparatorItemShareData : public IListItemTypeShareData
{
    UI_DECLARE_Ixxx_INTERFACE(IMenuSeparatorItemShareData, MenuSeparatorItemShareData);
};
class MenuSeparatorItem;
interface IMenuSeparatorItem : public IListItemBase
{
    UI_DECLARE_Ixxx_INTERFACE(IMenuSeparatorItem, MenuSeparatorItem);
};

//////////////////////////////////////////////////////////////////////////
class MenuStringItemShareData;
interface IMenuStringItemShareData : public IListItemTypeShareData
{
    UI_DECLARE_Ixxx_INTERFACE(IMenuStringItemShareData, MenuStringItemShareData);
};

class MenuStringItem;
interface IMenuStringItem : public IListItemBase
{
    UI_DECLARE_Ixxx_INTERFACE(IMenuStringItem, MenuStringItem);
};
//////////////////////////////////////////////////////////////////////////
class MenuPopupItemShareData;
interface IMenuPopupItemShareData : public IListItemTypeShareData
{
    UI_DECLARE_Ixxx_INTERFACE(IMenuPopupItemShareData, MenuPopupItemShareData);
};
class MenuPopupItem;
interface IMenuPopupItem : public IListItemBase
{
    UI_DECLARE_Ixxx_INTERFACE(IMenuPopupItem, MenuPopupItem);
};
//////////////////////////////////////////////////////////////////////////

interface UICTRLAPI IMenu : public IListCtrlBase
{
    UI_DECLARE_Ixxx_INTERFACE(IMenu, Menu)

    void  OnNewChildElement(IUIElement* pUIElement);
    int   TrackPopupMenu(UINT nFlag, int x, int y, IMessage* pNotifyObj, HWND hWndClickFrom = NULL, RECT* prcClickFrom = NULL);
    IWindow*  CreateMenuWindow();
 
    IMenuStringItem*  AppendString(LPCTSTR szText, UINT nId);
    IMenuSeparatorItem*  AppendSeparator(UINT nId);
    IMenuPopupItem*  AppendPopup(LPCTSTR szText, UINT nId, IMenu* pSubMenu);

    IMenu*  GetSubMenuByPos(int nPos);
    IMenu*  GetSubMenuById(long lId);

	IWindow*  GetPopupWindow();
	int   GetIconGutterWidth();
};


struct LoadMenuData
{
	ISkinRes*  pSkinRes;
	LPCTSTR  szWndId;
	LPCTSTR  szMenuId;	
    HWND  hWndParent;
};
struct DefaultMenuData
{
	TCHAR  szDefaultWndId[32];
	TCHAR  szDefaultMenuId[32];
};
}

extern "C" UICTRLAPI void  UISetDefaultMenuData(UI::DefaultMenuData* pData);
extern "C" UICTRLAPI UI::IMenu*  UILoadMenu(UI::LoadMenuData* pData);
extern "C" UICTRLAPI bool  UIDestroyMenu(UI::IMenu* pMenu);

#endif // IMENU_H_31481C5E_BC0B_4872_9C54_FF8D06736CFB