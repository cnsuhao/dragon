#pragma once
#include "UISDK\Kernel\Inc\Util\math\color.h"
#include "UISDK\Kernel\Inc\Util\math\rect.h"

//
// IA-Input Assembler State
// SO-Stream Output State
// OM-Output Merger State
// VS-Vertex Shader
// PS-Pixel Shader
// GS-Geometry Shader
//

// ��Դ���������ܱ�ֱ�Ӱ󶨵�һ�����߽׶Σ�����ֻ�ܰ�����Դ��������Դ��ͼ�󶨵���ͬ�Ĺ��߽׶Ρ�
// ���������ַ�ʽʹ������Direct3Dʼ��Ҫ�������ڳ�ʼ��ʱΪ��������ص���Դ��ͼ���������������
// ����Ч�ʣ�����SDK�ĵ���ָ����������������ʱ���������������������ͼ����ʱִ����Ӧ����֤��ӳ�䣬
// ���ٰ�ʱ�����ͼ�顱��
// Resource View����DirectX10ʱ������ĸ����ʵ����������resouce��pipeline�󶨵�֮���һ������������ͬһ����Դ���Զ�Ӧ�����ͬ��RV�����������󶨵���ͬ��pipline�׶Ρ�
// �����ֲ�ͬ���͵�RV���ֱ��Ӧ��pipeline�е�4���ɰ�λ�ã�
//   Render target view (ID3D11RenderTargetView)
//   Depth stencil view (ID3D11DepthStencilView)
//   Shader resource view (ID3D11ShaderResourceView)
//   Unordered access view (ID3D11UnorderedAccessView)

namespace UI
{

class Stage3D;
class Direct3DRenderTarget;

// ��Ӧfx�ļ��е�VS()����
struct DXUT_SCREEN_VERTEX_10
{
    float x, y, z;
    D3DCOLORVALUE color;
    float tu, tv;
};

class D3D10App
{
public:
	D3D10App();
	~D3D10App();

public:
	bool  Init();
    bool  InitRender();
	void  Release();
	HRESULT  ResetDevice();
    void  OnDeviceLost();
	void  FireResetDeviceNotify();
    void  FireDeviceLostNotify();

    bool  IsActiveSwapChain(HWND  hWnd);
    void  SetActiveSwapChain(HWND hWnd);

	void  ApplyTechnique(ID3D10EffectTechnique*  pTech, RECTF*  prcDraw, D3DCOLORVALUE color);
	void  ApplyTechnique(ID3D10EffectTechnique*  pTech, RECTF*  prcDraw, RECTF*  prcTexture, float fAlpha);
private:
	void  _ApplyTechnique(ID3D10EffectTechnique* pTech, DXUT_SCREEN_VERTEX_10 vertices[4]);

public:

private:

public:
	ID3D10Device*    m_pDevice;
	IDXGIFactory*    m_pDXGIFactory;
    ID3DX10Sprite*   m_pSprite10;

	// Effect
	ID3D10Effect*    m_pEffect;
	ID3D10EffectTechnique*   m_pTechDrawTexture;  // Technique: RenderUI
	ID3D10EffectTechnique*   m_pTechDrawRect;  // Technique: RenderUI without texture
	ID3D10EffectTechnique*   m_pTechFillRect; 
	ID3D10EffectTechnique*   m_pTechDrawTextureMatrix;

    ID3D10InputLayout*       m_pInputLayout;
    ID3D10Buffer*            m_pVertexBuffer;
    ID3D10EffectShaderResourceVariable*  m_pFxTexture10;
	ID3D10EffectMatrixVariable*  m_pFxMatrix;
    ID3D10EffectMatrixVariable*  m_pFxOrthMatrix;
	ID3D10RasterizerState*   m_pRasterizerState;
	
    HWND   m_hActiveWnd;  //  ��ǰdevice�б�ѡ���rendertarget��������	
private:
};


}
