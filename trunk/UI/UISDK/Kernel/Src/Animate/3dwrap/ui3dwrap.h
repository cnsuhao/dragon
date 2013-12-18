#pragma once
#include "UISDK\Kernel\Src\Base\Message\message.h"
#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\3dpipeline.h"
#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\texture_mapping_base.h"
#include "UISDK\Kernel\Src\Atl\image.h"
#include "UISDK\Kernel\Inc\Util\rendercontext.h"
#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\quad_perspective_texture_mapping.h"
#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\affine_texture_mapping.h"
#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\perspective_texture_mapping.h"

// ���ϣ�
// 3d�еķ��䡢͸������ӳ�����
// http://blog.csdn.net/popy007/article/details/5556639
// http://blog.csdn.net/popy007/article/details/5570803
//
//  1. ���ڱ任�����У��µı任ͼ�εĴ�С��ȷ�����޷�ֱ��ʹ��һ���̶���Сbuffer����װ�ء�
//     ���������дһ��3DLayer��ר��Ϊ3D��Ⱦ�ṩһ������
//
//  2. ����⣺���ڽ�����Ų����3DLayer���棬�޷���ʹ��������ˡ��Ժ�����취������ν��
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
    Object*  m_pOldParent; // ��¼begin֮ǰ�ĸ�������Endʱ�������û�ȥ
    IMessage*  m_pOldParentMsg;  // ���������ټ��

    Layer3d*  m_p3dLayer;


    bool  m_bBegin;
    bool  m_bParamChanged;

    _3dPipeline  m_3dpipeline;
    TextureMappingBase*  m_pTexturemapping;
    //QuadPerspectiveTextureMapping*  m_pTexturemapping;
};

}