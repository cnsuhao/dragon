#pragma  once
#include "Inc\Interface\ilistbox.h"
#include "..\ScrollBar\vscrollbar_creator.h"

/*
	listbox皮肤配置示例：

	  <ListBox id="listbox" border="1">
        <prop>
          <x key="layout" width="200" height="100" top="50" left="10"/>
          <x key="back.render" type="color" border="1" border.color="#000"/>
          <x key="fore.render" type="colorlist" color=";;;;#03B;#03B;#03B;;"/>
          <x key="text.render" type="colorlist" color=";;;;#FFF;#FFF;#FFF;;"/>
        </prop>

        <VScrollBar ncchild="1" id="__vscrollbar" width="11">
          <Button id="__scrollbar_thumb" back.render.type="color" back.render.color="#eee"/>
        </VScrollBar>
      </ListBox>
*/
namespace UI
{
class ListBoxItem;

class ListBox : public MessageProxy
{
public:
	ListBox(IListBox* p);
	~ListBox();

	UI_BEGIN_MSG_MAP()
        UIMSG_SERIALIZE(OnSerialize)
		UIMSG_QUERYINTERFACE(ListBox)
		UIMSG_INITIALIZE(OnInitialize)
	UIALT_MSG_MAP(UIALT_CALLLESS)
		UIMSG_CREATEBYEDITOR(OnCreateByEditor)
	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ListBox, IListCtrlBase)

    IListBox*  GetIListBox()
	{ 
		return m_pIListBox; 
	}

public:
    bool  SetSel(int nIndex);
    int   GetSel();
    void  SetBindObject(IObject* pCombobox);

	IListBoxItem*  AddString(LPCTSTR szText);
	bool  RemoveItem(IListBoxItem* pItem);

protected:
	void  OnSerialize(SERIALIZEDATA* pData);
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