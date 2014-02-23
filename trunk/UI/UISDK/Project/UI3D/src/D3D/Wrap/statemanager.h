#pragma once

namespace UI
{

class StateManager
{
public:
    StateManager(IDirect3DDevice9*  pDev);
    ~StateManager();

    void  SetRenderState(D3DRENDERSTATETYPE State, DWORD Value);
    void  SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value);
    void  SetFVF(DWORD FVF);
    void  SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture);

private:
    void  Restore();

private:
    IDirect3DDevice9*  m_pDev;

    map<D3DRENDERSTATETYPE, DWORD>   m_mapRenderState;
    map<D3DTEXTURESTAGESTATETYPE, DWORD>   m_mapTextureStageState[8];
    map<DWORD ,IDirect3DBaseTexture9*>   m_mapTexture;
    DWORD  m_dwFVF;
    
    
    short  m_bSetFVF : 1;
};

}