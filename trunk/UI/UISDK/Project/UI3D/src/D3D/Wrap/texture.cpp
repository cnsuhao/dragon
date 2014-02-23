#include "stdafx.h"
#include "texture.h"
#include "UISDK\Project\UI3D\src\D3D\Wrap\statemanager.h"

namespace UI
{

DxTexture::DxTexture()
{
    m_pTexture = NULL;
	memset(&m_imageInfo, 0, sizeof(m_imageInfo));
}

DxTexture::~DxTexture()
{
    Release();
}

void  DxTexture::Release()
{
	if (m_pTexture)
	{
		m_pTexture->Release();
		m_pTexture = NULL;
	}
	memset(&m_imageInfo, 0, sizeof(m_imageInfo));
}

IDirect3DTexture9*  DxTexture::GetPtr()
{
	return m_pTexture;
}

bool  DxTexture::Create()
{
    return false;
}

bool  DxTexture::LoadFile(const TCHAR* szFile, IDirect3DDevice9* pDevice)
{
    if (!szFile || !pDevice)
        return false;

    if (m_pTexture)
	{
		Release();
	}

	HRESULT hr = D3DXCreateTextureFromFileEx(
        pDevice,
        szFile,
        D3DX_DEFAULT, 
        D3DX_DEFAULT,
        D3DX_DEFAULT,
        0,
        D3DFMT_UNKNOWN,
        D3DPOOL_MANAGED,
        /*D3DX_FILTER_NONE*/D3DX_DEFAULT,
        D3DX_DEFAULT,
        D3DCOLOR_COLORVALUE(0.0f,0.0f,0.0f,1.0f),
        &m_imageInfo,
        NULL,
        &m_pTexture);

	if (FAILED(hr))
		return false;

    return true;
}

int  DxTexture::GetWidth()
{
	return m_imageInfo.Width;
}
int  DxTexture::GetHeight()
{
	return m_imageInfo.Height;
}

// bTransparency
//   表示纹理的整体透明度 255表示不透明。用于实现 fade in/out
void  DxTexture::DrawRect(IDirect3DDevice9* pDevice, float fLeft, float fTop, float fRight, float fBottom, float fz, bool bDirect, byte bTransparency)
{
	if (!pDevice || !m_pTexture)
		return;

	if (0 == bTransparency)
		return;

	struct VERTEX_TEXTURE
	{
		float x, y, z;
		float u, v;
	};
	struct VERTEX_TEXTURE_RHW
	{
		float x, y, z, rhw;
		float u, v;
	};
	const UINT D3DFVF_VERTEX_TEXTURE = (D3DFVF_XYZ|D3DFVF_TEX1);
	const UINT D3DFVF_VERTEX_TEXTURE_RHW = (D3DFVF_XYZRHW|D3DFVF_TEX1);
	
	StateManager  s(pDevice);

	// Alpha Test
	//		pDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	// 		pDev->SetRenderState(D3DRS_ALPHAREF, 0x00000000);
	// 		pDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	// Alpha
	if (m_imageInfo.ImageFileFormat == D3DXIFF_PNG)
	{
		s.SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );  
		s.SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );  
		s.SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );  
	}

	if (bTransparency != 255)
	{
		DWORD AlphaValue;
		AlphaValue = D3DCOLOR_ARGB(bTransparency,255,255,255);

		s.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		s.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		s.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		s.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		s.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

		s.SetTextureStageState(0, D3DTSS_CONSTANT, AlphaValue);
		s.SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CONSTANT);
		s.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		s.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	}
	s.SetTexture(0, m_pTexture);


	if (bDirect)
	{
		VERTEX_TEXTURE_RHW vertices[4] =
		{
			fLeft,   fTop,     fz,  1.0f, 0.0f, 0.0f,
			fRight,  fTop,     fz,  1.0f, 1.0f, 0.0f,
			fLeft,   fBottom,  fz,  1.0f, 0.0f, 1.0f,
			fRight,  fBottom,  fz,  1.0f, 1.0f, 1.0f,
		};

		s.SetFVF(D3DFVF_VERTEX_TEXTURE_RHW);
		pDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vertices, sizeof( VERTEX_TEXTURE_RHW ) );
	}
	else
	{
		VERTEX_TEXTURE vertices[4] =
		{
			fLeft,   fTop,     fz,  0.0f, 0.0f,
			fRight,  fTop,     fz,  1.0f, 0.0f,
			fLeft,   fBottom,  fz,  0.0f, 1.0f,
			fRight,  fBottom,  fz,  1.0f, 1.0f,
		};

		s.SetFVF(D3DFVF_VERTEX_TEXTURE);
		pDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vertices, sizeof( VERTEX_TEXTURE ) );
	}
}


}