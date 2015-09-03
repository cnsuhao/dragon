#include "stdafx.h"
#include "D3DApp.h"
#include "other\resource.h"

using namespace UI;

D3D10App::D3D10App()
{
	m_pDevice = NULL;
	m_pEffect = NULL;
	m_pTechDrawTexture = NULL;
	m_pTechDrawRect = NULL;
	m_pTechFillRect = NULL;
    m_pFxTexture10 = NULL;
	m_pTechDrawTextureMatrix = NULL;
    m_pFxOrthMatrix = NULL;
	m_pFxMatrix = NULL;
	m_pRasterizerState = NULL;
    m_pVertexBuffer = NULL;
    m_pInputLayout = NULL;
    m_pSprite10 = NULL;
    m_hActiveWnd = NULL;
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

	SAFE_RELEASE(m_pRasterizerState);
    SAFE_RELEASE(m_pInputLayout);
    SAFE_RELEASE(m_pVertexBuffer);
    m_pFxTexture10 = NULL;
	m_pFxMatrix = NULL;
    m_pTechDrawRect = NULL;
	m_pTechFillRect = NULL;
    m_pTechDrawTexture = NULL;
	m_pTechDrawTextureMatrix = NULL;
    m_pFxOrthMatrix = NULL;

    SAFE_RELEASE(m_pSprite10);
	SAFE_RELEASE(m_pEffect);
	SAFE_RELEASE(m_pDXGIFactory);

	if (m_pDevice)
	{
		ULONG lRet = m_pDevice->Release();
		UIASSERT(0 == lRet);
		m_pDevice = NULL;
	}
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

		/* 应用程序请求的操作依赖于已缺失或不匹配的 SDK 组件。

		 Windows 8 users: If you upgrade to Windows 8.1, remember that all 
		 attempts to use D3Dxx_CREATE_DEVICE_DEBUG will fail until you 
		 upgrade the Developer Runtime. You can do this by installing VS 
		 2013, the standalone Windows 8.1 SDK, or by installing the VS 2013 
		 Remote Debugging Tools. If you are missing the updated SDK Debug 
		 Layers, you should see the following message in your debug output:

         D3D11CreateDevice: Flags (0x2) were specified which require the
		 D3D11 SDK Layers for Windows 10, but they are not present on
		 the system.
		 These flags must be removed, or the Windows 10 SDK must be
		 installed.
		 Flags include: D3D11_CREATE_DEVICE_DEBUG

		*/
		if (hr == 0x887a002d && flags & D3D10_CREATE_DEVICE_DEBUG) 
		{
			flags &= ~D3D10_CREATE_DEVICE_DEBUG;
			return CreateD3DDevice(pAdapter, driverType, false, ppDevice);
		}

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
	// 若要使用 Direct2D，必须使用 D3D10_CREATE_DEVICE_BGRA_SUPPORT 
	// 标志创建提供 IDXGISurface 的 Direct3D 设备。
    UINT  nCreateDeviceFlags = D3D10_CREATE_DEVICE_BGRA_SUPPORT; 
#ifdef _DEBUG
    nCreateDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif

	ID3D10Device1*  pDevice = NULL;
	IDXGIDevice*    pDXGIDevice = NULL;
	IDXGIAdapter*   pAdapter = NULL;

	// Create device
	HRESULT hr = CreateD3DDevice(
		NULL,
		D3D10_DRIVER_TYPE_HARDWARE,
		nCreateDeviceFlags,
		&pDevice
		);

// 	if (FAILED(hr))
// 	{
// 		hr = CreateD3DDevice(
// 			NULL,
// 			D3D10_DRIVER_TYPE_WARP,
// 			nCreateDeviceFlags,
// 			&pDevice
// 			);
// 	}

	if (!pDevice)
		return false;

	hr = pDevice->QueryInterface(__uuidof(ID3D10Device), (void**)&m_pDevice);
	if (!m_pDevice)
		return false;

	hr = pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDevice);
	hr = pDXGIDevice->GetAdapter(&pAdapter);
	hr = pAdapter->GetParent(IID_PPV_ARGS(&m_pDXGIFactory));

	SAFE_RELEASE(pDevice);
	SAFE_RELEASE(pDXGIDevice);
	SAFE_RELEASE(pAdapter);

    D3DX10CreateSprite(m_pDevice, 512, &m_pSprite10);

	if (!InitRender())
        return false;

	// Create an orthographic projection matrix for 2D rendering.
	//D3DXMatrixOrthoLH(&m_orthoMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenDepth);

    return true;
}

bool  D3D10App::InitRender()
{
    // Create Effect
    DWORD dwShaderFlags = /*D3D10_SHADER_ENABLE_STRICTNESS |*/ D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY;
#ifdef _DEBUG
    dwShaderFlags |= D3D10_SHADER_DEBUG|D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	HRESULT  hCompileResult = S_OK;
	ID3D10Blob*  pCompileBlob = NULL;

#ifdef _DEBUG
	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(g_hInstance, szPath,MAX_PATH);
	TCHAR* p = _tcsrchr(szPath, TEXT('\\'));
	if (p)
		*(p+1) = 0;
	_tcscat(szPath, TEXT("..\\..\\..\\UISDK\\Project\\UICompositor\\Src\\d3d10\\d3d10.fx"));
	HRESULT hr = D3DX10CreateEffectFromFile(szPath, NULL, NULL, "fx_4_0", 
		dwShaderFlags, 0, m_pDevice, NULL,
		NULL, &m_pEffect, &pCompileBlob, &hCompileResult);
#else
    // 必须放在 RCDATA 资源当中
    HRESULT hr = D3DX10CreateEffectFromResource(g_hInstance, MAKEINTRESOURCE(fx_4_0), 0,
        NULL, NULL, "fx_4_0", dwShaderFlags, 0, m_pDevice, NULL, NULL, &m_pEffect, 
		&pCompileBlob, &hCompileResult);
#endif	
	if (pCompileBlob)
	{
		char* pErrorDesc = (char*)pCompileBlob->GetBufferPointer();
		MessageBoxA(NULL, pErrorDesc, "load fx failed.", MB_OK|MB_ICONWARNING);
		SAFE_RELEASE(pCompileBlob);
	}

    if (FAILED(hr))
    {
        return false;
    }

    m_pTechDrawTexture = m_pEffect->GetTechniqueByName( "DrawTexture" );
	m_pTechFillRect = m_pEffect->GetTechniqueByName( "FillRect" );
    m_pTechDrawRect = m_pEffect->GetTechniqueByName( "DrawRect" );
	m_pTechDrawTextureMatrix = m_pEffect->GetTechniqueByName( "DrawTextureMatrix" );
    m_pFxTexture10 = m_pEffect->GetVariableByName( "g_Texture" )->AsShaderResource();
	m_pFxMatrix = m_pEffect->GetVariableByName( "g_Matrix" )->AsMatrix();
    m_pFxOrthMatrix = m_pEffect->GetVariableByName( "g_orthMatrix" )->AsMatrix();

    // Create input layout
    const D3D10_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",     0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,       0, 28, D3D10_INPUT_PER_VERTEX_DATA, 0 },
    };

    D3D10_PASS_DESC PassDesc = {0};
    ID3D10EffectPass*  pPass = m_pTechDrawRect->GetPassByIndex( 0 );
    hr = pPass->GetDesc( &PassDesc );
    m_pDevice->CreateInputLayout( layout, 3, PassDesc.pIAInputSignature,
        PassDesc.IAInputSignatureSize, &m_pInputLayout );

    D3D10_BUFFER_DESC BufDesc;
    BufDesc.ByteWidth = sizeof( DXUT_SCREEN_VERTEX_10 ) * 4;
    BufDesc.Usage = D3D10_USAGE_DYNAMIC;
    BufDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    BufDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    BufDesc.MiscFlags = 0;

    hr = m_pDevice->CreateBuffer(&BufDesc, 
        NULL, 
        &m_pVertexBuffer);
    if (FAILED(hr))
        return false;

    m_pDevice->IASetInputLayout(m_pInputLayout);
    m_pDevice->IASetPrimitiveTopology( 
        D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

    UINT stride = sizeof( DXUT_SCREEN_VERTEX_10 );
    UINT offset = 0;
    m_pDevice->IASetVertexBuffers(0, 1, 
        &m_pVertexBuffer,
        &stride, &offset);

    
    D3D10_RASTERIZER_DESC drd = {
        D3D10_FILL_SOLID, //D3D11_FILL_MODE FillMode;
        D3D10_CULL_NONE,  //显示背面
        FALSE, //BOOL FrontCounterClockwise;
        0, //INT DepthBias;
        0.0f,//FLOAT DepthBiasClamp;
        0.0f,//FLOAT SlopeScaledDepthBias;
        FALSE,//BOOL DepthClipEnable;
        FALSE,//BOOL ScissorEnable;  // 是否允许剪裁
        FALSE,//BOOL MultisampleEnable;
        FALSE//BOOL AntialiasedLineEnable;        
    };
    m_pDevice->CreateRasterizerState(&drd, &m_pRasterizerState);
    m_pDevice->RSSetState(m_pRasterizerState); 

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

void  D3D10App::ApplyTechnique(ID3D10EffectTechnique* pTech, RECTF*  prcDraw, D3DCOLORVALUE color)
{
	DXUT_SCREEN_VERTEX_10 vertices[4] =
	{
		{ prcDraw->left,     prcDraw->top, 0, color, 0.0f, 0.0f },
		{ prcDraw->right,    prcDraw->top, 0, color, 1.0f, 0.0f },
		{ prcDraw->left,  prcDraw->bottom, 0, color, 0.0f, 1.0f },
		{ prcDraw->right, prcDraw->bottom, 0, color, 1.0f, 1.0f },
	};
	_ApplyTechnique(pTech, vertices);
}
void  D3D10App::ApplyTechnique(ID3D10EffectTechnique* pTech, RECTF* prcDraw, RECTF* prcTexture, float fAlpha)
{
	DXUT_SCREEN_VERTEX_10 vertices[4] =
	{
 		{ prcDraw->left,  prcDraw->top,    0.f, {1,1,1,fAlpha}, prcTexture->left,  prcTexture->top},   
 		{ prcDraw->right, prcDraw->top,    0.f, {1,1,1,fAlpha}, prcTexture->right, prcTexture->top},   
 		{ prcDraw->left,  prcDraw->bottom, 0.f, {1,1,1,fAlpha}, prcTexture->left,  prcTexture->bottom},
		{ prcDraw->right, prcDraw->bottom, 0.f, {1,1,1,fAlpha}, prcTexture->right, prcTexture->bottom},
	};
	_ApplyTechnique(pTech, vertices);
}
void  D3D10App::_ApplyTechnique(ID3D10EffectTechnique* pTech, DXUT_SCREEN_VERTEX_10 vertices[4])
{
	DXUT_SCREEN_VERTEX_10* pVB;
	if (SUCCEEDED(m_pVertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (LPVOID*)&pVB)))
	{
		CopyMemory( pVB, vertices, sizeof( DXUT_SCREEN_VERTEX_10 )*4 );
		m_pVertexBuffer->Unmap();
	}

	D3D10_TECHNIQUE_DESC techDesc;
	pTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		pTech->GetPassByIndex(p)->Apply(0);
		m_pDevice->Draw(4, 0);
	}
}


bool  D3D10App::IsActiveSwapChain(HWND  hWnd)
{
    return m_hActiveWnd == hWnd ? true : false;
}
void  D3D10App::SetActiveSwapChain(HWND hWnd)
{
    m_hActiveWnd = hWnd;
}