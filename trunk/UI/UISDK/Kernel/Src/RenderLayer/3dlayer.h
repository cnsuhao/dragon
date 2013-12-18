#pragma once
#include "bufferrenderlayer.h"

namespace UI
{
class Object3DWrap;

class Layer3d : public ControlRenderLayer
{
public:
    Layer3d();
    ~Layer3d();

    UI_DECLARE_OBJECT(Layer3d, OBJ_LAYER|LAYER_3D);

    UI_BEGIN_MSG_MAP
        UIMSG_WM_SIZE(OnSize)
    UI_END_MSG_MAP_CHAIN_PARENT(ControlRenderLayer)

    void  SetILayer3d(ILayer3d* p)
    { m_pILayer3d = p; }

    void  OnSize(UINT nType, int cx, int cy);

    long  AddRef();
    long  Release();

    void  SetDirty(bool b);
    bool  IsDirty();

    virtual void  Draw();
    virtual void  Commit(HDC hDstDC);
public:
    
public:
    ILayer3d*  m_pILayer3d;
 
    bool  m_bDirty;
    long  m_nRef;   // 外部的3d对象引用计数。当计数回0时，RenderChain删除该layer
    //list<Object3DWrap*>  m_list;
};

}