#pragma once
#include "..\element.h"
#include "..\unit.h"

namespace UI
{
namespace RichTextObjectModel
{

// ����Ҳ������TextUnit����
class CarriageReturnElement : public Element
{
public:
    CarriageReturnElement(Unit* p):Element(p){}

    virtual ElementType  GetElementType() { return ElementTypeCarriageReturn; }
	virtual void  Draw(HDC hDC, uint nStart, uint nEnd, RECT* prcRun) override {};
	virtual uint  GetWidth() override {return 0;}
	virtual uint  GetHeight() override {return 0;}
};

// class CarriageReturnUnit : public Unit 
// {
// public:
//     virtual Element*  CreateElement();
// };

}
}