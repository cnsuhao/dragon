#include "stdafx.h"
#include "D3DApp.h"
#include "UISDK\Project\UI3D\other\resource.h"
#include "UISDK\Project\UI3D\src\D3D\element\element.h"

namespace UI
{
HWND  CreateTempWindow();

D3D10App::D3D10App()
{
	m_hWnd = NULL;
	m_pDevice = NULL;
    m_pSwapChain = NULL;
    m_pRenderTargetView = NULL;

	m_pEffect = NULL;
	m_pET_FillRectWorld = NULL;
	m_pIL_FillRectScreen = NULL;
}

D3D10App::~D3D10App()
{
	Release();
}


void  D3D10App::Release()
{
	if (m_pDevice)
		m_pDevice->ClearState();

    // 释放所有元素的static变量
	Element::Unload();

	ReleaseBackBuffer();
	SAFE_RELEASE(m_pIL_FillRectScreen);
	m_pET_FillRectWorld = NULL;
	SAFE_RELEASE(m_pEffect);
	SAFE_RELEASE(m_pRenderTargetView);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pDevice);

	m_listStage3D.clear();
}



HRESULT CreateD3DDevice(
						IDXGIAdapter *pAdapter,
						D3D10_DRIVER_TYPE driverType,
						UINT flags,
						ID3D10Device1 **ppDevice
						)
{
	HRESULT hr = S_OK;

	static const D3D10_FEATURE_LEVEL1 levelAttempts[] =
	{
		D3D10_FEATURE_LEVEL_10_0,
		D3D10_FEATURE_LEVEL_9_3,
		D3D10_FEATURE_LEVEL_9_2,
		D3D10_FEATURE_LEVEL_9_1,
	};

	for (UINT level = 0; level < ARRAYSIZE(levelAttempts); level++)
	{
		ID3D10Device1 *pDevice = NULL;
		hr = D3D10CreateDevice1(
			pAdapter,
			driverType,
			NULL,
			flags,
			levelAttempts[level],
			D3D10_1_SDK_VERSION,
			&pDevice
			);

		if (SUCCEEDED(hr))
		{
			// transfer reference
			*ppDevice = pDevice;
			pDevice = NULL;
			break;
		}
	}

	return hr;
}
bool D3D10App::Init()
{
	if (!m_hWnd)
		m_hWnd = CreateTempWindow();
	if (!m_hWnd)
		return false;

	// 若要使用 Direct2D，必须使用 D3D10_CREATE_DEVICE_BGRA_SUPPORT 
	// 标志创建提供 IDXGISurface 的 Direct3D 设备。
    UINT  nCreateDeviceFlags = D3D10_CREATE_DEVICE_BGRA_SUPPORT; 
#ifdef _DEBUG
    nCreateDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif

	ID3D10Device1*  pDevice = NULL;
	IDXGIDevice*    pDXGIDevice = NULL;
	IDXGIAdapter*   pAdapter = NULL;
	IDXGIFactory*   pDXGIFactory = NULL;
	IDXGISurface*   pSurface = NULL;

	// Create device
	HRESULT hr = CreateD3DDevice(
		NULL,
		D3D10_DRIVER_TYPE_HARDWARE,
		nCreateDeviceFlags,
		&pDevice
		);

	if (FAILED(hr))
	{
		hr = CreateD3DDevice(
			NULL,
			D3D10_DRIVER_TYPE_WARP,
			nCreateDeviceFlags,
			&pDevice
			);
	}

	if (!pDevice)
		return false;

	hr = pDevice->QueryInterface(__uuidof(ID3D10Device), (void**)&m_pDevice);
	if (!m_pDevice)
		return false;

	hr = pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDevice);
	hr = pDXGIDevice->GetAdapter(&pAdapter);
	hr = pAdapter->GetParent(IID_PPV_ARGS(&pDXGIFactory));

	DXGI_SWAP_CHAIN_DESC swapDesc;
	::ZeroMemory(&swapDesc, sizeof(swapDesc));

	swapDesc.BufferDesc.Width = 100;   // TODO:
	swapDesc.BufferDesc.Height = 100;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.BufferCount = 1;
	swapDesc.OutputWindow = m_hWnd;
	swapDesc.Windowed = TRUE;
     

	hr = pDXGIFactory->CreateSwapChain(m_pDevice, &swapDesc, &m_pSwapChain);

	SAFE_RELEASE(pDevice);
	SAFE_RELEASE(pDXGIDevice);
	SAFE_RELEASE(pAdapter);
	SAFE_RELEASE(pDXGIFactory);

	ID3D10Texture2D*  pBuffer = NULL;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (VOID**)&pBuffer);
	if (FAILED(hr))
		return false;

#if 0

    D3D10_TEXTURE2D_DESC descText2D;
    ZeroMemory(&desc, sizeof(D3D10_TEXTURE2D_DESC));
    descText2D.Width = 64;
    descText2D.Height = 64;
    descText2D.ArraySize = 1;
    descText2D.SampleDesc.Count = 1;
    descText2D.SampleDesc.Quality = 0;
    descText2D.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    descText2D.MipLevels = 1;
    descText2D.MiscFlags = 0;
    descText2D.BindFlags = 0;
    descText2D.CPUAccessFlags = D3D10_CPU_ACCESS_READ;
    descText2D.Usage = D3D10_USAGE_STAGING;  // cpu可读写

    ID3D10Texture2D* pTexture;
    hr = m_pDevice->CreateTexture2D(&descText2D, NULL, &pTexture);;
    //http://m.blog.csdn.net/blog/ljlees0830/7853617
#endif
	// We need to create a render target view because we would like to bind the back buffer 
	// of our swap chain as a render target, so that Direct3D 10 can render onto it. 
    hr = m_pDevice->CreateRenderTargetView(/*pTexture*/pBuffer, NULL, &m_pRenderTargetView);
    pBuffer->Release();
    if (FAILED(hr))
        return false;

//     IDXGISurface1* pSurface = NULL;
//     pTexture->QueryInterface(__uuidof(IDXGISurface1), (void **)(&pSurface));
//     HDC hDC = NULL;
//     hr = pSurface->GetDC(FALSE, &hDC);
//     pSurface->ReleaseDC(NULL);

    m_pDevice->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);
    
    // Setup the viewport
	// The viewport maps clip space coordinates, where X and Y range from -1 to 1
	// and Z ranges from 0 to 1, to render target space, sometimes known as pixel 
	// space. In Direct3D 9, if the application does not set up a viewport, a default 
	// viewport is set up to be the same size as the render target. In Direct3D 10, 
	// no viewport is set by default. Therefore, we must do so before we can see 
	// anything on the screen. Since we would like to use the entire render target 
	// for the output, we set the top left point to (0, 0) and width and height to 
	// be identical to the render target's size. 

    D3D10_VIEWPORT vp;
    vp.Width = 100;
    vp.Height = 100;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    m_pDevice->RSSetViewports( 1, &vp );

    // Create Effect
    DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#ifdef _DEBUG
    dwShaderFlags |= D3D10_SHADER_DEBUG;
#endif
    // 必须放在 RCDATA 资料当中
    hr = D3DX10CreateEffectFromResource(g_hInstance, MAKEINTRESOURCE(IDR_SHADER_D3D10), 0,
                NULL, NULL, "fx_4_0", dwShaderFlags, 0, m_pDevice, NULL, NULL, &m_pEffect, NULL, NULL);
    if (FAILED(hr))
        return false;

   
	// Create an orthographic projection matrix for 2D rendering.
	//D3DXMatrixOrthoLH(&m_orthoMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenDepth);

    return true;
}


void  D3D10App::OnDeviceLost()
{
    FireDeviceLostNotify();
}

// 注：如果有哪个指针没有正确释放也会导致Reset返回InvalidCall
//     例如GetBackBuffer获得的指针必须释放 
HRESULT  D3D10App::ResetDevice()
{
    return E_NOTIMPL;
}

void  D3D10App::FireDeviceLostNotify()
{

}
void  D3D10App::FireResetDeviceNotify()
{
}

bool D3D10App::RequestBackBuffer(DWORD nWidth, DWORD nHeight )
{
    return true;
}

void  D3D10App::ReleaseBackBuffer()
{
}


// 规定从surface的左上角开始提交
void  D3D10App::CommitBackBuffer(IRenderTarget* pRenderTarget, int nWidth, int nHeight)
{
}


void  D3D10App::AddStage(Stage3D* pStage)
{
    if (!pStage)
        return;

    m_listStage3D.push_back(pStage);
}
void  D3D10App::RemoveStage(Stage3D* pStage)
{
    if (!pStage)
        return;

    list<Stage3D*>::iterator iter = std::find(m_listStage3D.begin(), m_listStage3D.end(), pStage);
    if (iter != m_listStage3D.end())
    {
        m_listStage3D.erase(iter);
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, message, wParam, lParam);
}

HWND  CreateTempWindow()
{
	WNDCLASSEX wcex = {0};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.lpszClassName	= _T("Direct3DWindow");
	RegisterClassEx(&wcex);

	return CreateWindow(wcex.lpszClassName, _T(""), WS_POPUP|WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, 500, 500, NULL, NULL, NULL, NULL);
}

}
