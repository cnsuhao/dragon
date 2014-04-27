#pragma once
//
// 2014.3.10
//
// 1. ����ͳһGraphics Render Library��ʹ�ã�һ�����ڲ��ٻ��ʹ�ã�ֻ��ʹ��һ��Graphics Render Library
//    ���򴦴�Ҫ���Ǵ���ʲô���͵���Ⱦ�⣬��Ĵ�����ң��߼�Ҳ��
// 2. ͳһ�ؼ�Require Alpha Channel��ֻ��ʹ��Gdiʱ����Ҫ��Gdiplus,D2D,D3D����ҪAlpha Channel
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
    bool     m_bFirstTimePaintWindow;  // <-- Ϊ�˽����WM_PAINT��Ϣ��֮ǰ��һЩ����߿ؼ�����ˢ�£����½���
                                        // ��ֻ��ʾ�˸�һ�������ݵ����⡣����ڵ�һ��WM_PAINT֮ǰ������
                                        // ����Ϊ�����ύ�����캯�������ã��ġ�
};

}