#include "stdafx.h"
#include "element.h"
#include "UISDK\Project\UI3D\src\D3D\element\rectangleelement.h"

namespace UI
{

Element::Element()
{
	m_byteTransparent = 255;
}

Element::~Element()
{
	
}

void  Element::SetTransparency(float fAlpha)
{
    if (fAlpha > 1.0f)
        fAlpha = 1.0f;
    if (fAlpha < 0)
        fAlpha = 0;

    m_byteTransparent = (byte)(fAlpha*255); 
}

DxTranslation*  Element::GetWorldMatrix()
{
	return &m_matWorld;
}

void Element::SetAttribute(IMapAttribute* pMapAttrib)
{
    
}

// 退出前，释放一些静态变量
void  Element::Unload()
{
    RectangleElement::Release();
}
}