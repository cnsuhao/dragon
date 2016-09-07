#include "stdafx.h"
#include "doc.h"
#include "page.h"
#include "element.h"
#include "line.h"
#include "run.h"
#include "font.h"
#include "Element\carriage_return_element.h"
#include "Element\image_element.h"
#include "Element\text_element.h"
#include "Element\link_element.h"
#include "layout_context.h"


using namespace UI;
using namespace UI::RichTextObjectModel;

#define DEFAULT_LINESPACE  0

Doc::Doc() : m_mkmgr(this)
{
	memset(&m_defaultcf, 0, sizeof(m_defaultcf));
	memset(&m_defaultcf_link, 0, sizeof(m_defaultcf_link));
	memset(&m_defaultpf, 0, sizeof(m_defaultpf));
    m_defaultpf.lLineSpace = DEFAULT_LINESPACE;

	m_ePageMode = /*SingleLine*/MultiLine;
	m_pFirstPage = NULL;
    SetRectEmpty(&m_rcInvalidInDoc);
    m_eInvalidState = InvalidStateEmpty;
    FontCache::Instance().AddRef();

    m_pIRichText = NULL;
}

Doc::~Doc()
{
	Release();
    FontCache::Instance().Release();
}

void  Doc::Init(IRichText* pRichText)
{
    m_pIRichText = pRichText;

	if (pRichText)
		m_mkmgr.SetUIApplication(pRichText->GetUIApplication());
}

void  Doc::Release()
{
	m_mkmgr.Reset();
	Release_Page();
	Release_Unit();
}

RichTextMKMgr*  Doc::GetMKMgr()
{
	return &m_mkmgr;
}

PageMode  Doc::GetPageMode()
{
    return m_ePageMode;
}   

IMessage*  Doc::GetIMessageObj()
{
	return static_cast<IMessage*>(m_pIRichText);
}

void  Doc::Release_Page()
{
	Page* p = m_pFirstPage;
	Page* pNext = NULL;
	while (p)
	{
		pNext = p->GetNextPage();
		delete p;
		p = pNext;
	}
	m_pFirstPage = NULL;
}

void  Doc::Release_Unit()
{
    list<Unit*>::iterator iter = m_listUnits.begin();
    for (; iter != m_listUnits.end(); iter++)
    {
        delete (*iter);
    }
    m_listUnits.clear();
}

void  Doc::Render2DC(HDC hDC, POINT ptDoc)
{
	Page* p = m_pFirstPage;
	while (p)
	{
		p->Draw(hDC, ptDoc);
		p = p->GetNextPage();
	}
}

Page*  Doc::CreatePage()
{
	Page* p = new Page(this);
	return p;
}

Page*  Doc::GetFirstPage()
{
	return m_pFirstPage;
}


void  Doc::CopyDefaultCharFormat(RichTextCharFormat* pcf)
{
    if (!pcf)
        return;

    memcpy(pcf, GetDefaultCharFormat(), sizeof(m_defaultcf));

	// 这些字段不返出去。外部调用该函数时，仅仅是为了便于创建一个类似字体
	pcf->hFont = NULL;
	pcf->nFontHeight = 0;
	pcf->nFontDescent = 0;
}
void  Doc::CopyDefaultLinkCharFormat(RichTextCharFormat* pcf)
{
	if (!pcf)
		return;

	memcpy(pcf, GetDefaultLinkCharFormat(), sizeof(m_defaultcf_link));

	// 这些字段不返出去。外部调用该函数时，仅仅是为了便于创建一个类似字体
	pcf->hFont = NULL;
	pcf->nFontHeight = 0;
	pcf->nFontDescent = 0;
}

const RichTextParaFormat*  Doc::GetDefaultParaFormat()
{
    return &m_defaultpf;
}
const RichTextCharFormat*  Doc::GetDefaultCharFormat()
{
	if (!m_defaultcf.hFont)
		CreateDefaultFont();
	
	return &m_defaultcf;
}
const RichTextCharFormat*  Doc::GetDefaultLinkCharFormat()
{
	if (!m_defaultcf_link.hFont)
		CreateDefaultLinkFont();

	return &m_defaultcf_link;
}

void  Doc::AppendText(LPCTSTR szText)
{
	if (!szText)
		return;

	CreateDefaultFont();

    AppendText2(szText, &m_defaultcf);
}


void  Doc::AppendText2(LPCTSTR szText, RichTextCharFormat* pcf)
{
    if (!pcf)
        return AppendText(szText);

    // 缓存该字体
    FontCache::Instance().GetFont(pcf);

    TextUnit* pTextUnit = new TextUnit(this);
    pTextUnit->SetCharFormat(pcf);
    append_text(szText, pTextUnit, NULL);

    m_listUnits.push_back(pTextUnit);
}
#if 0
// 过滤szText中的回车换行，然后依次调用insert_text_no_carriage_return
void  Doc::insert_text(LPCTSTR szText, TextUnit* pUnit, int nPos)
{
	if (-1 == nPos)
		return append_text(szText, pUnit);

	int  nLength = _tcslen(szText);
	if (0 == nLength)
		return;

	LPCTSTR pStart = szText;
	LPCTSTR pEnd = pStart + nLength;

	LPCTSTR p = NULL;
	do
	{
		p = _tcschr(pStart, TEXT('\n'));

		int nParaLength = 0;
		if (p)
		{
			nParaLength = p - pStart;
			if (nParaLength > 0 && *(p-1)==TEXT('\r'))  // \n前面的\r过滤掉
				nParaLength --;

			if (nParaLength)
				pUnit->AppendTextNoCarriageReturn(pStart, nParaLength);
			if (m_ePageMode != SingleLine)
				pUnit->AppendCarriageReturn();

			pStart = p+1;
		}
		else
		{
			nParaLength = pEnd - pStart;
			pUnit->AppendTextNoCarriageReturn(pStart, nParaLength);
		}        
	} 
	while (p);
}

#endif
// 过滤szText中的回车换行，然后依次调用append_text_no_carriage_return
void  Doc::append_text(LPCTSTR szText, TextUnit* pUnit, RichTextCharFormat* pcf)
{
    int  nLength = _tcslen(szText);
    if (0 == nLength)
        return;

    LPCTSTR pStart = szText;
    LPCTSTR pEnd = pStart + nLength;
    
    LPCTSTR p = NULL;
    do
    {
        p = _tcschr(pStart, TEXT('\n'));

        int nParaLength = 0;
        if (p)
        {
            nParaLength = p - pStart;
            if (nParaLength > 0 && *(p-1)==TEXT('\r'))  // \n前面的\r过滤掉
                nParaLength --;

            if (nParaLength)
                pUnit->AppendTextNoCarriageReturn(pStart, nParaLength, pcf);
            if (m_ePageMode != SingleLine)
                pUnit->AppendCarriageReturn();

            pStart = p+1;
        }
        else
        {
            nParaLength = pEnd - pStart;
            pUnit->AppendTextNoCarriageReturn(pStart, nParaLength, pcf);
        }        
    } 
    while (p);
}

ImageUnit*  Doc::AppendImage(LPCTSTR szPath)
{
	if (!szPath)
		return NULL;

    ImageUnit* pUnit = new ImageUnit(this);
    pUnit->AppendElement();
    m_listUnits.push_back(pUnit);

    pUnit->GetImageElement()->Load(szPath);
	return pUnit;
}

LinkUnit*  Doc::AppendLink(RichTextLinkInfo* pInfo)
{
	if (!pInfo)
		return NULL;

	CreateDefaultLinkFont();
	return AppendLink2(pInfo, &m_defaultcf_link);
}

LinkUnit*  Doc::AppendLink2(RichTextLinkInfo* pInfo, RichTextCharFormat* pcf)
{
	if (!pInfo || !pcf)
		return NULL;

	// 缓存该字体
	FontCache::Instance().GetFont(pcf);

    LinkUnit* pUnit = new LinkUnit(this);
    pUnit->SetCharFormat(pcf);
	append_text(pInfo->szText, pUnit, NULL);

	pUnit->SetToolTip(pInfo->szTooltip);
	pUnit->SetWParam(pInfo->wParam);
	pUnit->SetLParam(pInfo->lParam);

    m_listUnits.push_back(pUnit);
	return pUnit;
}

bool  Doc::AppendLink3(
		LinkUnit* pUnit, 
		LPCTSTR  szText, 
		RichTextCharFormat* pcf)
{
	if (!pUnit || !szText|| !pcf)
		return false;

	// 缓存该字体
	FontCache::Instance().GetFont(pcf);
	append_text(szText, pUnit, pcf);
	return true;
}

void  Doc::LayoutAll(uint nControlWidth, uint nControlHeight)
{
	PreLayoutAll();

    if (nControlWidth == 0 || nControlHeight == 0)
        return;

	switch (m_ePageMode)
	{
	case SingleLine:
		{
			LayoutAll_SingleLine(nControlWidth, nControlHeight);
		}
		break;

	case MultiLine:
		{
			LayoutAll_MultiLine(nControlWidth, nControlHeight);
		}
		break;

	case MultiPage:
		{
			LayoutAll_MultiPage(nControlWidth, nControlHeight);
		}
		break;
	}
}

// 布局前，释放布局对象:Page、Line、Run
void  Doc::PreLayoutAll()
{
	Release_Page();

	list<Unit*>::iterator iter = m_listUnits.begin();
	for (; iter != m_listUnits.end(); ++iter)
	{
		(*iter)->PreLayout();
	}
}

void  Doc::LayoutAll_SingleLine(uint nControlWidth, uint nControlHeight)
{
    if (m_listUnits.empty())
        return;

    m_pFirstPage = CreatePage();
    m_pFirstPage->SetSize(nControlWidth, nControlHeight);

    SingleLineLayoutContext context;
    context.pDoc = this;
    context.pPage = m_pFirstPage;
    context.pLine = m_pFirstPage->AppendLine(); // 唯一行
    context.xLineOffset = 0;  // 该行run横向起始位置

    list<Unit*>::iterator iter = m_listUnits.begin();
    for (; iter != m_listUnits.end(); ++iter)
    {
        (*iter)->LayoutSingleLine(&context);
    }
}

void  Doc::LayoutAll_MultiLine(uint nControlWidth, uint nControlHeight)
{
    if (m_listUnits.empty())
        return;

    m_pFirstPage = CreatePage();
    m_pFirstPage->SetSize(nControlWidth, nControlHeight);

    MultiLineLayoutContext  context = {0};
    context.pDoc = this;
    context.pPage = m_pFirstPage;
    context.nPageContentWidth =
        nControlWidth -
        m_defaultpf.rPagePadding.left - 
        m_defaultpf.rPagePadding.right;
    context.nPageContentHeight =
        nControlHeight -  
        m_defaultpf.rPagePadding.top - 
        m_defaultpf.rPagePadding.bottom;

    list<Unit*>::iterator iter = m_listUnits.begin();
    for (; iter != m_listUnits.end(); ++iter)
    {
        (*iter)->LayoutMultiLine(&context);
    }
}

void  Doc::LayoutAll_MultiPage(uint nControlWidth, uint nControlHeight)
{

}

Page*  Doc::PageHitTest(POINT ptDoc, __out POINT* ptInPage)
{
	if (!m_pFirstPage)
		return NULL;

	if (m_ePageMode == SingleLine || m_ePageMode == MultiLine)
	{
		RECT rcPage;
		POINT ptDocOffset = {0};
		m_pFirstPage->GetPageContentRegion(ptDocOffset, &rcPage);

		if (PtInRect(&rcPage, ptDoc))
		{
			if (ptInPage)
			{
				ptInPage->x = ptDoc.x - rcPage.left;
				ptInPage->y = ptDoc.y - rcPage.top;
			}
			return m_pFirstPage;
		}
		
		return NULL;
	}

	if (m_ePageMode == MultiPage)
	{
		Page* p = m_pFirstPage;
		while (p)
		{
			if (p->IsVisible())
			{
				RECT rcPage;
				POINT ptDocOffset = {0};
				p->GetPageContentRegion(ptDocOffset, &rcPage);

				if (PtInRect(&rcPage, ptDoc))
				{
					if (ptInPage)
					{
						ptInPage->x = ptDoc.x - rcPage.left;
						ptInPage->y = ptDoc.y - rcPage.top;
					}
					return p;
				}
			}
			p = p->GetNextPage();
		}

		return NULL;
	}

	return NULL;
}

void  Doc::InvalidRegion(RECT* prcInDoc)
{
    if (!prcInDoc)
    {
        m_eInvalidState = InvalidStateFull;
        SetRectEmpty(&m_rcInvalidInDoc);
        return;
    }

    if (!IsRectEmpty(prcInDoc))
    {
        UnionRect(&m_rcInvalidInDoc, &m_rcInvalidInDoc, prcInDoc);
        m_eInvalidState = InvalidStatePart;
    }
}

void  Doc::ClearInvlidRegion()
{
    m_eInvalidState = InvalidStateEmpty;
    SetRectEmpty(&m_rcInvalidInDoc);
}

void  Doc::Refresh()
{
    if (m_eInvalidState == InvalidStateEmpty)   
        return;

    if (!m_pIRichText)
        return;

	if (m_eInvalidState == InvalidStatePart)
		m_pIRichText->UpdateObjectEx(&m_rcInvalidInDoc, 1, true);
	else
		m_pIRichText->UpdateObject();

    ClearInvlidRegion();
}

void  Doc::SetDefaultFont(RichTextCharFormat* pcf)
{
	if (!pcf)
		return;

	memcpy(&m_defaultcf, pcf, sizeof(m_defaultcf));
	m_defaultcf.hFont = 0;
	m_defaultcf.nFontDescent = 0;
	m_defaultcf.nFontHeight = 0;

	// 生成默认字体
	FontCache::Instance().GetFont(&m_defaultcf);

}
void  Doc::SetDefaultLinkFont(RichTextCharFormat* pcf)
{
	if (!pcf)
		return;

	memcpy(&m_defaultcf_link, pcf, sizeof(m_defaultcf_link));
	m_defaultcf_link.hFont = 0;
	m_defaultcf_link.nFontDescent = 0;
	m_defaultcf_link.nFontHeight = 0;

	// 生成默认字体
	FontCache::Instance().GetFont(&m_defaultcf_link);
}

void  Doc::CreateDefaultFont()
{
	if (m_defaultcf.hFont)
		return;

	memset(&m_defaultcf, 0, sizeof(m_defaultcf));
	_tcscpy(m_defaultcf.szFontName, TEXT("微软雅黑"));
	m_defaultcf.lFontSize = 9;
	

	FontCache::Instance().GetFont(&m_defaultcf);
}
void  Doc::CreateDefaultLinkFont()
{
	if (m_defaultcf_link.hFont)
		return;

	memset(&m_defaultcf_link, 0, sizeof(m_defaultcf_link));
	_tcscpy(m_defaultcf_link.szFontName, TEXT("微软雅黑"));
	m_defaultcf_link.lFontSize = 9;
	m_defaultcf_link.bUnderline = false;  // 下划线由自己来画，避免多创建一种字体
	m_defaultcf_link.textColor = RGB(0,0,204);
	m_defaultcf_link.bTextColor = true;

	FontCache::Instance().GetFont(&m_defaultcf_link);
}

const Style&  Doc::GetStyle()
{
    return m_richtextStyle;
}