#include "stdafx.h"
#include "sprite.h"
#include "UISDK/Project/UI3D/src/D3D/Wrap/texture.h"

namespace UI
{

DxSprite::DxSprite()
{
    m_pSprite = NULL;
}

DxSprite::~DxSprite()
{
    Destroy();
}

bool  DxSprite::Create(IDirect3DDevice9*  pDevice)
{
    if (!pDevice)
        return false;

    if (m_pSprite)
        return false;

    D3DXCreateSprite(pDevice, &m_pSprite);
    if (!m_pSprite)
        return false;

    D3DXMATRIX identity;
    D3DXMatrixIdentity( &identity );
    m_pSprite->SetTransform(&identity);

    return true;
}

void  DxSprite::Destroy()
{
    if (m_pSprite)
    {
        m_pSprite->Release();
        m_pSprite = NULL;
    }
}

void  DxSprite::Lost()
{
    if (m_pSprite)
    {
        m_pSprite->OnLostDevice();
    }
}
void  DxSprite::Reset()
{
    if (m_pSprite)
    {
        m_pSprite->OnResetDevice();
    }
}

ID3DXSprite*  DxSprite::GetPtr()
{
    return m_pSprite;
}

void  DxSprite::Begin(DWORD dwFlag)
{
    if (!m_pSprite)
        return;

    m_pSprite->Begin(dwFlag);
}
void  DxSprite::End()
{
    if (!m_pSprite)
        return;

    m_pSprite->End();
}

bool  DxSprite::BitBltFull(DxTexture* pTexture, POINT* pPt)
{
	if (!pTexture || !pPt)
		return false;

	if (!m_pSprite)
		return false;

	D3DXVECTOR3  vecPos((float)pPt->x, (float)pPt->y, 0.0f);
	HRESULT hr = m_pSprite->Draw(pTexture->GetPtr(), NULL, NULL, &vecPos, 0xffffffff);
	if (FAILED(hr))
        return false;

    return true;
}
bool  DxSprite::StretchFull(DxTexture* pTexture, RECT* prcDest)
{
    if (!pTexture || !prcDest)
        return false;
    if (!m_pSprite)
        return false;

    D3DXMATRIX  oldMatrix;
    m_pSprite->GetTransform(&oldMatrix);
    
    if (0 == pTexture->GetWidth() || 0 == pTexture->GetHeight())
        return false;

    D3DXMATRIX matrix;
    float xScale, yScale;
    xScale = (float)(prcDest->right - prcDest->left)/(float)(pTexture->GetWidth());
    yScale = (float)(prcDest->bottom - prcDest->top)/(float)(pTexture->GetHeight());
    ::D3DXMatrixScaling(&matrix, xScale, yScale, 0.0f);
    m_pSprite->SetTransform(&matrix);

    D3DXVECTOR3  vecPos((float)prcDest->left, (float)prcDest->top, 0.0f);
    m_pSprite->Draw(pTexture->GetPtr(), NULL, NULL, &vecPos, 0xFFFFFFFF);

    m_pSprite->SetTransform(&oldMatrix);
    return false;
}

}