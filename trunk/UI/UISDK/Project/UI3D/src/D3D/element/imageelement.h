#pragma once
#include "UISDK\Project\UI3D\src\D3D\element\element.h"

namespace UI
{
interface IImageElement;
class ImageElement : public Element
{
public:
    ImageElement();
    ~ImageElement();
    
    IImageElement*  GetIImageElement();

public:
    void  OnPaint();

    static  bool  Init();
    static  void  Release();

private:
    static ID3D10EffectTechnique*  m_pEffectTechnique; // 不需要释放
    static ID3D10InputLayout*      m_pInputLayout;
    static D3D10_TECHNIQUE_DESC    m_techDesc;

    IImageElement*  m_pIImageElement;
    ID3D10Buffer*  m_pBuffer;
};
    
}