#ifndef UIDEFINE_H_4C746653_0625_41a4_8DE7_F18E7116CCFF
#define UIDEFINE_H_4C746653_0625_41a4_8DE7_F18E7116CCFF

#include <map>
using std::map;

// ���뵼������
#ifdef UI_DLL_EXPORTS
#define UISDKAPI __declspec(dllexport)
#else
#define UISDKAPI __declspec(dllimport)
#endif

#define IxxxMETHOD  inline

namespace UI
{
    
typedef map<String,String>  ATTRMAP;
#ifndef uint
typedef unsigned int uint;
#endif

// ����������
#define WND_CLASS_NAME                  _T("UI")
#define WND_ANIMATE_CLASS_NAME          _T("UI_Animate")
#define WND_POPUP_CONTROL_SHADOW_NAME   _T("UI_PopupControlShadow") // ��ϵͳ��Ӱ����
#define WND_POPUP_CONTROL_NAME          _T("UI_PopupControl")
#define WND_DRAGBITMAPWND_CLASS_NAME    _T("UI_DragBitmapWnd")

// ���⺬�峣��
#define NDEF  -1                               // û�ж���ı���
#define AUTO  -2                               // �Զ���������width/height=auto��

#define MAX_STRING_LEN    256

// ��ʱ�ļ���׺��
#define TEMP_FILE_EXT    _T(".tmp")

// �Ƿ�����ֵ 
enum HANDLED_VALUE
{
	NOT_HANDLED = 0,
	HANDLED = 1, 
};

//////////////////////////////////////////////////////////////////////////
// enum

// ��Դ���Ͷ���
typedef enum
{
	UI_RESOURCE_NULL       = 0,
	UI_RESOURCE_PROJECT    = 0x0001,
	UI_RESOURCE_SKIN       = 0x0002,
	UI_RESOURCE_IMAGE      = 0x0004,
	UI_RESOURCE_FONT       = 0x0008,
	UI_RESOURCE_COLOR      = 0x0010,
	UI_RESOURCE_STYLE      = 0x0020,
	UI_RESOURCE_LAYOUT     = 0x0040,
	UI_RESOURCE_I18N       = 0x0080,
	UI_RESOURCE_LOG        = 0x0100,
	UI_RESOURCE_LOGMODULE  = 0x0200,
	UI_RESOURCE_SOURCEFILE = 0x0400,  // ��Դ�������ڵ��ļ�������UIBuilder�鿴�ļ�
    UI_RESOURCE_MENU       = 0x0800,
}
UI_RESOURCE_TYPE;



// 32λ�Ķ��������У���Ϊ�����֣�OBJ_TYPE ռ��8λ��Ŀǰ��Ҫֻ��3�����͡�
// CONTROL_TYPE ռ�� 16 λ��CONTROL_SUB_TYPE ռ�� 8 λ
#define  OBJ_TYPE_OFFSET         24
#define  CONTROL_TYPE_OFFSET     8
#define  CONTROL_SUB_TYPE_OFFSET 0
enum OBJ_TYPE
{
    OBJ_NULL             = 0 << OBJ_TYPE_OFFSET,
    OBJ_CONTROL          = 1 << OBJ_TYPE_OFFSET,
    OBJ_HWNDHOST         = 2 << OBJ_TYPE_OFFSET,
    OBJ_PANEL            = 3 << OBJ_TYPE_OFFSET,
    OBJ_WINDOW           = 4 << OBJ_TYPE_OFFSET,
};
enum CONTROL_TYPE
{
    // Extent Type
    CONTROL_BUTTON       = 1 << CONTROL_TYPE_OFFSET,
    CONTROL_GROUPBOX     = 2 << CONTROL_TYPE_OFFSET,
    CONTROL_CHECKBUTTON  = 3 << CONTROL_TYPE_OFFSET,
    CONTROL_RADIOBUTTON  = 4 << CONTROL_TYPE_OFFSET,
    CONTROL_HYPERLINK    = 5 << CONTROL_TYPE_OFFSET,
    CONTROL_LABEL        = 6 << CONTROL_TYPE_OFFSET,
    CONTROL_PICTURE      = 7 << CONTROL_TYPE_OFFSET,
    CONTROL_EDIT         = 8 << CONTROL_TYPE_OFFSET,
    CONTROL_PROGRESSBAR  = 9 << CONTROL_TYPE_OFFSET,
    CONTROL_SLIDERCTRL   = 10 << CONTROL_TYPE_OFFSET,
    CONTROL_LISTBOX      = 11 << CONTROL_TYPE_OFFSET,
    CONTROL_POPUPLISTBOX = 12 << CONTROL_TYPE_OFFSET,
    CONTROL_HEADERCTRL   = 13 << CONTROL_TYPE_OFFSET,
    CONTROL_LISTVIEWCTRL = 14 << CONTROL_TYPE_OFFSET,
    CONTROL_HSCROLLBAR   = 15 << CONTROL_TYPE_OFFSET,
    CONTROL_VSCROLLBAR   = 16 << CONTROL_TYPE_OFFSET,
    CONTROL_COMBOBOX     = 17 << CONTROL_TYPE_OFFSET,
    CONTROL_RICHEDIT     = 18 << CONTROL_TYPE_OFFSET,
    CONTROL_LEDCTRL      = 19 << CONTROL_TYPE_OFFSET,
    CONTROL_GIFCTRL      = 20 << CONTROL_TYPE_OFFSET,
    CONTROL_MENU         = 21 << CONTROL_TYPE_OFFSET,
//    CONTROL_TOOLTIPWINDOW= 22 << CONTROL_TYPE_OFFSET, -> WINDOW_TOOLTIP
    CONTROL_TREEVIEWCTRL = 23 << CONTROL_TYPE_OFFSET,
    CONTROL_LISTTREEVIEWCTRL = 24 << CONTROL_TYPE_OFFSET,
    CONTROL_FLASH        = 25 << CONTROL_TYPE_OFFSET,
    CONTROL_IE           = 26 << CONTROL_TYPE_OFFSET,
    CONTROL_LISTCTRLITEM = 27 << CONTROL_TYPE_OFFSET, 
    CONTROL_TABCTRL      = 28 << CONTROL_TYPE_OFFSET, 
    CONTROL_SPLITTERBAR  = 29 << CONTROL_TYPE_OFFSET,
    CONTROL_CEF          = 30 << CONTROL_TYPE_OFFSET,
};

enum WINDOW_TYPE
{
    WINDOW_TOOLTIP = 1 << CONTROL_TYPE_OFFSET
};

enum PANEL_TYPE
{
    PANEL_SCROLL           = 1 << CONTROL_TYPE_OFFSET,
    PANEL_LISTCTRLITEMROOT = 2 << CONTROL_TYPE_OFFSET,
    PANEL_STAGE3D          = 3 << CONTROL_TYPE_OFFSET,
};

enum LAYER_TYPE
{
    LAYER_DIRECT   = 1 << CONTROL_TYPE_OFFSET,
    LAYER_BUFFER   = 2 << CONTROL_TYPE_OFFSET,
    LAYER_3D       = 3 << CONTROL_TYPE_OFFSET,
};


#define TESTWNDSTYLE(hWnd, flag) \
    GetWindowLong(hWnd, GWL_STYLE)&flag
#define ADDWNDSTYLE(hWnd, flag) \
    SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE)|flag)
#define REMOVEWNDSTYLE(hWnd, flag) \
    SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE)&~flag)

inline OBJ_TYPE GetObjectMajorType(int type)  { return  (OBJ_TYPE)((type)&0xFF000000);  } 
inline CONTROL_TYPE GetObjectExtentType(int type) { return  (CONTROL_TYPE)((type)&0x00FFFF00);  }  

#pragma region // Object, RenderBase ��չ��

#define UI_DECLARE_OBJECT(className, type)  \
    UI_DECLARE_OBJECT_INFO(className, type) \
    UI_DECLARE_QUERY_INTERFACE(className)

#define UI_DECLARE_OBJECT_INFO(className, type)   \
    static LPCTSTR  GetXmlName() { return _T(#className);  } \
    static UINT    GetObjType() { return type; }            \
    void  OnGetObjectInfo(TCHAR** ppszName, int* pObjType)  \
    {                                                       \
        if (ppszName)*ppszName = _T(#className);            \
        if (pObjType)*pObjType = type;                      \
    }                                                       \

#define UI_DECLARE_QUERY_INTERFACE(className)               \
    void*  QueryInterface(const IID* pIID)                  \
    {                                                       \
        if (IsEqualIID(*pIID, uiiidof(I##className)))       \
        {                                                   \
            return (void*)m_pI##className;                  \
        }                                                   \
        SetMsgHandled(FALSE);                               \
        return NULL;                                        \
    }                                                       \

#define UI_DECLARE_RENDERBASE(className, xml, rendertype)   \
    static LPCTSTR  GetXmlName() { return xml; }       \
    static int  GetType() { return rendertype; }      \
    UI_DECLARE_QUERY_INTERFACE(className)                   \
    typedef UI::UIObjCreator<I##className> _CreatorClass;


// ���궨����Ҫ������theme���͵�renderbase��Ҫ���ݿؼ����ͽ��д���
#define UI_DECLARE_RENDERBASE2(className, xml, rendertype) \
    static LPCTSTR  GetXmlName() { return xml; }       \
    static int  GetType() { return rendertype; }      \
    typedef UI::UIObjCreator<className> _CreatorClass;  // ע������û����I##className����ΪĬ������themeû��Ixxx����


#define UI_DECLARE_TEXTRENDERBASE(className, xml, rendertype) \
    static LPCTSTR  GetXmlName() { return xml; }       \
    static int  GetType() { return rendertype; }      \
    UI_DECLARE_QUERY_INTERFACE(className)                   \
    typedef UI::UIObjCreator<I##className> _CreatorClass;

#define UI_DECLARE_TEXTRENDERBASE2(className, xml, rendertype) \
    static LPCTSTR  GetXmlName() { return xml; }       \
    static int  GetType() { return rendertype; }      \
    typedef UI::UIObjCreator<className> _CreatorClass;   // ע������û����I##className����ΪĬ������themeû��Ixxx����

#define REGISTER_UI_OBJECT(pUIApp, category, classname)   \
        pUIApp->RegisterUIObjectCreateData(     \
                classname::GetXmlName(),        \
                category,                       \
                classname::GetObjType(),        \
				uiiidof(I##classname),          \
                (UI::pfnUICreateInstancePtr)I##classname::_CreatorClass::UICreateInstance);

#define REGISTER_UI_RENDERBASE(pUIApp, classname) \
        pUIApp->RegisterUIRenderBaseCreateData(   \
                classname::GetXmlName(),          \
                classname::GetType(),             \
                (UI::pfnUICreateRenderBasePtr)classname::_CreatorClass::UICreateInstance2);

#define REGISTER_UI_TEXTRENDERBASE(pUIApp, classname) \
        pUIApp->RegisterUITextRenderBaseCreateData( \
                classname::GetXmlName(),          \
                classname::GetType(),             \
                (UI::pfnUICreateTextRenderBasePtr)classname::_CreatorClass::UICreateInstance2);

#define REGISTER_UI_LAYOUT(pUIApp, layoutname, layouttype, func) \
        pUIApp->RegisterLayoutCreateData(layoutname, layouttype, func);

#define REGISTER_XML_TAG_PARSE(pUIApp, tagname, func) \
	pUIApp->RegisterControlTagParseFunc(tagname, func);

#pragma endregion

// object��state bit��״̬λ
#define OSB_UNVISIBLE     0x0001        // �Լ��Ƿ�ɼ�
#define OSB_COLLAPSED     0x0002        // listitemΪ������������ʾ�ӽ�㣨TODO:�Ժ��������չΪ���ز��Ҳ�ռ�ò���?��
#define OSB_DISABLE       0x0004        // �Լ��Ƿ����

#define OSB_PRESS         0x0010
#define OSB_HOVER         0x0020
#define OSB_FORCEPRESS    0x0040
#define OSB_FORCEHOVER    0x0080         
#define OSB_READONLY      0x0100        // �����ɸ����ؼ��Լ�ʵ�ָ���һЩ��
#define OSB_FOCUS         0x0200
#define OSB_DEFAULT       0x0400
#define OSB_SELECTED      0x0800         // ��ѡ��
#define OSB_CHECKED       0x1000
#define OSB_RADIOCHECKED  0x2000
#define OSB_DRAGDROPHOVER 0x4000        // ��ק�����У�λ������¡�Ŀǰ��listitemʹ��

#define WSB_ACTIVE        0x1000         // ����Ϊactive

// ����
// STYLEEX���ھ���ؼ���ʽ��ͬʱ��STYLEEX�������λ���ڱ�ʶ�ؼ����ͣ����簴ť����;
// #define GETCONTROLSUBTYPE(nStyleEx)  (nStyleEx & 0x000000FF)
// #define MAKECONTROLSUBTYPE(nStyleEx, subStyle)  ((nStyleEx & 0xFFFFFF00) | subStyle)


// UI_WM_NOTIFY ��Ϣ��lParam��ʹ�õĽṹ��
struct UINMHDR
{
    int reverse;
};


// UI_WM_DROPTARGETEVENT ��Ϣ����
enum DROPTARGETEVENT_TYPE
{
    _DragEnter,
    _DragOver,
    _DragLeave,
    _Drop,
};
interface IDragFeedback;
struct DROPTARGETEVENT_DATA
{
    IDataObject*  pDataObj;
    DWORD         grfKeyState;
    POINTL        pt;
    DWORD*        pdwEffect;
    IDragFeedback*  pDragFeedback;
};

}
// These two messages aren't defined in winuser.h, but they are sent to windows  
// with captions. They appear to paint the window caption and frame.  
// Unfortunately if you override the standard non-client rendering as we do  
// with CustomFrameWindow, sometimes Windows (not deterministically  
// reproducibly but definitely frequently) will send these messages to the  
// window and paint the standard caption/title over the top of the custom one.  
// So we need to handle these messages in CustomFrameWindow to prevent this  
// from happening.  
#define WM_NCUAHDRAWCAPTION 0xAE
#define WM_NCUAHDRAWFRAME   0xAF  


#pragma region // Ixxx�ӿ���ʵ�ֺ�

// ����һ���ӿ���Ļ�����Ա�ͷ�����������������Ϊprotected��Ա��ǿ���ⲿʹ��
// SAFE_DELETE_Ixxx���ͷŸ�����
#define UI_DECLARE_Ixxx_INTERFACE(interfacename, classname)  \
    UI_DECLARE_Ixxx_INTERFACE_Construct(interfacename, classname) \
    UI_DECLARE_Ixxx_INTERFACE_ProcessMessage(interfacename, classname) \
    UI_DECLARE_Ixxx_INTERFACE_CreateImpl(interfacename, classname)

#define UI_DECLARE_Ixxx_INTERFACE_Construct(interfacename, classname) \
public:                                    \
    typedef UI::UIObjCreator<interfacename> _CreatorClass; \
    static void  CreateInstance(UI::IUIApplication* p, UI::ISkinRes* pSkinRes, interfacename** pp); \
	static void  CreateInstance(UI::ISkinRes* pSkinRes, interfacename** pp); \
    interfacename();                       \
protected:                                 \
    ~interfacename();                      \
protected:                                 \
    classname*  m_p##classname##Impl;      \
    BOOL  m_bCreate##classname##Impl;      \
public:

// ��UIObject����Ҳ����pimplģʽ(���ᱻ�̳�)��
#define UI_DECLARE_Ixxx_INTERFACE_Construct2(interfacename, classname) \
public:                                    \
    interfacename();                       \
    void  delete_this();                   \
protected:                                 \
    ~interfacename();                      \
protected:                                 \
    classname*  m_p##classname##Impl;      \
    BOOL  m_bCreate##classname##Impl;      \
public:

#define UI_DECLARE_Ixxx_INTERFACE_ProcessMessage(interfacename, classname) \
    BOOL  nvProcessMessage(UI::UIMSG* pMsg, int nMsgMapID, bool bDoHook); \
private:                                   \
    BOOL  virtualProcessMessage(UI::UIMSG* pMsg, int nMsgMapID, bool bDoHook); \
public:

#define UI_DECLARE_Ixxx_INTERFACE_CreateImpl(interfacename, classname) \
    void  CreateImpl(classname* p=NULL);   \
    classname*  GetImpl();                 \

//
// ʵ��һ���ӿ���
// ע��
//   1. ��Щ�ӿ�Ҫ�ڹ��캯���г�ʼ����ĳ�Ա��������long m_lRef����˽����캯���������
//   2. ������IButtonBase���ֽӿڣ�����ButtonBaseû��ֱ�Ӵ�Control��̳У������Ҫ��
//      IButtonBase::CreateImpl�е�������һ��IControl::CreateImpl(NULL)����˽�CreateImpl�������
//

// ��ǰģ��̳�
#define UI_IMPLEMENT_Ixxx_INTERFACE(interfacename, classname, basename)  \
    UI_IMPLEMENT_Ixxx_INTERFACE_Construct(interfacename, classname)      \
    UI_IMPLEMENT_Ixxx_INTERFACE_ProcessMessage(interfacename, classname) \
    UI_IMPLEMENT_Ixxx_INTERFACE_CreateImpl(interfacename, classname, basename)

// ��ģ��̳�
#define UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(interfacename, classname, baseinterface)  \
    UI_IMPLEMENT_Ixxx_INTERFACE_Construct(interfacename, classname)      \
    UI_IMPLEMENT_Ixxx_INTERFACE_ProcessMessage(interfacename, classname) \
    UI_IMPLEMENT_Ixxx_INTERFACE_CreateImpl_ACROSSMODULE(interfacename, classname, baseinterface)

#define UI_IMPLEMENT_Ixxx_INTERFACE_Construct(interfacename, classname) \
    interfacename::interfacename()                          \
    {                                                       \
        m_p##classname##Impl = NULL;                        \
        m_bCreate##classname##Impl = FALSE;                 \
    }                                                       \
    interfacename::~interfacename()                         \
    {                                                       \
        if (m_bCreate##classname##Impl)                     \
            SAFE_DELETE(m_p##classname##Impl);              \
    }                                                       \
	void  interfacename::CreateInstance(UI::IUIApplication* p, UI::ISkinRes* pSkinRes, interfacename** pp) \
    {                                                       \
        interfacename::_CreatorClass::UICreateInstance(p, pSkinRes, pp); \
    }                                                       \
	void  interfacename::CreateInstance(UI::ISkinRes* pSkinRes, interfacename** pp) \
	{                                                       \
		interfacename::_CreatorClass::UICreateInstance(pSkinRes->GetUIApplication(), pSkinRes, pp); \
	}

// ��UIObject����Ҳ����pimplģʽ(���ᱻ�̳�)��
#define UI_IMPLEMENT_Ixxx_INTERFACE_Construct2(interfacename, classname) \
    interfacename::interfacename()                          \
    {                                                       \
        m_p##classname##Impl = NULL;                        \
        m_bCreate##classname##Impl = FALSE;                 \
    }                                                       \
    interfacename::~interfacename()                         \
    {                                                       \
        if (m_bCreate##classname##Impl)                     \
            SAFE_DELETE(m_p##classname##Impl);              \
    }                                                       \
    void  interfacename::delete_this() { delete this; }

#define UI_IMPLEMENT_Ixxx_INTERFACE_ProcessMessage(interfacename, classname) \
    BOOL  interfacename::nvProcessMessage(UI::UIMSG* pMsg, int nMsgMapID, bool bDoHook) \
    {                                                       \
        return m_p##classname##Impl->nvProcessMessage(pMsg, nMsgMapID, bDoHook); \
    }                                                       \
    /*virtual*/ BOOL  interfacename::virtualProcessMessage(UI::UIMSG* pMsg, int nMsgMapID, bool bDoHook) \
    {                                                       \
        return m_p##classname##Impl->nvProcessMessage(pMsg, nMsgMapID, bDoHook); \
    }

// ��ǰģ��̳�
#define UI_IMPLEMENT_Ixxx_INTERFACE_CreateImpl(interfacename,classname,baseclass) \
    void  interfacename::CreateImpl(classname* p)           \
    {                                                       \
        if (p)                                              \
        {                                                   \
            m_p##classname##Impl = p;                       \
        }                                                   \
        else                                                \
        {                                                   \
            m_p##classname##Impl = new classname;           \
            m_bCreate##classname##Impl = TRUE;              \
        }                                                   \
        m_p##classname##Impl->SetI##classname(this);        \
        I##baseclass::CreateImpl(static_cast<baseclass*>(m_p##classname##Impl)); \
    }                                                       \
    classname*  interfacename::GetImpl()                    \
    {                                                       \
        return m_p##classname##Impl;                        \
    }

// ��ģ��̳�
#define UI_IMPLEMENT_Ixxx_INTERFACE_CreateImpl_ACROSSMODULE(interfacename,classname,baseinterface) \
    void  interfacename::CreateImpl(classname* p)           \
    {                                                       \
        baseinterface::CreateImpl(NULL);                    \
        if (p)                                              \
        {                                                   \
            m_p##classname##Impl = p;                       \
        }                                                   \
        else                                                \
        {                                                   \
            m_p##classname##Impl = new classname;           \
            m_bCreate##classname##Impl = TRUE;              \
        }                                                   \
        m_p##classname##Impl->SetI##classname(this);        \
    }                                                       \
    classname*  interfacename::GetImpl()                    \
    {                                                       \
        return m_p##classname##Impl;                        \
    }

// û�и���������ʹ��
#define UI_IMPLEMENT_Ixxx_INTERFACE_CreateImpl3(interfacename,classname) \
    void  interfacename::CreateImpl(classname* p)           \
    {                                                       \
        if (p)                                              \
        {                                                   \
            m_p##classname##Impl = p;                       \
        }                                                   \
        else                                                \
        {                                                   \
            m_p##classname##Impl = new classname;           \
            m_bCreate##classname##Impl = TRUE;              \
        }                                                   \
        m_p##classname##Impl->SetI##classname(this);        \
    }                                                       \
    classname*  interfacename::GetImpl()                    \
    {                                                       \
        return m_p##classname##Impl;                        \
    }
#pragma endregion

#define uiiidof(xxx)   IID_UI_##xxx
//#define uiclsidof(xxx) CLSID_UI_##xxx



#endif // UIDEFINE_H_4C746653_0625_41a4_8DE7_F18E7116CCFF