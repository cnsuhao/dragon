#pragma once
#include "UISDK\Control\Inc\Interface\ilistview.h"
#include "UISDK\Control\Inc\Interface\iheaderctrl.h"

// ϵͳ�Դ���listview���ݽṹ��
// 1. �ؼ���һ��HDPA hdpaSubItems������������Ա������HDPA; ��ListView_GetSubItemDPA
// 2. ÿ����һ��HDPA������������Ա��ÿ��subitem������LISTSUBITEM;

//
// ��ȡĳһ�е�hdpa����
//
// #define ListView_GetSubItemDPA(plv, idpa) \
// 	((HDPA)DPA_GetPtr((plv)->hdpaSubItems, (idpa)))

// 
// ����item���ݶ���
//
// Report view sub-item structure
// typedef struct _LISTSUBITEM
// {
// 	LPTSTR pszText;
// 	short iImage;
// 	WORD state;
// } LISTSUBITEM, *PLISTSUBITEM;

//
// ��ȡ�������ݴ���
//
// PLISTSUBITEM plsiSrc = NULL;
// HDPA hdpa;
// hdpa = ListView_GetSubItemDPA(plv, iSubItem - 1);  // ��HDPA
// if (hdpa) {
// 	plsiSrc = DPA_GetPtr(hdpa, i);   // ����HDPA��ȡ��ָ���е�subitem���ݣ�LISTSUBITEM*
// }

//
// �����ؼ�HDPA
//
// if (!plv->hdpaSubItems)
// {
// 	plv->hdpaSubItems = DPA_CreateEx(8, plv->hheap);
// 	if (!plv->hdpaSubItems)
// 		return -1;
// }

//
//  ������ʱ�������е�HDPA
//
// idpa = DPA_InsertPtr(plv->hdpaSubItems, max(0, iCol - 1), NULL);
//

namespace UI
{

struct DrawSubItemData
{
    int               nColumnId;
    int               nColumnIndex;
    IRenderTarget*    pRenderTarget;
    ITextRenderBase*  pTextRender;
    IListItemBase*    pItem;
    const CRect*      prc;
    int               nRenderState;
};

struct SubItemInfo
{
    LPTSTR szText;  // ����û��String���������ʡЩ�ռ�

	void  Init()
	{
		szText = NULL;
	}

	void  Destroy()
	{
		if (this->szText)
		{
			free((void*)this->szText);
			this->szText = NULL;
		}
	}

	void  SetText(LPCTSTR sz)
	{
		if (!sz)
		{
			if (this->szText)
			{
				free(this->szText);
				this->szText = NULL;
			}
		}
		else
		{
			unsigned int cbSize = _tcslen(sz)+1;
			cbSize *= sizeof(TCHAR);

			if (this->szText)
				this->szText = (LPTSTR)realloc(this->szText, cbSize);
			else
				this->szText = (LPTSTR)malloc(cbSize);

			if (!this->szText)
				return;

			_tcscpy(this->szText, sz);
		}
	}
};

class ListViewItemBaseShareData : public MessageProxy
{
public:
    ListViewItemBaseShareData();
    ~ListViewItemBaseShareData();

    UI_BEGIN_MSG_MAP
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ListViewItemBaseShareData, IListItemTypeShareData)

    void  SetIListViewItemBaseShareData(IListViewItemBaseShareData* p)
    { m_pIListViewItemBaseShareData = p; SetIMessageProxy(static_cast<IMessage*>(p)); }

public:
    IListViewItemBaseShareData*  m_pIListViewItemBaseShareData;
    IHeaderListCtrl*  m_pHeaderCtrl;
};

class ListViewItemBase : public MessageProxy
{
public:
    ListViewItemBase();
    ~ListViewItemBase();	

    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
        UIMESSAGE_HANDLER_EX(UI_LVIM_SETSUBITEMTEXT, OnSetSubItemText)
        UIMESSAGE_HANDLER_EX(UI_LVIM_GETSUBITEMTEXT, OnGetSubItemText)
        UIMESSAGE_HANDLER_EX(UI_LVIM_ADDCOLUMN,      OnAddColumn)
        UIMESSAGE_HANDLER_EX(UI_LVIM_REMOVECOLUMN,   OnRemoveColumn)
        UIMESSAGE_HANDLER_EX(UI_LVIM_CLEARCOLUMN,    OnClearColumn)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ListViewItemBase, IListItemBase)

    UI_DECLARE_QUERY_INTERFACE(ListViewItemBase);

public:
    void  SetIListViewItemBase(IListViewItemBase* p) 
    { m_pIListViewItemBase = p; SetIMessageProxy(static_cast<IMessage*>(p)); }
	bool  SetSubItemText(int nSub, LPCTSTR szText);
	LPCTSTR  GetSubItemText(int nSub);

protected:
    void  OnInitialize();
    void  OnPaint(IRenderTarget* pRenderTarget);
    LRESULT  OnSetSubItemText(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnGetSubItemText(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnAddColumn(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnRemoveColumn(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnClearColumn(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    void  OnDrawSubItem(DrawSubItemData* pSubItem);
    void  DestroySubItems();

protected:
    IListViewItemBase*  m_pIListViewItemBase;
    ListViewItemBaseShareData*  m_pShareData;

    typedef vector<SubItemInfo>::iterator _MyIter;
    vector<SubItemInfo>  m_vecSubItem;
};

}
