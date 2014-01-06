#pragma once
#include "UISDK\Kernel\Inc\Interface\irenderlayer.h"
#include "UISDK\Kernel\Src\Atl\image.h"

namespace UI
{


void AlphaBitBlt(HDC hDest, HDC hSrc, int xDest, int yDest, int xSrc, int ySrc, int width, int height);

interface IWndTransMode;
interface IRenderBase;
class RenderLayer;
class WindowRenderLayer;
class Object;
class WindowBase;
class RenderContext;
class Layer3d;

// ǰ�᣺��ֻ��һ��layerʱ����ô���layerһ����һ��control layer!
class RenderChain
{
public:
	RenderChain(IRenderChain* pIRenderChain);
	~RenderChain();

public:
    static HRESULT  LoadRenderChain(IUIElement*, IUIApplication*, IObject* pObjParent, IObject**);
    HRESULT  OnNewChildElement(IUIElement* pChildElement);

public:
    IRenderLayer*  FindLayer(const TCHAR*  szLyerID);
    void  Init(WindowBase* pWindow);

public:
	int   GetLayerCount();
	bool  InsertLayer(RenderLayer*  pRenderLayer);
    void  RemoveLayer(RenderLayer*  pRenderLayer);

    WindowRenderLayer* GetWindowLayer() { return m_pWindowLayer; }
    RenderLayer*   GetFirstLayer() { return m_pFirstLayer; }
    RenderLayer*   GetLastLayer();

    IRenderChain*  GetIRenderChain() { return m_pIRenderChain; }
	HDC   GetMemoryDC();
    Image*  GetMemoryBuffer();

    void  OnWindowResize(int nType, int nWidth, int nHeight);
	void  OnWindowPaint(HDC hDC);
	void  UpdateObject(Object* pObj, bool bOnlyRedrawBkgnd, bool bUpdateNow);

    IRenderTarget*  BeginRedrawObjectPart(Object* pRedrawObj, RECT* prcArray, int nCount, bool* pbIsDrawing, RenderContext* pOutRenderContext);
    void  EndRedrawObjectPart(IRenderTarget* pRenderTarget, RECT* prcArray, int nCount);

    void  SetCanCommit(bool);
    bool  CanCommit();
    void  CombineAllLayer(HRGN hRgn);
	void  Commit(RECT* prc);

    bool  GetRequireAlphaChannel();
    void  OnWindowTransparentModeChanged(IWndTransMode* pMode);

    // 3d���
    Layer3d*  Get3DLayer();
    void  On3dObjectBegin();
    void  On3dObjectEnd();


protected:
	void  CreateDoubleBuffer(int nWidth, int nHeight);
	void  DestroyDoubleBuffer();
    
protected:
    IRenderChain*  m_pIRenderChain;
	RenderLayer*   m_pFirstLayer;
	WindowRenderLayer*  m_pWindowLayer;
    Layer3d*       m_p3DLayer;

    WindowBase*  m_pWindow;
	int  m_nLayerCount;

    // ���ڶ����ʱ������Ҫ������������϶����
	HDC      m_hMultiLayerMemDC;
    Image    m_MultiLayerBuffer;
	HBITMAP  m_hOldBitmap;

    long     m_lRefCanCommit;

    bool     m_bRequireAlphaChannel;
    bool     m_bFirstTimePaintWindow;  // <-- Ϊ�˽����WM_PAINT��Ϣ��֮ǰ��һЩ����߿ؼ�����ˢ�£����½���
                                       // ��ֻ��ʾ�˸�һ�������ݵ����⡣����ڵ�һ��WM_PAINT֮ǰ��renderchain
                                       // ����Ϊ�����ύ�����캯�������ã��ġ�
};

}