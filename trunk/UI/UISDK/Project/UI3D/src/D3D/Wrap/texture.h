#pragma once

// ע��Sprite����ͼƬ�Ĵ�С������2��N�η������򻭳����ͻ��ģ����(Ҫô�ͱ����ڼ���ʱָ��D3DX_FILTER_NONE)
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