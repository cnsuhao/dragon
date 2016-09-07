#pragma once
//#include "UISDK\Project\UI3D\inc\inc.h"
#include "UISDK\Project\UICompositor\inc\ihardwarecompositor.h"
#pragma comment(lib, "UICompositor.lib")

//
// 2014.3.10
//
// 统一Graphics Render Library的使用，一个窗口不再混合使用，只能使用一种Graphics Render Library
// 否则处处要考虑创建什么类型的渲染库，搞的代码很乱，逻辑也乱
//
namespace UI
{
class Object;
class WindowBase;
class RenderLayer;
interface IWindowTransparent;
interface IWindowRender;

class WindowRender
{
public:
    WindowRender(WindowBase*  p);
    ~WindowRender();

    IWindowRender*  GetIWindowRender();

public:
    RenderLayer*  CreateRenderLayer(Object* pObj);
	IGpuLayerTexture*  CreateGpuLayerTexture(RenderLayer* p);
    void  OnRenderLayerDestroy(RenderLayer* p);

	void  OnSerialize(SERIALIZEDATA* pData);
    bool  CreateRenderTarget(IRenderTarget** pp);
	void  UpdateWindow(HDC hDC, RECT* prcDamage);
    void  OnWindowSize(UINT nWidth, UINT nHeight);
    void  OnWindowTransparentModeChanged(IWindowTransparent* pMode);
	void  SetGraphicsRenderType(GRAPHICS_RENDER_LIBRARY_TYPE  eTpye);
    GRAPHICS_RENDER_LIBRARY_TYPE  GetGraphicsRenderType();
    bool  GetRequireAlphaChannel();
    void  SetCanCommit(bool b);
    bool  CanCommit() ;
    void  Commit(HDC hDC, RECT* prc, int nCount=1);
	void  OnHardCompositeChanged(bool bEnable);    
    void  HardComposite();

protected:
    void  RebuildCompositingLayerTree();
    void  rebuildCompositingLayerTree(
        Object* pParent, 
        RenderLayer* pParentLayer);

public:
    IWindowRender*  m_pIWindowRender;
    GRAPHICS_RENDER_LIBRARY_TYPE  m_eGRL;
    WindowBase*    m_pWindow;

#pragma region // 硬件合成相关

    // 窗口交换链
    IHardwareComposition*  m_pHardwareComposition;
    // 构建与objtree对应的layertree
    bool     m_bNeedRebuildGpuLayerTree; 

#pragma endregion

    long  m_lRefCanCommit;
    bool  m_bNeedAlphaChannel;       // 该窗口的渲染是否需要alpha通道。不再根据graphics render lib type来决定。由用户自己设置
    bool  m_bFirstTimePaintWindow;   // <-- 为了解决在WM_PAINT消息来之前，一些层或者控件单独刷新，导致界面
                                     // 上只显示了该一部分数据的问题。因此在第一个WM_PAINT之前将窗口
                                     // 设置为不可提交（构造函数中设置）的。
};

}