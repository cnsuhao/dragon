#include "stdafx.h"
#include "htmlnode_a.h"

HtmlNode_A::HtmlNode_A()
{
	m_pLinkUnit = NULL;
}

HtmlNode* HtmlNode_A::CreateInstance(HtmlParser* pParser)
{
	HtmlNode_A* p = new HtmlNode_A(); 
	p->SetHtmlParser(pParser);
	p->AddRef();
	return p;
}

void  HtmlNode_A::SetLinkUnit(RichTextObjectModel::LinkUnit* p)
{
	m_pLinkUnit = p;
}
RichTextObjectModel::LinkUnit*  HtmlNode_A::GetLinkUnit()
{
	return m_pLinkUnit;
}

void  HtmlNode_A::ParseTag(PARSETAGDATA* pData)
{
	if (pData->bStartBacklash)
	{
		// 取消堆栈上的一个<a>
		m_pHtmlParser->RemoveLastTagNode(GetTagType());
	}
	else
	{
		// 没有闭合，放入堆栈中，等待下一个</a>
		if (!pData->bEndBacklash)
			m_pHtmlParser->AddNode(this);
	}
}