#pragma once
#include "UISDK\Kernel\Inc\Interface\ilistctrlbase.h"
#include "UISDK\Kernel\Src\UIObject\Control\control.h"
#include "UISDK\Kernel\Src\UIObject\ScrollBar\scrollbarmanager.h"
#include "ListItemLayout\listitemlayout.h"
#include "MouseKeyboard\listctrlmkmgr.h"
#include "float/float_item_mgr.h"

namespace UI
{
class ListCtrlBase;
interface IListItemBase;
enum LISTCTRL_NEED_UPDATE_FLAG
{
	LISTCTRL_NEED_UPDATE_FLAG_NONE = 0,  // ����Ҫˢ��
	LISTCTRL_NEED_UPDATE_FLAG_ITEM = 1,  // ����Ҫˢ��item��
	LISTCTRL_NEED_UPDATE_FLAG_ALL  = 2   // ��Ҫˢ�������ؼ�����������ˣ�
};
#define LISTCTRL_MAX_INVALIDATE_ITEM_COUNT 3

//////////////////////////////////////////////////////////////////////////
// �ڲ���Ϣ

// �ӳ�ɾ��
// message: UI_WM_NOTIFY
// code:    UI_LCN_INNER_DELAY_REMOVE_ITEM  
// wparam:  ListCtrlBase*
// lParam:  int nFlags
#define UI_LCN_INNER_DELAY_REMOVE_ITEM  138072146

class ListItemTypeShareData : public Message
{
public:
    ListItemTypeShareData();
    ~ListItemTypeShareData();

    UI_BEGIN_MSG_MAP
    UI_END_MSG_MAP

    void  SetIListItemTypeShareData(IListItemTypeShareData* p) { m_pIListItemTypeShareData = p; }
    void  SetListCtrlBase(ListCtrlBase* p);
    IListCtrlBase*  GetListCtrlBase();

private:
    IListItemTypeShareData*  m_pIListItemTypeShareData;
    ListCtrlBase*  m_pListCtrlBase;
};

// �б�ؼ�����
class  ListCtrlBase : public Control
{
public:
	ListCtrlBase();
	~ListCtrlBase();

	UI_BEGIN_MSG_MAP
        UIMSG_WM_KEYDOWN(OnKeyDown)
        if (m_pMKMgr)
        {
            if (m_pMKMgr->ProcessMessage(pMsg))
            {
                return TRUE;
            }
        }
        UIMESSAGE_HANDLER_EX(UI_LCM_HITTEST, OnHitTest)
		UIMSG_WM_PAINT2(OnPaint)
        UIMSG_WM_POSTPAINT(OnPostPaint)
        UIMESSAGE_HANDLER_EX(UI_WM_INERTIAVSCROLL, OnInertiaVScroll)
		UIMSG_WM_STATECHANGED2(OnStateChanged)
		UIMSG_WM_SIZE(OnSize) 
        UIMESSAGE_HANDLER_EX(UI_WM_GET_MOUSEKEYBOARD_MANAGER, OnGetMouseKeyboardMgr)
		UIMSG_WM_VSCROLL(OnVScroll)
        UIMSG_WM_HSCROLL(OnHScroll)
        UIMSG_WM_MOUSEWHEEL(OnMouseWheel)
		UIMESSAGE_HANDLER_EX(UI_WM_GESTURE_PAN, OnGesturePan)
		UICHAIN_MSG_MAP_MEMBER(m_mgrScrollBar)
        UIMSG_WM_NOTIFY(UI_LCN_INNER_DELAY_REMOVE_ITEM, OnDelayRemoveItem)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
		UIMSG_WM_SERIALIZE(OnSerialize)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)            
        UIMSG_WM_FINALRELEASE(FinalRelease)
	UIALT_MSG_MAP(UIALT_CALLLESS)
		UIMSG_WM_CREATEBYEDITOR(OnCreateByEditor)
	UI_END_MSG_MAP_CHAIN_PARENT(Control)

    void  SetIListCtrlBase(IListCtrlBase* p);
    IListCtrlBase*  GetIListCtrlBase();

protected:
	// ��Ϣ����
    HRESULT  FinalConstruct(IUIApplication* p);
    void  FinalRelease();
	void  OnPaint(IRenderTarget* pRenderTarget, RenderContext* proc);
    void  OnPostPaint(IRenderTarget* pRenderTarget);
	void  OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
	void  OnSize( UINT nType, int cx, int cy );
	void  OnStateChanged(UINT nMask);
	void  OnVScroll(int nSBCode, int nPos, IMessage* pMsgFrom);
    void  OnHScroll(int nSBCode, int nPos, IMessage* pMsgFrom);
    LRESULT  OnInertiaVScroll(
            UINT uMsg, WPARAM wParam, LPARAM lParam);
	void  GetDesiredSize(SIZE* pSize);
	void  OnSerialize(SERIALIZEDATA* pData);
    LRESULT  OnHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam);
    BOOL  OnMouseWheel(UINT nFlags, short zDelta, POINT pt);
    HANDLED_VALUE  OnDrawItem(
            IRenderTarget* pRenderTarget, ListItemBase* p);
    LRESULT  OnGetMouseKeyboardMgr(
            UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnDelayRemoveItem(WPARAM w, LPARAM l);
    LRESULT  OnGesturePan(UINT, WPARAM, LPARAM);
	void  OnCreateByEditor(CREATEBYEDITORDATA* pData);

public:
	// ���ýӿ�
    bool  AddItem(ListItemBase* pItem, int nAddItemFlags=0);
    bool  InsertItem(
            ListItemBase* pItem, 
            UINT nPos, 
            int nAddItemFlags=0);
    bool  InsertItem(
            ListItemBase* pItem, 
            ListItemBase* pInsertAfter, 
            int nAddItemFlags=0);
    bool  InsertItem(
            ListItemBase* pItem, 
            IListItemBase* pParent = UITVI_ROOT, 
            IListItemBase* InsertAfter = UITVI_LAST, 
            int nInsertFlags=LF_NONE);
    void  RemoveItem(ListItemBase* pItem, int nRemoveFlag=0);
	void  RemoveItem(int nIndex, int nRemoveFlag);
    void  RemoveAllChildItems(ListItemBase* pParent, int nRemoveFlag);
	void  RemoveAllItem(int nRemoveFlag);
    void  DelayRemoveItem(ListItemBase* pItem, int nRemoveFlag = 0);
    void  ItemDestructed(ListItemBase* pItem);

	void  SetSortCompareProc(ListItemCompareProc p);
    void  Sort();
    void  SortChildren(ListItemBase* pParent);
    void  SwapItemPos(ListItemBase*  p1, ListItemBase* p2);
    void  RemoveItemFromTree(ListItemBase*  p);
    bool  MoveItem(
            ListItemBase*  p, 
            IListItemBase* pNewParent, 
            IListItemBase* pInsertAfter, 
            long lFlags);

	int   GetItemCount() { return m_nItemCount; }
    int   GetVisibleItemCount();
    int   GetRootItemCount();
	LPCTSTR   GetItemText(ListItemBase* pItem);
	
	ListItemBase*  GetItemByPos(UINT i, bool bVisibleOnly=true);
    ListItemBase*  GetItemByWindowPoint(POINT pt);
    ListItemBase*  GetItemUnderCursor();
    ListItemBase*  GetItemById(long lId);
    ListItemBase*  FindItemByText(
            LPCTSTR  szText, 
            ListItemBase* pStart=NULL);
    ListItemBase*  FindChildItemByText(
            LPCTSTR  szText, 
            ListItemBase* pParent=NULL, 
            ListItemBase* pStart=NULL);
	ListItemBase*  GetFirstItem() { return m_pFirstItem; }
	ListItemBase*  GetLastItem() { return m_pLastItem; }
	ListItemBase*  GetFirstDrawItem();
	ListItemBase*  GetLastDrawItem();
    ListItemBase*  FindVisibleItemFrom(ListItemBase* pFindFrom=NULL);
    ListItemBase*  FindSelectableItemFrom(
            ListItemBase* pFindFrom=NULL);
    ListItemBase*  FindFocusableItemFrom(
            ListItemBase* pFindFrom=NULL);
    ListItemBase*  EnumItemByProc(
            ListItemEnumProc pProc, 
            ListItemBase* pEnumFrom = NULL, 
            WPARAM w = 0, 
            LPARAM l = 0);
    void  ItemIdChanged(
            IListItemBase* pItem, 
            long lOldId, 
            long lNewId);

	void  SetLayout(IListCtrlLayout* pLayout);
    void  SetLayoutFixedHeight();
    void  SetLayoutVariableHeight();
    IListCtrlLayout*  GetLayout();

	void  SetMinWidth(int n)  { m_sizeMin.cx = n; }
	void  SetMinHeight(int n) { m_sizeMin.cy = n; }
	void  SetMaxWidth(int n)  { m_sizeMax.cx = n; }
	void  SetMaxHeight(int n) { m_sizeMax.cy = n; }
	int   GetMinWidth()  { return m_sizeMin.cx; }
	int   GetMinHeight() { return m_sizeMin.cy; }
	int   GetMaxWidth()  { return m_sizeMax.cx; }
	int   GetMaxHeight() { return m_sizeMax.cy; }
	SIZE  GetAdaptWidthHeight(int nWidth, int nHeight);

	short  GetVertSpacing() { return m_nVertSpacing; }
	short  GetHorzSpacing() { return m_nHorzSpacing; }
	int   GetItemHeight() { return m_nItemHeight;}
	void  SetItemHeight(int nHeight, bool bUpdate=true);
    int   GetChildNodeIndent();
    void  SetChildNodeIndent(int n);
	void  GetItemContentPadding(REGION4* prc) {
            ::CopyRect(prc, &m_rItemContentPadding); }
	void  SetItemContentPadding(REGION4* prc) { 
            ::CopyRect(&m_rItemContentPadding, prc); }

    void  CalcFirstLastDrawItem();
    void  SetCalcFirstLastDrawItemFlag();

	void  LayoutAllItemsAndRedraw();
	void  LayoutItem(ListItemBase* pStart, bool bRedraw);
    void  UpdateItemIndex(ListItemBase* pStart);
    IScrollBarManager*  GetIScrollBarMgr();
    FloatItemMgr&  GetFloatItemMgr() { return m_MgrFloatItem; }

    void  ToggleItemExpand(ListItemBase* pItem, bool bUpdate);
    void  CollapseItem(ListItemBase* pItem, bool bUpdate);
    void  ExpandItem(ListItemBase* pItem, bool bUpdate);
    void  CollapseAll(bool bUpdate);
    void  ExpandAll(bool bUpdate);

	// InvalidateItem ����
	void  InvalidateItem(ListItemBase* pItem);
    void  RemoveInvavlidateItem(ListItemBase* pItem);
	void  SetInvalidateAllItems();
	void  ClearInvalidateItems();
	int   GetInvalidateItemCount();
	void  Refresh();
	void  RedrawItemByInnerCtrl(
            IRenderTarget* pRenderTarget, 
            RenderContext* pContext, 
            ListItemBase* pItem);
    void  MakeItemVisible(ListItemBase* pItem, bool* pbNeedUpdate);

    IRenderBase*  GetFocusRender() { return m_pFocusRender; }
    void  SetFocusRender(IRenderBase* p);

	// select ����
    void  SelectItem(
            ListItemBase* pItem, 
            bool bUpdate, 
            bool bNotify=true, 
            bool bMakeVisible=true);
    void  AddSelectItem(ListItemBase* pItem, bool bNotify);
	void  RemoveSelectItem(ListItemBase* pItem, bool bNotify);
	void  ClearSelectItem(bool bNotify);
	bool  IsSelected(ListItemBase* pItem);
	ListItemBase*  GetFirstSelectItem() { 
            return m_pFirstSelectedItem; }
	ListItemBase*  GetLastSelectItem();
    UINT  GetSelectedItemCount();
    bool  IsSelectMulti();
    void  SelectAll(bool bUpdate);

    void  SetHoverItem(ListItemBase* pItem);
    void  SetPressItem(ListItemBase* pItem);
	ListItemBase*  GetHoverItem();
	ListItemBase*  GetPressItem();
	ListItemBase*  GetFocusItem();
	void  SetFocusItem(ListItemBase* pItem);
    Object*  GetHoverObject();
    Object*  GetPressObject();
    void  SetFocusObject(Object* pObj);
    Object*  GetFocusObject();
//	void  CancelEdit();

    // item share data op
    IListItemTypeShareData*  GetItemTypeShareData(int lType);
    void  SetItemTypeShareData(
            int lType, 
            IListItemTypeShareData* pData);
    void  RemoveItemTypeShareData(int lType);
    
    void  ObjectRect2WindowRect(RECT* prcObj, RECT* prcWindow);
    void  ItemRect2ObjectRect(RECT* prcItem, RECT* prcObject);
    void  ItemRect2WindowRect(LPCRECT prc, LPRECT prcRet);
    void  WindowPoint2ItemPoint(
            ListItemBase* pItem, 
            const POINT* pt, 
            POINT* ptRet);
    BOOL  ProcessItemMKMessage(UIMSG* pMsg, ListItemBase* pItem);

    void  FireSelectItemChanged(ListItemBase* pOldValue);
    void  ListItemDragDropEvent(
            UI::DROPTARGETEVENT_TYPE eEvent, 
            IListItemBase* pItem);
    void  ListCtrlDragScroll();

    void  Scroll2Y(int nY, bool bUpdate);
    void  Scroll2X(int nX, bool bUpdate);
    void  ScrollY(int nY, bool bUpdate);
    void  ScrollX(int nX, bool bUpdate);
    void  SetScrollPos(int nX, int nY, bool bUpdate);

	void  ModifyListCtrlStyle(
            ListCtrlStyle* add, 
            ListCtrlStyle* remove);
	bool  TestListCtrlStyle(ListCtrlStyle* test);

protected:
    bool  _InsertItemToTree(
            ListItemBase*  p, 
            IListItemBase* pParent, 
            IListItemBase* pAfter);
	bool  _InsertItem(
            ListItemBase* pItem, 
            ListItemBase* pInsertAfter);
    bool  _InsertLastChild(
            ListItemBase* pNewItem, 
            ListItemBase* pParent);
    bool  _InsertFirstChild(
            ListItemBase* pNewItem, 
            ListItemBase* pParent);
    bool  _InsertBefore(
            ListItemBase* pNewItem, 
            ListItemBase* pInsertFront);
    bool  _InsertAfter(
            ListItemBase* pNewItem, 
            ListItemBase* pInsertAfter);
    bool  _InsertRoot(ListItemBase* pNewItem);

	bool  _RemoveItem(ListItemBase* pItem);
	bool  _RemoveAllItem();
    bool  _RemoveAllChildItems(ListItemBase* pParent);

    bool  IsItemVisibleInScreen(ListItemBase* pItem);
    bool  IsItemVisibleInScreenEx(
            ListItemBase* pItem, 
            /*LISTITEM_VISIBLE_POS_TYPE*/long& ePos);

    bool  is_my_item(IListItemBase* pItem);
    ListItemBase*  get_valid_list_item(IListItemBase* pItem);

    // ���ṩ���ⲿ���ã�Ҫˢ��һ��ITEMֻ�ܵ���InvalidateItem+Refresh
    // ��Ϊ���뽫��Чitem����������������OnPaint�е��á��������̣�
    void  redraw_item(ListItemBase** ppItemArray, int nCount);
    void  sort_by_first_item(
            ListItemBase* pFirstItem, 
            int nNeighbourCount, 
            bool bSortChildren);
	void  arrange_item(ListItemBase* pFrom, __out SIZE* pContent);
    void  update_mouse_mgr_type();

protected:
    IListCtrlBase*  m_pIListCtrlBase;
    
    friend class  ListCtrlMouseMgr;
    ListCtrlMKMgrBase*    m_pMKMgr;

    ListCtrlLayoutMgr     m_MgrLayout;      // ���ֶ���
    ScrollBarManager      m_mgrScrollBar;   // ������
    FloatItemMgr          m_MgrFloatItem;   // ����Ԫ��

	// ���ݵ���
	ListItemBase*  m_pFirstItem;
	ListItemBase*  m_pLastItem;   // ���Ϳؼ���lastitemָlastrootitem
	ListItemBase*  m_pFirstDrawItem;   
	ListItemBase*  m_pLastDrawItem;   
	ListItemBase*  m_pFirstSelectedItem;
    ListItemBase*  m_pEditingItem; // ���ڱ༭�Ķ���

    // ��Ҫʹ��win32edit������ʹ��uiedit��win32edit������ȱ�ݣ�
    // 1. ����ʱҪ�����㣬ˢ��ʱ�򲻺ÿ���
    // 2. ���οؼ����޷�ʵ��Բ�Ǳ�����
    // 3. popup����ʽ���ڣ����������layered�����ϣ��ᵼ����˸
    // TODO: IEdit*  m_pEdit;  // �༭�ؼ�

    // ����
	ListCtrlStyle  m_listctrlStyle;

    // �б�������ݼ�ࡣ��������ʵ���ı���������
    CRegion4  m_rItemContentPadding;          

    short  m_nVertSpacing;                   // �м��
    short  m_nHorzSpacing;                   // ������
    int    m_nChildNodeIndent;               // �ӽ������
    UINT   m_nItemCount;                     // �б�������
    long   m_nItemHeight;                    // �б���ĸ߶�

    // auto size�б�����С�ߴ磬����nonclient region��
    // ������ָcontent size�����ǿؼ���С
    SIZE   m_sizeMin;                        

    // auto size�б������ߴ硣������������ width/height
    // ����ʱ��m_sizeMin/m_sizeMax������Ч��
    SIZE   m_sizeMax;                        

    // ��ʶ���¼�����Ҫ���ƵĶ���
    bool   m_bNeedCalcFirstLastDrawItem;  

    // ����ˢ����Ч�������OnPaint���ж�ˢ����Щ
    bool   m_bRedrawInvalidItems;            

    // �ؼ�����ˢ�¡��ڼ䲻�����ٴ���Refresh������
    // ��ֹͬһ��ջ���ж��ˢ�²���������ˢ��״̬����
    byte   m_bPaintingCtrlRef;               

    // ������
    ListItemCompareProc   m_pCompareProc;    

    // item��չ��Ϊÿһ�����͵�item�ṩһ�����湲�����ݵķ���
    map<int, IListItemTypeShareData*>  m_mapItemTypeShareData;  

    // ���ڿ��ٶ�λ
    typedef  map<long, IListItemBase*>::iterator _mapItemIter;
    map<long, IListItemBase*>  m_mapItem;

    // ����
	LISTCTRL_NEED_UPDATE_FLAG  m_eInvalidateFlag;
    // ��Ҫˢ�µ�ITEM
	ListItemBase*  m_pInvalidateItems
            [LISTCTRL_MAX_INVALIDATE_ITEM_COUNT];  

    IRenderBase*   m_pFocusRender;
};
}