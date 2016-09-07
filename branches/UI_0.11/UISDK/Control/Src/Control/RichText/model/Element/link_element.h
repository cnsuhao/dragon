#pragma once
#include "text_element.h"

namespace UI
{
namespace RichTextObjectModel
{

class LinkElement : public TextElement
{
public:
    LinkElement(Unit* p):TextElement(p){}

	virtual TextElementType  GetMinorType() { return TET_LINK; }
	virtual void  Draw(HDC hDC, uint nStart, uint nEnd, RECT* prcRun);
};

class LinkUnit : public TextUnit
{
public:
    LinkUnit(Doc* p);
    ~LinkUnit(){};

	IRichTextLinkUnit*  GetIRichTextLinkUnit() {
		return &m_oIRichTextLinkUnit; }
    
    void  SetId(LPCTSTR szId);
    LPCTSTR  GetId();

	void  SetToolTip(LPCTSTR);
	void  SetWParam(WPARAM);
	void  SetLParam(LPARAM);
	WPARAM  GetWParam();
	LPARAM  GetLParam();

public:
    virtual Element*  CreateElement() override;
	virtual BOOL  OnSetCursor() override;
	virtual void  OnClick() override;
	virtual LPCTSTR  GetToolTipText() override; 

protected:
	IRichTextLinkUnit  m_oIRichTextLinkUnit;
    String  m_strId;

	String  m_strToolTip;
	WPARAM  m_wParam;
	LPARAM  m_lParam;
};

}
}