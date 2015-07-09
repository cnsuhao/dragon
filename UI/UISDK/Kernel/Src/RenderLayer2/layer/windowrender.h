#pragma once
//#include "UISDK\Project\UI3D\inc\inc.h"
#include "UISDK\Project\UICompositor\inc\ihardwarecompositor.h"
#pragma comment(lib, "UICompositor.lib")

//
// 2014.3.10
//
// ͳһGraphics Render Library��ʹ�ã�һ�����ڲ��ٻ��ʹ�ã�ֻ��ʹ��һ��Graphics Render Library
// ���򴦴�Ҫ���Ǵ���ʲô���͵���Ⱦ�⣬��Ĵ�����ң��߼�Ҳ��
//
namespace UI
{
class Object;
class WindowBase;
class RenderLayer;
interface IWindowTransparent;
interface IWindowRender;

class WindowRender
{
public:
    WindowRender(WindowBase*  p);
    ~WindowRender();

    IWindowRender*  GetIWindowRender();

public:
    RenderLayer*  CreateRenderLayer(Object* pObj);
	IGpuLayerTexture*  CreateGpuLayerTexture(RenderLayer* p);
    void  OnRenderLayerDestroy(RenderLayer* p);

	void  OnSerialize(SERIALIZEDATA* pData);
    bool  CreateRenderTarget(IRenderTarget** pp);
	void  UpdateWindow(HDC hDC, RECT* prcDamage);
    void  OnWindowSize(UINT nWidth, UINT nHeight);
    void  OnWindowTransparentModeChanged(IWindowTransparent* pMode);
	void  SetGraphicsRenderType(GRAPHICS_RENDER_LIBRARY_TYPE  eTpye);
    GRAPHICS_RENDER_LIBRARY_TYPE  GetGraphicsRenderType();
    bool  GetRequireAlphaChannel();
    void  SetCanCommit(bool b);
    bool  CanCommit() ;
    void  Commit(HDC hDC, RECT* prc, int nCount=1);
	void  OnHardCompositeChanged(bool bEnable);    
    void  HardComposite();

protected:
    void  RebuildCompositingLayerTree();
    void  rebuildCompositingLayerTree(
        Object* pParent, 
        RenderLayer* pParentLayer);

public:
    IWindowRender*  m_pIWindowRender;
    GRAPHICS_RENDER_LIBRARY_TYPE  m_eGRL;
    WindowBase*    m_pWindow;

#pragma region // Ӳ���ϳ����

    // ���ڽ�����
    IHardwareComposition*  m_pHardwareComposition;
    // ������objtree��Ӧ��layertree
    bool     m_bNeedRebuildGpuLayerTree; 

#pragma endregion

    long  m_lRefCanCommit;
    bool  m_bNeedAlphaChannel;       // �ô��ڵ���Ⱦ�Ƿ���Ҫalphaͨ�������ٸ���graphics render lib type�����������û��Լ�����
    bool  m_bFirstTimePaintWindow;   // <-- Ϊ�˽����WM_PAINT��Ϣ��֮ǰ��һЩ����߿ؼ�����ˢ�£����½���
                                     // ��ֻ��ʾ�˸�һ�������ݵ����⡣����ڵ�һ��WM_PAINT֮ǰ������
                                     // ����Ϊ�����ύ�����캯�������ã��ġ�
};

}