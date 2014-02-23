#pragma once

// 注：Sprite纹理图片的大小必须是2的N次方，否则画出来就会很模糊。(要么就必须在加载时指定D3DX_FILTER_NONE)
namespace UI
{

class DxTexture
{
public:
    DxTexture();
    ~DxTexture();

	IDirect3DTexture9*  GetPtr();

    bool  Create();
    bool  LoadFile(const TCHAR* szFile, IDirect3DDevice9* pDevice);
	void  Release();

    int  GetWidth();
    int  GetHeight();

	void  DrawRect(IDirect3DDevice9* pDevice, float fLeft, float fTop, float fRight, float fBottom, float fz, bool bDirect, byte bTransparency);

public:
    IDirect3DTexture9*  m_pTexture;
    D3DXIMAGE_INFO  m_imageInfo;
};

}