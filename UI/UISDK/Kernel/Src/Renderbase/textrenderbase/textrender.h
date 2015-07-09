#pragma  once
#include "uxtheme.h"
#include "Vsstyle.h"
#pragma comment(lib, "uxtheme.lib")

#include "UISDK\Kernel\Inc\Interface\itextrenderbase.h"
#include "UISDK\Kernel\Src\Base\Message\message.h"

namespace UI
{
class AttributeSerializer;

class TextRenderBase : public Message
{
public:
	TextRenderBase();
	~TextRenderBase();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_GETDESIREDSIZE2(OnGetDesiredSize)
    UI_END_MSG_MAP

    void  SetITextRenderBase(ITextRenderBase* p) { m_pITextRenderBase = p; };
    void  Serialize(AttributeSerializer* ps);
    void  OnGetDesiredSize(SIZE* pSize, GETDESIREDSIZEINFO* pInfo);
    SIZE  GetDesiredSize(LPCTSTR szText, int nLimitWidth=-1);

public:
    void  SetRenderFont(IRenderFont* pFont) {}

	void  SetTextAlignment(int nDrawFlag);
    int   GetTextAlignment() 
    {
        return m_nDrawTextFlag; 
    }
    void  SetTextEffect(TEXT_EFFECT e) 
    {
        m_eDrawTextEffect = e;
    }
    TEXT_EFFECT  GetTextEffect()
    {
        return m_eDrawTextEffect;
    }
    void  SetDrawTextParam(WPARAM w, LPARAM l)
    {
        m_wparamDrawText = w;
        m_lparamDrawText = l;
    }

	void     SetObject(IUIApplication*  pUIApp, Object* pObject) { m_pUIApplication = pUIApp; m_pObject = pObject; }
    Object*  GetObject() { return m_pObject; }

	void  SetTextRenderType(const TEXTRENDER_TYPE& nType){ m_nTextRenderType = nType ; }
	TEXTRENDER_TYPE  GetTextRenderType() { return m_nTextRenderType; }
	
	IColorRes*  GetActiveSkinColorRes();
	IFontRes*  GetActiveSkinFontRes();

	bool  IsThemeRender();
    void  CheckSkinTextureChanged();

    void  _LoadFont(LPCTSTR szFontId, IRenderFont*& pRenderFont);
    LPCTSTR  _SaveFont(IRenderFont*& pRenderFont);
    void  _LoadDefalutFont(IRenderFont*& pRenderFont);
    void  _LoadColor(LPCTSTR szColorId, Color*& pColor);
    LPCTSTR  _SaveColor(Color*& pColor);

    void  LoadHaloColor(LPCTSTR szColorId);
    LPCTSTR  GetHaloColorId();

protected:
    IUIApplication*   m_pUIApplication;
    ITextRenderBase*  m_pITextRenderBase;

	Object*  m_pObject;     // 绑定的对象，要绘制谁的文字
	TEXTRENDER_TYPE  m_nTextRenderType;  // 自己的类型
	long  m_nDrawTextFlag;    // DrawText的flag标识
    
    TEXT_EFFECT  m_eDrawTextEffect;
    Color*  m_pColorTextBkgnd;
    WPARAM  m_wparamDrawText;
    LPARAM  m_lparamDrawText;
};

class SimpleTextRender : public TextRenderBase
{
public:
	SimpleTextRender();
	~SimpleTextRender();

    UI_DECLARE_TEXTRENDERBASE(SimpleTextRender, XML_TEXTRENDER_TYPE_SIMPLE, TEXTRENDER_TYPE_SIMPLE)

    UI_BEGIN_MSG_MAP
        UIMSG_WM_TEXTRENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_GETRENDERFONT(GetRenderFont)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_SERIALIZE(OnSerialize)
    UI_END_MSG_MAP_CHAIN_PARENT(TextRenderBase)

    void  SetISimpleTextRender(ISimpleTextRender* p) { m_pISimpleTextRender = p; };

	void  OnSerialize(SERIALIZEDATA* pData);
	void  DrawState(TEXTRENDERBASE_DRAWSTATE* pDrawStruct);
    IRenderFont*  GetRenderFont() { return m_pRenderFont; }

	void  SetRenderFont(IRenderFont*);
	void  SetColor(Color*  pColText);

	void  LoadColor(LPCTSTR szColorId);
	LPCTSTR  GetColorId();
	
	void  LoadFont(LPCTSTR szFontId);
	LPCTSTR  GetFontId();

private:
    ISimpleTextRender*  m_pISimpleTextRender;
	Color*  m_pColorText;
	IRenderFont*  m_pRenderFont;
};


// 根据皮肤颜色，自适应设置文本的颜色是白还是黑
class ContrastColorTextRender : public TextRenderBase
{
public:
    ContrastColorTextRender();
    ~ContrastColorTextRender();

    UI_DECLARE_TEXTRENDERBASE(ContrastColorTextRender, XML_TEXTRENDER_TYPE_CONTRAST_COLOR, TEXTRENDER_TYPE_CONTRASTCOLOR)

    UI_BEGIN_MSG_MAP
        UIMSG_WM_TEXTRENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_GETRENDERFONT(GetRenderFont)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMESSAGE_HANDLER_EX(UI_WM_SKINTEXTURECHANGED, OnSkinTextureChanged)
        UIMSG_WM_SERIALIZE(OnSerialize)
    UI_END_MSG_MAP_CHAIN_PARENT(TextRenderBase)

    void  SetIContrastColorTextRender(IContrastColorTextRender* p) { m_pIContrastColorTextRender = p; };

public:
    void  SetRenderFont(IRenderFont*);

protected:
    void  OnSerialize(SERIALIZEDATA* pData);
    void  DrawState(TEXTRENDERBASE_DRAWSTATE* pDrawStruct);
    IRenderFont*  GetRenderFont() { return m_pRenderFont; }
    LRESULT  OnSkinTextureChanged(UINT, WPARAM, LPARAM);

    void  LoadFont(LPCTSTR szFontId){
        _LoadFont(szFontId, m_pRenderFont);
    }
    LPCTSTR  GetFontId(){
        return _SaveFont(m_pRenderFont);
    }
    void  LoadColor(LPCTSTR szColorId){
        _LoadColor(szColorId, m_pColorText);
    }
    LPCTSTR  GetColorId(){
        return _SaveColor(m_pColorText);
    }

private:
    IContrastColorTextRender*  m_pIContrastColorTextRender;
    
    Color*   m_pColorText;
    IRenderFont*  m_pRenderFont;

    bool  m_bNeedCalcColor;
};


class ContrastColorListTextRender : public TextRenderBase
{
public:
    ContrastColorListTextRender();
    ~ContrastColorListTextRender();

    UI_DECLARE_TEXTRENDERBASE(ContrastColorListTextRender, XML_TEXTRENDER_TYPE_CONTRASTCOLORLIST, TEXTRENDER_TYPE_CONTRASTCOLORLIST)

    UI_BEGIN_MSG_MAP
        UIMSG_WM_TEXTRENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_GETRENDERFONT(GetRenderFont)
        UIMESSAGE_HANDLER_EX(UI_WM_SKINTEXTURECHANGED, OnSkinTextureChanged)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_SERIALIZE(OnSerialize)
    UI_END_MSG_MAP_CHAIN_PARENT(TextRenderBase)

    void  SetIContrastColorListTextRender(IContrastColorListTextRender* p) { m_pIContrastColorListTextRender = p; }

    void  OnSerialize(SERIALIZEDATA* pData);
    void  DrawState(TEXTRENDERBASE_DRAWSTATE* pDrawStruct);
    LRESULT  OnSkinTextureChanged(UINT, WPARAM, LPARAM);

    IRenderFont*  GetRenderFont();
    void  SetRenderFont(IRenderFont*);

    void  SetCount(long nCount);
    long  GetCount();

    void  LoadFont(LPCTSTR szFontId){
        _LoadFont(szFontId, m_pRenderFont);
    }
    LPCTSTR  GetFontId(){
        return _SaveFont(m_pRenderFont);
    }

protected:
    void  Clear();

private:
    IContrastColorListTextRender*  m_pIContrastColorListTextRender;

    vector<Color*>  m_vTextColor;
    IRenderFont*   m_pRenderFont;
    int   m_nCount;
};

//////////////////////////////////////////////////////////////////////////

class ColorListTextRender : public TextRenderBase
{
public:
	ColorListTextRender();
	~ColorListTextRender();

    UI_DECLARE_TEXTRENDERBASE(ColorListTextRender, XML_TEXTRENDER_TYPE_COLORLIST, TEXTRENDER_TYPE_COLORLIST)

    UI_BEGIN_MSG_MAP
        UIMSG_WM_TEXTRENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_GETRENDERFONT(GetRenderFont)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_SERIALIZE(OnSerialize)
    UI_END_MSG_MAP_CHAIN_PARENT(TextRenderBase)

    void  SetIColorListTextRender(IColorListTextRender* p) { m_pIColorListTextRender = p; }

	void  OnSerialize(SERIALIZEDATA* pData);
	void  DrawState(TEXTRENDERBASE_DRAWSTATE* pDrawStruct);

    IRenderFont*  GetRenderFont();
	void  SetRenderFont(IRenderFont*);

    void  LoadFont(LPCTSTR szFontId){
        _LoadFont(szFontId, m_pRenderFont);
    }
    LPCTSTR  GetFontId(){
        return _SaveFont(m_pRenderFont);
    }

    void  SetCount(long nCount);
    long  GetCount();

    void  LoadColor(LPCTSTR szText);
    LPCTSTR  GetColor();

	void  SetColor(int nIndex, COLORREF col);

protected:
	void  Clear();

private:
    IColorListTextRender*  m_pIColorListTextRender;

	vector<ColorPtr>  m_vTextColor;
	IRenderFont*      m_pRenderFont;
	int               m_nCount;
};
#if 0
	class FontListTextRender : public TextRenderBase
	{
	private:
		Color*  m_pTextColor;
		vector<IRenderFont*>  m_vTextFont;
	};
#endif

class FontColorListTextRender : public TextRenderBase
{
public:
	FontColorListTextRender();
	~FontColorListTextRender();

    UI_DECLARE_TEXTRENDERBASE(FontColorListTextRender, XML_TEXTRENDER_TYPE_FONTCOLORLIST, TEXTRENDER_TYPE_FONTCOLORLIST)

    UI_BEGIN_MSG_MAP
        UIMSG_WM_TEXTRENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_GETRENDERFONT(GetRenderFont)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_SERIALIZE(OnSerialize)
    UI_END_MSG_MAP_CHAIN_PARENT(TextRenderBase)

    void  SetIFontColorListTextRender(IFontColorListTextRender* p) { m_pIFontColorListTextRender = p; }
    void  OnSerialize(SERIALIZEDATA* pData);
    void  DrawState(TEXTRENDERBASE_DRAWSTATE* pDrawStruct);

	IRenderFont*  GetRenderFont();
	void  SetRenderFont(IRenderFont*);

	void  SetCount(long nCount);
    long  GetCount();

    void  LoadColor(LPCTSTR szText);
    LPCTSTR  GetColor();
    void  LoadFont(LPCTSTR szText);
    LPCTSTR  GetFont();

	void  SetColor(int nIndex, DWORD color);
	void  SetFont(int nIndex, IRenderFont*);

protected:
	void  Clear();

private:
    IFontColorListTextRender*  m_pIFontColorListTextRender;

	vector<Color*>     m_vTextColor;
	vector<IRenderFont*> m_vTextFont;

	int   m_nCount;
};

//
//	主题背景按钮
//
class ThemeTextRenderBase : public TextRenderBase
{
public:
	ThemeTextRenderBase();
	~ThemeTextRenderBase();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_THEMECHANGED(OnThemeChanged)
        UIMSG_WM_INITIALIZE(OnInit)
    UI_END_MSG_MAP_CHAIN_PARENT(TextRenderBase)

    void  SetIThemeTextRenderBase(IThemeTextRenderBase* p) { m_pIThemeTextRenderBase = p; } 

    void  OnInit();
    void  OnThemeChanged();
    void  Serialize(AttributeSerializerWrap* ps);

public:
    void  CreateTheme();
    void  SetNoTheme(bool b) { 
		m_bNoTheme = b; CreateTheme(); 
	}
	bool  GetNoTheme() { 
		return m_bNoTheme;
	}
    HTHEME  GetTHEME() {
		return m_hTheme; 
	}

protected:
    IThemeTextRenderBase*  m_pIThemeTextRenderBase;
	HTHEME  m_hTheme;
    bool    m_bNoTheme;  // 是否强制使用无主题的样式。
};

}