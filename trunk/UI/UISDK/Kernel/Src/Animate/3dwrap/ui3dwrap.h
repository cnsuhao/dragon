#pragma once
#include "UISDK\Kernel\Src\Base\Message\message.h"
#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\3dpipeline.h"
#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\texture_mapping_base.h"
#include "UISDK\Kernel\Src\Atl\image.h"
#include "UISDK\Kernel\Inc\Util\rendercontext.h"
#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\quad_perspective_texture_mapping.h"
#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\affine_texture_mapping.h"
#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\perspective_texture_mapping.h"

// 资料：
// 3d中的仿射、透视纹理映射相关
// http://blog.csdn.net/popy007/article/details/5556639
// http://blog.csdn.net/popy007/article/details/5570803
//
//  1. 由于变换过程中，新的变换图形的大小不确定，无法直接使用一个固定大小buffer进行装载。
//     解决方案：写一个3DLayer，专门为3D渲染提供一个缓存
//
//  2. 鼠标检测：由于将对象挪到了3DLayer下面，无法再使用鼠标检测了。以后再想办法看看如何解决
//  
namespace UI
{
class Object;
class IRenderTarget;
class Layer3d;

class Object3DWrap : public Message
{
public:
    Object3DWrap(Object* pObj);
    ~Object3DWrap();

    UI_BEGIN_MSG_MAP
//         UIMSG_WM_HITTEST(OnHitTest)
//         UIMSG_WM_MOUSEMOVE(OnMouseMove)
    UI_END_MSG_MAP

    IObject3DWrap*  GetIObject3DWrap();

public:
    void  Begin();
    void  End();
    void  EndByDestroy();
    bool  IsRunning();

    void  SetOriginPoint(POINT pt);
    void  Rotate(int x, int y, int z);
    void  Move(int x, int y, int z);

    void  OnDrawObject(IRenderTarget* pRenderTarget, RenderContext roc);

protected:
    UINT  OnHitTest( POINT* pt );
    void  OnMouseMove(UINT nFlags, POINT point);

protected:
    IObject3DWrap*  m_pIObject3DWrap;
    Object*  m_pObject;
    Object*  m_pOldParent; // 记录begin之前的父对象，在End时重新设置回去
    IMessage*  m_pOldParentMsg;  // 父对象销毁检测

    Layer3d*  m_p3dLayer;


    bool  m_bBegin;
    bool  m_bParamChanged;

    _3dPipeline  m_3dpipeline;
    TextureMappingBase*  m_pTexturemapping;
    //QuadPerspectiveTextureMapping*  m_pTexturemapping;
};

}