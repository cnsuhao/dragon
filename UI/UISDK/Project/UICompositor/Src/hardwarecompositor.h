#pragma once
#include "UISDK\Project\UICompositor\Inc\ihardwarecompositor.h"

//
// 如何开启DX10的抗锯齿？
//
// 1. CreateSwapChain时，指定SampleDesc的Count大于1
// 2. 在hlsl中指定SetRasterizerState, 将MultisampleEnable = TRUE;
//
//

namespace UI
{
class GpuLayerTexture;
class HardwareComposition
{
public:
	HardwareComposition(HWND hWnd);
	~HardwareComposition();

	GpuLayerTexture*  CreateLayerTexture();
    bool  BeginCommit();
    void  EndCommit();
    void  Resize(UINT nWidth, UINT nHeight);
    void  SetRootLayerTexture(GpuLayerTexture* p);
    GpuLayerTexture*  GetRootLayerTexture();

protected:
    void  CreateSwapChain();
    void  ReCreateRenderTargetView();

public:
    IHardwareComposition  m_IHardwareComposition;

private:
	GpuLayerTexture*  m_pRootTexture;
	HWND  m_hWnd;

    IDXGISwapChain*  m_pSwapChain;
    ID3D10RenderTargetView*  m_pRenderTargetView;
    SIZE   m_sizeBackBuffer;
};
}