#ifndef IPROPERTYCTRL_H_2D8F8B87_E8DC_4995_82DF_F9D6155B970E
#define IPROPERTYCTRL_H_2D8F8B87_E8DC_4995_82DF_F9D6155B970E
#include "UISDK\Control\Inc\Interface\itreeview.h"

namespace UI
{

// {87AE2A12-319E-4dde-9C5A-7CDA620F4944}
static const GUID IID_UI_IPropertyCtrlEditItem = 
{ 0x87ae2a12, 0x319e, 0x4dde, { 0x9c, 0x5a, 0x7c, 0xda, 0x62, 0xf, 0x49, 0x44 } };

// {8530DBFB-493E-49be-852D-52FA42FA13A4}
static const GUID IID_UI_IPropertyCtrlGroupItem = 
{ 0x8530dbfb, 0x493e, 0x49be, { 0x85, 0x2d, 0x52, 0xfa, 0x42, 0xfa, 0x13, 0xa4 } };

// {19236792-9745-4d21-B04F-A44FEB740B2B}
static const GUID IID_UI_IPropertyCtrlComboBoxItem = 
{ 0x19236792, 0x9745, 0x4d21, { 0xb0, 0x4f, 0xa4, 0x4f, 0xeb, 0x74, 0xb, 0x2b } };

// {320CA5CF-9DD6-4032-996F-9B4C04AF2C9D}
static const GUID IID_UI_IPropertyCtrlButtonItem = 
{ 0x320ca5cf, 0x9dd6, 0x4032, { 0x99, 0x6f, 0x9b, 0x4c, 0x4, 0xaf, 0x2c, 0x9d } };

// {0A153049-988F-48fe-AAD6-4962687387AF}
static const GUID IID_UI_IPropertyCtrlLongItem = 
{ 0xa153049, 0x988f, 0x48fe, { 0xaa, 0xd6, 0x49, 0x62, 0x68, 0x73, 0x87, 0xaf } };

// {E9F3CADE-38AE-41ed-8274-A8155DC02BA3}
static const GUID IID_UI_IPropertyCtrlBoolItem = 
{ 0xe9f3cade, 0x38ae, 0x41ed, { 0x82, 0x74, 0xa8, 0x15, 0x5d, 0xc0, 0x2b, 0xa3 } };

// {AB2329E5-E947-4946-91F8-88F091DEDE68}
static const GUID IID_UI_IPropertyCtrlAdditionItem = 
{ 0xab2329e5, 0xe947, 0x4946, { 0x91, 0xf8, 0x88, 0xf0, 0x91, 0xde, 0xde, 0x68 } };

// 属性控件线条及背景颜色
#define PROPERTYCTRL_LINE_COLOR 212,208,200,255  //(233,236,250,255);

// 获取属性控件分隔条的位置指针（相对于控件）
// RETURN：UINT*
#define UI_PROPERTYCTRL_MSG_GETSPLITTERLINEPOS_PTR 136152136

// 获取EDIT控件
// RETURN: IEdit*
#define UI_PROPERTYCTRL_MSG_GETEDITCTRL 136162254
#define UI_PROPERTYCTRL_MSG_GETCOMBOBOXCTRL 136162255
#define UI_PROPERTYCTRL_MSG_GETBUTTONCTRL 136162256


interface IPropertyCtrlEditItem;
struct PROPERTYCTRL_EDIT_ACCEPTCONTENT
{
	IPropertyCtrlEditItem*  pItem;
	LPCTSTR  szKey;
	LPCTSTR  szNewValue;
};
// Edititem接受编辑内容
// WPARAM: IPropertyCtrlEditItem*
// LPARAM: LPCTSTR newText
#define UI_PROPERTYCTRL_NM_EDITITEM_ACCEPTCONTENT 136221752

interface IPropertyCtrlComboBoxItem;
struct PROPERTYCTRL_COMBOBOX_ITEM_ACCEPTCONTENT
{
	IPropertyCtrlComboBoxItem*  pItem;
	LPCTSTR  szKey;
	LPCTSTR  szNewValue;
    long  lNewValue;
};
#define UI_PROPERTYCTRL_NM_COMBOBOXITEM_ACCEPTCONTENT 136221753

// 外部给IListItemBase发送消息，设置属性的值
// WPARAM: LPCTSTR szText
#define UI_PROPERTYCTRLITEM_MSG_SETVALUESTRING  130201219

// 外部给IListItemBase发送消息，设置属性的默认值
// WPARAM: LPCTSTR szText
#define UI_PROPERTYCTRLITEM_MSG_SETDEFAULTVALUESTRING  130202239

// 外部给IListItemBase发送消息，获取key字段
// Return: LPCTSTR szText
#define UI_PROPERTYCTRLITEM_MSG_GETKEYSTRING  132311511
// 外部给IListItemBase发送消息，获取value字段
// Return: LPCTSTR szText
#define UI_PROPERTYCTRLITEM_MSG_GETVALUESTRING   157081316

// AdditionItem接受编辑内容
// MESSAGE: UI_WM_NOITFY
// WPARAM: IPropertyCtrlAdditionItem*
// LPARAM: LPCTSTR newText
// Result: ACCEPT_EDIT_RESULT
#define UI_PROPERTYCTRL_NM_ADDITIONITEM_ACCEPTCONTENT 154190837

class PropertyCtrlItemBaseShareData;
interface IPropertyCtrlItemBaseShareData : public IListItemTypeShareData
{
	enum {FLAG = 154202108};
    UI_DECLARE_Ixxx_INTERFACE(IPropertyCtrlItemBaseShareData, PropertyCtrlItemBaseShareData);
};

class PropertyCtrlItemBase;
interface UIEDITORCTRLAPI IPropertyCtrlItemBase : public IListItemBase
{
    UI_DECLARE_Ixxx_INTERFACE(IPropertyCtrlItemBase, PropertyCtrlItemBase);

    LPCTSTR  GetKey();
};

class PropertyCtrlEditItem;
interface UIEDITORCTRLAPI IPropertyCtrlEditItem : public IPropertyCtrlItemBase
{
	enum {FLAG = 154202056};
    UI_DECLARE_Ixxx_INTERFACE(IPropertyCtrlEditItem, PropertyCtrlEditItem);

    void  SetValueText(LPCTSTR szText);
	void  SetDefaultValueText(LPCTSTR szText);
};

class PropertyCtrlAdditionItem;
interface UIEDITORCTRLAPI IPropertyCtrlAdditionItem : public IPropertyCtrlItemBase
{
	enum {FLAG = 154202057};
	UI_DECLARE_Ixxx_INTERFACE(IPropertyCtrlAdditionItem, PropertyCtrlAdditionItem);

};

class PropertyCtrlLongItem;
interface UIEDITORCTRLAPI IPropertyCtrlLongItem : public IPropertyCtrlEditItem
{
	enum {FLAG = 154202058};
    UI_DECLARE_Ixxx_INTERFACE(IPropertyCtrlLongItem, PropertyCtrlLongItem);
	void  SetValue(long lValue);
};

// 列表项，是取字符串，还是取映射值
enum ComboBoxItemValueType
{
	ComboBoxItemValueString,
	ComboBoxItemValueLong,
};

class PropertyCtrlComboBoxItem;
interface UIEDITORCTRLAPI IPropertyCtrlComboBoxItem : public IPropertyCtrlItemBase
{
	enum {FLAG = 154202059};
	UI_DECLARE_Ixxx_INTERFACE(IPropertyCtrlComboBoxItem, PropertyCtrlComboBoxItem);

	void  SetValueString(LPCTSTR szText);
	void  SetValueLong(long lValue);
	void  AddOption(LPCTSTR szText, long lValue);
};

class PropertyCtrlBoolItem;
interface UIEDITORCTRLAPI IPropertyCtrlBoolItem : public IPropertyCtrlComboBoxItem
{
	enum {FLAG = 154202060};
    UI_DECLARE_Ixxx_INTERFACE(IPropertyCtrlBoolItem, PropertyCtrlBoolItem);
	void  SetBool(bool b);
};

class PropertyCtrlButtonItem;
interface UIEDITORCTRLAPI IPropertyCtrlButtonItem : public IPropertyCtrlItemBase
{
	enum {FLAG = 154202061};
	UI_DECLARE_Ixxx_INTERFACE(IPropertyCtrlButtonItem, PropertyCtrlButtonItem);

	void  SetValueText(LPCTSTR szText);
	void  SetDefaultValueText(LPCTSTR szText);
};

class PropertyCtrlGroupItem;
interface UIEDITORCTRLAPI IPropertyCtrlGroupItem : public INormalTreeItem
{
    enum {FLAG = 154011324};
    UI_DECLARE_Ixxx_INTERFACE(IPropertyCtrlGroupItem, PropertyCtrlGroupItem)
};

struct PropertyCtrlBoolItemInfo
{
    LPCTSTR szText;
    LPCTSTR szKey;
    LPCTSTR szDesc;
    bool  bValue;
    bool  bDefault;
};

struct PropertyCtrlComboBoxItemInfo
{
	LPCTSTR szText;
	LPCTSTR szKey;
	LPCTSTR szDesc;

	bool  bReadOnly; 
    bool  bMultiSel;   // 可多选。例如flags attribute

	ComboBoxItemValueType eType;
	union
	{
		LPCTSTR szValue;
		long lValue;
	};
};

struct PropertyCtrlLongItemInfo
{
	LPCTSTR szText;
	LPCTSTR szKey;
	LPCTSTR szDesc;
	long lValue;
};

struct PropertyCtrlEditItemInfo
{
	LPCTSTR szText;
	LPCTSTR szValue;
	LPCTSTR szDesc;
	LPCTSTR szKey;
};

class PropertyCtrl;
interface UIEDITORCTRLAPI IPropertyCtrl : public ITreeView
{
    UI_DECLARE_Ixxx_INTERFACE(IPropertyCtrl, PropertyCtrl);

	IPropertyCtrlGroupItem*  FindGroupItem(IListItemBase* pParentItem, LPCTSTR szName);
    IListItemBase*  FindItemByKey(LPCTSTR szKey);

    IPropertyCtrlGroupItem*  InsertGroupItem(LPCTSTR szName, LPCTSTR szDesc, 
        IListItemBase* pParent = UITVI_ROOT, IListItemBase* pInsertAfter = UITVI_LAST, LISTITEM_OPFLAGS nInsertFlags=0);

    IPropertyCtrlEditItem*   InsertEditProperty(PropertyCtrlEditItemInfo* pInfo,
        IListItemBase* pParentItem, IListItemBase* pInsertAfter = UITVI_LAST, LISTITEM_OPFLAGS nInsertFlags=0);

    IPropertyCtrlLongItem*   InsertLongProperty(PropertyCtrlLongItemInfo* pInfo, 
        IListItemBase* pParentItem, IListItemBase* pInsertAfter = UITVI_LAST, LISTITEM_OPFLAGS nInsertFlags=0);

	IPropertyCtrlBoolItem*   InsertBoolProperty(PropertyCtrlBoolItemInfo* pInfo,
		IListItemBase* pParentItem, IListItemBase* pInsertAfter = UITVI_LAST, LISTITEM_OPFLAGS nInsertFlags=0);

	IPropertyCtrlComboBoxItem*   InsertComboBoxProperty(PropertyCtrlComboBoxItemInfo* pInfo,
		IListItemBase* pParentItem, IListItemBase* pInsertAfter = UITVI_LAST, LISTITEM_OPFLAGS nInsertFlags=0);

	IPropertyCtrlButtonItem*   InsertButtonProperty(LPCTSTR szText, LPCTSTR szValue, LPCTSTR szDesc, LPCTSTR szKey,
		IListItemBase* pParentItem, IListItemBase* pInsertAfter = UITVI_LAST, LISTITEM_OPFLAGS nInsertFlags=0);

	IPropertyCtrlAdditionItem*  AddAdditionItem(LISTITEM_OPFLAGS nFlags);

	IPropertyCtrlEditItem*  FindEditItem(LPCTSTR szKey);
    IPropertyCtrlLongItem*  FindLongItem(LPCTSTR szKey);
    IPropertyCtrlComboBoxItem*  FindComboBoxItem(LPCTSTR szKey);
	IPropertyCtrlBoolItem*  FindBoolItem(LPCTSTR szKey);

};

}

#endif // IPROPERTYCTRL_H_2D8F8B87_E8DC_4995_82DF_F9D6155B970E