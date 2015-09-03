#pragma once
#include "UISDK\Project\UICompositor\Inc\ihardwarecompositor.h"

//
// ��ο���DX10�Ŀ���ݣ�
//
// 1. CreateSwapChainʱ��ָ��SampleDesc��Count����1
// 2. ��hlsl��ָ��SetRasterizerState, ��MultisampleEnable = TRUE;
//
// ���ʵ�ּ��ã�
// 1. ͨ��RSSetViewport�����ƻ��������ǲ��Եġ����ַ�ʽ�ǽ������������Ż��Ƶ��µ�
//    viewport���У�������������Ҫ�Ľ����õ�Ч����
//
// 2. RSSetScissorRects
//    The scissor rectangles will only be used if ScissorEnable is set to true 
//    in the rasterizer state
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