#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\irichtext.h"
#include "richtext.h"
#include "model\Element\link_element.h"

using namespace UI;

UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(IRichText, RichText, IControl)

void  IRichText::AppendText(LPCTSTR szText)
{
	m_pRichTextImpl->AppendText(szText);
}
void  IRichText::AppendText2(LPCTSTR szText, RichTextCharFormat* pcf)
{
    m_pRichTextImpl->AppendText2(szText, pcf);
}
void  IRichText::AppendImage(LPCTSTR szPath)
{
	m_pRichTextImpl->AppendImage(szPath);
}
void  IRichText::AppendLink(RichTextLinkInfo* pInfo)
{
	m_pRichTextImpl->AppendLink(pInfo);
}
void  IRichText::AppendLink2(RichTextLinkInfo* pInfo, RichTextCharFormat* pcf)
{
	m_pRichTextImpl->AppendLink2(pInfo, pcf);
}
void  IRichText::GetDefaultCharFormat(RichTextCharFormat* pcf)
{
    m_pRichTextImpl->GetDefaultCharFormat(pcf);
}
void  IRichText::GetDefaultLinkCharFormat(RichTextCharFormat* pcf)
{
	m_pRichTextImpl->GetDefaultLinkCharFormat(pcf);
}
void  IRichText::LoadHtml(LPCTSTR szHtml)
{
	m_pRichTextImpl->LoadHtml(szHtml);
}
IRichTextLinkUnit::IRichTextLinkUnit(RichTextObjectModel::LinkUnit* p)
{
	m_pImpl = p;
}
long  IRichTextLinkUnit::GetId()
{
	return m_pImpl->GetId();
}