#pragma once

//
// IA-Input Assembler State
// SO-Stream Output State
// OM-Output Merger State
// VS-Vertex Shader
// PS-Pixel Shader
// GS-Geometry Shader
//
namespace UI
{

class Stage3D;

class D3D10App
{
public:
	D3D10App();
	~D3D10App();

public:
	bool  Init();
	void  Release();
	HRESULT  ResetDevice();
    void  OnDeviceLost();
	void  FireResetDeviceNotify();
    void  FireDeviceLostNotify();

    bool  RequestBackBuffer(DWORD nWidth, DWORD nHeight);
    void  ReleaseBackBuffer();
    void  CommitBackBuffer(IRenderTarget* pRenderTarget, int width, int height);

    void  AddStage(Stage3D* pStage);
    void  RemoveStage(Stage3D* pStage);

public:

private:

public:
	ID3D10Device*    m_pDevice;
    IDXGISwapChain*  m_pSwapChain;
    ID3D10RenderTargetView*  m_pRenderTargetView;


	// Effect
	ID3D10Effect*  m_pEffect;
	ID3D10EffectTechnique*  m_pET_FillRectWorld;  // 不需要释放
	ID3D10InputLayout*  m_pIL_FillRectScreen;

private:
	HWND  m_hWnd;

    // stage list
    list<Stage3D*>  m_listStage3D;
};


}
