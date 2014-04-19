#ifndef IRENDERLAYER_H_8500D1A2_903C_4c76_B3B0_5248E65E95C3
#define IRENDERLAYER_H_8500D1A2_903C_4c76_B3B0_5248E65E95C3
#include "UISDK\Kernel\Inc\Interface\ipanel.h"

namespace UI
{
interface IRenderTarget;
class RenderLayer2;
class WindowRender;


// 旋转中心位置
enum ROTATE_CENTER_TYPE
{
    ROTATE_CENTER_TOPLEFT = 0,
    ROTATE_CENTER_CENTER = 1,
    // ...
};

interface UISDKAPI IRenderLayer2
{
public:
	IRenderLayer2(RenderLayer2* p);
	~IRenderLayer2();

	RenderLayer2*  GetImpl();
	void  SetOpacity(byte b);
	byte  GetOpacity();
    HDC   GetHDC();
    void  DirectCommit(bool bUpdate);
	void  Rotate3D(int x, int y, int z);
    void  Rotate2D(int degree);
    void  SetRotateCenterPos(ROTATE_CENTER_TYPE eType, int xOffset, int yOffset);
    IRenderTarget*  GetRenderTarget();
    
public:
	RenderLayer2*  m_pRenderLayer2Impl;
};

interface UISDKAPI IWindowRender
{
public:
    IWindowRender(WindowRender* p);
    ~IWindowRender();

    WindowRender*  GetImpl();
    void  SetCanCommit(bool b);

    GRAPHICS_RENDER_LIBRARY_TYPE  GetGraphicsRenderType();
	void  SetGraphicsRenderType(GRAPHICS_RENDER_LIBRARY_TYPE  eTpye);

public:
    WindowRender*  m_pWindowRenderImpl;
};

}

#endif // IRENDERLAYER_H_8500D1A2_903C_4c76_B3B0_5248E65E95C3