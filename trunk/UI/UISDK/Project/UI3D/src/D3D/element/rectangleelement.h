#pragma once
#include "UISDK\Project\UI3D\src\D3D\element\element.h"

namespace UI
{
interface IRectangleElement;
class RectangleElement : public Element
{
public:
    RectangleElement();
    ~RectangleElement();
    
    IRectangleElement*  GetIRectangleElement();

public:
    bool  CreateBuffer(RECTF* prc, float fz);
    void  OnPaint();

    static  bool  Init();
    static  void  Release();

private:
    static ID3D10EffectTechnique*  m_pEffectTechnique; // 不需要释放
    static ID3D10InputLayout*      m_pInputLayout;
    static D3D10_TECHNIQUE_DESC    m_techDesc;

    IRectangleElement*  m_pIRectangleElement;
    ID3D10Buffer*  m_pBuffer;


    struct Vertex
    {
        D3DXVECTOR3 Pos;
    };
};
    
}