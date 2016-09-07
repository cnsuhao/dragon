#pragma once
#include "..\element.h"
#include "..\unit.h"
#include <usp10.h>

namespace UI
{
namespace RichTextObjectModel
{

enum TextElementType
{
	TET_TEXT = 0,
	TET_LINK = 1
};

class TextElement : public Element
{
public:
    TextElement(Unit* pUnit);
	~TextElement();

	virtual void  Draw(HDC hDC, uint nStart, uint nEnd, RECT* prcRun) override;
	virtual uint  GetWidth() override;
	virtual uint  GetHeight() override;
	virtual uint  GetMaxGlyphIndex() override;

public:
	void  SetText(LPCTSTR szText, int nLength);

	void  SetScriptAnalysis(SCRIPT_ANALYSIS* psa);
	void  CalcGlyphSize(HDC hTempDC, SCRIPT_CACHE script_cache);
	uint  GetWidthByFrom(uint nFrom);
	bool  GetRangeToByWidthFrom(uint nFrom, __inout uint* pWidth, __out uint* pnTo);

    const RichTextCharFormat*  GetCharFormat();
	void  SetCharFormat(RichTextCharFormat* pcf);

protected:
	String            m_strText;
	SCRIPT_ANALYSIS   m_sa;
	vector<int>       m_advances;
	vector<GOFFSET>   m_offsets;
	uint              m_nABCWidth;

	// 为NULL表示使用unit的字体
	RichTextCharFormat*  m_pcf;  
};

class TextUnit : public Unit
{
public:
    TextUnit(Doc* pDoc);
	~TextUnit();
    virtual Element*  CreateElement() override;
    virtual void  LayoutMultiLine(MultiLineLayoutContext* pContext) override;

public:
    void  SetCharFormat(RichTextCharFormat* pcf);
    const RichTextCharFormat*  GetCharFormat();

    void  AppendTextNoCarriageReturn(LPCTSTR szText, uint nLength, RichTextCharFormat* pcf);
    void  AppendCarriageReturn();

private:
    bool  layout_all_multiline_textelement(MultiLineLayoutContext* pContext);

protected:
	// 为NULL表示使用doc的默认字体
    RichTextCharFormat*  m_pcf;
};

}
}