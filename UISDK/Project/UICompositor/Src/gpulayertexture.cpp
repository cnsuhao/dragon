#include "stdafx.h"
#include "GpuLayerTexture.h"
#include "hardwarecompositor.h"
#include "D3D10/d3dapp.h"
#include <atlimage.h>
#include "hard3dtransform.h"

using namespace UI;


// http://www.chromium.org/developers/design-documents/gpu-accelerated-compositing-in-chrome

GpuLayerTexture::GpuLayerTexture()
{
	m_pParent = m_pChild = m_pNext = NULL;
	m_size.cx = m_size.cy = 0;
    m_pTextureBuffer = NULL;
    m_pShaderResourceView = NULL;

    m_pCompositor = NULL;
    m_IGpuLayerTexture.SetImpml(this);

#if 0
	D3DXMatrixIdentity(&m_transform);
	m_bNeedUpdateTransform = false;
    m_bTransformValid = false;

	m_fTranslationX = m_fTranslationY = m_fTranslationZ = 0;
	m_fRotationX = m_fRotationY = m_fRotationZ = 0;
	m_fScaleX = m_fScaleY = m_fScaleZ = 1;
#endif
}

GpuLayerTexture::~GpuLayerTexture()
{
	GpuLayerTexture* pChild = m_pChild;
	while (pChild)
	{
		GpuLayerTexture* pTemp = pChild->m_pNext;
		SAFE_DELETE(pChild);
		pChild = pTemp;
	}
	m_pChild = m_pNext = NULL;
	m_pParent = NULL;

    SAFE_RELEASE(m_pTextureBuffer);
    SAFE_RELEASE(m_pShaderResourceView);
}

IGpuLayerTexture*  GpuLayerTexture::GetIGpuLayerTexture()
{
    return &m_IGpuLayerTexture;
}

void  GpuLayerTexture::Release()
{
    delete this;
}

void  GpuLayerTexture::SetHardwareComposition(HardwareComposition* p)
{
    m_pCompositor = p;
}

void  GpuLayerTexture::ClearChildren()
{
    m_vecChild.clear();
}
void  GpuLayerTexture::AddChild(GpuLayerTexture* pChild)
{
    m_vecChild.push_back(pChild);
}
//----------------------------------------------------------------------------------------
//
// D3D11_USAGE_DYNAMIC
// Dynamic资源可以用于实现每一帧中将将数据从CPU上传到GPU
//
// A resource that is accessible by both the GPU (read only) and the CPU (write only). 
// A dynamic resource is a good choice for a resource that will be updated by the CPU 
// at least once per frame. To update a dynamic resource, use a Map method.
// 
// Dynamic资源的使用方法 （How to: Use dynamic resources）
// http://msdn.microsoft.com/zh-cn/subscriptions/downloads/dn508285.aspx
//
// 资源类型 （D3D11_USAGE enumeration）
// http://msdn.microsoft.com/zh-cn/subscriptions/downloads/ff476259.aspx
//
//---------------------------------------------------------------------------------------
//
// D3D11_MAP_WRITE_DISCARD的机制：
//
// 它将抛弃原有的数据，返回一块新的空缓存给CPU，GPU就不用等待CPU了
// 因此你也不能对这块buffer使用脏矩形了，必须完整更新整个buffer. <<--
//    怪不得用脏区域更新会出现脏区域之外的数据花屏、内容丢失的问题了
//
// To receive a performance improvement when you use dynamic vertex buffers, 
// your app must call Map with the appropriate D3D11_MAP values. 
// D3D11_MAP_WRITE_DISCARD indicates that the app doesn't need to keep the old
// vertex or index data in the buffer. If the GPU is still using the buffer 
// when you call Map with D3D11_MAP_WRITE_DISCARD, the runtime returns a 
// pointer to a new region of memory instead of the old buffer data. This 
// allows the GPU to continue using the old data while the app places data in 
// the new buffer. No additional memory management is required in the app; the
// old buffer is reused or destroyed automatically when the GPU is finished 
// with it.
//
//---------------------------------------------------------------------------------------
//
void  GpuLayerTexture::UploadHBITMAP(
		HBITMAP hBitmap, 
		LPRECT prcArray, 
		UINT nCount, 
		bool bTopdown, 
		bool bHasAlphaChannel)
{
    prcArray = 0;
    nCount = 0;

    if (!hBitmap)
        return;

    DIBSECTION dib = {0};
    if (sizeof(dib) != GetObject(hBitmap, sizeof(dib), &dib))
        return;

    BITMAP& bm = dib.dsBm;
    if (bm.bmBitsPixel != 32)
        return;

    if (!m_pTextureBuffer || !m_pShaderResourceView)
    {
        if (!CreateSurface(bm.bmWidth, bm.bmHeight))
            return;

        if (!m_pShaderResourceView || !m_pTextureBuffer)
            return;
    }

    if (!bTopdown)
    {
        bm.bmBits = (byte*)bm.bmBits + (bm.bmHeight-1)*bm.bmWidthBytes;
        bm.bmWidthBytes = -bm.bmWidthBytes;
    }

    RECT  rcFull = {0, 0, bm.bmWidth, bm.bmHeight};
    if (0 == nCount || NULL == prcArray)
    {
        nCount = 1;
        prcArray = &rcFull;
    }
    for (UINT i = 0; i < nCount; i++)
    {
        RECT  rc = {0, 0, bm.bmWidth, bm.bmHeight};
        if (prcArray)
        {
            IntersectRect(&rc, &rc, &prcArray[i]);
        }

        D3D10_MAPPED_TEXTURE2D mappedTexture;
        UINT  nSub = D3D10CalcSubresource(0, 0, 1);
        HRESULT hr = m_pTextureBuffer->Map(nSub, D3D10_MAP_WRITE_DISCARD, 0, &mappedTexture);
        if (SUCCEEDED(hr))
        {
            D3D10_TEXTURE2D_DESC desc;     
            m_pTextureBuffer->GetDesc( &desc );     

            BYTE*  pSrcBits = (BYTE*)bm.bmBits;
            byte*  pTexels = (byte*)mappedTexture.pData;    

            pSrcBits += rc.top * bm.bmWidthBytes;
            pTexels += rc.top * mappedTexture.RowPitch;

            if (bHasAlphaChannel)
            {
                for( int row = rc.top; row < rc.bottom; row++ )  
                {       
                    for( int col = rc.left; col < rc.right; col++ )  
                    {            
                        pTexels[col*4 + 0] = pSrcBits[col*4 + 2]; // Red       
                        pTexels[col*4 + 1] = pSrcBits[col*4 + 1]; // Green     
                        pTexels[col*4 + 2] = pSrcBits[col*4 + 0]; // Blue        
                        pTexels[col*4 + 3] = pSrcBits[col*4 + 3]; // Alpha   
                    }    

                    pSrcBits += bm.bmWidthBytes;
                    pTexels += mappedTexture.RowPitch;
                }  
            }
            else
            {
                for( int row = rc.top; row < rc.bottom; row++ )  
                {       
                    for( int col = rc.left; col < rc.right; col++ )  
                    {        
                        pTexels[col*4 + 0] = pSrcBits[col*4 + 2]; // Red       
                        pTexels[col*4 + 1] = pSrcBits[col*4 + 1]; // Green     
                        pTexels[col*4 + 2] = pSrcBits[col*4 + 0]; // Blue        
                        pTexels[col*4 + 3] = 255;
                    }    

                    pSrcBits += bm.bmWidthBytes;
                    pTexels += mappedTexture.RowPitch;
                }  
            }

            m_pTextureBuffer->Unmap(nSub);

        }// m_pTextureBuffer->Map
    
    }// for

#ifdef _DEBUG
	static bool bDebug = false;
	if (bDebug)
	{
		static int i = 0; 
		i++;
		TCHAR szTest[256];
		_stprintf(szTest, TEXT("c:\\AAA\\%d.dds"),i);
		D3DX10SaveTextureToFile(m_pTextureBuffer, D3DX10_IFF_DDS, szTest);

		CImage image;
		image.Attach(hBitmap);
		if (prcArray)
			_stprintf(szTest, TEXT("c:\\AAA\\%d_(%d_%d_%d_%d).png"),i,
				prcArray[0].left,prcArray[0].top,prcArray[0].right,prcArray[0].bottom);            
		else
			_stprintf(szTest, TEXT("c:\\AAA\\%d_.png"),i);
		image.Save(szTest, Gdiplus::ImageFormatPNG);
		image.Detach();
	}
#endif

}

bool  GpuLayerTexture::CreateSurface(UINT nWidth, UINT nHeight)
{
    if (m_pTextureBuffer || m_pShaderResourceView)
        return false;

    if (!g_pD3D10App || !g_pD3D10App->m_pDevice)
        return false;

    if (nWidth == 0 || nHeight == 0)
        return false;

    D3D10_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(textureDesc));

    textureDesc.Width = nWidth;
    textureDesc.Height = nHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; 
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
    textureDesc.Usage = D3D10_USAGE_DYNAMIC;
    textureDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    textureDesc.MiscFlags = 0;

    HRESULT hr = g_pD3D10App->m_pDevice->CreateTexture2D(&textureDesc, 
        NULL, &m_pTextureBuffer);
    if (FAILED(hr))
        return false;

    m_size.cx = nWidth;
    m_size.cy = nHeight;

    D3D10_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    hr = g_pD3D10App->m_pDevice->CreateShaderResourceView(m_pTextureBuffer, 
        &shaderResourceViewDesc, &m_pShaderResourceView);
    if (FAILED(hr))
    {
        SAFE_RELEASE(m_pTextureBuffer);
        return false;
    }

    return true;
}

void  GpuLayerTexture::Resize(UINT nWidth, UINT nHeight)
{
    if (m_size.cx == (int)nWidth && m_size.cy == (int)nHeight)
        return ;

    SAFE_RELEASE(m_pTextureBuffer);
    SAFE_RELEASE(m_pShaderResourceView);   //?? 只能这样？
    CreateSurface(nWidth, nHeight);
}

void  GpuLayerTexture::CalcDrawDestRect(__in RECTF* prc, __out RECTF* prcfOut)
{
    prcfOut->left = prc->left * 2.0f / m_size.cx - 1.0f;
    prcfOut->right = prc->right * 2.0f / m_size.cx - 1.0f;
    prcfOut->top = 1.0f - prc->top * 2.0f / m_size.cy;
    prcfOut->bottom = 1.0f - prc->bottom * 2.0f / m_size.cy;
}
void  GpuLayerTexture::CalcDrawDestRect(int xDest, int yDest, UINT wDest, UINT hDest, __out RECTF* prcfOut)
{
    prcfOut->left = xDest * 2.0f / m_size.cx - 1.0f;
    prcfOut->right = (xDest+wDest) * 2.0f / m_size.cx - 1.0f;
    prcfOut->top = 1.0f - yDest * 2.0f / m_size.cy;
    prcfOut->bottom = 1.0f - (yDest+hDest) * 2.0f / m_size.cy;
}
void  GpuLayerTexture::CalcDrawDestRect(float xDest, float yDest, float wDest, float hDest, __out RECTF* prcfOut)
{
    prcfOut->left = xDest * 2.0f / m_size.cx - 1.0f;
    prcfOut->right = (xDest+wDest) * 2.0f / m_size.cx - 1.0f;
    prcfOut->top = 1.0f - yDest * 2.0f / m_size.cy;
    prcfOut->bottom = 1.0f - (yDest+hDest) * 2.0f / m_size.cy;
}

void  GpuLayerTexture::Compositor(GpuLayerCommitContext* pContext, IRenderLayerTransform2* pTransform)
{
    if (!m_pShaderResourceView)
        return ;

    if (0 == m_size.cx || 0 == m_size.cy)
        return;

	if (pTransform && pTransform->get_type() != TRANSFORM_HARD3D)
		return;

	// TODO:
// 	{
// 		Matrix44  matrix;
// 		static_cast<IRenderLayerTransform3D*>(m_pTransform2)->get_matrix(&matrix);
// 		context.MultiMatrix((float*)&matrix);
// 	}

    RECTF  rcTexture;
    rcTexture.left   = (float)0;
    rcTexture.right  = (float)1;
    rcTexture.top    = (float)0;
    rcTexture.bottom = (float)1;

	pContext->UpdateScissorRect(g_pD3D10App->m_pDevice);
    if (pContext->m_bTransformValid || pTransform)
    {
		Hard3DTransform* pHard3dTransform = static_cast<Hard3DTransform*>(pTransform);
        pHard3dTransform->set_pos(pContext->m_xOffset, pContext->m_yOffset);

		D3DXMATRIX  matrix;
        pHard3dTransform->GetWorldMatrix(&matrix);

        RECTF rcLocal;
        pHard3dTransform->GetLocalPos(&rcLocal);

        oStretchBltMatrix(
            &matrix, 
            &rcLocal,
            &rcTexture,
            pContext->m_fAlpha);
    }
    else
    {
        RECTF rcLocal;
        rcLocal.left = (float)pContext->m_xOffset;
        rcLocal.top = (float)pContext->m_yOffset;
        rcLocal.right = (float)(pContext->m_xOffset+m_size.cx);
        rcLocal.bottom = (float)(pContext->m_yOffset+m_size.cy);

        oStretchBlt(&rcLocal, &rcTexture, pContext->m_fAlpha);
    }
}

// void   GpuLayerTexture::oBitBlt(int xDest, int yDest, int /*wDest*/, int /*hDest*/, int xSrc, int ySrc)
// {
//     oStretchBlt((float)xDest, (float)yDest, (float)m_size.cx, (float)m_size.cy, xSrc, ySrc, m_size.cx, m_size.cy);
// }

void   GpuLayerTexture::oStretchBlt(RECTF* prcLocal, RECTF* prcTexture, float fAlpha)
{
    HRESULT hr = 0;
    hr = g_pD3D10App->m_pFxTexture10->SetResource(m_pShaderResourceView);
    g_pD3D10App->ApplyTechnique(g_pD3D10App->m_pTechDrawTexture, prcLocal, prcTexture, fAlpha);

//     g_pD3D10App->m_pFxTexture10->SetResource(NULL);
//     g_pD3D10App->m_pTechDrawTexture->GetPassByIndex( 0 )->Apply( 0 );


#if 0  // 透视投影的代码，按比例设置大小。改成正交投影就没这么麻烦了
//     void  oStretchBlt(
//         float xDest, float yDest, float wDest, float hDest, 
//         int xSrc, int ySrc, int wSrc, int hSrc);

	GpuLayerTexture* pRoot = m_pCompositor->GetRootLayerTexture();
	if (!pRoot)
		return;

    RECTF  rcDrawDest;
    RECTF  rcDrawSrc;

    pRoot->CalcDrawDestRect(xDest,yDest,wDest,hDest, &rcDrawDest);

    rcDrawSrc.left   = (float)xSrc / m_size.cx;
    rcDrawSrc.right  = (float)(xSrc+wSrc) / m_size.cx;
    rcDrawSrc.top    = (float)ySrc / m_size.cy;
    rcDrawSrc.bottom = (float)(ySrc+hSrc) / m_size.cy;

    /*HRESULT hr = */g_pD3D10App->m_pFxTexture10->SetResource(m_pShaderResourceView);
    g_pD3D10App->ApplyTechnique(g_pD3D10App->m_pTechDrawTexture, &rcDrawDest, &rcDrawSrc, 1.0f);

    g_pD3D10App->m_pFxTexture10->SetResource(NULL);
    g_pD3D10App->m_pTechDrawTexture->GetPassByIndex( 0 )->Apply( 0 );
#endif

}

 void  GpuLayerTexture::oStretchBltMatrix(
            D3DXMATRIX* pMatrix, 
            RECTF* prcLocal,
            RECTF* prcTexture,
            float fAlpha)
 {
     if (!pMatrix)
         return;

	 HRESULT hr = 0;
	 hr = g_pD3D10App->m_pFxTexture10->SetResource(m_pShaderResourceView);
	 hr = g_pD3D10App->m_pFxMatrix->SetMatrix((float*)pMatrix);

     g_pD3D10App->ApplyTechnique(g_pD3D10App->m_pTechDrawTextureMatrix, 
		    prcLocal, prcTexture, fAlpha);

//      g_pD3D10App->m_pFxTexture10->SetResource(NULL);
//      g_pD3D10App->m_pTechDrawTexture->GetPassByIndex( 0 )->Apply( 0 );
}

#if 0
void  GpuLayerTexture::CalcTransform()
{
	if (!m_bNeedUpdateTransform)
		return;

	m_bNeedUpdateTransform = false;


    D3DXMATRIX  temp;
	D3DXMatrixIdentity(&m_transform);

	D3DXMatrixScaling(&m_transform, m_fScaleX, m_fScaleY, m_fScaleZ);

    // 单位弧度
	D3DXMatrixRotationYawPitchRoll(&temp, m_fRotationX, m_fRotationY, m_fRotationZ);
    m_transform *= temp;

	D3DXMatrixTranslation(&temp, m_fTranslationX, m_fTranslationY, m_fTranslationZ);
    m_transform *= temp;

    D3DXMATRIX  identify;
    D3DXMatrixIdentity(&identify);
    if (m_transform == identify)
    {
        m_bTransformValid = false;
    }
    else
    {
        m_bTransformValid = true;
    }
}


void  GpuLayerTexture::Translation(float xPos, float yPos, float zPos)
{
//     GpuLayerTexture* pRoot = m_pCompositor->GetRootLayerTexture();
//     if (!pRoot)
//         return;
// 
//     if (0 == pRoot->m_size.cx || 0 == pRoot->m_size.cy)
//         return;
// 
// 	   m_fTranslationZ = zPos;  // TODO:
// 
//     m_fTranslationX = xPos * 2.0f / pRoot->m_size.cx - 1.0f;
//     m_fTranslationY = 1.0f - yPos * 2.0f / pRoot->m_size.cy;

    m_fTranslationX = xPos;
    m_fTranslationY = yPos;
    m_fTranslationZ = zPos;
	m_bNeedUpdateTransform = true;

// 	TCHAR szText[32] = {0};
// 	_stprintf(szText, TEXT("%f\n"), yPos);
// 	::OutputDebugString(szText);
}
void  GpuLayerTexture::TranslationBy(float xPos, float yPos, float zPos)
{
	m_fTranslationX += xPos;
	m_fTranslationY += yPos;
	m_fTranslationZ += zPos;

	m_bNeedUpdateTransform = true;
}

void  GpuLayerTexture::Rotate(float xRotate, float yRotate, float zRotate)
{
    m_fRotationX = xRotate;
    m_fRotationY = yRotate;
    m_fRotationZ = zRotate;

    m_bNeedUpdateTransform = true;
}
void  GpuLayerTexture::RotateBy(float xRotate, float yRotate, float zRotate)
{
    m_fRotationX += xRotate;
    m_fRotationY += yRotate;
    m_fRotationZ += zRotate;

    m_bNeedUpdateTransform = true;
}

void  GpuLayerTexture::Scale(float xScale, float yScale, float zScale)
{
    m_fScaleX = xScale;
    m_fScaleY = yScale;
    m_fScaleZ = zScale;

    m_bNeedUpdateTransform = true;
}
void  GpuLayerTexture::ScaleBy(float xScale, float yScale, float zScale)
{
    m_fScaleX += xScale;
    m_fScaleY += yScale;
    m_fScaleZ += zScale;

    m_bNeedUpdateTransform = true;
}
#endif