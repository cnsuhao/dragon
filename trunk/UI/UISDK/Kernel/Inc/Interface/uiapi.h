#ifndef UIAPI_H_C4207403_E0A4_418b_B141_663726A982E7
#define UIAPI_H_C4207403_E0A4_418b_B141_663726A982E7

namespace UI
{
interface IMessage;
interface IUIApplication;
interface IObject;
struct    UIMSG;
interface IRenderTarget;
interface IRenderBitmap;
interface IRenderBase;
interface ITextRenderBase;
interface IUIElement;
interface ISkinRes;
interface ILayout;
interface IBuffer;

// UI对象创建函数
typedef  HRESULT (*funcUICreateInstancePtr)(IUIApplication* pUIApp, void** pOut);
typedef  HRESULT (*funcUICreateRenderBasePtr)(void** ppOut);
typedef  HRESULT (*funcUICreateTextRenderBasePtr)(void** ppOut);
typedef  HRESULT (*funcUIParseSkinElement)(IUIElement*, ISkinRes* pSkinRes);
typedef  HRESULT (*funcUIParseLayoutElement)(IUIElement*, IUIApplication*, IObject* pObjParent, IObject**);
typedef  HRESULT (*funcUICreateLayoutPtr)(IObject* pObject, ILayout**  ppLayout);

// 外部UI对象注册入口，由外部DLL实现该导出函数
#define FUN_RegisterUIObject_NAME  "RegisterUIObject"
typedef  HRESULT (*funRegisterUIObjectPtr)(IUIApplication* p);

UISDKAPI HRESULT CreateUIApplicationInstance(IUIApplication** pp);

UISDKAPI long    UIPostMessage(IUIApplication* pUIApp, UIMSG* pMsg, int nMsgMapID=0);
UISDKAPI long    UIPostMessage(HWND   hForwardMsgWnd,  UIMSG* pMsg, int nMsgMapID=0);
UISDKAPI long    UISendMessage(UIMSG* pMsg, int nMsgMapID=0, BOOL* pbHandled = NULL);
UISDKAPI long    UISendMessage(IMessage* pMsgTo, UINT message, WPARAM wParam = 0, LPARAM lParam = 0, 
                               UINT nCode = 0, IMessage* pMsgFrom = NULL, int nMsgMapID = 0, BOOL* pbHandled = NULL);

UISDKAPI void    _cdecl UI_LOG_DEBUG( TCHAR*, ... );
UISDKAPI void    _cdecl UI_LOG_INFO( TCHAR*, ... );
UISDKAPI void    _cdecl UI_LOG_WARN( TCHAR*, ... );
UISDKAPI void    _cdecl UI_LOG_ERROR( TCHAR*, ... );
UISDKAPI void    _cdecl UI_LOG_FATAL( TCHAR*, ... );
#define UI_LOG_TEST  UI_LOG_DEBUG

UISDKAPI IRenderTarget*  UICreateRenderTarget(IUIApplication* pUIApp, GRAPHICS_RENDER_LIBRARY_TYPE eType);   // IRenderTarget*没有引用计数机制，但仍采用release释放
UISDKAPI void  UICreateRenderBitmap(IUIApplication*  pUIApp, GRAPHICS_RENDER_LIBRARY_TYPE eGraphicsRenderType, IMAGE_ITEM_TYPE eType, IRenderBitmap** ppOut);
UISDKAPI GRAPHICS_RENDER_LIBRARY_TYPE  UIParseGraphicsRenderLibraryType(const TCHAR* szText);

interface IUIDocument;
UISDKAPI void  UILoadXmlDocument(const TCHAR* szXmlPath, IUIDocument** ppDoc);

// 创建一个内存位图，由外部负责Destroy
UISDKAPI HBITMAP CreateMemBitmap(int nWidth, int nHeight, int* pnPitch, byte** ppBits);

// 创建一个TCHAR buffer
// 用于解决返回一个TCHAR*字符串，但仍然由分配都去释放内存，调用者只需要调用Release
UISDKAPI void  CreateTCHARBuffer(IBuffer** pBuffer);

// 创建一个IDataObject，用于实现拖拽源。避免每个拖拽源都得再实现一次IDataObject IDropSource接口
UISDKAPI void  CreateDataObjectInstance(IDataObject**  pp);
UISDKAPI void  CreateDropSourceInstance(IDropSource**  pp);

}
#endif  // UIAPI_H_C4207403_E0A4_418b_B141_663726A982E7