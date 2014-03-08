#include "stdafx.h"
#include "texture.h"

namespace UI
{
	Texture::Texture()
	{
		m_pResourceView = NULL;
		memset(&m_SrcInfo, 0, sizeof(m_SrcInfo));
	}
	Texture::~Texture()
	{
		Destroy();
	}

	void  Texture::Destroy()
	{
		memset(&m_SrcInfo, 0, sizeof(m_SrcInfo));
		SAFE_RELEASE(m_pResourceView);
	}

	bool  Texture::LoadFromFile(const TCHAR*  szPath)
	{
		Destroy();

		if (!szPath)
			return false;

		D3DX10_IMAGE_LOAD_INFO ZeroInfo;	//D3DX10_IMAGE_LOAD_INFO has a default constructor
		D3DX10_IMAGE_LOAD_INFO* pLoadInfo = NULL;

		D3DX10GetImageInfoFromFile( szPath, NULL, &m_SrcInfo, NULL );

		pLoadInfo = &ZeroInfo;

		ID3D10Texture2D* pRes = NULL;
		HRESULT hr = D3DX10CreateTextureFromFile( GetDevice(), szPath, pLoadInfo, NULL, ( ID3D10Resource** )&pRes, NULL );
		if( FAILED( hr ) )
			return false;

		D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		SRVDesc.Format = ZeroInfo.Format;
		SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MostDetailedMip = 0;
		SRVDesc.Texture2D.MipLevels = pLoadInfo->pSrcInfo->MipLevels;


		hr = GetDevice()->CreateShaderResourceView( pRes, &SRVDesc, &m_pResourceView);
		pRes->Release();
		if( FAILED( hr ) )
			return false;

		return true;
	}

    bool  Texture::DrawInWorld(RECTF rcWorld)
    {
        if (!m_pResourceView)
            return false;

        m_rect.CopyFromm(&rcWorld);


        return true;
    }
    bool  Texture::DrawInScreen(RECT rcScreen)
    {
        if (!m_pResourceView)
            return false;

        return true;
    }
}