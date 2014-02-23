#pragma once

// ps: ò��sprite�������Ķ�����Զ��3D���ݵ�������
//     �����Щ������Կ���ʹ��RHW������
namespace UI
{
class DxTexture;

class DxSprite
{
public:
    DxSprite();
    ~DxSprite();

    bool  Create(IDirect3DDevice9*  pDevice);
    void  Destroy();
    void  Reset();
    void  Lost();
    ID3DXSprite*  GetPtr();

    void  Begin(DWORD dwFlag);
    void  End();

	bool  BitBltFull(DxTexture* pTexture, POINT* pPt);
    bool  StretchFull(DxTexture* pTexture, RECT* prcDest);

public:
    ID3DXSprite*  m_pSprite;
};

}