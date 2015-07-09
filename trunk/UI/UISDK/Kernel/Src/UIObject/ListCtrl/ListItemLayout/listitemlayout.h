#ifndef LISTITEMLAYOUT_H_6F45FF31_A2CF_4c4d_B411_899A5CC12D61
#define LISTITEMLAYOUT_H_6F45FF31_A2CF_4c4d_B411_899A5CC12D61
#include "UISDK\Kernel\Inc\Interface\ilistctrlbase.h"

namespace UI
{
interface IListItemBase;
interface IListCtrlBase;
class ListCtrlBase;

enum LISTCTRL_ITEM_LAYOUT_TYPE
{
    // 1. ��ͨϵͳlistbox. [�̶��и�]
    //    ��������  ����NONE������AUTO
    //    ��  �֣�  ����ռ��client width��������������
    //    AUTOSIZE: ����0(��֧��AUTO), ����total height
    LISTCTRL_ITEM_LAYOUT_TYPE_1,

    // 2. ��ͨϵͳlistbox��֧�ֺ������. [�̶��и�]
    //    ��������  ����AUTO������AUTO
    //    ��  �֣�  ����ȡmax width������client width��������������
    //    AUTOSIZE: ����max width, ����total height
    LISTCTRL_ITEM_LAYOUT_TYPE_2,
};


class  ListCtrlLayoutMgr
{
public:
    ListCtrlLayoutMgr();
    ~ListCtrlLayoutMgr();
    
    void  SetListCtrlBase(ListCtrlBase* pListCtrlBase);

    IListCtrlLayout*  GetLayout();
    void  SetLayout(IListCtrlLayout* p);

    bool  Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent);
    void  Measure(SIZE* pSize);

private:
    ListCtrlBase*  m_pListCtrl;
    IListCtrlLayout*  m_pLayout;
};


class ListCtrlItemSimpleLayout : public IListCtrlLayout
{
public:
    ListCtrlItemSimpleLayout();

    virtual void  SetIListCtrlBase(IListCtrlBase* p) { m_pIListCtrlBase = p; }
    virtual void  Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent);
    virtual void  Measure(SIZE* pSize);
    virtual void  Release() { delete this; }

protected:
    IListCtrlBase*  m_pIListCtrlBase;
};

class ListCtrlItemVariableHeightLayout : public IListCtrlLayout
{
public:
    ListCtrlItemVariableHeightLayout();

    virtual void  SetIListCtrlBase(IListCtrlBase* p) { m_pIListCtrlBase = p; }
    virtual void  Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent);
    virtual void  Measure(SIZE* pSize);
    virtual void  Release() { delete this; }

protected:
    IListCtrlBase*  m_pIListCtrlBase;
};


// TAB���֣�����ƽ���Ų�
class ListCtrlItemHorzAverageLayout : public IListCtrlLayout
{
    virtual void  SetIListCtrlBase(IListCtrlBase* p) { m_pIListCtrlBase = p; }
    virtual void  Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent);
    virtual void  Measure(SIZE* pSize){};
    virtual void  Release() { delete this; }

protected:
    IListCtrlBase*  m_pIListCtrlBase;
};


#define UI_MSGMAPID_LC_LAYOUT 142901215

// ���б�ؼ���ȡ�м��
#define UI_LCM_LAYOUT_GET_LINESPACE  142901216
// ���б�ؼ���ȡitem���
#define UI_LCM_LAYOUT_GET_ITEMSPACE  142901217

// �ȸߣ���ʽ���֣�һ���������� ��ֻ�������������
class ListCtrlItemFixHeightFlowLayout : public IListCtrlLayout
{
    virtual void  SetIListCtrlBase(IListCtrlBase* p) { m_pIListCtrlBase = p; }
    virtual void  Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent);
    virtual void  Measure(SIZE* pSize);
    virtual void  Release() { delete this; }

protected:
    IListCtrlBase*  m_pIListCtrlBase;
};


IListCtrlLayout*  CreateListCtrlLayout(int nType, IListCtrlBase* p);
}

#endif // LISTITEMLAYOUT_H_6F45FF31_A2CF_4c4d_B411_899A5CC12D61