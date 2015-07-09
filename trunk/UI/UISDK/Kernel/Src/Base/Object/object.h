#ifndef OBJECT_H_763AA6BD_6435_456f_A41F_E5D7CCF39E7C
#define OBJECT_H_763AA6BD_6435_456f_A41F_E5D7CCF39E7C

#include "UISDK\Kernel\Src\Base\Object\objtree.h"
#include "UISDK\Kernel\Inc\Util\rendercontext.h" 

namespace UI
{
interface IRenderFont;
interface ITextRenderBase;
interface IRenderBase;
interface IMKMgr;
class UICursor;
class WindowBase;
interface ILayoutParam;
class RenderLayer;
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

    UI_BEGIN_MSG_MAP
        UIMSG_WM_SETCURSOR(OnSetCursor)
        UIMSG_WM_ERASEBKGND(OnEraseBkgnd)
        UIMSG_WM_HITTEST(OnHitTest)
        UIMSG_WM_GETRENDERFONT(GetRenderFont)
        UIMSG_WM_THEMECHANGED(OnThemeChanged)
        UIMESSAGE_HANDLER_EX(UI_WM_SKINTEXTURECHANGED, OnSkinTextureChanged)
        UIMESSAGE_HANDLER_EX(UI_WM_GET_TOOLTIPINFO, OnGetTooltipInfo)
        UIMSG_WM_VISIBLE_CHANGED(OnVisibleChanged)
        UIMSG_WM_REDRAWOBJECT(OnRedrawObject)

        UIMSG_WM_QUERYINTERFACE(QueryInterface)
		UIMSG_WM_SERIALIZE(OnSerialize)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
        UIMSG_WM_FINALRELEASE(FinalRelease)
    UI_END_MSG_MAP

    IObject* GetIObject();
    void     SetIObject(IObject* p);

public:

    //////////////////////////////////////////////////////////////////////////
    //  Get/Set
    //////////////////////////////////////////////////////////////////////////
	LPCTSTR  GetId();
	void  SetId(LPCTSTR szText);

    TCHAR*  GetObjectName();
    OBJ_TYPE  GetObjectType();
    CONTROL_TYPE  GetObjectExtentType();
	bool  CreateAccesible(IAccessible** pp);

	WindowBase*  GetWindowObject();
    WindowRender*  GetWindowRender();
    RenderLayer*  GetSelfRenderLayer2();   
    RenderLayer*  GetRenderLayer2();
	HWND  GetHWND();
	Object*  FindChildObject(LPCTSTR szObjId);
	Object*  FindNcChildObject(LPCTSTR szObjId);
    Object*  GetObjectByPos(POINT* pt);
    Object*  GetRenderLayerCreateObject();
    void  SetRenderLayer(bool b);
    bool  HasRenderLayer();
	Object*  GetChildObjectByIndex(unsigned long lIndex);
	unsigned long  GetChildCount();
	unsigned long  GetChildObjectIndex(Object* pChild);

    IRenderFont*  GetRenderFont();
    ITextRenderBase*  GetTextRender();
    IRenderBase*  GetBkRender();
    IRenderBase*  GetForeRender();
    void  SetBkgndRender(IRenderBase* p);
    void  SetForegndRender(IRenderBase* p);
    void  SetTextRender(ITextRenderBase* p);

    void  LoadBkgndRender(LPCTSTR szName)   { 
            loadRenderBase(szName, m_pBkgndRender); }
    void  LoadForegndRender(LPCTSTR szName) { 
            loadRenderBase(szName, m_pForegndRender); }
    void  LoadTextRender(LPCTSTR szName)    { 
            loadTextRender(szName, m_pTextRender); }
    LPCTSTR  SaveBkgndRender()   { 
            return getRenderBaseName(m_pBkgndRender); }
    LPCTSTR  SaveForegndRender() { 
            return getRenderBaseName(m_pForegndRender); }
    LPCTSTR  SaveTextRender()    { 
            return getTextRenderName(m_pTextRender); }

    // 扩展属性操作
    void     InitDefaultAttrib();
    void     SetUserData(LPVOID p);
    LPVOID   GetUserData();
    UIApplication*  GetUIApplication();
    IUIApplication*  GetIUIApplication();
    void     SetOutRef(void** ppOutRef);
    HRGN     GetRgn();
    int      GetZOrder();
    void     SetZorderDirect(int z);

    // 其它
    bool     SetCursorId(LPCTSTR szCursorId);
	LPCTSTR  SaveCursorId();
    LPCTSTR  GetToolTipText();
    void     SetToolTipText(LPCTSTR szText);
    void     LoadColor(LPCTSTR szColor, Color*& pColorRef);
    LPCTSTR  SaveColor(Color*& pColorRef);

    DWORD    CalcContrastTextColor();
    bool     SetMouseCapture(int nNotifyMsgId);
    bool     ReleaseMouseCapture();
    bool     SetKeyboardCapture(int nNotifyMsgId);
    bool     ReleaseKeyboardCapture();
    IMKMgr*  GetIMKMgr();

    //////////////////////////////////////////////////////////////////////////
    // 绘制
    //////////////////////////////////////////////////////////////////////////

    bool     DrawObject(IRenderTarget* pRenderTarget, RenderContext c);
    void     RealDrawObject(IRenderTarget* pRenderTarget, RenderContext c);
    void     DrawChildObject(IRenderTarget* pRenderTarget, RenderContext& c);
    HBITMAP  TakeSnapshot();
    HBITMAP  TakeBkgndSnapshot();
    GRAPHICS_RENDER_LIBRARY_TYPE  GetGraphicsRenderLibraryType();
    void  DoPostPaint(IRenderTarget* pRenderTarget, 
        RenderContext context);

    void  GetUpdateObjectBoundRect(RECT*  prc);
	void  UpdateObject(bool bUpdateNow = true);
    void  UpdateObjectEx(RECT* prcObjArray, int nCount, bool bUpdateNow);
    void  RealUpdateObjectEx(RECT* prcObjArray, int nCount, bool bUpdateNow);
	void  UpdateObjectBkgnd(bool bUpdateNow);
	void  UpdateLayout(bool bUpdate);
    void  UpdateMyLayout(bool bUpdate);
	void  UpdateObjectNonClientRegion();
	void  ClearMyTreeRelationOnly();

    void  HandleDamageStackContext(Object* pObjDamage, 
        IRenderTarget*  pRenderTarget,
        RenderContext context, 
        deque<AncestorDamageInfo>& stackContext);
	void  TestZorderOverlapLess(Object* pObj, IRenderTarget* pRenderTarget, RECT* prcClip, const RenderContext* context);
    void  TestZorderOverlapGreat(Object* pObj, IRenderTarget* pRenderTarget, RECT* prcClip, const RenderContext* context);


    //////////////////////////////////////////////////////////////////////////
    //  区域操作函数
    //////////////////////////////////////////////////////////////////////////

#pragma region 
	void  GetNonClientRegion(REGION4* prc);
    void  SetExtNonClientRegion(REGION4* prc ) { m_rcExtNonClient.CopyRect(prc); }
    void  GetExtNonClientRegion(REGION4* prc) { ::CopyRect(prc, &m_rcExtNonClient); }
	
	void  GetBorderRegion( REGION4* prc ) { ::CopyRect(prc, &m_rcBorder); }
	int   GetBorderL() { return m_rcBorder.left; }
	int   GetBorderT() { return m_rcBorder.top; }
	int   GetBorderR() { return m_rcBorder.right; }
	int   GetBorderB() { return m_rcBorder.bottom; }
	void  SetBorderRegion( REGION4* prc );
    void  GetPaddingRegion( REGION4* prc ) { ::CopyRect(prc, &m_rcPadding); }
	void  SetPaddingRegion( REGION4* prc );
	int   GetPaddingL() { return m_rcPadding.left; }
	int   GetPaddingT() { return m_rcPadding.top; }
	int   GetPaddingR() { return m_rcPadding.right; }
	int   GetPaddingB() { return m_rcPadding.bottom; }
	int   GetPaddingW() { return m_rcPadding.left + m_rcPadding.right; }
	int   GetPaddingH() { return m_rcPadding.top + m_rcPadding.bottom; }
	void  SetMarginRegion( REGION4* prc) { m_rcMargin.CopyRect(prc); }
	void  GetMarginRegion( REGION4* prc) { ::CopyRect(prc, &m_rcMargin); }
	int   GetMarginL() { return m_rcMargin.left; }
	int   GetMarginT() { return m_rcMargin.top; }
	int   GetMarginR() { return m_rcMargin.right; }
	int   GetMarginB() { return m_rcMargin.bottom; }
	int   GetMarginW() { return m_rcMargin.left + m_rcMargin.right; }
	int   GetMarginH() { return m_rcMargin.top + m_rcMargin.bottom; }
	void  GetParentRect( RECT* prc );
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

	void  GetClientRectInObject(RECT* prc);
	void  GetClientRectInParent(RECT* prc);
	void  GetObjectClientRect(RECT* prc);
	void  GetClientRectInWindow(RECT* prc);
	
	bool  GetRectInLayer(RECT* prc, bool bOnlyVisiblePart);
	bool  GetRectInWindow(RECT* prc, bool bOnlyVisiblePart);
    bool  GetVisibleClientRectInLayer(RECT* prc);
	bool  CalcRectInAncestor(__in Object*  pObjAncestor,
		__in const RECT* prcObjPart, 
		__in bool bCalcVisible, 
		__out RECT* prcOut);

	void  SetObjectPos(RECT* prc, int nFlag=0 );
	virtual void  SetObjectPos( int x, int y, int cx, int cy, int nFlag=0);
    void  UpdateLayoutPos();

	POINT  GetWindowPoint();
	void  GetWindowRect(RECT* lprc);
	void  WindowPoint2ObjectPoint(const POINT* ptWindow, POINT* ptObj, bool bCalcTransform);
	void  WindowPoint2ObjectClientPoint(const POINT* ptWindow, POINT* ptObj, bool bCalcTransform);
	void  WindowRect2ObjectClientRect(const RECT* rcWindow, RECT* rcObj);
	void  WindowRect2ObjectRect(const RECT* rcWindow, RECT* rcObj);

    void  notify_WM_SIZE(UINT nType, UINT nWidth, UINT nHeight);
    void  notify_WM_MOVE(int x, int y);
    SIZE  GetDesiredSize();

	static void  ParentClientPoint2ChildPoint(Object* pObjChild, const POINT* pt, POINT* pOut);

	static void  ObjectPoint2ObjectClientPoint(Object* pObj, const POINT* pt, POINT* pOut);
    static void  ObjectPoint2ObjectNonClientPoint(Object* pObj, const POINT* pt, POINT* pOut);
	static void  ObjectRect2ObjectClientRect(Object* pObj, const RECT* prc, RECT* pOut);
	static void  ObjectClientRect2WindowRect(Object* pObj, LPCRECT prcClient, LPRECT prcWnd);
	static void  ObjectRect2WindowRect(Object* pObj, LPCRECT prcObj, LPRECT prcWnd);

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
	bool  SetFocusInWindow();
    bool  CanTabstop();
	bool  IsTabstop();
	void  SetTabstop(bool b);
    void  SetDefaultTabstop(bool b);
    bool  GetDefaultTabstop();
	virtual bool  IsVisible();
	bool  IsSelfVisible();
    bool  IsCollapsed();
    bool  IsSelfCollapsed();
	virtual bool  IsEnable();
	
	void  SetVisible(bool b, bool bRedraw=true, bool bUpdateLayout=true);
    void  SetVisibleEx(VISIBILITY_TYPE eType, bool bRedraw=true, bool bUpdateLayout=true);
	void  LoadVisibleEx(long);
	long  SaveVisibleEx();

	void  SetEnable(bool b, bool bUpdateNow=true, bool bNotify=true);
	void  SetDisableDirect(bool b);
	bool  IsSelfDisable();
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
	void  ModifyObjectStyle(ObjStyle* add, ObjStyle* remove);
	bool  TestObjectStyle(const ObjStyle& test);
    void  ModifyStyleEx(UINT add, UINT remove, bool bNotify);
    UINT  GetStyleEx() { return m_nStyle2; }
    void  SetStyleEx(UINT n) { m_nStyle2 = n; }
    bool  TestStyleEx(UINT nStyleEx){ if(m_nStyle2&nStyleEx)return true; return false; }
	bool  IsTransparent();
    void  SetTransparent(bool b);
    void  SetDefaultTransparent(bool b);
    bool  GetDefaultTransparent();
    bool  NeedClip();
	bool  IsNoClip();
    void  SetNoClip(bool b);
    void  SetClipClient(bool b);
    bool  NeedClipClient();

	void  SetAsNcObject(bool b);  // ObjTree中有引用
	bool  IsNcObject();           // ObjTree中有引用
	bool  IsRejectMouseMsgAll();
	void  SetRejectMouseMsgAll(bool b);
    void  SetRejectMouseMsgSelf(bool b);
    bool  IsRejectMouseMsgSelf();
    void  SetDefaultRejectMsgAll(bool b);
    bool  GetDefaultRejectMsgAll();

	void  SetCanRedraw(bool bReDraw);
	bool  CanRedraw();

	bool  testStateBit(UINT bit);
	void  setStateBit(UINT bit);
	void  clearStateBit(UINT bit);

public:
#pragma  endregion
	
protected:
	Object*  _findChildObjectItem(LPCTSTR szobjId, bool bFindDecendant);
	void  drawNcChildObject(IRenderTarget*, RenderContext& roc);
	void  parseLayoutAttribute(IMapAttribute* pMapAttrib);

    void  loadRenderBase(LPCTSTR szName, IRenderBase*& pRender);
    void  loadTextRender(LPCTSTR szName, ITextRenderBase*& pTextRender);
    LPCTSTR  getRenderBaseName(IRenderBase*& pRender);
    LPCTSTR  getTextRenderName(ITextRenderBase*& pTextRender);


public:
    void  SetAttributeByPrefix(LPCTSTR szPrefix, IMapAttribute* pMatAttrib, bool bReload, bool bErase);
    void  ParseStyleAndLoadAttribute(IMapAttribute* pMatAttrib, bool bReload);
	void  LoadAttributeFromXml(IUIElement* pElement, bool bReload);

    LPCTSTR  GetAttribute(LPCTSTR szKey, bool bErase);
    void  AddAttribute(LPCTSTR szKey, LPCTSTR  szValue);
    void  GetMapAttribute(IMapAttribute** ppMapAttribute);  // 需要release

protected:
    LRESULT  OnSkinTextureChanged(UINT, WPARAM, LPARAM);
    LRESULT  OnGetTooltipInfo(UINT, WPARAM, LPARAM);

    void*  QueryInterface(const IID* pIID);
	void  OnEraseBkgnd( IRenderTarget* );
	BOOL  OnSetCursor( HWND hWnd, UINT nHitTest, UINT message );
	UINT  OnHitTest(POINT* pt, __out POINT* ptInChild);
	void  OnThemeChanged();
    void  OnVisibleChanged(BOOL bVisible, IObject* pObjChanged);
    void  OnRedrawObject();

	HRESULT  FinalConstruct(IUIApplication* p);    
	void  FinalRelease();                       
	void  OnSerialize(SERIALIZEDATA* pData);

	//////////////////////////////////////////////////////////////////////////
	// virtual function
	//////////////////////////////////////////////////////////////////////////

protected:
	virtual void  virtualOnSize(UINT nType, UINT nWidth, UINT nHeight);
    virtual void  virtualOnMove(int x, int y);
	virtual void  virtualOnPostDrawObjectErasebkgnd(){};   // 用于CustomWindow更新窗口异形

	virtual void  virtualSetVisibleEx(VISIBILITY_TYPE eType);
	virtual void  virtualSetEnable(bool b);

    //////////////////////////////////////////////////////////////////////////
    // Member Var
    //////////////////////////////////////////////////////////////////////////

protected:

    IObject*   m_pIObject;
    UIApplication*   m_pUIApplication;  // 该对象所在的App

    String  m_strId;                    // 该对象在XML中的标识
    String  m_strStyleClass;            // 样式配置
    String  m_strTooltip;               // 提示信息

#pragma region //坐标相关数据
    CRect      m_rcParent;              // 该对象的范围，相对于parent的client区域.对于Window对象是客户区域位置，即左上角为0，0
    CRegion4   m_rcExtNonClient;        // 扩展的非客户区，与border、padding共同做为对象的非客户区。
    CRegion4   m_rcMargin;
    CRegion4   m_rcPadding;
    CRegion4   m_rcBorder;       
    HRGN       m_hRgn;                  // （未使用）如果该对象是一个不规则区域，必须设置该值，该值对window类型对象无效. rgn是相对于窗口左上角的。
    ILayoutParam*  m_pLayoutParam;      // 布局参数。由Object负责释放
#pragma endregion

    // 运行时对象数据
    short      m_lControlTypeEx; // 如位于sliderctrl中的button

	ObjStyle   m_objStyle;
	ObjState   m_objState;
    UINT       m_nStateBit;    // TODO:全部转换成ObjState。 对象的状态，如visible/disable

    long       m_lCanRedrawRef;         // 用于解决多处调用SetRedraw(false)后，必须所有地方放开SetRedraw(true)才能进行绘制
    UINT       m_nStyle2;               // 控件样式，与的关系
    long       m_lzOrder;               // 控件z序，用于实现控件重叠时的刷新判断依据

    IMapAttribute*    m_pIMapAttributeRemain;  // 用于扩展。未解析的属性
    IRenderBase*      m_pBkgndRender;          // 背景渲染
    IRenderBase*      m_pForegndRender;        // 前景渲染
    ITextRenderBase*  m_pTextRender;           // 文字渲染，由control负责读取该属性
    IUICursor*        m_pCursor;               // 对象的鼠标样式
    RenderLayer*      m_pRenderLayer;          // 该对象是否创建了一个layer
	IAccessible*      m_pAccessible;

    void*      m_pUserData;             // 自定义数据（目前是用于在UIBuilder做数据关联）
    void**     m_ppOutRef;              // 为了解决一个类成员对象，有可能被自己的父对象删除后，这个类却不知道，再删除该对象时崩溃了.
};

}

#endif  // OBJECT_H_763AA6BD_6435_456f_A41F_E5D7CCF39E7C

