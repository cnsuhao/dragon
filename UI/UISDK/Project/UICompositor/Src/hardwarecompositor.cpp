#include "stdafx.h"
#include "hardwarecompositor.h"
#include "d3d10\D3DApp.h"
#include "GpuLayerTexture.h"
using namespace UI;


//////////////////////////////////////////////////////////////////////////

HardwareComposition::HardwareComposition(HWND hWnd)
{
    m_IHardwareComposition.SetImpl(this);
	m_pRootTexture = NULL;
    m_hWnd = hWnd;

    m_pSwapChain = NULL;
    m_pRenderTargetView = NULL;
    m_sizeBackBuffer.cx = m_sizeBackBuffer.cy = 8;

    CreateSwapChain();
}

HardwareComposition::~HardwareComposition()
{
    if (g_pD3D10App->IsActiveSwapChain(m_hWnd))
    {
        g_pD3D10App->m_pDevice->OMSetRenderTargets(0, NULL, NULL);
        g_pD3D10App->SetActiveSwapChain(NULL);
    }

    SAFE_RELEASE(m_pRenderTargetView);
	SAFE_RELEASE(m_pSwapChain);
}

GpuLayerTexture*  HardwareComposition::CreateLayerTexture()
{
	GpuLayerTexture* p = new GpuLayerTexture;
    p->SetHardwareComposition(this);
    return p;
}
void  HardwareComposition::SetRootLayerTexture(GpuLayerTexture* p)
{
    m_pRootTexture = p;
}
GpuLayerTexture*  HardwareComposition::GetRootLayerTexture()
{
    return m_pRootTexture;
}
void  HardwareComposition::CreateSwapChain()
{
    if (!m_hWnd)
        return;
    if (!g_pD3D10App || !g_pD3D10App->m_pDXGIFactory)
        return;

    RECT  rc;
    GetClientRect(m_hWnd, &rc);
    if (rc.right == rc.left || rc.bottom == rc.top)
        return;

    DXGI_SWAP_CHAIN_DESC swapDesc;
    ::ZeroMemory(&swapDesc, sizeof(swapDesc));

    m_sizeBackBuffer.cx = rc.right-rc.left;
    m_sizeBackBuffer.cy = rc.bottom-rc.top;

    swapDesc.BufferDesc.Width = m_sizeBackBuffer.cx;
    swapDesc.BufferDesc.Height = m_sizeBackBuffer.cy;
    swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapDesc.SampleDesc.Count = 4;

    UINT quality = 0;
    HRESULT hr = 0;
	hr = g_pD3D10App->m_pDevice->CheckMultisampleQualityLevels(
			DXGI_FORMAT_R8G8B8A8_UNORM, swapDesc.SampleDesc.Count, &quality);
	if (quality <= 0)
	{
		swapDesc.SampleDesc.Count = 1;
		swapDesc.SampleDesc.Quality = 0;
	}
	else
	{
		swapDesc.SampleDesc.Quality = quality-1;
	}

    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.BufferCount = 1;
    swapDesc.OutputWindow = m_hWnd;
    swapDesc.Windowed = TRUE;

    /*HRESULT hr = */g_pD3D10App->m_pDXGIFactory->CreateSwapChain(
        g_pD3D10App->m_pDevice, &swapDesc, &m_pSwapChain);

    if (!m_pSwapChain)
        return;

    ReCreateRenderTargetView();

    D3D10_VIEWPORT vp = {0, 0, m_sizeBackBuffer.cx, m_sizeBackBuffer.cy, 0, 1};
    g_pD3D10App->m_pDevice->RSSetViewports( 1, &vp );

}

void  HardwareComposition::ReCreateRenderTargetView()
{
    if (m_pRenderTargetView)
    {
        g_pD3D10App->m_pDevice->OMSetRenderTargets(0, NULL, NULL);
        SAFE_RELEASE(m_pRenderTargetView);
    }

    ID3D10Texture2D*  pBuffer = NULL;
    HRESULT hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (VOID**)&pBuffer);
    if (SUCCEEDED(hr))
    {
        // We need to create a render target view because 
        // we would like to bind the back buffer 
        // of our swap chain as a render target, so that 
        // Direct3D 10 can render onto it. 
        hr = g_pD3D10App->m_pDevice->CreateRenderTargetView(pBuffer, NULL, &m_pRenderTargetView);
        g_pD3D10App->m_pDevice->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);
    }
    pBuffer->Release();
}
void  HardwareComposition::Resize(UINT nWidth, UINT nHeight)
{
    if (!m_pSwapChain)
    {
        CreateSwapChain();
        if (!m_pSwapChain)
            return;
    }

    if (m_sizeBackBuffer.cx == (int)nWidth && m_sizeBackBuffer.cy == (int)nHeight)
        return;

    // 光ResizeBuffers有问题，还得重新创建rendertargetview
	// Release all outstanding references to the swap chain's buffers.
    g_pD3D10App->m_pDevice->OMSetRenderTargets(0, NULL, NULL);
    SAFE_RELEASE(m_pRenderTargetView);

    m_pSwapChain->ResizeBuffers(1, nWidth, nHeight, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
    ReCreateRenderTargetView();

    m_sizeBackBuffer.cx = nWidth;
    m_sizeBackBuffer.cy = nHeight;
    D3D10_VIEWPORT vp = {0, 0, m_sizeBackBuffer.cx, m_sizeBackBuffer.cy, 0, 1};
    g_pD3D10App->m_pDevice->RSSetViewports( 1, &vp );

    D3DXMATRIX  matrix;
    D3DXMatrixOrthoOffCenterLH(&matrix, 
        0, (float)m_sizeBackBuffer.cx, 
        (float)m_sizeBackBuffer.cy, 0,
        -2000.f, 2000.f);
    g_pD3D10App->m_pFxOrthMatrix->SetMatrix((float*)&matrix);
}


bool  HardwareComposition::BeginCommit()
{
    if (!m_pSwapChain)
        return false;

    if (!g_pD3D10App->IsActiveSwapChain(m_hWnd))
    {
        D3D10_VIEWPORT vp = {0, 0, m_sizeBackBuffer.cx, m_sizeBackBuffer.cy, 0, 1};
        g_pD3D10App->m_pDevice->RSSetViewports( 1, &vp );
        g_pD3D10App->m_pDevice->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);
        
        D3DXMATRIX  matrix;
        D3DXMatrixOrthoOffCenterLH(&matrix, 
            0, (float)m_sizeBackBuffer.cx, 
            (float)m_sizeBackBuffer.cy, 0,
            -2000.f, 2000.f);
        g_pD3D10App->m_pFxOrthMatrix->SetMatrix((float*)&matrix);

        g_pD3D10App->SetActiveSwapChain(m_hWnd);
    }

    // FLOAT rgba[4] =  { 1,1,1,0 };  // !!! 使用{0,0,0,0}会导致渲染不正常，我也不知道，反正花了好多天才发现的
    // g_pD3D10App->m_pDevice->ClearRenderTargetView(
    //         m_pRenderTargetView, rgba);

    return true;

}
void  HardwareComposition::EndCommit()
{
//     if (m_pRootTexture)
//         m_pRootTexture->Commit();

    //TODO: 什么场景会触发Dx10 lost device?  
    // 修改分辨率、颜色深度、锁屏都没出现什么问题
    HRESULT hr = m_pSwapChain->Present(1, 0);
    if( DXGI_STATUS_OCCLUDED == hr )
    {
        // There is a window covering our entire rendering area.
        // Don't render until we're visible again.
        // 或者将窗口的大小拉伸为0时，也会出现这个返回值
    }
    else if( DXGI_ERROR_DEVICE_RESET == hr )
    {
        // If a mode change happened, we must reset the device
    }
    else if( DXGI_ERROR_DEVICE_REMOVED == hr )
    {
    }
    else if( SUCCEEDED( hr ) )
    {
    }
}
