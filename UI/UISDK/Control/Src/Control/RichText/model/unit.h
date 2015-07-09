#pragma once
namespace UI
{
namespace RichTextObjectModel
{
class Element;
class Doc;
class SingleLineLayoutContext;
class MultiLineLayoutContext;

// element基类
enum UnitType
{
    UT_TEXT = 0,
    UT_LINK = 1,
    UT_IMAGE = 2,

    EMT_INVALID,
};


class Unit 
{
public:
    Unit(Doc* pDoc);
    virtual ~Unit();

    virtual Element*  CreateElement() PURE;
    virtual void  LayoutSingleLine(SingleLineLayoutContext* pContext);
    virtual void  LayoutMultiLine(MultiLineLayoutContext* pContext){}; // 由派生类实现
	virtual BOOL  OnSetCursor() { return FALSE; }
	virtual void  OnClick() {}
	virtual LPCTSTR  GetToolTipText() { return NULL; }

public:
    void  Release_Element();
    Element*  AppendElement();
    void  AppendElement2(Element* p);
    Element*  GetFirstElement();
    Element*  GetLastElement();

    void  SetHover(bool b);
    void  SetPress(bool b);
    bool  IsHover();
    bool  IsPress();

	void  PreLayout();
    void  Invalidate();

protected:
    Doc*  m_pDoc;
    Element*  m_pFirstElement;

	short  m_leftPadding;
	short  m_rightPadding;
    bool  m_bHover;
    bool  m_bPress;
};

}
}