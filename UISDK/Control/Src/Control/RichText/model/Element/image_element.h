#pragma once
#include "..\element.h"
#include "..\unit.h"
#include "UISDK\Kernel\Inc\Util\iimage.h"

// 还不支持动画
namespace UI
{
namespace RichTextObjectModel
{

class ImageElement : public Element
{
public:
    ImageElement(Unit* pUnit):Element(pUnit) {};

    virtual ElementType  GetElementType() { return ElementTypeImage; }
	virtual void  Draw(HDC hDC, uint nStart, uint nEnd, RECT* prcRun) override;
	virtual uint  GetWidth() override;
	virtual uint  GetHeight() override;

public:
	bool  Load(LPCTSTR szPath);

protected:
	ImageWrap  m_image;
};

class ImageUnit : public Unit
{
public:
    ImageUnit(Doc* pDoc):Unit(pDoc){}

    virtual Element*  CreateElement();
    virtual void  LayoutMultiLine(MultiLineLayoutContext* pContext);

public:
    ImageElement*  GetImageElement();
};

}
}