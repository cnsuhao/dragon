#include "stdafx.h"
#include "d2dapp.h"

namespace UI
{

	D2DApp::D2DApp()
	{
		m_pD2DFactory = NULL;
		m_pDWriteFactory = NULL;
		m_pWICFactory = NULL;
		m_pD3DRenderTarget = NULL;
	}

	//
	// Remark;
	//	不要尝试在静态/全局类的析构函数中释放COM对象。
	//  因为此时COM库可能已经卸载了
	//
	D2DApp::~D2DApp()
	{
		Release();
	}

	void  D2DApp::Release()
	{
		SAFE_RELEASE(m_pD3DRenderTarget);
		SAFE_RELEASE(m_pWICFactory);
		SAFE_RELEASE(m_pDWriteFactory);
		SAFE_RELEASE(m_pD2DFactory);
	}

	bool D2DApp::Init()
	{
		if (m_pD2DFactory)
			return true;

		HRESULT hr = S_OK;
		IDXGISurface*  pBackBuffer = NULL;

		bool bOk = false;
		do 
		{
			// Create a Direct2D factory.
			hr = D2D1CreateFactory(
				D2D1_FACTORY_TYPE_SINGLE_THREADED,
				&m_pD2DFactory);
			if (FAILED(hr))
			{
				UI_LOG_ERROR(_T("%s D2D1CreateFactory failed. hr=0x%08x"), FUNC_NAME, hr);
				break;
			}

			// Create a DirectWrite factory.
			hr = DWriteCreateFactory(
				DWRITE_FACTORY_TYPE_SHARED, 
				__uuidof(m_pDWriteFactory),
				reinterpret_cast<IUnknown **>(&m_pDWriteFactory)
				);
			if (FAILED(hr))
			{
				UI_LOG_ERROR(_T("%s DWriteCreateFactory failed. hr=0x%08x"), FUNC_NAME, hr);
				break;
			}

			// Create WIC
			hr = CoCreateInstance(
				CLSID_WICImagingFactory,
				NULL,
				CLSCTX_INPROC_SERVER,
				IID_IWICImagingFactory,
				reinterpret_cast<void **>(&m_pWICFactory)
				);
			if (FAILED(hr))
			{
				UI_LOG_ERROR(_T("%s Create WICImagingFactory failed. hr=0x%08x"), FUNC_NAME, hr);
				break;
			}

#if 0 // 创建d2d->d3d的rendertarget
			// Get a surface in the swap chain
			hr = g_pD3DApp->m_pSwapChain->GetBuffer(
				0,
				IID_PPV_ARGS(&pBackBuffer)
				);
			if (FAILED(hr))
			{
				UI_LOG_ERROR(_T("%s GetBuffer failed. hr=0x%08x"), FUNC_NAME, hr);
				break;
			}

			// Create the DXGI Surface Render Target.
			FLOAT dpiX;
			FLOAT dpiY;
			m_pD2DFactory->GetDesktopDpi(&dpiX, &dpiY);

			D2D1_RENDER_TARGET_PROPERTIES props =
				D2D1::RenderTargetProperties(
				D2D1_RENDER_TARGET_TYPE_DEFAULT,
				D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
				dpiX,
				dpiY
				);

			// Create a Direct2D render target which can draw into the surface in the swap chain
			hr = m_pD2DFactory->CreateDxgiSurfaceRenderTarget(
				pBackBuffer,
				&props,
				&m_pD3DRenderTarget
				);

			if (FAILED(hr))
			{
				UI_LOG_ERROR(_T("%s CreateDxgiSurfaceRenderTarget failed. hr=0x%08x"), FUNC_NAME, hr);
				break;
			}
#endif

#if 0 // 创建d3d->d2d的d2d1bitmap
            // Get a surface in the swap chain
            hr = g_pD3DApp->m_pSwapChain->GetBuffer(
                0,
                IID_PPV_ARGS(&pBackBuffer)
                );
            if (FAILED(hr))
            {
                UI_LOG_ERROR(_T("%s GetBuffer failed. hr=0x%08x"), FUNC_NAME, hr);
                break;
            }

            // Create the DXGI Surface Render Target.
            FLOAT dpiX;
            FLOAT dpiY;
            m_pD2DFactory->GetDesktopDpi(&dpiX, &dpiY);

            D2D1_RENDER_TARGET_PROPERTIES props =
                D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
                dpiX,
                dpiY
                );

            // Create a Direct2D render target which can draw into the surface in the swap chain
            hr = m_pD2DFactory->CreateDxgiSurfaceRenderTarget(
                pBackBuffer,
                &props,
                &m_pD3DRenderTarget
                );

            if (FAILED(hr))
            {
                UI_LOG_ERROR(_T("%s CreateDxgiSurfaceRenderTarget failed. hr=0x%08x"), FUNC_NAME, hr);
                break;
            }
#endif
			bOk = true;
		}while (0);

		if (!bOk)
		{
			Release();
		}
		SAFE_RELEASE(pBackBuffer);

		return bOk;
	}
}