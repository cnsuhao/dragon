#include "stdafx.h"
#include "mesh.h"
#include "texture.h"

namespace UI
{
DxMesh::DxMesh( IDirect3DDevice9* pDev )
{
    m_pDevice = pDev;
    m_pMesh = NULL;
    m_dwMaterialNum = 0;
    m_ppMaterial = NULL;
    m_ppTexture = NULL;
}

DxMesh::~DxMesh()
{
    Destroy();
}
void  DxMesh::Destroy()
{
    if (m_ppTexture)
    {
        delete[] m_ppTexture;
    }
    if (m_ppMaterial)
    {
        delete[] m_ppMaterial;
    }
    if (m_pMesh)
    {
        m_pMesh->Release();
        m_pMesh = NULL;
    }
    m_dwMaterialNum = 0;
}

bool DxMesh::LoadFromXFile( const TCHAR* szText )
{
    if (!m_pDevice)
        return false;
    if (!szText)
        return false;

    ID3DXBuffer*  pBuffer;
    HRESULT hr = D3DXLoadMeshFromX(szText, D3DXMESH_SYSTEMMEM, m_pDevice, NULL, &pBuffer, NULL, &m_dwMaterialNum, &m_pMesh);
    if (FAILED(hr) || !m_pMesh || !pBuffer)
        return false;

    if (m_dwMaterialNum > 0)
    {
        TCHAR szDir[MAX_PATH] = {0};
        _tcscpy(szDir, szText);
        TCHAR* p = _tcsrchr(szDir, '\\');
        if(p != NULL)
            *(p+1) = 0;

        D3DXMATERIAL*  pMaterial = (D3DXMATERIAL*)pBuffer->GetBufferPointer();

        m_ppMaterial = new D3DMATERIAL9[m_dwMaterialNum];
        m_ppTexture = new DxTexture[m_dwMaterialNum];

        for (unsigned int i = 0; i < m_dwMaterialNum; i++)
        {
            m_ppMaterial[i] = pMaterial[i].MatD3D;
            // Set the ambient color for the material (D3DX does not do this)
            m_ppMaterial[i].Ambient = m_ppMaterial[i].Diffuse;

            if (pMaterial[i].pTextureFilename && strlen(pMaterial[i].pTextureFilename) > 0)
            {
                TCHAR szTexturePath[MAX_PATH];
                _tcscpy(szTexturePath, szDir);
                _tcscat(szTexturePath, CA2T(pMaterial[i].pTextureFilename));
                m_ppTexture[i].LoadFile(szTexturePath, m_pDevice);
            }
        }
    }
    pBuffer->Release();
    return true;
}

void DxMesh::Render()
{
    if (!m_pMesh || !m_pDevice)
        return;

    for (unsigned int i = 0; i < m_dwMaterialNum; i++)
    {
        m_pDevice->SetMaterial(&m_ppMaterial[i]);
        m_pDevice->SetTexture(0, m_ppTexture[i].GetPtr());
        m_pMesh->DrawSubset(i);
    }
}

    
}