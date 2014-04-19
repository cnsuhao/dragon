#pragma once

// To compile a Direct2D application, add d2d1.lib to the list of libraries. 
// You can find d2d1.h and d2d1.lib in Windows Software Development Kit (SDK)
// for Windows 7(http://msdn.microsoft.com/en-us/windows/bb980924.aspx).

// msdn url:
// http://msdn.microsoft.com/zh-cn/library/dd370994(v=vs.85).aspx
// http://technet.microsoft.com/zh-tw/library/dd370971.aspx
// http://msdn.microsoft.com/en-us/magazine/ee819134.aspx
// http://www.cnblogs.com/graphics/archive/2011/04/02/1964156.html
//
// Direct2D �� Direct3D �������Ը���
// http://msdn.microsoft.com/zh-cn/library/windows/apps/dd370966.aspx
//
namespace UI
{
class D2DApp
{
public:
	D2DApp();
	~D2DApp();

public:
	bool  Init();
	void  Release();

public:
	ID2D1Factory*        m_pD2DFactory;    // ò��������ͷ�������һֱռ�úܴ��ڴ�
	IDWriteFactory*      m_pDWriteFactory;
	IWICImagingFactory*  m_pWICFactory;

	ID2D1RenderTarget*   m_pD3DRenderTarget;   // ��D3D10���н�����rendertarget
    ID2D1RenderTarget*   m_pHWNDRenderTarget;  // ������Ⱦ
};

}