#pragma once
//
// 2014.3.10
//
// 1. 决定统一Graphics Render Library的使用，一个窗口不再混合使用，只能使用一种Graphics Render Library
//    否则处处要考虑创建什么类型的渲染库，搞的代码很乱，逻辑也乱
// 2. 统一控件Require Alpha Channel，只有使用Gdi时不需要，Gdiplus,D2D,D3D都需要Alpha Channel
//
//
namespace UI
{
class WindowBase;
interface IWndTransMode;
interface IWindowRender;

class WindowRender
{
public:
    WindowRender(WindowBase*  p);
    ~WindowRender();

    IWindowRender*  GetIWindowRender();

public:
	void  ResetAttribute();
	void  SetAttribute(IMapAttribute* pMapAttrib, bool bReload);
    bool  CreateRenderTarget(IRenderTarget** pp);
	void  OnWindowPaint(HDC hDC);
    void  OnWindowTransparentModeChanged(IWndTransMode* pMode);
	void  SetGraphicsRenderType(GRAPHICS_RENDER_LIBRARY_TYPE  eTpye);
    GRAPHICS_RENDER_LIBRARY_TYPE  GetGraphicsRenderType();
    bool  GetRequireAlphaChannel();
    void  SetCanCommit(bool b);
    bool  CanCommit() ;
    void  Commit(HDC hDC, RECT* prc, int nCount=1);

public:
    IWindowRender*  m_pIWindowRender;
    GRAPHICS_RENDER_LIBRARY_TYPE  m_eGRL;
    WindowBase*    m_pWindow;
    
    long     m_lRefCanCommit;
    bool     m_bFirstTimePaintWindow;  // <-- 为了解决在WM_PAINT消息来之前，一些层或者控件单独刷新，导致界面
                                        // 上只显示了该一部分数据的问题。因此在第一个WM_PAINT之前将窗口
                                        // 设置为不可提交（构造函数中设置）的。
};

}