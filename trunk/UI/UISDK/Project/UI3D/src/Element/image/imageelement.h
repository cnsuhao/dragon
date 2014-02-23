#pragma once
#include "UISDK\Project\UI3D\src\Element\element.h"
#include "UISDK\Project\UI3D\src\D3D\Wrap\texture.h"

namespace UI
{
interface IImageElement;

class ImageElement : public Element
{
public:
    ImageElement();
    ~ImageElement();

    virtual void OnPaint();

public:
	IImageElement*  GetIImageElement();
    bool  LoadFromFile(const TCHAR* szFile);
    void  SetDrawRect(float fLeft, float fTop, float fRight, float fBottom, float fZ);
	void  SetDirectDrawRect(LPRECT prc, float fz);

private:
	IImageElement*  m_pIImageElement;
    DxTexture  m_texture;

	// 绘制坐标。
    float  m_fDrawLeft;
    float  m_fDrawTop;
    float  m_fDrawRight;
    float  m_fDrawBottom;
	float  m_fZ;
	bool  m_bDirectDraw;  // 采用rhw直接绘制2D元素

};
}
