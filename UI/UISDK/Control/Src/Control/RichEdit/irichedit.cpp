#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\irichedit.h"
#include "UISDK\Control\Src\Control\RichEdit\richedit.h"
#include "OLE\reoleapi.h"

using namespace UI;
UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(IRichEdit, RichEdit, IControl)

void  IRichEdit::SetWindowlessRichEdit(WindowlessRichEditEx* pRE)
{
    m_pWindowlessRE = pRE;
}
bool  IRichEdit::SetText(LPCTSTR szText) 
{ 
    return m_pWindowlessRE->SetText(szText);
}

int   IRichEdit::GetText(TCHAR* szBuf, int nLen)
{
    return m_pWindowlessRE->GetText(szBuf, nLen);
}
const char*  IRichEdit::GetRtfBuffer(int nFlags)
{
    return m_pWindowlessRE->GetRtfBuffer(nFlags);
}
void   IRichEdit::ReleaseRtfBuffer(const char* p)
{
    m_pWindowlessRE->ReleaseRtfBuffer(p);
}
bool  IRichEdit::AppendText(LPCTSTR szText, int nLength)
{
    return m_pWindowlessRE->AppendText(szText, nLength);
}

bool  IRichEdit::GetEncodeTextW(IBuffer** ppBuffer)
{
    return m_pWindowlessRE->GetEncodeTextW(ppBuffer);
}

bool  IRichEdit::AppendEncodeText(LPCTSTR szText, int nSize)
{
    return m_pWindowlessRE->AppendEncodeText(szText, nSize);
}
bool  IRichEdit::InsertEncodeText(int nPos, LPCTSTR szText, int nSize)
{
    return m_pWindowlessRE->InsertEncodeText(nPos, szText, nSize);
}
bool  IRichEdit::InsertEncodeText(LPCTSTR szText, int nSize)
{
    return m_pWindowlessRE->InsertEncodeText(szText, nSize);
}

HANDLE  IRichEdit::InsertMessageStart(BubbleType eType, BubbleAlign eAlign)
{
    return m_pWindowlessRE->InsertMessageStart(eType, eAlign);
}
void  IRichEdit::InsertMessageFinish(HANDLE hInsertStart)
{
    m_pWindowlessRE->InsertMessageFinish(hInsertStart);
}

LONG  IRichEdit::GetSelBarWidth() 
{ 
    return m_pWindowlessRE->GetSelBarWidth(); 
}
LONG  IRichEdit::SetSelBarWidth(LONG l_SelBarWidth) 
{ 
    return m_pWindowlessRE->SetSelBarWidth(l_SelBarWidth); 
}
bool  IRichEdit::GetRichTextFlag()
{
    return m_pWindowlessRE->GetRichTextFlag();
}
void  IRichEdit::SetRichTextFlag(bool b)
{
    m_pWindowlessRE->SetRichTextFlag(b); 
}
void  IRichEdit::SetMultiLine(bool b)
{
    m_pWindowlessRE->SetMultiLine(b);
}
bool  IRichEdit::IsMultiLine()
{
    return m_pWindowlessRE->IsMultiLine();
}
bool  IRichEdit::IsReadOnly()
{
    return m_pWindowlessRE->IsReadOnly();
}
void  IRichEdit::SetReadOnly(bool fReadOnly)
{
    m_pRichEditImpl->SetReadOnly(fReadOnly);
}
void  IRichEdit::SetAutoResize(bool b, int nMaxSize)
{
    m_pWindowlessRE->SetAutoResize(b, nMaxSize);
}
bool  IRichEdit::SetCharFormatByLogFont(LOGFONT* plf, COLORREF color)
{
    return m_pWindowlessRE->SetCharFormatByLogFont(plf, color);
}
bool  IRichEdit::SetCharFormat(CHARFORMAT2* pcf)
{
    return m_pWindowlessRE->SetCharFormat(pcf);
}
void  IRichEdit::GetDefaultCharFormat(CHARFORMAT2* pcf)
{
    return m_pWindowlessRE->GetDefaultCharFormat(pcf);
}
void  IRichEdit::SetCharFormatEx(UINT nPart, CHARFORMAT2* pcf)
{
	m_pWindowlessRE->SetCharFormatEx(nPart, pcf);
}
void  IRichEdit::DisableUndoOp()
{
    return m_pWindowlessRE->DisableUndoOp();
}
void  IRichEdit::EnableDragDrop(bool b)
{
    if (b)
        m_pWindowlessRE->RegisterDragDrop();
    else
        m_pWindowlessRE->RevokeDragDrop();
}
bool  IRichEdit::CanDragDrop()
{
    return m_pWindowlessRE->CanDragDrop();
}
bool  IRichEdit::InsertGif(LPCTSTR szGifFile)
{
    return m_pWindowlessRE->InsertGif(szGifFile);
}
bool  IRichEdit::InsertSkinGif(LPCTSTR szGifId)
{
    return m_pWindowlessRE->InsertSkinGif(szGifId);
}
IPictureREOle* IRichEdit::InsertPicture(PictureREOleInfo* pInfo)
{
    return m_pWindowlessRE->InsertPicture(pInfo);
}
void  IRichEdit::SetSel(int nPos, int nLen)
{
	return m_pWindowlessRE->SetSel(nPos, nLen);
}
void  IRichEdit::SelectEnd()
{
    return m_pWindowlessRE->SelectEnd();
}

void  IRichEdit::GetSel(int* pnPos, int* pnLen)
{
    return m_pWindowlessRE->GetSel(pnPos, pnLen);
}

void  IRichEdit::EnableBubbleMode(bool b)
{
    m_pRichEditImpl->EnableBubbleMode(b);
}
bool  IRichEdit::IsBubbleMode()
{
    return m_pRichEditImpl->IsBubbleMode();
}
void  IRichEdit::AppendBubbleSpace()
{
	m_pWindowlessRE->InsertBubbleSpace();
}
void  IRichEdit::ScrollEnd()
{
    m_pWindowlessRE->ScrollEnd();
}
void  IRichEdit::EnableWordBreak(bool b)
{
	m_pWindowlessRE->EnableWordBreak(b);
}
