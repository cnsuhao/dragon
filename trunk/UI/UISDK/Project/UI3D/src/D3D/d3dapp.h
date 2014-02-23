#pragma once

namespace UI
{
class Stage3D;
class DxSprite;

class D3DApp
{
public:
	D3DApp();
	~D3DApp();

public:
	bool  Init();
	void  Clear();
	HRESULT  ResetDevice();
    void  OnDeviceLost();
	void  FireResetDeviceNotify();
    void  FireDeviceLostNotify();

    bool  RequestBackBuffer(DWORD nWidth, DWORD nHeight);
    void  ReleaseBackBuffer();
    void  CommitBackBuffer(IRenderTarget* pRenderTarget, int width, int height);

    void  InitDeviceSetting();

	void  AddStage(Stage3D* pStage);
	void  RemoveStage(Stage3D* pStage);

public:
	LPDIRECT3D9  m_pD3D;
	LPDIRECT3DDEVICE9  m_pDevice;
    DxSprite*  m_pSprite;

	D3DPRESENT_PARAMETERS  m_d3dpp;
private:
	
	HWND  m_hWnd;

    // RenderTarget£¬Ìæ»»BackBuffer
    IDirect3DTexture9*  m_pRenderTargetTexture;
    IDirect3DSurface9*  m_pTextureSurface;
    IDirect3DSurface9*  m_pOffScreenPlainSurface;
    IDirect3DSurface9*  m_pOldBackSurface;

	// stage list
	list<Stage3D*>  m_listStage3D;
};


}