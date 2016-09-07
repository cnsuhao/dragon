#ifndef IRENDERBASE_H_E6A9A029_AD8D_4ae2_9137_E2D0943CBEC7
#define IRENDERBASE_H_E6A9A029_AD8D_4ae2_9137_E2D0943CBEC7

#include <uxtheme.h>

namespace UI
{
    // 注：libo 20120927
    //     每次光给render传递一个数值，里面无法只能用这个数值与图片项索引进行匹配，无法做更多的判断
    //     因此决定扩展DrawState参数。低位为图片索引值，高位为状态值

#define RENDER_STATE_MASK     0xFFFF0000
#define RENDER_STATE_NORMAL   0x00010000
#define RENDER_STATE_HOVER    0x00020000
#define RENDER_STATE_PRESS    0x00040000
#define RENDER_STATE_DISABLE  0x00080000
#define RENDER_STATE_SELECTED 0x00100000
#define RENDER_STATE_DEFAULT  0x00200000
#define RENDER_STATE_READONLY 0x00400000
#define RENDER_STATE_NOTFOCUS 0x00800000  // 被选中了，但失去了焦点


const UINT  LISTCTRLITEM_FOREGND_RENDER_STATE_NORMAL  = RENDER_STATE_NORMAL|0;
const UINT  LISTCTRLITEM_FOREGND_RENDER_STATE_HOVER   = RENDER_STATE_HOVER|1;
const UINT  LISTCTRLITEM_FOREGND_RENDER_STATE_PRESS   = RENDER_STATE_PRESS|2;
const UINT  LISTCTRLITEM_FOREGND_RENDER_STATE_DISABLE = RENDER_STATE_DISABLE|3;
const UINT  LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_NORMAL = RENDER_STATE_NORMAL|RENDER_STATE_SELECTED|4;
const UINT  LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_HOVER  = RENDER_STATE_HOVER|RENDER_STATE_SELECTED|5;
const UINT  LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_PRESS  = RENDER_STATE_PRESS|RENDER_STATE_SELECTED|6;
const UINT  LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_DISABLE = RENDER_STATE_DISABLE|RENDER_STATE_SELECTED|7;
const UINT  LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_NOT_FOCUS = RENDER_STATE_SELECTED|RENDER_STATE_NOTFOCUS|8;

const UINT  LISTCTRLITEM_FOCUS_RENDER_STATE_NORMAL = RENDER_STATE_NORMAL|0;
const UINT  LISTCTRLITEM_FOCUS_RENDER_STATE_CTRLNOFOCUS = RENDER_STATE_NOTFOCUS|1;


enum RENDER_TYPE
{
    RENDER_TYPE_NULL,             
    RENDER_TYPE_COLOR,       
    RENDER_TYPE_THEME_SYSCOLOR,
    RENDER_TYPE_GRADIENT,    
    RENDER_TYPE_COLORLIST,

    RENDER_TYPE_IMAGE,  
    RENDER_TYPE_IMAGELIST,
    RENDER_TYPE_IMAGELISTITEM,

    //	RENDER_TYPE_NOTHEME,    // 强制使用系统无主题样式的样式（win2000样式），目前仅GroupBox支持
    RENDER_TYPE_THEME_FIRST = 1000,
    //	RENDER_TYPE_THEME,
    RENDER_TYPE_THEME_TOOLTIP_WINDOW_BKGND,

    RENDER_TYPE_THEME_LAST = 9999,
};

class RenderBase;
interface UISDKAPI IRenderBase : public IMessage
{
    UI_DECLARE_Ixxx_INTERFACE(IRenderBase, RenderBase)

    long  AddRef();
    long  Release();

    void  CheckThemeChanged();
    bool  IsThemeRender();

    void  SetObject(IUIApplication*  pUIApp, IObject* pObject);
    IObject*  GetObject();

    void  SetType(RENDER_TYPE nType);
    RENDER_TYPE  GetType();

    IColorRes*  GetSkinColorRes();
    IImageRes*  GetSkinImageRes();

    void  Serialize(SERIALIZEDATA* pData);
    void  DrawState(IRenderTarget*, const CRect* prc, int nState);
    SIZE  GetDesiredSize();
    void  Init();

protected:
    long  m_lRef;
};


// -- 2015.4.1过期，不再使用
class NullRender;
interface INullRender : public IRenderBase
{
    UI_DECLARE_Ixxx_INTERFACE(INullRender, NullRender);
};
//////////////////////////////////////////////////////////////////////////

class ThemeRenderBase;
interface UISDKAPI IThemeRenderBase : public IRenderBase
{
    UI_DECLARE_Ixxx_INTERFACE(IThemeRenderBase, ThemeRenderBase)

    const TCHAR*  GetThemeName();
    HTHEME  GetTHEME();

};
}

#endif // IRENDERBASE_H_E6A9A029_AD8D_4ae2_9137_E2D0943CBEC7