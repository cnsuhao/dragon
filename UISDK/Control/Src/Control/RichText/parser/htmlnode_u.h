#pragma once
#include "htmlparser.h"

namespace UI
{

class HtmlNode_U : public HtmlNode
{
public:
	virtual HTMLTAGTYPE  GetTagType() override { return TAG_U; }
	virtual void  ParseTag(PARSETAGDATA* pData) override;
	virtual bool  FillCharFormat(RichTextCharFormat* pcf) override;

	static HtmlNode* CreateInstance(HtmlParser* pParser);
};

}