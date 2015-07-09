#ifndef ILISTITEMBASE_H_5825D731_6107_4ced_A0C5_FA6097119404
#define ILISTITEMBASE_H_5825D731_6107_4ced_A0C5_FA6097119404
#include "UISDK\Kernel\Inc\Interface\imessage.h"

namespace UI
{
interface IPanel;
class ListItemBase;
interface IListCtrlBase;
class RenderContext;

//////////////////////////////////////////////////////////////////////////
// ListItem��������Ϣ

// �ڻ���֮ǰ�����ӳٲ���
#define UI_LISTITEM_MSG_DELAY_OP  139222216

// ��ȡitem��Ⱦ״̬�����ڻ����Լ�
#define UI_LISTITEM_MSG_GETRENDERSTATE 147201333

// �б�ؼ����б������ק�ͷ�ʱ��״̬
// wParam: ״̬DROPTARGETEVENT_TYPE
#define  UI_LISTITEM_MSG_DRAGDROP_EVENT  141431104

enum LISTITEM_DELAY_OP
{
    // �ڿؼ��Ĵ�С�����ı��֪ͨ�Ӷ�������Ӷ���Ŀǰû�д��ڿ����������Ժ����Լ��ɼ�ʱ�ٸ����Լ��Ĳ���
    DELAY_OP_SIZE_CHANGED = 0x01,

    // �ڿؼ��������¸ı�֮��֪ͨ�Ӷ����Ӷ������Լ�ˢ�µ�ʱ���ٸ����Լ��Ĳ���
    DELAY_OP_LAYOUT_CHANGED = 0x02,

    // �״λ��ƣ��������ͷ�����ݡ�������
    DELAY_OP_LAZYLOADDATA = 0x04,
};

// lbuttondown_return_value
enum ITEMMOUSEKEYEVENTRET
{
    IMKER_NONE = 0,    
    IMKER_HANDLED_BY_ITEM = 0x0001,         //  �Ѵ���
    IMKER_HANDLED_BY_INNERCONTROL = 0x0002, //  �Ѵ���
    IMKER_DONT_CHANGE_SELECTION = 0x0004,   //  ��Ҫ�ı䵱ǰ��ѡ����
};

// Style
struct ListItemStyle
{
    bool  bNotSelectable:1;  // ���ܱ�ѡ�С�����ָ���
    bool  bNotFocusable:1;   // �����н��㡣
    bool  bNoChildIndent:1;  // �ý�㲻��������
    bool  bFloat:1;          // ��ITEM���ڸ��������ܹ���ƫ����Ӱ�죩���縡����ͷ
    bool  bOwnerDraw:1;      // �Ƿ�item�Ļ�����Ϣ�ȷ���ListCtrl; ControlStyle�е�ownerdraw��ʾ���͸��ⲿ����

    // Not Implement: 
    // bool  bNotArrage:1;      // �������б��֡�����ĸ���Ԫ��
};

interface UISDKAPI IListItemBase : public IMessage
{
    UI_DECLARE_Ixxx_INTERFACE(IListItemBase, ListItemBase);    

    void  SetIListCtrlBase(IListCtrlBase* p);
    IListCtrlBase*  GetIListCtrlBase();

    IListItemBase*  GetNextItem();
    IListItemBase*  GetPrevItem();
    IListItemBase*  GetParentItem();
    IListItemBase*  GetChildItem();
    IListItemBase*  GetLastChildItem();
    IListItemBase*  GetPrevSelection();
    IListItemBase*  GetNextSelection();

    IListItemBase*  GetNextVisibleItem();
    IListItemBase*  GetPrevVisibleItem();
    IListItemBase*  GetNextTreeItem(); 
    IListItemBase*  GetPrevTreeItem();

    bool  IsMyChildItem(IListItemBase* pChild, bool bTestdescendant);
    bool  HasChild();

    void  SetNextItem(IListItemBase* p);
    void  SetPrevItem(IListItemBase* p);
    void  SetParentItem(IListItemBase* p);
    void  SetChildItem(IListItemBase* p);
    void  SetLastChildItem(IListItemBase* p);
    void  SetNextSelection(IListItemBase* p);
    void  SetPrevSelection(IListItemBase* p);
	void  AddChild(IListItemBase* p);
	void  AddChildFront(IListItemBase* p);
    void  RemoveMeInTheTree();

    const CRect* GetParentRectPtr();
    void  GetParentRect(CRect* prc);
    void  SetParentRect(CRect* prc);

    long  GetId();
    void  SetId(long n);
    int   GetItemType();
    void  SetItemType(long);
    UINT  GetItemFlag();
    void  SetItemFlag(int n);
    void  SetText(const TCHAR* szText);
    const TCHAR*  GetText();
    void  SetToolTip(const TCHAR* szText);
    const TCHAR*  GetToolTip();
    LPARAM  GetData();
    void  SetData(LPARAM p);
    LPARAM  GetCtrlData();
    void  SetCtrlData(LPARAM p);
    int   GetLineIndex() ;
    void  SetLineIndex(int n);
    void  SetTreeIndex(int n);
    void  SetNeighbourIndex(int n);
    int   GetTreeIndex();
    int   GetNeighbourIndex();
    int   CalcDepth();
    void  SetConfigWidth(int n);
    void  SetConfigHeight(int n);
    int   GetConfigWidth();
    int   GetConfigHeight();
    long  GetSort();
    void  SetSort(long);

    void  GetDesiredSize(SIZE* pSize);

    IRenderBase*  GetIconRender();
    void  SetIconRender(IRenderBase* p);
    void  SetIconRenderType(RENDER_TYPE eType);
    void  SetIconFromFile(const TCHAR* szIconPath);
    void  SetIconFromImageId(const TCHAR* szImageId);

    UINT  GetItemState();
    UINT  GetItemDelayOp();
    void  AddItemDelayOp(int n);
    void  RemoveDelayOp(int n);
    void  ClearDelayOp();
    bool  IsMySelfVisible();
    bool  IsVisible();
    bool  IsDisable();
    bool  IsHover();
    bool  IsPress();
    bool  IsFocus();
    bool  IsCollapsed();
    bool  IsExpand();
    bool  IsSelected();
    bool  IsSelectable();
    bool  IsChecked();
    bool  IsRadioChecked();
    bool  IsDragDropHover();

    void  SetPress(bool b, bool bNotify=true);
    void  SetHover(bool b, bool bNotify=true);
    void  SetExpand(bool b, bool bNotify=true);
    void  SetFocus(bool b, bool bNotify=true);
    void  SetSelected(bool b, bool bNotify=true);
    void  SetDisable(bool b, bool bNotify=true);
    void  SetChecked(bool b, bool bNotify=true);
    void  SetRadioChecked(bool b, bool bNotify=true);
    void  SetDragDropHover(bool b, bool bNotify=true);
    void  SetSelectable(bool b);

    IPanel*  GetRootPanel();
    bool  CreateRootPanel();
    void  DrawItemInnerControl(IRenderTarget* pRenderTarget, RenderContext* prenderContext);
    int   GetItemRenderState();

};

}

#endif // ILISTITEMBASE_H_5825D731_6107_4ced_A0C5_FA6097119404