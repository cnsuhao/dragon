#include "stdafx.h"
#include "text_element.h"
#include "carriage_return_element.h"
#include "..\layout_context.h"
#include "..\line.h"
#include "..\doc.h"
#include "..\..\uniscribe\uniscribehelper.h"


using namespace UI;
using namespace UI::RichTextObjectModel;

TextElement::TextElement(Unit* pUnit):Element(pUnit)
{
	m_pcf = NULL;
	memset(&m_sa, 0, sizeof(m_sa));
	m_nABCWidth = 0;
}

TextElement::~TextElement()
{
	SAFE_DELETE(m_pcf);
}

void  TextElement::SetText(LPCTSTR  szText, int nLength)
{
	if (!szText || 0 == nLength)
		return;

	if (-1 == nLength)
	{
		m_strText = szText;
	}
	else
	{
		m_strText.append(szText, nLength);
	}
}

// 获取最大索引
uint  TextElement::GetMaxGlyphIndex()
{
	return m_advances.size()-1;
}

const RichTextCharFormat*  TextElement::GetCharFormat()
{
	if (m_pcf)
		return m_pcf;

    return static_cast<TextUnit*>(m_pUnit)->GetCharFormat();
}

void  TextElement::SetCharFormat(RichTextCharFormat* pcf)
{
	if (!pcf)
	{
		SAFE_DELETE(m_pcf);
		return;
	}

	if (!m_pcf)
		m_pcf = new RichTextCharFormat;

	memcpy(m_pcf, pcf, sizeof(RichTextCharFormat));
}

void  TextElement::SetScriptAnalysis(SCRIPT_ANALYSIS* psa)
{
	memcpy(&m_sa, psa, sizeof(m_sa));
}

// 计算文字符号的大小
void  TextElement::CalcGlyphSize(HDC hTempDC, SCRIPT_CACHE script_cache)
{
	HFONT hfont = this->GetCharFormat()->hFont;
	if (!hfont)
		return;

	std::vector<WORD> logs;
	std::vector<WORD> glyphs;
	std::vector<SCRIPT_VISATTR> visattr;
	ScriptShapeWrap(
		m_strText.c_str(),
		m_strText.length(),
		hfont,
		&script_cache, 
		&m_sa,
		&logs,
		&glyphs, 
		&visattr);

	m_advances.resize(glyphs.size());
	m_offsets.resize(glyphs.size());
	ABC abc;

	HFONT hOldFont = (HFONT)SelectObject(hTempDC, hfont);

	HRESULT hr = ScriptPlace(
		hTempDC,
		&script_cache,
		&glyphs[0], glyphs.size(),  // From previous call to ScriptShape
		&visattr[0],                // From previous call to ScriptShape
		&m_sa,                      // From previous call to ScriptItemize
		&m_advances[0],               // Output: glyph advances
		&m_offsets[0],                // Output: glyph offsets
		&abc);                      // Output: size of run

	m_nABCWidth = abc.abcA + abc.abcB + abc.abcC;
	if (hOldFont)
		SelectObject(hTempDC, hOldFont);
}


// 布局过程中，给定一个当前行的剩余宽度，求出还能放下多少字
bool  TextElement::GetRangeToByWidthFrom(uint nFrom, __inout uint* pWidth, __out uint* pnTo)
{
	uint nSize = m_advances.size();

	int nWidthReq = *pWidth;
	int nWidthRet = 0;

	int nTo = -1;
	for (uint i = nFrom; i < nSize; i++)
	{
		if (nWidthRet + m_advances[i] > nWidthReq)
			break;

		nWidthRet += m_advances[i];
		nTo = i;
	}

	// 一个都放不下
	if (-1 == nTo)
		return false;

	// 全部能放下
	*pWidth = nWidthRet;
	if (pnTo)
		*pnTo = nTo;

	return true;
}

// 获取从nFrom开始，到文字结束之间的内容宽度
uint  TextElement::GetWidthByFrom(uint nFrom)
{
	if (0 == nFrom)
		return m_nABCWidth;

	uint nSize = (uint)m_advances.size();
	if (nFrom >= nSize)
		return 0;

	uint nHalfSize = nSize>>1;
	uint nRet = 0;

	if (nFrom < nHalfSize)
	{
		// 总大小减去 0~nFrom
		for (uint i = 0; i < nFrom; i++)
			nRet += m_advances[i];

		nRet = m_nABCWidth - nRet;
	}
	else
	{
		for (uint i = nFrom; i < nSize; i++)
			nRet += m_advances[i];
	}

	return nRet;
}

uint  TextElement::GetWidth()
{
	return m_nABCWidth;
}

uint  TextElement::GetHeight()
{
	return GetCharFormat()->nFontHeight;
}


void  TextElement::Draw(HDC hDC, uint nStart, uint nEnd, RECT* prcRun)
{
	UIASSERT(nStart <= nEnd && nEnd < m_strText.length());

    const RichTextCharFormat* pcf = this->GetCharFormat();
	LPCTSTR szText = m_strText.c_str()+nStart;
	ULONG nLen = nEnd-nStart+1;

	__super::DrawText(hDC, prcRun, szText, nLen, pcf);
}

//////////////////////////////////////////////////////////////////////////

TextUnit::TextUnit(Doc* pDoc):Unit(pDoc)
{
	m_pcf = NULL;
}
TextUnit::~TextUnit()
{
	SAFE_DELETE(m_pcf);
}
Element*  TextUnit::CreateElement()
{
    return new TextElement(this);
}

void  TextUnit::SetCharFormat(RichTextCharFormat* pcf)
{
	if (!pcf)
	{
		SAFE_DELETE(m_pcf);
		return;
	}

	if (!m_pcf)
		m_pcf = new RichTextCharFormat;

	memcpy(m_pcf, pcf, sizeof(RichTextCharFormat));
}

const RichTextCharFormat*  TextUnit::GetCharFormat()
{
	if (m_pcf)
		return m_pcf;

	return m_pDoc->GetDefaultCharFormat();
}


//
// 外部添加了一段文字后，先将文字进行拆分为若干个Element。这个Element共用一种字体
// 例如"abc要不要123" -->  abc + 要不要 + 123
//
void  TextUnit::AppendTextNoCarriageReturn(LPCTSTR szText, uint nLength, RichTextCharFormat* pcf)
{
    if (!szText || !nLength)
        return;

    // 拆分
    vector<SCRIPT_ITEM>  vecScriptIetm;
    if (!ScriptItemizeWrap(szText, nLength, &vecScriptIetm) || 
        0 == vecScriptIetm.size())
        return;

    // 计算字符大小
    HDC hTempDC = CreateCompatibleDC(NULL);
    HFONT hOldFont = (HFONT)GetCurrentObject(hTempDC, OBJ_FONT);
    SCRIPT_CACHE script_cache = NULL;  // Initialize to NULL, will be filled lazily.

    SCRIPT_ITEM* script_item = &vecScriptIetm[0];
    for (uint i = 0; i < nLength; )
    {
        int nStart = i;
        int nEnd = (script_item + 1)->iCharPos;

        // TODO: 这里默认要求unit是一个text类型，创建一个textelement
        TextElement* pElement = static_cast<TextElement*>(this->AppendElement());

        pElement->SetText(szText+nStart, nEnd-nStart);
		pElement->SetCharFormat(pcf);

        pElement->SetScriptAnalysis(&script_item->a);
        pElement->CalcGlyphSize(hTempDC, script_cache);

        script_item++;
        i = nEnd;
    }

    ScriptFreeCache(&script_cache);
    if (hOldFont)
        SelectObject(hTempDC, hOldFont);  // Put back the previous font.
    DeleteDC(hTempDC);
}

void  TextUnit::AppendCarriageReturn()
{
    __super::AppendElement2(new CarriageReturnElement(this));
}

void  TextUnit::LayoutMultiLine(MultiLineLayoutContext* pContext)
{
    Element* pElement = m_pFirstElement;
    while (pElement)
    {
        pContext->pElement = pElement;

        if (!pContext->pLine)
            pContext->AppendNewLine();

        // TODO: 各种element 逻辑如何统一

#pragma region // 回车处理
        ElementType eType = pContext->pElement->GetElementType();
        if (ElementTypeCarriageReturn == eType)
        {
            if (!pContext->pLine->HasRun())
                pContext->pLine->SetFirstParaLine(true);

            pContext->SetLineFinish();
            pContext->SetElementLayoutFinish();

            pElement = pElement->GetNextElement();
            continue;
        }
#pragma endregion


#pragma region // 正常文本布局
        while (1)
        {
            // 如果element布局完成，context.pElement将为NULL
            if (!layout_all_multiline_textelement(pContext)) // 出错了
                break;

            if (!pContext->pLine)
                pContext->AppendNewLine();

            if (pContext->IsElementLayoutFinish())
                break;
        }
#pragma endregion

        pElement = pElement->GetNextElement();
    }
}

bool  TextUnit::layout_all_multiline_textelement(MultiLineLayoutContext* pContext)
{
    if (!pContext->pPage || !pContext->pElement || !pContext->pLine)
        return false;

//     if (pContext->pElement->GetMajorType() != EMT_TEXT)
//         return false;

    TextElement* pElement = static_cast<TextElement*>(pContext->pElement);

    // 元素剩余宽度
    int nElementRemainWidth = pElement->GetWidthByFrom(pContext->nElementFrom);  
    // 当前行剩余宽度
    int nLineRemain = pContext->nPageContentWidth - pContext->xLineOffset;  

    uint nTo = pContext->nElementFrom;
    uint nRunWidth = nElementRemainWidth;

    // 本行能够放下该元素
    if (nElementRemainWidth <= nLineRemain)
    {
        nTo = pElement->GetMaxGlyphIndex();
    }
    else
    {
        // 本行放不下该元素
        nRunWidth = nLineRemain;
        if (!pElement->GetRangeToByWidthFrom(pContext->nElementFrom, &nRunWidth, &nTo))
        {
            // 一个都放不下，另起一行。或者这已经是一个空行，强制放一个
            if (!pContext->pLine->HasRun())
            {
                nTo = pContext->nElementFrom+1;
            }
            else
            {
                pContext->SetLineFinish();
                return true;
            }
        }
    }

    Run* pRun = pContext->pLine->AppendRun(pElement, nRunWidth, pElement->GetHeight());
    pRun->SetRange(pContext->nElementFrom, nTo);
    pRun->SetPosInLine(pContext->xLineOffset);

    // 该行放满
    pContext->xLineOffset += nRunWidth;
    if (pContext->xLineOffset >= pContext->nPageContentWidth)
        pContext->SetLineFinish();

    // element放完
    if (nTo == pElement->GetMaxGlyphIndex())
        pContext->SetElementLayoutFinish();
    else
        pContext->nElementFrom = nTo+1;

    return true;
}
