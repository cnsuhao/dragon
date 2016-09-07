#pragma once
#include "UISDK\Kernel\Inc\Interface\irenderlayer.h"
#include "UISDK\Project\UICompositor\Inc\ihardwarecompositor.h"

//
// 2014.3.7 用于实现支持控件级别的分层设计。
// 原有的render layer只支持窗口级别的，而且只支持控件层的鼠标点击
// 
// Q1: 子renderlayer，是直接提交到窗口上，还是提交到上一层？
// A1: 提交到上一层，用于做到设置一个层的opacity
//
// Q1: RenderLayer什么时候需要被重绘？
// A1: 1. 初始化时
//     2. Resize
//     3. 采用自顶向下的绘制方式时(即调用了UpdateLayer。例如按钮状态发生改变，但刷新窗口，这时按钮所在层也需要更新。
//

// 2014.11.22
// 将子renderlayer画到父renderlayer上解决不了延迟刷新的问题。
// 例如：Object::SetEnable  GetRenderLayer2()->SetDirty(true); 
// 在下一次update这个控件时，将控件所在层刷完后，再向上一层刷新时，
// 上一层发现这层是dirty的，会触发这一层的全量刷新。
//
// 如果不将dirty状态递归传递到父renderlayer，也存在一个问题，父对象不刷新
// 本renderlayer也更不会被刷新
//
//  
// 
namespace UI
{
class  Object;
class  WindowRender;
class  WindowBase;
class  RenderLayer2Soft3D;

// RENDER_LAYER2_STYLE

// - 2014.11.22废弃。使用一个无效区域来标识。
// 原因：1. 禁用一个控件，但不刷新，此时需要将层设置为dirty。但下一次窗口刷新遍历到这个layer时，发现这个render
//       是dirty的，便为将整个layer重新刷新。但其实只需要刷新原控件所在区域即可。
//
//       2. 修改某个控件的属性，但不立即刷新，该控件所在的层还是干净的，下一次刷新窗口时，该层的数据不会去更新
//
//       因此仅用一个bool类型来标识layer dirty是不够的，会出现很多问题，所以废弃掉
//
// 该层的内容是否需要重新渲染 
// #define  RENDER_LAYER2_DIRTY    0x02  


// 该标志有两个时机被置上：1. 创建时  2. Resize后，层缓存被重置
// 表示层数据需要全量刷新
#define RENDERLAYER2_FLAG_NEED_UPDATE  0x02

// 禁止本层的updatelayer操作。由于调用UpdateObject会刷新自己，在作特效的时候自己并不需要刷新，则可将加上该标志
#define  RENDERLAYER2_DISABLE_UPDATELAYER  0x04  

// 表示这个层不保留缓存数据，每次刷新时都重新绘制，只用于支持层的一些特性，如支持alpha渐变
// 例如hover visible的scrollbar
#define RENDERLAYER_FLAG_ALWRAYS_UPDATE  0x0008

#if 0
interface IRenderLayerTransform
{
    virtual  ~IRenderLayerTransform() = 0 {};
    virtual  TRANSFORM_TYPE  GetTransformType() = 0;
	// 将分层数据提交到parent layer
    virtual  bool  Commit(IRenderTarget*  pRenderTarget, Render2TargetParam* pParam) = 0;
    virtual  void  GetBufferPadding(RECT* prc) = 0;
    virtual  void  OnSize(unsigned int nWidth, unsigned int nHeight) = 0;
    virtual  void  Rotate(float x, float y, float z) = 0;
    virtual  void  Move(int x, int y, int z) = 0;
	// 将位于变换图形中的坐标映射到未变换前的坐标，用于点击测试
    virtual  void  NormalPoint(POINT* pptLayer, __out POINT* pptTransform) = 0;
	// 用于将脏区域进行变换，计算出新的无效区域提交到窗口上
    virtual  void  GetRelativeBoundRect(RECT* prcInLayer, RECT* prcBound) = 0;
    virtual  void  GetInvalidRect(RECT* prc) = 0;
	// 鼠标点击测试，内部将调用NormalPoint
    virtual  bool  HitTest(POINT*  pptParent, POINT*  ptInChild) = 0;
};
#endif

class RenderLayer
{
public:
    RenderLayer(Object* pObj);
    virtual ~RenderLayer();
	
	IRenderLayer*  GetIRenderLayer2();

public:
//     bool   IsDirty();
//     void   SetDirty(bool b);
	bool  NeedUpdate();
	void  SetNeedUpdate(bool b);
    void  SetAlwaysUpdate(bool b);

	void  InvalidateFull();
	void  InvalidateRect(RECT* prc);
	void  ClearInvalidateRegions();
	void  Refresh();

    void  EnableUpdateLayer(bool b);
    bool  CanUpdateLayer();
    bool  IsTransparent();
    byte  GetOpacity();
	void  SetOpacity(byte b);

    Object*  GetCreateObject();
    IRenderTarget*  GetRenderTarget();
	RenderLayer*  GetParentRenderLayer();
    IGpuLayerTexture*  GetGpuTexture();

	void  CreateTransform(TRANSFORM_TYPE eType);
	bool  HasTransform();
	IRenderLayerTransform2D*  GetTransform2D(bool bForceCreate=false);
	IRenderLayerTransform3D*  GetTransform3D(bool bForceCreate=false);
	IRenderLayerTransform2*   GetTransformPtr();
	
    void  OnSize(uint nWidth, uint nHeight);
    void  UpdateLayer(LPCRECT  rcDamage);
    void  UpdateLayer(LPCRECT  rcDamageArray, int nCount);

    bool  UpdateDescendantObject(Object*  pObjDamage, LPCRECT prcObjDamageArray, int nCount, RECT* prcOutDamageInLayerArray);

    void  DirectCommit(bool bUpdate);
	void  Commit(IRenderTarget*  pRenderTarget);

    void  GetRectDrawInBuffer(RECT* prc);
    void  GetOffsetDrawInBuffer(POINT* ppt);

    void  ClearChildren();
    void  AddChild(RenderLayer* pChild);
    void  Compositor2Gpu(GpuLayerCommitContext context);
	void  OnHardCompositeChanged(bool bEnable);
	bool  IsHardComposite();

protected:
	IRenderLayer*  m_pIRenderLayer2;
    long  m_lFlags;  
	byte  m_nOpcity;

    Object*         m_pObjCreate;    // 哪个对象创建了这个layer
    WindowRender*   m_pWindowRender;
    IRenderTarget*  m_pRenderTarget;


    // 为了实现3D光栅化线性插值时，需要一个哨兵边界，
    // 层的数据可能不是直接位于缓存的0，0处
    CRect   m_rcDrawInBuffer;  // 与RenderContext的m_ptBufferOffset对应

	IRenderLayerTransform2*  m_pTransform2;

public:
    ROTATE_CENTER_TYPE      m_eRotateCenterType;
    POINT                   m_ptRotateCenterOffset;  // anchorPoint

	// 硬件合成
    IGpuLayerTexture*  m_pGpuTexture;

    typedef vector<RenderLayer*>::iterator _ChildIter;
    vector<RenderLayer*>  m_vecChild;
};

}
