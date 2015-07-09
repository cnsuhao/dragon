// UIEditorCtrl.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "UISDK\Project\UIEditorCtrl\Src\ProjectTreeView\projecttreeview.h"
#include "UISDK\Project\UIEditorCtrl\Src\ProjectTreeView\controlitem\controltreeviewitem.h"
#include "UISDK\Project\UIEditorCtrl\Src\ToolBoxTreeView\toolboxtreeitem.h"
#include "UISDK\Project\UIEditorCtrl\Src\PropertyCtrl\propertyctrl.h"

extern "C" 
{
    __declspec(dllexport) bool RegisterUIObject(UI::IUIApplication* p)
    {
        REGISTER_UI_OBJECT(p, TEXT("EditorCtrl"), ProjectTreeView);
		REGISTER_UI_OBJECT(p, TEXT("EditorCtrl"), PropertyCtrl);
        return true;
    }

    // 可以直接由IM_UI工程调用的导出接口
    bool UIEditor_Ctrl_RegisterUIObject(UI::IUIApplication* p)
    {
        return RegisterUIObject(p);
    }

    UINT  GetToolBoxItemDragClipboardFormat()
    {
        return ToolBoxTreeItem::s_nLayoutItemDragCF;
    }

    UINT  GetProjectTreeControlDragClipboardFormat()
    {
        return ControlTreeViewItem::s_nControlDragCF;
    }
}
