#pragma once

namespace UI
{

namespace RichTextObjectModel
{
	class Doc;
	class Page;
	class Line;
	class Run;
    class Element;
    class Unit;
}

class RichTextMKMgr
{
public:
	RichTextMKMgr(RichTextObjectModel::Doc*);
	~RichTextMKMgr();

public:
	void  Reset();

    bool  IsPageHover(RichTextObjectModel::Page*  pPage);
    bool  IsLineHover(RichTextObjectModel::Line*  pLine);
    bool  IsRunHover(RichTextObjectModel::Run*  pRun);
    bool  IsElementHover(RichTextObjectModel::Element*  pElement);

	void  OnMouseMove(UINT nFlags, POINT ptDoc);
	void  OnLButtonDown(UINT nFlags, POINT point);
	void  OnLButtonUp(UINT nFlags, POINT point);
	void  OnMouseLeave();
	BOOL  OnSetCursor();
	void  SetUIApplication(IUIApplication*);

	void  HitTest(POINT ptDoc, 
		RichTextObjectModel::Page** ppHoverPage, 
		RichTextObjectModel::Line** ppHoverLine, 
		RichTextObjectModel::Run** ppHoverRun);

    void  SetHoverUnit(RichTextObjectModel::Unit*);
	void  SetPressUnit(RichTextObjectModel::Unit*);

private:
	RichTextObjectModel::Doc*  m_pDoc;
	IUIApplication*  m_pUIApp;  // ”√”⁄tooltip

	RichTextObjectModel::Page*  m_pHoverPage;
	RichTextObjectModel::Line*  m_pHoverLine;
	RichTextObjectModel::Run*   m_pHoverRun;
    RichTextObjectModel::Element*   m_pHoverElement;

    RichTextObjectModel::Unit*   m_pHoverUnit;
	RichTextObjectModel::Unit*   m_pPressUnit;
};

}