#include "stdafx.h"
#include "d3dapp.h"
#include "UISDK\Project\UI3D\src\D3D\stage3d.h"
#include "UISDK\Project\UI3D\src\D3D\Wrap\sprite.h"

namespace UI
{
HWND  CreateTempWindow();

D3DApp::D3DApp()
{
	m_hWnd = NULL;
	m_pD3D = NULL;
	m_pDevice = NULL;
    m_pSprite = NULL;
	memset(&m_d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));

    m_pRenderTargetTexture = NULL;
    m_pTextureSurface = NULL;
    m_pOffScreenPlainSurface = NULL;
    m_pOldBackSurface = NULL;
}

D3DApp::~D3DApp()
{
	Clear();
}

bool D3DApp::Init()
{
	if (!m_pD3D)
		m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (!m_pD3D)
		return false;

	if (!m_hWnd)
		m_hWnd = CreateTempWindow();
	if (!m_hWnd)
		return false;

	D3DDISPLAYMODE dispMode = {0};
	m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dispMode);

	memset(&m_d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
	m_d3dpp.Windowed = TRUE;
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8/*dispMode.Format*/;
	m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	m_d3dpp.EnableAutoDepthStencil = TRUE;
	m_d3dpp.BackBufferCount = 1;
	// m_d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;  // 没有这个标识，GetBackBuffer将不能Lock，也不能直接调用GetDC，但会导致效率很低，不要用

	struct CreateDeviceType
	{
		D3DDEVTYPE  eDevType;
		DWORD  dwBehavior;
	};

	CreateDeviceType arr[4] = {
		{D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING},
		{D3DDEVTYPE_HAL, D3DCREATE_MIXED_VERTEXPROCESSING},
		{D3DDEVTYPE_HAL, D3DCREATE_SOFTWARE_VERTEXPROCESSING},
		{D3DDEVTYPE_REF, D3DCREATE_SOFTWARE_VERTEXPROCESSING},
	};

	for (int i = 0; i < 4; i++)
	{
		m_pD3D->CreateDevice(
			D3DADAPTER_DEFAULT,
			arr[i].eDevType,
			m_hWnd,
			arr[i].dwBehavior,
			&m_d3dpp,
			&m_pDevice);

		if (m_pDevice)
			break;
	}

	if (!m_pDevice)
		return false;

//     m_pSprite = new DxSprite;
//     m_pSprite->Create(m_pDevice);

    InitDeviceSetting();
    return true;
}

void  D3DApp::InitDeviceSetting()
{
	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);  // 正反面都显示
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);         // 关闭光照
	m_pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_AMBIENT, 0xffffffff);
    //m_pDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);   // 抗锯齿

    // 设置转换坐标
    D3DXMATRIX  matWorld;
    D3DXMatrixIdentity(&matWorld);
    m_pDevice->SetTransform( D3DTS_WORLD, &matWorld );

    D3DXVECTOR3 vEyePt( 0.0f, 3.0f,-5.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    m_pDevice->SetTransform( D3DTS_VIEW, &matView );

    D3DXMATRIX  matrixProj;
    D3DXMatrixPerspectiveFovLH( &matrixProj, D3DX_PI/4, 1.0f, 1.0f, 1000.f);
    m_pDevice->SetTransform(D3DTS_PROJECTION, &matrixProj);
}

void  D3DApp::Clear()
{
    SAFE_DELETE(m_pSprite);
    ReleaseBackBuffer();
    SAFE_RELEASE(m_pDevice);
    SAFE_RELEASE(m_pD3D);
    m_listStage3D.clear();
}

void  D3DApp::OnDeviceLost()
{
    if (m_pSprite)
        m_pSprite->Lost();

    FireDeviceLostNotify();
}

// 注：如果有哪个指针没有正确释放也会导致Reset返回InvalidCall
//     例如GetBackBuffer获得的指针必须释放 
HRESULT  D3DApp::ResetDevice()
{
	HRESULT hr = m_pDevice->Reset(&m_d3dpp);
	if (FAILED(hr))
		return hr;

    if (m_pSprite)
        m_pSprite->Reset();
    
	InitDeviceSetting();
	FireResetDeviceNotify();
	return hr;
}

void  D3DApp::FireDeviceLostNotify()
{

}
void  D3DApp::FireResetDeviceNotify()
{
	list<Stage3D*>::iterator iter = m_listStage3D.begin();
	for (; iter != m_listStage3D.end(); iter++)
	{
		(*iter)->OnResetDevice();
	}
}

bool D3DApp::RequestBackBuffer(DWORD nWidth, DWORD nHeight )
{
//     if (m_pRenderTargetTexture)
//     {
//         D3DSURFACE_DESC  desc;
//         memset(&desc, 0, sizeof(desc));
// 
//         m_pRenderTargetTexture->GetLevelDesc(0, &desc);
//         if (desc.Width < nWidth || desc.Height < nHeight)
//         {
//             ReleaseBackBuffer();
//         }
//     }

	if (nWidth != m_d3dpp.BackBufferWidth || nHeight != m_d3dpp.BackBufferHeight)
	{
		//SetWindowPos(m_hWnd, NULL, 0, 0, nWidth, nHeight, SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);  // 不用改变窗口大小也可以
		m_d3dpp.BackBufferWidth = nWidth;
		m_d3dpp.BackBufferHeight = nHeight;

		ReleaseBackBuffer();
		if (FAILED(ResetDevice()))
		{
			return false;
		}
	}

	if (m_pRenderTargetTexture) // 不需要重新创建
		return true;

    bool bOk = false;
    do 
    {
        HRESULT hr = m_pDevice->CreateOffscreenPlainSurface(nWidth, nHeight, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &m_pOffScreenPlainSurface, NULL);
        if (FAILED(hr))
            break;

        hr = D3DXCreateTexture(m_pDevice, nWidth, nHeight, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pRenderTargetTexture);
        if (FAILED(hr))
            break;

        hr = m_pRenderTargetTexture->GetSurfaceLevel(0, &m_pTextureSurface);
        if (FAILED(hr))
            break;

        m_pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pOldBackSurface);
        m_pDevice->SetRenderTarget(0, m_pTextureSurface);

        bOk = true;
    } while (0);

    if (!bOk)
    {
        ReleaseBackBuffer();
        return false;
    }
    return true;
}

void  D3DApp::ReleaseBackBuffer()
{
	if (m_pOldBackSurface)
	{
		m_pDevice->SetRenderTarget(0, m_pOldBackSurface);
		SAFE_RELEASE(m_pOldBackSurface);
	}

    SAFE_RELEASE(m_pRenderTargetTexture);
    SAFE_RELEASE(m_pTextureSurface);
    SAFE_RELEASE(m_pOffScreenPlainSurface);
}


// 规定从surface的左上角开始提交
void  D3DApp::CommitBackBuffer(IRenderTarget* pRenderTarget, int nWidth, int nHeight)
{
    HRESULT hr = m_pDevice->GetRenderTargetData(m_pTextureSurface, m_pOffScreenPlainSurface);  // 最耗时函数！

    HDC hDC = NULL;
    if (FAILED(m_pOffScreenPlainSurface->GetDC(&hDC)))
        return;

	BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
	//if (!AlphaBlend(pRenderTarget->GetBindHDC(), 0, 0, nWidth, nHeight, hDC, 0, 0, nWidth, nHeight, bf))
	{
		// TODO: Win7 64位 TNND，为什么会失败！！！
//		UIASSERT(0 && _T("AlphaBlend Failed!"));
        BitBlt(pRenderTarget->GetBindHDC(), 0, 0, nWidth, nHeight, hDC, 0, 0, SRCCOPY);
	}

    m_pOffScreenPlainSurface->ReleaseDC(hDC);
}

void  D3DApp::AddStage(Stage3D* pStage)
{
	if (!pStage)
		return;

	m_listStage3D.push_back(pStage);
}
void  D3DApp::RemoveStage(Stage3D* pStage)
{
	if (!pStage)
		return;

	list<Stage3D*>::iterator iter = std::find(m_listStage3D.begin(), m_listStage3D.end(), pStage);
	if (iter != m_listStage3D.end())
	{
		m_listStage3D.erase(iter);
	}
}

// 将一个RECT

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

	return CreateWindow(wcex.lpszClassName, _T(""), WS_POPUP,
		CW_USEDEFAULT, CW_USEDEFAULT, 1, 1, NULL, NULL, NULL, NULL);
}

}
