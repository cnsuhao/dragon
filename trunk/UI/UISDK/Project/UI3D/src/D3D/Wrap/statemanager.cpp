#include "stdafx.h"
#include "statemanager.h"

namespace UI
{

StateManager::StateManager( IDirect3DDevice9* pDev )
{
    m_pDev = pDev;
    m_dwFVF = 0;
}

StateManager::~StateManager()
{
    Restore();
}

void  StateManager::Restore()
{
    //////////////////////////////////////////////////////////////////////////
    if (m_mapRenderState.size())
    {
        map<D3DRENDERSTATETYPE, DWORD>::iterator iter = m_mapRenderState.begin();
        for ( ;iter != m_mapRenderState.end(); iter++)
        {
            m_pDev->SetRenderState(iter->first, iter->second);
        }
        m_mapRenderState.clear();
    }

    //////////////////////////////////////////////////////////////////////////
    for (int i = 0; i < 8; i++)
    {
        map<D3DTEXTURESTAGESTATETYPE, DWORD>& m = m_mapTextureStageState[i];

        if (m.size())
        {
            map<D3DTEXTURESTAGESTATETYPE, DWORD>::iterator iter = m.begin();
            for ( ;iter != m.end(); iter++)
            {
                m_pDev->SetTextureStageState(i, iter->first, iter->second);
            }
            m.clear();
        }
        
    }

    //////////////////////////////////////////////////////////////////////////
    if (m_mapTexture.size())
    {
        map<DWORD ,IDirect3DBaseTexture9*>::iterator iter = m_mapTexture.begin();
        for ( ;iter != m_mapTexture.end(); iter++)
        {
            m_pDev->SetTexture(iter->first, iter->second);
            SAFE_RELEASE(iter->second);
        }
        m_mapTexture.clear();
    }


    //////////////////////////////////////////////////////////////////////////
    if (m_bSetFVF)
    {
        m_pDev->SetFVF(m_dwFVF);
        m_bSetFVF = false;
    }
}


void  StateManager::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
    DWORD dwOldValue = 0;
    m_pDev->GetRenderState(State, &dwOldValue);

    if (dwOldValue == Value)
        return;

    m_pDev->SetRenderState(State, Value);
    
    if (m_mapRenderState.find(State) == m_mapRenderState.end())
    {
        m_mapRenderState[State] = dwOldValue;
    }
}

void  StateManager::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
    if (Stage >= 8)
        return;

    DWORD dwOldValue = 0;
    m_pDev->GetTextureStageState(Stage, Type, &dwOldValue);

    if (dwOldValue == Value)
        return;

    m_pDev->SetTextureStageState(Stage, Type, Value);

    if (m_mapTextureStageState[Stage].find(Type) == m_mapTextureStageState[Stage].end())
    {
        (m_mapTextureStageState[Stage])[Type] = dwOldValue;
    }
}

void  StateManager::SetFVF(DWORD FVF)
{
    DWORD dwOldValue = 0;
    m_pDev->GetFVF(&dwOldValue);

    if (dwOldValue == FVF)
        return;

    m_pDev->SetFVF(FVF);

    if (!m_bSetFVF)
    {
        m_bSetFVF = true;
        m_dwFVF = dwOldValue;
    }
}

void  StateManager::SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture)
{
    IDirect3DBaseTexture9* pOldTexture = NULL;
    m_pDev->GetTexture(Stage, &pOldTexture);

    if (pOldTexture == pTexture)
    {
        SAFE_RELEASE(pOldTexture);
        return;
    }

    m_pDev->SetTexture(Stage, pTexture);
    if (m_mapTexture.find(Stage) == m_mapTexture.end())
    {
        m_mapTexture[Stage] = pOldTexture;
    }
    else
    {
        SAFE_RELEASE(pOldTexture);
    }
}


}