#pragma once
namespace UI
{
namespace RT
{
class Element;
class Doc;
struct SingleLineLayoutContext;
struct MultiLineLayoutContext;

// element»ùÀà
enum class UnitType
{
    TEXT = 0,
    LINK = 1,
    IMAGE = 2,

    INVALID,
};


class Unit 
{
public:
    Unit();
    virtual ~Unit();

	virtual BOOL  OnSetCursor() { return FALSE; }
	virtual void  OnClick() {}
	virtual LPCTSTR  GetToolTipText() { return NULL; }

public:

    void  SetHover(bool b);
    void  SetPress(bool b);
    bool  IsHover();
    bool  IsPress();

	void  PreLayout();
    void  Invalidate();

protected:
	short  m_leftPadding;
	short  m_rightPadding;
    bool  m_bHover;
    bool  m_bPress;
};

}
}