#include "stdafx.h"
#include "d2drenderbuffer.h"
#include "UISDK\Kernel\Inc\Util\iimage.h"

namespace UI
{
#if 0
Direct2DRenderBuffer::Direct2DRenderBuffer()
{
    m_pWICBitmap = NULL;
    m_nWidth = m_nHeight = 0;
}
Direct2DRenderBuffer::~Direct2DRenderBuffer()
{
    Destroy();
}

void   Direct2DRenderBuffer::Resize(unsigned int width, unsigned int height)
{
    if (width == 0 || height == 0)
    {
        Destroy();
        return;
    }
    if (width == m_nWidth && height == m_nHeight)
    {
        return;
    }
    m_nWidth = width;
    m_nHeight = height;

    SAFE_RELEASE(m_pWICBitmap)
    HRESULT hr = g_pD2DApp->m_pWICFactory->CreateBitmap(
            width,
            height,
            GUID_WICPixelFormat32bppPBGRA/*GUID_WICPixelFormat32bppBGRA*/,  // 为什么使用GUID_WICPixelFormat32bppBGRA去创建RenderTarget会失败？
            WICBitmapCacheOnLoad,  // ??这个变量是什么用途
            &m_pWICBitmap
            );
    
}
void   Direct2DRenderBuffer::Clear(DWORD dwColor, RECT* prc)
{
    UIASSERT(0);
}
void   Direct2DRenderBuffer::Destroy()
{
    SAFE_RELEASE(m_pWICBitmap);
}
void   Direct2DRenderBuffer::Dump()
{
    IWICBitmapLock*  pLock = NULL;
	LockFull(&pLock);
	if (!pLock)
		return;

	UINT cbBufferSize = 0;
	UINT cbStride = 0;
	BYTE *pv = NULL;
	HRESULT hr = S_OK;

	do 
	{
	
		// Retrieve the stride.
		hr = pLock->GetStride(&cbStride);
		if (FAILED(hr))
			break;
	
		hr = pLock->GetDataPointer(&cbBufferSize, &pv);
		if (FAILED(hr))
			break;

		IImage  image;
		image.Create(m_nWidth, -(int)m_nHeight, 32, IImage::createAlphaChannel);
		byte*  pBits = (byte*)image.GetBits();

		memcpy(pBits, pv, min((int)cbBufferSize, image.GetPitch()*image.GetHeight()));
		image.SaveAsPng(L"C:\\aaa.png");

	} while (0);

	SAFE_RELEASE(pLock);
}
void   Direct2DRenderBuffer::GetImageData(ImageData*  pData)
{
    UIASSERT(0);
}

// 获取IWICBitmapLock*指针，由外部释放
bool Direct2DRenderBuffer::LockFull(IWICBitmapLock** ppLock)
{
	if (!m_pWICBitmap || !ppLock)
		return NULL;

	WICRect rcLock = { 0, 0, m_nWidth, m_nHeight };

	if (SUCCEEDED(m_pWICBitmap->Lock(&rcLock, WICBitmapLockRead, ppLock)))
		return true;

	return false;
}

#endif
}