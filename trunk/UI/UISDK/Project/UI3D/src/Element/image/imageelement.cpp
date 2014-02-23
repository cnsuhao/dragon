#include "stdafx.h"
#include "imageelement.h"
#include "UISDK\Project\UI3D\inc\ielement.h"


namespace UI
{
ImageElement::ImageElement()
{
	m_pIImageElement = NULL;

    // Ä¬ÈÏ¾ÓÖÐ
    m_fDrawLeft = -0.5f;
    m_fDrawTop = 0.5f;
    m_fDrawRight = 0.5f;
    m_fDrawBottom = -0.5f;
	m_fZ = 0.f;
	m_bDirectDraw = false;
}

ImageElement::~ImageElement()
{
	SAFE_DELETE(m_pIImageElement);
}	

IImageElement*  ImageElement::GetIImageElement()
{
	if (m_pIImageElement)
		return m_pIImageElement;

	m_pIImageElement = new IImageElement(this);
	return m_pIImageElement;
}

void ImageElement::OnPaint()
{
	m_matWorld.ApplyDevice(GetDevice());
	m_texture.DrawRect(GetDevice(), m_fDrawLeft, m_fDrawTop, m_fDrawRight, m_fDrawBottom, m_fZ, m_bDirectDraw, m_byteTransparent);
}

bool  ImageElement::LoadFromFile(const TCHAR* szFile)
{
    return m_texture.LoadFile(szFile, GetDevice());
}

void  ImageElement::SetDrawRect(float fLeft, float fTop, float fRight, float fBottom, float fZ)
{
    m_fDrawLeft = fLeft;
    m_fDrawTop = fTop;
    m_fDrawRight = fRight;
    m_fDrawBottom = fBottom;
	m_fZ = fZ;
	m_bDirectDraw = false;
}
void  ImageElement::SetDirectDrawRect(LPRECT prc, float fz)
{
	m_fDrawLeft = (float)prc->left;
	m_fDrawTop = (float)prc->top;
	m_fDrawRight = (float)prc->right;
	m_fDrawBottom = (float)prc->bottom;
	m_fZ = fz;
	m_bDirectDraw = true;
}

}