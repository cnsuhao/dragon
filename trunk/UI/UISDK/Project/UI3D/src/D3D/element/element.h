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
    byte  m_byteTransparent;  // ͸���� 0-255 255��ʾ��͸��
    DxTranslation  m_matWorld;  // ƫ�� + ��ת + ���ţ���model���뵽�������굱��
};

}