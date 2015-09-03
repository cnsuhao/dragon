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
typedef  HRESULT (*pfnUICreateInstancePtr)(IUIApplication* pUIApp, ISkinRes* pSkinRes, void** pOut);
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
typedef  PARSE_CONTROL_RETURN (*pfnParseControlTag)(IUIElement*, ISkinRes*, IObject* pObjParent, IObject**);

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

// DROPEFFECT_COPY/DROPEFFECT_MOVE�ȵĻ����ϣ����һ��WAITING��
// ���ڿ��Խ�����ק������û��׼���õ������
#define DROPEFFECT_NOTREADY  0x00001000

// ������ק�����У���ʾ����ϸ����Ϣ�������ק�ı�
interface IDragFeedback
{
    virtual void  SetDescription(HBITMAP hIcon, LPCWSTR szText) PURE;
    virtual HRESULT  GiveFeedback(DWORD dwEffect) PURE;
};

//
// 1. ��������/ȡ��IDataObject��IDataObjectSource���󣬱���IDataObject��
//    AddRef���ã���IDataObjectSourceȴ���ͷ��ˣ����������ֱ���������
// 2. ���ڱ���/��ȡIDragFeedback�ӿ�
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