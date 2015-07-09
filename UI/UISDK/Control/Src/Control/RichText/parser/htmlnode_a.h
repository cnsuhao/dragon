#pragma once
#include "htmlparser.h"

namespace UI
{

namespace RichTextObjectModel
{
	class LinkUnit;
}

class HtmlNode_A : public HtmlNode
{
public:
	HtmlNode_A();

	virtual HTMLTAGTYPE  GetTagType() override 
	{ 
		return TAG_A; 
	}

	virtual void  ParseTag(PARSETAGDATA* pData) override;
	static HtmlNode* CreateInstance(HtmlParser* pParser);

	void  SetLinkUnit(RichTextObjectModel::LinkUnit* p);
	RichTextObjectModel::LinkUnit*  GetLinkUnit();

private:
	RichTextObjectModel::LinkUnit*  m_pLinkUnit;
};




}