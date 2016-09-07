#ifndef UIDEFINE_H_4C746653_0625_41a4_8DE7_F18E7116CCFF
#define UIDEFINE_H_4C746653_0625_41a4_8DE7_F18E7116CCFF

#include <map>
using std::map;

// 导入导出定义
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

// 窗口类名称
#define WND_CLASS_NAME                  _T("UI")
#define WND_ANIMATE_CLASS_NAME          _T("UI_Animate")
#define WND_POPUP_CONTROL_SHADOW_NAME   _T("UI_PopupControlShadow") // 带系统阴影功能
#define WND_POPUP_CONTROL_NAME          _T("UI_PopupControl")
#define WND_DRAGBITMAPWND_CLASS_NAME    _T("UI_DragBitmapWnd")

// 特殊含义常量
#define NDEF  -1                               // 没有定义的变量
#define AUTO  -2                               // 自动调整（如width/height=auto）

#define MAX_STRING_LEN    256

// 临时文件后缀名
#define TEMP_FILE_EXT    _T(".tmp")

// 是否处理返回值 
enum HANDLED_VALUE
{
	NOT_HANDLED = 0,
	HANDLED = 1, 
};

//////////////////////////////////////////////////////////////////////////
// enum

// 资源类型定义
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
	UI_RESOURCE_SOURCEFILE = 0x0400,  // 资源所保存在的文件。用于UIBuilder查看文件
    UI_RESOURCE_MENU       = 0x0800,
}
UI_RESOURCE_TYPE;



// 32位的对象类型中，分为三部分，OBJ_TYPE 占用8位，目前主要只有3种类型。
// CONTROL_TYPE 占用 16 位，CONTROL_SUB_TYPE 占用 8 位
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

#pragma region // Object, RenderBase 扩展宏

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


// 本宏定义主要是用于theme类型的renderbase，要根据控件类型进行创建
#define UI_DECLARE_RENDERBASE2(className, xml, rendertype) \
    static LPCTSTR  GetXmlName() { return xml; }       \
    static int  GetType() { return rendertype; }      \
    typedef UI::UIObjCreator<className> _CreatorClass;  // 注：这里没有用I##className，因为默认这种theme没有Ixxx基类


#define UI_DECLARE_TEXTRENDERBASE(className, xml, rendertype) \
    static LPCTSTR  GetXmlName() { return xml; }       \
    static int  GetType() { return rendertype; }      \
    UI_DECLARE_QUERY_INTERFACE(className)                   \
    typedef UI::UIObjCreator<I##className> _CreatorClass;

#define UI_DECLARE_TEXTRENDERBASE2(className, xml, rendertype) \
    static LPCTSTR  GetXmlName() { return xml; }       \
    static int  GetType() { return rendertype; }      \
    typedef UI::UIObjCreator<className> _CreatorClass;   // 注：这里没有用I##className，因为默认这种theme没有Ixxx基类

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

// object的state bit，状态位
#define OSB_UNVISIBLE     0x0001        // 自己是否可见
#define OSB_COLLAPSED     0x0002        // listitem为收起来，不显示子结点（TODO:以后可用于扩展为隐藏并且不占用布局?）
#define OSB_DISABLE       0x0004        // 自己是否可用

#define OSB_PRESS         0x0010
#define OSB_HOVER         0x0020
#define OSB_FORCEPRESS    0x0040
#define OSB_FORCEHOVER    0x0080         
#define OSB_READONLY      0x0100        // 还是由各个控件自己实现更好一些。
#define OSB_FOCUS         0x0200
#define OSB_DEFAULT       0x0400
#define OSB_SELECTED      0x0800         // 被选中
#define OSB_CHECKED       0x1000
#define OSB_RADIOCHECKED  0x2000
#define OSB_DRAGDROPHOVER 0x4000        // 拖拽过程中，位于鼠标下。目前仅listitem使用

#define WSB_ACTIVE        0x1000         // 窗口为active

// 废弃
// STYLEEX用于具体控件样式，同时将STYLEEX的最后两位用于标识控件类型，例如按钮的用途
// #define GETCONTROLSUBTYPE(nStyleEx)  (nStyleEx & 0x000000FF)
// #define MAKECONTROLSUBTYPE(nStyleEx, subStyle)  ((nStyleEx & 0xFFFFFF00) | subStyle)


// UI_WM_NOTIFY 消息的lParam可使用的结构体
struct UINMHDR
{
    int reverse;
};


// UI_WM_DROPTARGETEVENT 消息参数
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


#pragma region // Ixxx接口类实现宏

// 申明一个接口类的基本成员和方法，将析构函数作为protected成员，强制外部使用
// SAFE_DELETE_Ixxx来释放该类型
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

// 非UIObject，但也采用pimpl模式(不会被继承)，
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
// 实现一个接口类
// 注：
//   1. 有些接口要在构造函数中初始化别的成员变量，如long m_lRef，因此将构造函数脱离出来
//   2. 对于像IButtonBase这种接口，由于ButtonBase没法直接从Control类继承，因此需要在
//      IButtonBase::CreateImpl中单独调用一次IControl::CreateImpl(NULL)，因此将CreateImpl脱离出来
//

// 当前模块继承
#define UI_IMPLEMENT_Ixxx_INTERFACE(interfacename, classname, basename)  \
    UI_IMPLEMENT_Ixxx_INTERFACE_Construct(interfacename, classname)      \
    UI_IMPLEMENT_Ixxx_INTERFACE_ProcessMessage(interfacename, classname) \
    UI_IMPLEMENT_Ixxx_INTERFACE_CreateImpl(interfacename, classname, basename)

// 跨模块继承
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

// 非UIObject，但也采用pimpl模式(不会被继承)，
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

// 当前模块继承
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

// 跨模块继承
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

// 没有父类的情况下使用
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