#ifndef OBJECT_H_763AA6BD_6435_456f_A41F_E5D7CCF39E7C
#define OBJECT_H_763AA6BD_6435_456f_A41F_E5D7CCF39E7C

#include "UISDK\Kernel\Src\Base\Object\objtree.h"
#include "UISDK\Kernel\Inc\Util\rendercontext.h" 

namespace UI
{
interface IRenderFont;
interface ITextRenderBase;
interface IRenderBase;
class UICursor;
class WindowBase;
interface ILayoutParam;
class Object3DWrap;
class RenderLayer2;
class WindowRender;
class Object;

struct AncestorDamageInfo
{
	Object*  pAncestor; // 
	RECT  rcDamage;     // client rect. 被刷新的对象在自己的区域内的脏区域
    bool  bDrawBkgnd;   // 是否需要重新绘制自己的背景
};

class Object : public ObjTree
{
public:
    Object();
    virtual ~Object();

    UI_DECLARE_OBJECT_INFO(Object, OBJ_NULL)

    UI_BEGIN_MSG_MAP
        UIMSG_WM_SETCURSOR(OnSetCursor)
        UIMSG_WM_ERASEBKGND(OnEraseBkgnd)
        UIMSG_WM_HITTEST(OnHitTest)
        UIMSG_WM_GETRENDERFONT(GetRenderFont)
        UIMSG_WM_THEMECHANGED(OnThemeChanged)
        UIMESSAGE_HANDLER_EX(UI_WM_SKINTEXTURECHANGED, OnSkinTextureChanged)
        UIMSG_WM_VISIBLE_CHANGED(OnVisibleChanged)
        UIMSG_WM_REDRAWOBJECT(OnRedrawObject)

		UIMSG_WM_EDITORGETATTRLIST(OnEditorGetAttrList)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
		UIMSG_WM_SERIALIZE(OnSerialize)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
        UIMSG_WM_FINALRELEASE(FinalRelease)
    UI_END_MSG_MAP

    IObject* GetIObject() { return m_pIObject; }
    void     SetIObject(IObject* p) { m_pIObject = p; }

	HRESULT  FinalConstruct(IUIApplication* p);    // 子类在实现该虚函数必须先调用父类的实现
	void     FinalRelease();                       // 子类在实现该虚函数必须先调用父类的实现           

public:

    //////////////////////////////////////////////////////////////////////////
    //  Get/Set
    //////////////////////////////////////////////////////////////////////////
	const  TCHAR*       GetId();
	void                SetId(const TCHAR* szText);

    TCHAR*              GetObjectName();
    OBJ_TYPE            GetObjectType();
    CONTROL_TYPE        GetObjectExtentType();

	WindowBase*         GetWindowObject();
    WindowRender*       GetWindowRender();
    RenderLayer2*       GetSelfRenderLayer2();   
    RenderLayer2*       GetRenderLayer2();
	HWND                GetHWND();
	Object*             FindChildObject(const TCHAR* szObjId);
    Object*             GetObjectByPos(POINT* pt);
    Object*             GetRenderLayerCreateObject();

    IRenderFont*        GetRenderFont();
    ITextRenderBase*    GetTextRender();
    IRenderBase*        GetBkRender();
    IRenderBase*        GetForeRender();
    void                SetBkgndRender(IRenderBase* p);
    void                SetForegndRender(IRenderBase* p);
    void                SetTextRender(ITextRenderBase* p);
    bool                CreateRenderLayer();

    // 扩展属性操作
    void                InitDefaultAttrib();
    void                SetUserData(LPVOID p);
    LPVOID              GetUserData();
    IUIApplication*     GetUIApplication();
    void                SetUIApplication(IUIApplication* p);
    void                SetOutRef(void** ppOutRef);
    HRGN                GetRgn();
    int                 GetZOrder();
    void                SetZorderDirect(int z);

    // 其它
    bool                SetCursor(const TCHAR* szCursorId);
    DWORD               CalcContrastTextColor();
    bool                SetMouseCapture(int nNotifyMsgId);
    bool                ReleaseMouseCapture();
    bool                SetKeyboardCapture(int nNotifyMsgId);
    bool                ReleaseKeyboardCapture();

    //////////////////////////////////////////////////////////////////////////
    // 绘制
    //////////////////////////////////////////////////////////////////////////

    bool               DrawObject(IRenderTarget* pRenderTarget, RenderContext c);
    void               RealDrawObject(IRenderTarget* pRenderTarget, RenderContext c);
    void               DrawChildObject(IRenderTarget* pRenderTarget, RenderContext& c);
    HBITMAP            TakeSnapshot();
    HBITMAP            TakeBkgndSnapshot();
    GRAPHICS_RENDER_LIBRARY_TYPE  GetGraphicsRenderLibraryType();

    void               GetUpdateObjectBoundRect(RECT*  prc);
	void               UpdateObject(bool bUpdateNow = true);
    void               UpdateObjectEx(RECT* prcObjArray, int nCount, bool bUpdateNow);
    void               RealUpdateObjectEx(RECT* prcObjArray, int nCount, bool bUpdateNow);
	void               UpdateObjectBkgnd(bool bUpdateNow);
	void               UpdateLayout(bool bUpdate);
    void               UpdateMyLayout(bool bUpdate);
	void               UpdateObjectNonClientRegion();
	void               ClearMyTreeRelationOnly();

    void               HandleDamageStackContext(Object* pObjDamage, IRenderTarget*  pRenderTarget, RenderContext context, stack<AncestorDamageInfo>& stackContext);
	void               TestZorderOverlapLess(Object* pObj, IRenderTarget* pRenderTarget, RECT* prcClip, RenderContext context);
    void               TestZorderOverlapGreat(Object* pObj, IRenderTarget* pRenderTarget, RECT* prcClip, RenderContext context);


    //////////////////////////////////////////////////////////////////////////
    //  区域操作函数
    //////////////////////////////////////////////////////////////////////////

#pragma region 
	void  GetNonClientRegion(CRegion4* prc) { prc->CopyRect(m_rcNonClient); }
	void  SetNonClientRegionExcludePaddingBorder( CRegion4* prc );
	void  SetNonClientRegionExcludePaddingBorderL( int n );
	void  SetNonClientRegionExcludePaddingBorderT( int n );
	void  SetNonClientRegionExcludePaddingBorderR( int n );
	void  SetNonClientRegionExcludePaddingBorderB( int n );
	int   GetNonClientL() { return m_rcNonClient.left; }
	int   GetNonClientT() { return m_rcNonClient.top; }
	int   GetNonClientR() { return m_rcNonClient.right; }
	int   GetNonClientB() { return m_rcNonClient.bottom; }
	int   GetNonClientW() { return m_rcNonClient.left + m_rcNonClient.right; }
	int   GetNonClientH() { return m_rcNonClient.top + m_rcNonClient.bottom; }
	void  GetBorderRegion( CRegion4* prc ) { prc->CopyRect(&m_rcBorder); }
	int   GetBorderL() { return m_rcBorder.left; }
	int   GetBorderT() { return m_rcBorder.top; }
	int   GetBorderR() { return m_rcBorder.right; }
	int   GetBorderB() { return m_rcBorder.bottom; }
	void  SetBorderRegion( CRegion4* prc );
	void  GetPaddingRegion( CRegion4* prc ) { prc->CopyRect(&m_rcPadding); }
	void  SetPaddingRegion( CRegion4* prc );
	int   GetPaddingL() { return m_rcPadding.left; }
	int   GetPaddingT() { return m_rcPadding.top; }
	int   GetPaddingR() { return m_rcPadding.right; }
	int   GetPaddingB() { return m_rcPadding.bottom; }
	int   GetPaddingW() { return m_rcPadding.left + m_rcPadding.right; }
	int   GetPaddingH() { return m_rcPadding.top + m_rcPadding.bottom; }
	void  SetMarginRegion( CRegion4* prc) { m_rcMargin.CopyRect(prc); }
	void  GetMarginRegion( CRegion4* prc) { prc->CopyRect(&m_rcMargin); }
	int   GetMarginL() { return m_rcMargin.left; }
	int   GetMarginT() { return m_rcMargin.top; }
	int   GetMarginR() { return m_rcMargin.right; }
	int   GetMarginB() { return m_rcMargin.bottom; }
	int   GetMarginW() { return m_rcMargin.left + m_rcMargin.right; }
	int   GetMarginH() { return m_rcMargin.top + m_rcMargin.bottom; }
	void  GetParentRect( CRect* prc );
	int   GetParentRectL() { return m_rcParent.left; }
	int   GetParentRectT() { return m_rcParent.top; }
	int   GetParentRectR() { return m_rcParent.right; }
	int   GetParentRectB() { return m_rcParent.bottom; }

    ILayoutParam*  GetLayoutParam();
    void  CreateLayoutParam();
    void  SetLayoutParam(ILayoutParam* p);

    // Canvas布局专用接口
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
#pragma endregion

    //////////////////////////////////////////////////////////////////////////
    // Position
    //////////////////////////////////////////////////////////////////////////

#pragma region

	int   GetWidth();
	int   GetHeight();
	int   GetWidthWithMargins();
	int   GetHeightWithMargins();

    bool  IntersectWindowRect(const RECT* rcWindow, RECT* rcIntersectWnd, RECT* rcIntersectObj);
	bool  GetScrollOffset(int* pxOffset, int* pyOffset);
	bool  GetScrollRange(int* pxRange, int* pyRange);

	void  GetClientRect( CRect* prc );
	void  GetClientRectAsWin32(CRect* prc);
	void  GetClientRectInWindow(CRect* prc);

	bool  GetVisibleRectInWindow(RECT* prc);
    bool  GetVisibleRectInLayer(RECT* prc);
    bool  GetVisibleClientRectInLayer(RECT* prc);
    bool  CalcVisibleRectInAncestor(__in Object*  pObjAncestor, __in const RECT* prcObjPart, __out RECT* prcOut);

	void  SetObjectPos( int x, int y, int cx, int cy, int nFlag=0 );
	void  SetObjectPos( CRect* prc, int nFlag=0 );
    void  UpdateLayoutPos();

	POINT  GetWindowPoint();
	void  GetWindowRect(CRect* lprc);
	void  WindowPoint2ObjectPoint(const POINT* ptWindow, POINT* ptObj, bool bCalcTransform);
	void  WindowPoint2ObjectClientPoint(const POINT* ptWindow, POINT* ptObj, bool bCalcTransform);
	void  WindowRect2ObjectClientRect(const RECT* rcWindow, RECT* rcObj);
	void  WindowRect2ObjectRect(const RECT* rcWindow, RECT* rcObj);

    void  notify_WM_SIZE(UINT nType, UINT nWidth, UINT nHeight);
    SIZE  GetDesiredSize();

	static void  ParentClientPoint2ChildPoint(Object* pObjChild, const POINT* pt, POINT* pOut);

	static void  ObjectPoint2ObjectClientPoint(Object* pObj, const POINT* pt, POINT* pOut);
    static void  ObjectPoint2ObjectNonClientPoint(Object* pObj, const POINT* pt, POINT* pOut);
	static void  ObjectRect2ObjectClientRect(Object* pObj, const RECT* prc, RECT* pOut);

	static void  ParentClientPoint2ChildClientPoint(Object* pObjChild, const POINT* pt, POINT* pOut);
	static void  ParentClientRect2ChildClientRect(Object* pObjChild, const RECT* prc, RECT* pOut);

	static void  ParentPoint2ChildPoint(Object* pObjChild, const POINT* pt, POINT* pOut);
	static void  ParentRect2ChildRect(Object* pObjChild, const RECT* prc, RECT* pOut);

	static void  ChildPoint2ParentClientPoint(Object* pObjChild, const POINT* ptChild, POINT*  ptOut);
	static void  ChildRect2ParentClientRect(Object* pObjChild, const RECT* prc, RECT*  pOut);

	static void  ObjectClientPoint2ObjectPoint(Object*  pObj, const POINT* ptChild, POINT*  ptOut);
	static void  ObjectClientRect2ObjectRect(Object*  pObj, const RECT* prc, RECT*  pOut);

	static void  ChildPoint2ParentPoint(Object* pObjChild, const POINT* ptChild, POINT*  ptOut);
	static void  ChildRect2ParentRect(Object* pObjChild, const RECT* prc, RECT*  pOut);	

#pragma endregion

    //////////////////////////////////////////////////////////////////////////
    // 样式操作
    //////////////////////////////////////////////////////////////////////////

#pragma region 
	int   GetStateBit() { return m_nStateBit; }
	bool  IsFocus();
	void  SetFocus(bool b, bool bNoitfy=true);
	bool  IsTabstop();
	bool  CanTabstop();
	bool  IsGroup();
	void  SetTabstop(bool b);
	void  SetGroup(bool b);
	bool  IsVisible();
	bool  IsCollapsed();
	bool  IsMySelfVisible();
	bool  IsEnable();
	void  SetVisible(bool b, bool bRedraw=true, bool bUpdateLayout=true);
	void  SetEnable(bool b, bool bUpdateNow=true, bool bNotify=true);
	bool  IsDefault();
	bool  IsHover();
	bool  IsPress();
	bool  IsForcePress();
	bool  IsForceHover();
    bool  IsSelected();
    void  SetSelected(bool b, bool bNotify=true);
	void  SetDefault(bool b, bool bNotify=true);
	void  SetForceHover(bool b, bool bNotify=true);
	void  SetForcePress(bool b, bool bNotify=true);
	void  SetHover(bool b, bool bNotify=true);
	void  SetPress(bool b, bool bNotify=true);
	void  ModifyStyle(UINT add, UINT remove, bool bNotify);
    void  ModifyStyleEx(UINT add, UINT remove, bool bNotify);
    UINT  GetStyle() { return m_nStyle; }
    UINT  GetStyleEx() { return m_nStyle2; }
    void  SetStyle(UINT n) { m_nStyle = n; }
    void  SetStyleEx(UINT n) { m_nStyle2 = n; }
	bool  TestStyle(UINT nStyle){ if(m_nStyle&nStyle)return true; return false; }
    bool  TestStyleEx(UINT nStyleEx){ if(m_nStyle2&nStyleEx)return true; return false; }
	bool  IsTransparent();
    void  SetTransparent(bool b);

	void  SetAsNcObject(bool b);  // ObjTree中有引用
	bool  IsNcObject();           // ObjTree中有引用
	bool  IsRejectMouseMsgAll();
    bool  IsRejectMouseMsgSelf();

	void  SetCanRedraw(bool bReDraw);
	bool  CanRedraw();

	bool  testStateBit(UINT bit);
	void  setStateBit(UINT bit);
	void  clearStateBit(UINT bit);

public:
#pragma  endregion

#pragma region // 3d op
//     Object3DWrap*  Begin3D();
//     void  End3D();
//     Object3DWrap*  Get3DWrap();
#pragma endregion

	
protected:
	Object*    _findChildObjectItem(const TCHAR* szobjId);
	void       _drawNcChildObject(IRenderTarget*, RenderContext& roc);
	void       _parseLayoutAttribute(IMapAttribute* pMapAttrib);

    //////////////////////////////////////////////////////////////////////////
    //  属性
    //////////////////////////////////////////////////////////////////////////

public:
	// 将xml用的配置转成对象的属性，注意，子类重载该函数时，必须先调用父类的该方法
	// bReload表示为换肤时调用，此时不能再给某些属性赋值，例如text属性
	bool  SetAttribute(IMapAttribute* pMapAttrib, bool bReload);
	void  OnSerialize(SERIALIZEDATA* pData);

	// 在UIObjCreator构造Object和换肤时，将触发该函数。因为换肤时会先销毁所有资源，因此在ResetAttribute中要释放所有资源对象
	// 注意，子类重载该函数时，必须先调用父类的该方法
	void  ResetAttribute();

	void  OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA*  pData);
//	bool  SetChildObjectAttribute(Object* pChildObj, const String& strPrefix, IMapAttribute* pMapAttrib, bool bReload);
    void  SetAttributeByPrefix(const TCHAR* szPrefix, IMapAttribute* pMatAttrib, bool bReload, bool bErase);
    void  ParseStyleAndSetAttribute(IMapAttribute* pMatAttrib, bool bReload);

    const TCHAR*  GetAttribute(const TCHAR* szKey, bool bErase);
    void  AddAttribute(const TCHAR* szKey, const TCHAR*  szValue);
    void  GetMapAttribute(IMapAttribute** ppMapAttribute);  // 需要release

    //////////////////////////////////////////////////////////////////////////
    // 消息处理
    //////////////////////////////////////////////////////////////////////////
protected:
    void*     QueryInterface(const IID* pIID);
	void      OnEraseBkgnd( IRenderTarget* );
	BOOL      OnSetCursor( HWND hWnd, UINT nHitTest, UINT message );
	UINT      OnHitTest(POINT* pt, __out POINT* ptInChild);
	void      OnThemeChanged();
    LRESULT   OnSkinTextureChanged(UINT, WPARAM, LPARAM);
    void      OnVisibleChanged(BOOL bVisible, IObject* pObjChanged);
    void      OnRedrawObject();

protected:
	//////////////////////////////////////////////////////////////////////////
	// virtual function
	//////////////////////////////////////////////////////////////////////////

	virtual void  virtualOnSize(UINT nType, UINT nWidth, UINT nHeight);
	virtual void  virtualOnPostDrawObjectErasebkgnd(){};   // 用于CustomWindow更新窗口异形

    //////////////////////////////////////////////////////////////////////////
    // Member Var
    //////////////////////////////////////////////////////////////////////////

protected:

    IObject*          m_pIObject;
    IUIApplication*   m_pUIApplication;        // 该对象所在的App(为了实现多实例，将app不再做为全局变量，每个Object能够快速引用到app接口)
    String            m_strId;                 // 该对象在XML中的标识
    
#pragma region //坐标相关数据
    CRect             m_rcParent;              // 该对象的范围，相对于parent的client区域.对于Window对象是客户区域位置，即左上角为0，0
    CRegion4          m_rcNonClient;           // 如果没有滚动条、Header等其它占用空间的控件，m_rcNonClient即为m_rcPadding+m_rcBorder
    CRegion4          m_rcMargin;
    CRegion4          m_rcPadding;
    CRegion4          m_rcBorder;       
    HRGN              m_hRgn;                  // （未使用）如果该对象是一个不规则区域，必须设置该值，该值对window类型对象无效. rgn是相对于窗口左上角的。
    ILayoutParam*     m_pLayoutParam;          // 布局参数。由Object负责释放
#pragma endregion

    int               m_nCanRedrawRef;         // TODO: 用于解决多处调用SetRedraw(false)后，必须所有地方放开SetRedraw(true)才能进行绘制
    UINT              m_nStateBit;             // 对象的状态，如visible/disable
    UINT              m_nStyle;                // 普通样式
    UINT              m_nStyle2;               // 控件样式，与的关系
    int               m_nzOrder;               // 控件z序，用于实现控件重叠时的刷新判断依据
    LPVOID            m_pUserData;             // 自定义数据（目前是用于在UIBuilder做数据关联）
    IMapAttribute*    m_pIMapAttributeRemain;  // 用于扩展。未解析的属性，例如Tooltip

    IRenderBase*      m_pBkgndRender;          // 背景渲染
    IRenderBase*      m_pForegndRender;        // 前景渲染
    ITextRenderBase*  m_pTextRender;           // 文字渲染
    IUICursor*        m_pCursor;               // 对象的鼠标样式
    Object3DWrap*     m_pObject3DWrap;         // 3d包装器
    RenderLayer2*     m_pRenderLayer;          // 该对象是否创建了一个layer
    
    void**            m_ppOutRef;              // 为了解决一个类成员对象，有可能被自己的父对象删除后，这个类却不知道，再删除该对象时崩溃了.
};

}

#endif  // OBJECT_H_763AA6BD_6435_456f_A41F_E5D7CCF39E7C

