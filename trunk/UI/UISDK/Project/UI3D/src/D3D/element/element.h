#pragma once
#include "UISDK\Project\UI3D\src\D3D\Wrap\translation\translation.h"

namespace UI
{

class Element
{
public:
    Element();
    virtual ~Element();

    void  SetAttribute(IMapAttribute* pMapAttrib);
    virtual void OnPaint() = 0;

    void  SetTransparency(float fAlpha);
	DxTranslation*  GetWorldMatrix(); 

    static  void  Unload();

protected:
    byte  m_byteTransparent;  // 透明度 0-255 255表示不透明
    DxTranslation  m_matWorld;  // 偏移 + 旋转 + 缩放，将model加入到世界坐标当中
};

}