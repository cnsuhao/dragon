#ifndef IOBJECT_H_E0069B93_FD58_46f0_9FCC_78B2F7D0A96D
#define IOBJECT_H_E0069B93_FD58_46f0_9FCC_78B2F7D0A96D

#include "UISDK\Kernel\Inc\Interface\imessage.h"

namespace UI
{
interface IWindowBase;
interface IPanel;
interface IRenderLayer;
interface IRenderLayer;
interface IObject3DWrap;
interface ILayoutParam;
class Object; 


// // object style
// #define OBJECT_STYLE_TRANSPARENT        0x00010000    // �����Ƿ�͸������
// #define OBJECT_STYLE_HSCROLL            0x00020000    // ����������
// #define OBJECT_STYLE_VSCROLL            0x00040000    // �����������
// #define OBJECT_STYLE_NCOBJ              0x00080000    // �ö���λ�ڸ�����ķǿͻ�����������ƫ��
// #define OBJECT_STYLE_REJECT_MOUSE_MSG_ALL   0x00100000    // �ö�������Լ����Ӷ��󲻽��������Ϣ����label
// #define OBJECT_STYLE_REJECT_MOUSE_MSG_SELF  0x00200000    // ���ö��󲻽��������Ϣ�����Ӷ����ǽ��������Ϣ�ģ���panel
// #define OBJECT_STYLE_RECEIVE_DRAGDROPEVENT  0x00400000    // �ö��������ק��Ϣ
// #define OBJECT_STYLE_ENABLE_IME         0x00800000    // �ö�����Ҫ�������뷨����EDIT,RICHEDIT
// //#define OBJECT_STYLE_ZINDEX_OVERLAP     0x01000000    // �ö�����ͬһ����������������ص��ˣ�����ˢ��ʱ�쳣�����ϸ���ʽ�󣬽�ֱ��ˢ�¸�����
// #define OBJECT_STYLE_NOCLIP             0x02000000    // �ö�����Ҫά���������򣨵��Լ���Ȼ�ᱻ�����ڸ�������������ڣ�
// #define CONTROL_STYLE_TABSTOP           0x00001000    // �Ƿ����TAB��ݼ��Ľ���

typedef struct tagObjStyle
{
	bool  transparent : 1;
    bool  float_on_parent_content : 1;   // ��ˢ�¸ÿؼ�ʱ����������Ҫ�ػ游���󱳾�(wm_erasebkgbkgnd)����Ҫ�ػ游��������(wm_paint)
    bool  post_paint : 1;
	bool  hscroll : 1;
	bool  vscroll : 1;
	bool  ncobject : 1;
	bool  reject_all_mouse_msg : 1;
	bool  reject_self_mouse_msg : 1;
	bool  receive_dragdrop_event : 1;
	bool  enable_ime : 1;
	bool  zindex_overlap : 1;
	bool  noclip : 1;
    bool  clip_client : 1;   // ���������ı������ٻ��ƶ����client����ʱ���Ƿ���Ҫ��client���������á���һ��Ķ������������ڻ������listctrl����Ҫ���ã�������������ݻᳬ�������Ǳ���
	bool  tabstop : 1;
	bool  layer;  // �����ֲ�

    // Ĭ��ֵ�ֶΡ����������า�Ǹ����Ĭ����Ϊ
    // ���Կ���ר��ΪdefaultҲ����һ��ͬ����tagObjStyle�����ڶ�Ӧÿһ����ʽ��Ĭ��ֵ
	bool  default_ncobject: 1;  // scrollbarĬ��Ϊncobj
    bool  default_reject_all_mouse_msg : 1; // Ĭ�ϸÿؼ��Ͳ����������Ϣ����panel/label
	bool  default_reject_self_mouse_msg : 1;
    bool  default_transparent : 1;  // �ÿؼ�Ĭ����͸���ġ�Ĭ�����еĿؼ���͸����������������Ч��
    bool  default_tabstop : 1;  // Ĭ�ϸÿؼ��ܷ�tabstop

}ObjStyle;
typedef struct tagObjState
{
	byte  visibility_ : 2;  // 0:visible 1:hidden 2:collapsed
	byte  disable : 1;
	byte  press : 1;
	byte  hover : 1;
	byte  force_press : 1;
	byte  force_hover : 1;
	byte  focus : 1;
	byte  default_ : 1;
	byte  selected : 1;
	byte  checked : 1;
    byte  radio_checked : 1; // ??
}ObjState;

interface UISDKAPI IObject : public IMessage
{
    UI_DECLARE_Ixxx_INTERFACE(IObject, Object)

    LPCTSTR  GetId();
    void     SetId(LPCTSTR szText);

    IWindowBase*     GetWindowObject();
	IRenderLayer*   GetRenderLayer2();
    IRenderLayer*   GetSelfRenderLayer2();
    HWND     GetHWND();

    void  SetOutRef(void** ppOutRef);
    IUIApplication* GetUIApplication();
	ISkinRes*  GetSkinRes();
    
    void  InitDefaultAttrib();
    void  SetObjectPos(int x, int y, int cx, int cy, int nFlag=0);
    void  SetObjectPos(RECT* prc, int nFlag=0);
    void  SetUserData(LPVOID p);
    LPVOID   GetUserData();
    void  ModifyObjectStyle(ObjStyle* add, ObjStyle* remove);
    bool  TestObjectStyle(const ObjStyle& test);
    void  ModifyStyleEx( UINT add, UINT remove, bool bNotify);
    bool  TestStyleEx(UINT nStyleEx);
    void  SetCanRedraw(bool bReDraw);
    UINT  GetStyleEx();
    void  SetStyleEx(UINT n);

    bool  testStateBit(UINT bit);
    void  setStateBit(UINT bit);
    void  clearStateBit(UINT bit);
    bool  CanRedraw();
    IRenderFont* GetRenderFont();
    void  SetRenderLayer(bool b);

    int   GetStateBit();
    bool  IsFocus();
	bool  SetFocusInWindow();
    bool  CanTabstop();
    bool  IsTabstop();
    void  SetTabstop(bool);
    bool  IsVisible();
    bool  IsCollapsed();
    bool  IsSelfVisible();
    bool  IsEnable();
    void  SetVisible(bool b, bool bRedraw=true, bool bUpdateLayout=true);
    void  SetEnable(bool b, bool bUpdateNow=true);
    bool  IsDefault();
    bool  IsHover();
    bool  IsSelected();
    bool  IsPress();
    bool  IsForcePress();
    bool  IsForceHover();
    void  SetDefault(bool, bool bNotify=true);
    void  SetForceHover(bool, bool bNotify=true);
    void  SetForcePress(bool, bool bNotify=true);
    void  SetHover(bool, bool bNotify=true);
    void  SetPress(bool, bool bNotify=true);
    void  SetSelected(bool, bool bNotify=true);
	void  SetZorderDirect(int lz);
	int   GetZOrder();

    bool  IsTransparent();
    void  SetTransparent(bool b);
    bool  IsNcObject();
    void  SetAsNcObject(bool b);
    bool  IsRejectMouseMsgAll();
    bool  IsRejectMouseMsgSelf();

    IObject*  FindChildObject(LPCTSTR szObjId);
	IObject*  FindNcChildObject(LPCTSTR szObjId);
    IObject*  GetParentObject();
    IObject*  GetChildObject();
    IObject*  GetNcChildObject();
    IObject*  GetLastChildObject();
    IObject*  GetNextObject();
    IObject*  GetPrevObject();
    IObject*  GetRootObject();
//	IObject*  GetChildObjectByIndex(unsigned long lIndex)

    IObject*  EnumChildObject(IObject*);
    IObject*  REnumChildObject(IObject*);
    IObject*  EnumNcChildObject(IObject*);
    IObject*  EnumAllChildObject(IObject*);
    IObject*  EnumParentObject(IObject*);
    IObject*  REnumParentObject(IObject*);

    IObject*  GetNextTreeItemObject();
    IObject*  GetPrevTreeItemObject();
    IObject*  GetNextTreeTabstopItemObject();
    IObject*  GetPrevTreeTabstopItemObject();
    IObject*  GetNextTabObject();
    IObject*  GetPrevTabObject();

    void  SetParentObjectDirect(IObject* p);
    void  SetChildObjectDirect(IObject* p);
    void  SetNcChildObjectDirect(IObject* p);
    void  SetNextObjectDirect(IObject* p);
    void  SetPrevObjectDirect(IObject* p); 

    void  AddChild(IObject*);
    void  InsertChild(IObject* pObj, IObject* pInsertAfter);
    void  AddNcChild(IObject*);
    bool  IsMyChild(IObject* pChild, bool bFindInGrand);
    bool  RemoveChildInTree(IObject* pChild);
    void  RemoveMeInTheTree();
    void  UpdateObjectNonClientRegion();
    void  Standalone();
    void  MoveToAsFirstChild();
    void  MoveToAsLastChild();
    void  InsertAfter(IObject* pInsertAfter);
    void  InsertBefore(IObject* pInsertBefore);
    bool  SwapObject(IObject* pObj1, IObject* pObj2);

    void  UpdateObject(bool bUpdateNow=true);
	void  UpdateObjectEx(RECT* prcObjArray, int nCount, bool bUpdateNow);
    void  UpdateObjectBkgnd(bool bUpdateNow);
    void  UpdateLayout(bool bUpdate);    
    void  UpdateMyLayout(bool bUpdate);

    void  GetNonClientRegion(REGION4* prc);
    void  SetExtNonClientRegion(REGION4*);
    void  GetExtNonClientRegion(REGION4*);
    void  GetBorderRegion(REGION4*);
    void  SetBorderRegion(REGION4*);
    void  GetPaddingRegion(REGION4*);
    void  SetPaddingRegion(REGION4*);
    void  SetMarginRegion(REGION4*);
    void  GetMarginRegion(REGION4*);
    void  GetClientRectInObject(RECT*);
    void  GetObjectClientRect(RECT*);
    void  GetClientRectInWindow(RECT*);
    void  GetParentRect(RECT*);

    POINT  GetRealPosInWindow();
    void  GetWindowRect(RECT*);
    void  WindowRect2ObjectRect(const RECT* rcWindow, RECT* rcObj);
    void  WindowPoint2ObjectPoint(const POINT* ptWindow, POINT* ptObj, bool bCalcTransform);
    void  WindowPoint2ObjectClientPoint(const POINT* ptWindow, POINT* ptObj, bool bCalcTransform);
    void  ObjectPoint2ObjectClientPoint(const POINT* ptWindow, POINT* ptObj);
    void  ClientRect2ObjectRect(const RECT* rcClient, RECT* rcObj);
	void  ClientRect2WindowRect(const RECT* rcClient, RECT* rcObj);
    void  ObjectRect2ClientRect(const RECT* rcObj, RECT* rcClient);
    bool  GetScrollOffset(int* pxOffset, int* pyOffset);
    bool  GetScrollRange(int* pxRange, int* pyRange);
    bool  GetRectInWindow(RECT* prc, bool bOnlyVisiblePart);
    bool  GetRectInLayer(RECT* prc, bool bOnlyVisiblePart);
    bool  GetVisibleClientRectInLayer(RECT* prc);

    int   GetWidth();
    int   GetHeight();
    int   GetWidthWithMargins();
    int   GetHeightWithMargins(); 

	ILayoutParam*  GetLayoutParam();
	void  CreateLayoutParam();
    int   GetConfigWidth();
    int   GetConfigHeight();
    int   GetConfigLayoutFlags();
    int   GetConfigLeft();
    int   GetConfigRight();
    int   GetConfigTop();
    int   GetConfigBottom();

    void  SetConfigWidth(int n);
    void  SetConfigHeight(int n);
    void  SetConfigLayoutFlags(int n);
    void  SetConfigLeft(int n);
    void  SetConfigRight(int n);
    void  SetConfigTop(int n);
    void  SetConfigBottom(int n);

    // ���า��
    TCHAR*   GetObjectName();
    OBJ_TYPE  GetObjectType();
    CONTROL_TYPE  GetObjectExtentType();

    ITextRenderBase*  GetTextRender();
    IRenderBase*  GetBkRender();
    IRenderBase*  GetForeRender();
    void  SetBkgndRender(IRenderBase* p);
    void  SetForegndRender(IRenderBase* p);
    void  SetTextRender(ITextRenderBase* p);
    void  SetAttributeByPrefix(LPCTSTR szPrefix, IMapAttribute* pMatAttrib, bool bReload, bool bErase);
    void  ParseStyleAndSetAttribute(IMapAttribute* pMatAttrib, bool bReload);
	void  LoadAttributeFromXml(IUIElement* pXmlElement, bool bReload);
    LPCTSTR  GetAttribute(LPCTSTR szKey, bool bErase);
    void  AddAttribute(LPCTSTR szKey, LPCTSTR  szValue);
    void  GetMapAttribute(IMapAttribute** ppMapAttribute);
    SIZE  GetDesiredSize();
    HBITMAP  TakeSnapshot();
    HBITMAP  TakeBkgndSnapshot();
    GRAPHICS_RENDER_LIBRARY_TYPE  GetGraphicsRenderLibraryType();
    void     LoadColor(LPCTSTR szColor, Color*& pColorRef);
    LPCTSTR  SaveColor(Color*& pColorRef);
	void  LoadTextRender(LPCTSTR szName);
	LPCTSTR  SaveTextRender();

    LPCTSTR  GetToolTipText();
    void  SetToolTipText(LPCTSTR szText);
    bool  SetCursorId(LPCTSTR szCursorId);
    bool  SetMouseCapture(int nNotifyMsgId);
    bool  ReleaseMouseCapture();
    bool  SetKeyboardCapture(int nNotifyMsgId);
    bool  ReleaseKeyboardCapture();
};

}

#endif  // IOBJECT_H_E0069B93_FD58_46f0_9FCC_78B2F7D0A96D