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
		// ȡ����ջ�ϵ�һ��<a>
		m_pHtmlParser->RemoveLastTagNode(GetTagType());
	}
	else
	{
		// û�бպϣ������ջ�У��ȴ���һ��</a>
		if (!pData->bEndBacklash)
			m_pHtmlParser->AddNode(this);
	}
}