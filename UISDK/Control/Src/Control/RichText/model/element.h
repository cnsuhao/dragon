#pragma once
#include "UISDK\Control\Inc\Interface\irichtext.h"

namespace UI
{
namespace RichTextObjectModel
{
class Run;
class Doc;
class Unit;
class Line;

enum ElementType
{
    ElementTypeText = 0, 
    ElementTypeCarriageReturn = 1,
    ElementTypeImage = 1,
};

class Element
{
public:
	Element(Unit* pUnit);
	virtual ~Element();

public:
    virtual ElementType  GetElementType() { return ElementTypeText; }
	virtual void  Draw(HDC hDC, uint nStart, uint nEnd, RECT* prcRun) PURE;
	virtual uint  GetWidth() PURE;
	virtual uint  GetHeight() PURE;
	// 获取字符数量
	virtual uint  GetMaxGlyphIndex() { return 0; } 

public:
    Unit*  GetUnit();

    void  SetNextElement(Element* p);
    void  SetPrevElement(Element* p);
    Element*  GetNextElement();
    Element*  GetPrevElement();
    
	void  AddRun(Run* p);
	void  ClearRun();
	void  GetRelativeLines(list<Line*>& listLines);

protected:
	void  DrawText(
			HDC hDC, RECT* prc, LPCTSTR szText, 
			ULONG nLen, const RichTextCharFormat* pcf);
    
protected:
    Unit*  m_pUnit;
	list<Run*>  m_listRuns;

	// 一段文本被uniscribe拆分成多个element
	// 在这里将原整体文本维护起来
 	Element*  m_pPrevElement;
 	Element*  m_pNextElement;
	
};


}
}