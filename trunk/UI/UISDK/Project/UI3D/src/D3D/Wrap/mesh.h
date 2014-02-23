#pragma once

namespace UI
{
class DxTexture;

class DxMesh
{
public:
    DxMesh(IDirect3DDevice9*  pDev);
    ~DxMesh();

    bool  LoadFromXFile(const TCHAR* szText);
    void  Render();
    void  Destroy();
    bool  IsLoaded() { return m_pMesh?true:false; }

private:
    ID3DXMesh*  m_pMesh;
    DWORD  m_dwMaterialNum;

    D3DMATERIAL9*  m_ppMaterial;
    DxTexture*  m_ppTexture;

    IDirect3DDevice9*  m_pDevice;
};

}