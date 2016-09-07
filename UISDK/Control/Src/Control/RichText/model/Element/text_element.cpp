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

// ��ȡ�������
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

// �������ַ��ŵĴ�С
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


// ���ֹ����У�����һ����ǰ�е�ʣ���ȣ�������ܷ��¶�����
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

	// һ�����Ų���
	if (-1 == nTo)
		return false;

	// ȫ���ܷ���
	*pWidth = nWidthRet;
	if (pnTo)
		*pnTo = nTo;

	return true;
}

// ��ȡ��nFrom��ʼ�������ֽ���֮������ݿ��
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
		// �ܴ�С��ȥ 0~nFrom
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
// �ⲿ�����һ�����ֺ��Ƚ����ֽ��в��Ϊ���ɸ�Element�����Element����һ������
// ����"abcҪ��Ҫ123" -->  abc + Ҫ��Ҫ + 123
//
void  TextUnit::AppendTextNoCarriageReturn(LPCTSTR szText, uint nLength, RichTextCharFormat* pcf)
{
    if (!szText || !nLength)
        return;

    // ���
    vector<SCRIPT_ITEM>  vecScriptIetm;
    if (!ScriptItemizeWrap(szText, nLength, &vecScriptIetm) || 
        0 == vecScriptIetm.size())
        return;

    // �����ַ���С
    HDC hTempDC = CreateCompatibleDC(NULL);
    HFONT hOldFont = (HFONT)GetCurrentObject(hTempDC, OBJ_FONT);
    SCRIPT_CACHE script_cache = NULL;  // Initialize to NULL, will be filled lazily.

    SCRIPT_ITEM* script_item = &vecScriptIetm[0];
    for (uint i = 0; i < nLength; )
    {
        int nStart = i;
        int nEnd = (script_item + 1)->iCharPos;

        // TODO: ����Ĭ��Ҫ��unit��һ��text���ͣ�����һ��textelement
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

        // TODO: ����element �߼����ͳһ

#pragma region // �س�����
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


#pragma region // �����ı�����
        while (1)
        {
            // ���element������ɣ�context.pElement��ΪNULL
            if (!layout_all_multiline_textelement(pContext)) // ������
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

    // Ԫ��ʣ����
    int nElementRemainWidth = pElement->GetWidthByFrom(pContext->nElementFrom);  
    // ��ǰ��ʣ����
    int nLineRemain = pContext->nPageContentWidth - pContext->xLineOffset;  

    uint nTo = pContext->nElementFrom;
    uint nRunWidth = nElementRemainWidth;

    // �����ܹ����¸�Ԫ��
    if (nElementRemainWidth <= nLineRemain)
    {
        nTo = pElement->GetMaxGlyphIndex();
    }
    else
    {
        // ���зŲ��¸�Ԫ��
        nRunWidth = nLineRemain;
        if (!pElement->GetRangeToByWidthFrom(pContext->nElementFrom, &nRunWidth, &nTo))
        {
            // һ�����Ų��£�����һ�С��������Ѿ���һ�����У�ǿ�Ʒ�һ��
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

    // ���з���
    pContext->xLineOffset += nRunWidth;
    if (pContext->xLineOffset >= pContext->nPageContentWidth)
        pContext->SetLineFinish();

    // element����
    if (nTo == pElement->GetMaxGlyphIndex())
        pContext->SetElementLayoutFinish();
    else
        pContext->nElementFrom = nTo+1;

    return true;
}
