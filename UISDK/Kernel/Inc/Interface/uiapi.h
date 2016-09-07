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
typedef  HRESULT (*pfnUICreateInstancePtr)(IUIApplication* pUIApp, ISkinRes* pSkinRes, void** pOut);
typedef  HRESULT (*pfnUICreateRenderBasePtr)(void** ppOut);
typedef  HRESULT (*pfnUICreateTextRenderBasePtr)(void** ppOut);
typedef  HRESULT (*pfnParseSkinTag)(IUIElement*, ISkinRes* pSkinRes);
typedef  HRESULT (*pfnUICreateLayoutPtr)(IObject* pObject, ILayout**  ppLayout);

// uiapplication中的枚举回调
typedef  bool (*pfnEnumLayoutTypeCallback)(long, LPCTSTR, WPARAM, LPARAM);
typedef  void (*pfnEnumRenderBaseNameCallback)(LPCTSTR, WPARAM, LPARAM);
typedef  void (*pfnEnumTextRenderBaseNameCallback)(LPCTSTR, WPARAM, LPARAM);

typedef  void (*pfnEnumGraphicsRenderLibraryTypeCallback)(long, LPCTSTR, WPARAM, LPARAM);

// 返回值:
enum  PARSE_CONTROL_RETURN
{
	ParseControl_Failed,  // 解析错误
	ParseControl_CreateObject, // 解析，并创建了该对象，但没有加载属性
	ParseControl_LoadObject,   // 创建对象并加载其属性，但处没处理子结点
	ParseControl_LoadDescendants,  // 加载自己和所有子结点的属性
};
typedef  PARSE_CONTROL_RETURN (*pfnParseControlTag)(IUIElement*, ISkinRes*, IObject* pObjParent, IObject**);

// 外部UI对象注册入口，由外部DLL实现该导出函数
#define FUN_RegisterUIObject_NAME  "RegisterUIObject"
typedef  HRESULT (*funRegisterUIObjectPtr)(IUIApplication* p);


// 编译时版本定义
#define UISDK_VERSION_MAJOR   0    // 主版本
#define UISDK_VERSION_MINOR   11   // 次版本
#define UISDK_VERSION_PATCH   0    // 补丁版本

UISDKAPI bool  CreateUIApplicationInstance(IUIApplication** pp);

UISDKAPI long  UIPostMessage(IUIApplication* pUIApp, UIMSG* pMsg, int nMsgMapID=0);
UISDKAPI long  UIPostMessage(HWND   hForwardMsgWnd,  UIMSG* pMsg, int nMsgMapID=0);
UISDKAPI long  UISendMessage(UIMSG* pMsg, int nMsgMapID=0, BOOL* pbHandled = NULL);
UISDKAPI long  UISendMessage(IMessage* pMsgTo, UINT message, WPARAM wParam = 0, LPARAM lParam = 0, 
                               UINT nCode = 0, IMessage* pMsgFrom = NULL, int nMsgMapID = 0, BOOL* pbHandled = NULL);


// IRenderTarget*没有引用计数机制，但仍采用release释放
UISDKAPI IRenderTarget*  UICreateRenderTarget(IUIApplication* pUIApp, GRAPHICS_RENDER_LIBRARY_TYPE eType, bool bNeedAlphaChannel);
UISDKAPI void  UICreateRenderBitmap(IUIApplication*  pUIApp, GRAPHICS_RENDER_LIBRARY_TYPE eGraphicsRenderType, IMAGE_ITEM_TYPE eType, IRenderBitmap** ppOut);
UISDKAPI GRAPHICS_RENDER_LIBRARY_TYPE  UIParseGraphicsRenderLibraryType(const TCHAR* szText);

interface IUIDocument;
UISDKAPI void  UILoadXmlDocument(const TCHAR* szXmlPath, IUIDocument** ppDoc);

// 创建一个内存位图，由外部负责Destroy
UISDKAPI HBITMAP CreateMemBitmap(int nWidth, int nHeight, int* pnPitch, byte** ppBits);

// 创建一个TCHAR buffer
// 用于解决返回一个TCHAR*字符串，但仍然由分配都去释放内存，调用者只需要调用Release
UISDKAPI void  CreateTCHARBuffer(IBuffer** pBuffer);

// 创建一个IDataObject，用于实现拖拽源、Ole对象。避免每个拖拽源都得再实现一次IDataObject IDropSource接口
interface  IDataObjectSource
{
    virtual BOOL OnRenderData(
		IDataObject* pDataObject,
        LPFORMATETC lpFormatEtc,
        LPSTGMEDIUM lpStgMedium 
        ) PURE;

	virtual void  OnDataRelease(
		IDataObject* p
		) PURE;
};

// DROPEFFECT_COPY/DROPEFFECT_MOVE等的基础上，添加一个WAITING，
// 用于可以接受拖拽，但还没有准备好的情况。
#define DROPEFFECT_NOTREADY  0x00001000

// 用于拖拽过程中，显示更详细的信息。如果拖拽文本
interface IDragFeedback
{
    virtual void  SetDescription(HBITMAP hIcon, LPCWSTR szText) PURE;
    virtual HRESULT  GiveFeedback(DWORD dwEffect) PURE;
};

//
// 1. 用于设置/取消IDataObject的IDataObjectSource对象，避免IDataObject被
//    AddRef引用，但IDataObjectSource却先释放了，导致最后出现崩溃的问题
// 2. 用于保存/获取IDragFeedback接口
//
// {30BF2E89-84D8-4d54-98B8-7A5CCE9E46C1}
static const GUID IID_IDataObjectEx = 
{ 0x30bf2e89, 0x84d8, 0x4d54, { 0x98, 0xb8, 0x7a, 0x5c, 0xce, 0x9e, 0x46, 0xc1 } };
interface IDataObjectEx : public IUnknown
{
    virtual void  SetSource(IDataObjectSource* p) PURE;
    virtual void  SetDragFeedback(IDragFeedback* p) PURE;
    virtual IDragFeedback*  GetDragFeedback() PURE;
};

// {4F0BD04A-BBD4-497b-91EA-C3362E573AC6}
static const GUID IID_IDropSourceEx = 
{ 0x4f0bd04a, 0xbbd4, 0x497b, { 0x91, 0xea, 0xc3, 0x36, 0x2e, 0x57, 0x3a, 0xc6 } };
interface IDropSourceEx : public IUnknown
{
    virtual void  SetDragFeedback(IDragFeedback* p) PURE;
    virtual IDragFeedback*  GetDragFeedback() PURE;
};

UISDKAPI void  CreateDataObjectInstance(IDataObject**  pp);
UISDKAPI void  CreateDropSourceInstance(IDropSource**  pp);
UISDKAPI LRESULT  UIDoDragDrop(
					IDataObject* pDataObject,   // Pointer to the data object
					IDropSource* pDropSource,   // Pointer to the source
					DWORD dwOKEffect,           // Effects allowed by the source
					DWORD* pdwEffect,           // Pointer to effects on the source
					HBITMAP hbmpDragImage,      // The drag image's bitmap handle 
					POINT ptOffset);            // A POINT structure that specifies the location of the cursor within the drag image.

}
#endif  // UIAPI_H_C4207403_E0A4_418b_B141_663726A982E7