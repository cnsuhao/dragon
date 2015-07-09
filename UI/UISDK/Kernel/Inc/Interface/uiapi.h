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

// UI���󴴽�����
typedef  HRESULT (*pfnUICreateInstancePtr)(IUIApplication* pUIApp, void** pOut);
typedef  HRESULT (*pfnUICreateRenderBasePtr)(void** ppOut);
typedef  HRESULT (*pfnUICreateTextRenderBasePtr)(void** ppOut);
typedef  HRESULT (*pfnParseSkinTag)(IUIElement*, ISkinRes* pSkinRes);
typedef  HRESULT (*pfnUICreateLayoutPtr)(IObject* pObject, ILayout**  ppLayout);

// uiapplication�е�ö�ٻص�
typedef  bool (*pfnEnumLayoutTypeCallback)(long, LPCTSTR, WPARAM, LPARAM);
typedef  void (*pfnEnumRenderBaseNameCallback)(LPCTSTR, WPARAM, LPARAM);
typedef  void (*pfnEnumTextRenderBaseNameCallback)(LPCTSTR, WPARAM, LPARAM);

typedef  void (*pfnEnumGraphicsRenderLibraryTypeCallback)(long, LPCTSTR, WPARAM, LPARAM);

// ����ֵ:
enum  PARSE_CONTROL_RETURN
{
	ParseControl_Failed,  // ��������
	ParseControl_CreateObject, // �������������˸ö��󣬵�û�м�������
	ParseControl_LoadObject,   // �������󲢼��������ԣ�����û�����ӽ��
	ParseControl_LoadDescendants,  // �����Լ��������ӽ�������
};
typedef  PARSE_CONTROL_RETURN (*pfnParseControlTag)(IUIElement*, IUIApplication*, IObject* pObjParent, IObject**);

// �ⲿUI����ע����ڣ����ⲿDLLʵ�ָõ�������
#define FUN_RegisterUIObject_NAME  "RegisterUIObject"
typedef  HRESULT (*funRegisterUIObjectPtr)(IUIApplication* p);


// ����ʱ�汾����
#define UISDK_VERSION_MAJOR   0    // ���汾
#define UISDK_VERSION_MINOR   11   // �ΰ汾
#define UISDK_VERSION_PATCH   0    // �����汾

UISDKAPI bool  CreateUIApplicationInstance(IUIApplication** pp);

UISDKAPI long  UIPostMessage(IUIApplication* pUIApp, UIMSG* pMsg, int nMsgMapID=0);
UISDKAPI long  UIPostMessage(HWND   hForwardMsgWnd,  UIMSG* pMsg, int nMsgMapID=0);
UISDKAPI long  UISendMessage(UIMSG* pMsg, int nMsgMapID=0, BOOL* pbHandled = NULL);
UISDKAPI long  UISendMessage(IMessage* pMsgTo, UINT message, WPARAM wParam = 0, LPARAM lParam = 0, 
                               UINT nCode = 0, IMessage* pMsgFrom = NULL, int nMsgMapID = 0, BOOL* pbHandled = NULL);


// IRenderTarget*û�����ü������ƣ����Բ���release�ͷ�
UISDKAPI IRenderTarget*  UICreateRenderTarget(IUIApplication* pUIApp, GRAPHICS_RENDER_LIBRARY_TYPE eType, bool bNeedAlphaChannel);
UISDKAPI void  UICreateRenderBitmap(IUIApplication*  pUIApp, GRAPHICS_RENDER_LIBRARY_TYPE eGraphicsRenderType, IMAGE_ITEM_TYPE eType, IRenderBitmap** ppOut);
UISDKAPI GRAPHICS_RENDER_LIBRARY_TYPE  UIParseGraphicsRenderLibraryType(const TCHAR* szText);

interface IUIDocument;
UISDKAPI void  UILoadXmlDocument(const TCHAR* szXmlPath, IUIDocument** ppDoc);

// ����һ���ڴ�λͼ�����ⲿ����Destroy
UISDKAPI HBITMAP CreateMemBitmap(int nWidth, int nHeight, int* pnPitch, byte** ppBits);

// ����һ��TCHAR buffer
// ���ڽ������һ��TCHAR*�ַ���������Ȼ�ɷ��䶼ȥ�ͷ��ڴ棬������ֻ��Ҫ����Release
UISDKAPI void  CreateTCHARBuffer(IBuffer** pBuffer);

// ����һ��IDataObject������ʵ����קԴ��Ole���󡣱���ÿ����קԴ������ʵ��һ��IDataObject IDropSource�ӿ�
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
UISDKAPI void  CreateDataObjectInstance(IDataObject**  pp, IDataObjectSource* pDataobjectSource);
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