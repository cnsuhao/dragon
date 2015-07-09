#pragma  once
#include "UISDK\Control\Inc\Interface\ilistbox.h"
#include "UISDK\Control\Src\Control\ScrollBar\vscrollbar_creator.h"


namespace UI
{
class ListBoxItem;

class ListBox : public MessageProxy
{
public:
	ListBox();
	~ListBox();

	UI_DECLARE_OBJECT(ListBox, OBJ_CONTROL|CONTROL_LISTBOX)

	UI_BEGIN_MSG_MAP
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
		UIMSG_WM_QUERYINTERFACE(QueryInterface)
		UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
		UIMSG_WM_INITIALIZE(OnInitialize)
	UIALT_MSG_MAP(UIALT_CALLLESS)
		UIMSG_WM_CREATEBYEDITOR(OnCreateByEditor)
	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ListBox, IListCtrlBase)

    void  SetIListBox(IListBox* p) 
	{ 
		m_pIListBox = p;
		SetIMessageProxy(static_cast<IMessage*>(p));
	}
    IListBox*  GetIListBox()
	{ 
		return m_pIListBox; 
	}

public:
    bool  SetSel(int nIndex);
    void  SetBindObject(IObject* pCombobox);

	IListBoxItem*  AddString(LPCTSTR szText, int nAddItemFlags=0);
	bool  RemoveItem(IListBoxItem* pItem, int nRemoveFlag=0);

protected:
    void  SetAttribute(IMapAttribute* pMapAttrib, bool bReload);
	void  OnInitialize();
	void  OnCreateByEditor(CREATEBYEDITORDATA* pData);

private:
    bool  IsPopup();

protected:
    IListBox*  m_pIListBox;
    IObject*  m_pBindObject;   // 例如从combobox弹出来的列表框，m_pBindObject将指向Combobox*

	VScrollbarCreator  m_vscrollbarCreator;
};
}