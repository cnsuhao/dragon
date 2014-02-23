#pragma once

// ps: 貌似sprite画出来的东西永远在3D内容的最上面
//     因此有些情况可以考虑使用RHW来绘制
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