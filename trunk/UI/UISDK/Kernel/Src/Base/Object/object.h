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
	RECT  rcDamage;     // client rect. ��ˢ�µĶ������Լ��������ڵ�������
    bool  bDrawBkgnd;   // �Ƿ���Ҫ���»����Լ��ı���
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

	HRESULT  FinalConstruct(IUIApplication* p);    // ������ʵ�ָ��麯�������ȵ��ø����ʵ��
	void     FinalRelease();                       // ������ʵ�ָ��麯�������ȵ��ø����ʵ��           

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

    // ��չ���Բ���
    void                InitDefaultAttrib();
    void                SetUserData(LPVOID p);
    LPVOID              GetUserData();
    IUIApplication*     GetUIApplication();
    void                SetUIApplication(IUIApplication* p);
    void                SetOutRef(void** ppOutRef);
    HRGN                GetRgn();
    int                 GetZOrder();
    void                SetZorderDirect(int z);

    // ����
    bool                SetCursor(const TCHAR* szCursorId);
    DWORD               CalcContrastTextColor();
    bool                SetMouseCapture(int nNotifyMsgId);
    bool                ReleaseMouseCapture();
    bool                SetKeyboardCapture(int nNotifyMsgId);
    bool                ReleaseKeyboardCapture();

    //////////////////////////////////////////////////////////////////////////
    // ����
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
    //  �����������
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

    // Canvas����ר�ýӿ�
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
    // ��ʽ����
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

	void  SetAsNcObject(bool b);  // ObjTree��������
	bool  IsNcObject();           // ObjTree��������
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
    //  ����
    //////////////////////////////////////////////////////////////////////////

public:
	// ��xml�õ�����ת�ɶ�������ԣ�ע�⣬�������ظú���ʱ�������ȵ��ø���ĸ÷���
	// bReload��ʾΪ����ʱ���ã���ʱ�����ٸ�ĳЩ���Ը�ֵ������text����
	bool  SetAttribute(IMapAttribute* pMapAttrib, bool bReload);
	void  OnSerialize(SERIALIZEDATA* pData);

	// ��UIObjCreator����Object�ͻ���ʱ���������ú�������Ϊ����ʱ��������������Դ�������ResetAttribute��Ҫ�ͷ�������Դ����
	// ע�⣬�������ظú���ʱ�������ȵ��ø���ĸ÷���
	void  ResetAttribute();

	void  OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA*  pData);
//	bool  SetChildObjectAttribute(Object* pChildObj, const String& strPrefix, IMapAttribute* pMapAttrib, bool bReload);
    void  SetAttributeByPrefix(const TCHAR* szPrefix, IMapAttribute* pMatAttrib, bool bReload, bool bErase);
    void  ParseStyleAndSetAttribute(IMapAttribute* pMatAttrib, bool bReload);

    const TCHAR*  GetAttribute(const TCHAR* szKey, bool bErase);
    void  AddAttribute(const TCHAR* szKey, const TCHAR*  szValue);
    void  GetMapAttribute(IMapAttribute** ppMapAttribute);  // ��Ҫrelease

    //////////////////////////////////////////////////////////////////////////
    // ��Ϣ����
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
	virtual void  virtualOnPostDrawObjectErasebkgnd(){};   // ����CustomWindow���´�������

    //////////////////////////////////////////////////////////////////////////
    // Member Var
    //////////////////////////////////////////////////////////////////////////

protected:

    IObject*          m_pIObject;
    IUIApplication*   m_pUIApplication;        // �ö������ڵ�App(Ϊ��ʵ�ֶ�ʵ������app������Ϊȫ�ֱ�����ÿ��Object�ܹ��������õ�app�ӿ�)
    String            m_strId;                 // �ö�����XML�еı�ʶ
    
#pragma region //�����������
    CRect             m_rcParent;              // �ö���ķ�Χ�������parent��client����.����Window�����ǿͻ�����λ�ã������Ͻ�Ϊ0��0
    CRegion4          m_rcNonClient;           // ���û�й�������Header������ռ�ÿռ�Ŀؼ���m_rcNonClient��Ϊm_rcPadding+m_rcBorder
    CRegion4          m_rcMargin;
    CRegion4          m_rcPadding;
    CRegion4          m_rcBorder;       
    HRGN              m_hRgn;                  // ��δʹ�ã�����ö�����һ�����������򣬱������ø�ֵ����ֵ��window���Ͷ�����Ч. rgn������ڴ������Ͻǵġ�
    ILayoutParam*     m_pLayoutParam;          // ���ֲ�������Object�����ͷ�
#pragma endregion

    int               m_nCanRedrawRef;         // TODO: ���ڽ���ദ����SetRedraw(false)�󣬱������еط��ſ�SetRedraw(true)���ܽ��л���
    UINT              m_nStateBit;             // �����״̬����visible/disable
    UINT              m_nStyle;                // ��ͨ��ʽ
    UINT              m_nStyle2;               // �ؼ���ʽ����Ĺ�ϵ
    int               m_nzOrder;               // �ؼ�z������ʵ�ֿؼ��ص�ʱ��ˢ���ж�����
    LPVOID            m_pUserData;             // �Զ������ݣ�Ŀǰ��������UIBuilder�����ݹ�����
    IMapAttribute*    m_pIMapAttributeRemain;  // ������չ��δ���������ԣ�����Tooltip

    IRenderBase*      m_pBkgndRender;          // ������Ⱦ
    IRenderBase*      m_pForegndRender;        // ǰ����Ⱦ
    ITextRenderBase*  m_pTextRender;           // ������Ⱦ
    IUICursor*        m_pCursor;               // ����������ʽ
    Object3DWrap*     m_pObject3DWrap;         // 3d��װ��
    RenderLayer2*     m_pRenderLayer;          // �ö����Ƿ񴴽���һ��layer
    
    void**            m_ppOutRef;              // Ϊ�˽��һ�����Ա�����п��ܱ��Լ��ĸ�����ɾ���������ȴ��֪������ɾ���ö���ʱ������.
};

}

#endif  // OBJECT_H_763AA6BD_6435_456f_A41F_E5D7CCF39E7C

