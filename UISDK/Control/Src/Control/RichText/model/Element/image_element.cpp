#include "stdafx.h"
#include "image_element.h"
#include "..\layout_context.h"
#include "..\line.h"

using namespace UI;
using namespace UI::RichTextObjectModel;


bool  ImageElement::Load(LPCTSTR szPath)
{
	m_image.Destroy();
	
	if (!szPath)
		return false;

	m_image.Load(szPath);
	return !m_image.IsNull();
}

void  ImageElement::Draw(HDC hDC, uint nStart, uint nEnd, RECT* prcRun)
{
	if (m_image.IsNull())
		return;

	int x = prcRun->left;
	int y = prcRun->bottom - m_image.GetHeight();

	m_image.Draw(hDC, x, y);
}
uint  ImageElement::GetWidth()
{
    if (m_image.IsNull())
        return 0;
	return m_image.GetWidth();
}
uint  ImageElement::GetHeight()
{
    if (m_image.IsNull())
        return 0;
	return m_image.GetHeight();
}

//////////////////////////////////////////////////////////////////////////

Element*  ImageUnit::CreateElement()
{
    return new ImageElement(this);
}
ImageElement*  ImageUnit::GetImageElement()
{
    return (ImageElement*)m_pFirstElement;
}

void  ImageUnit::LayoutMultiLine(MultiLineLayoutContext* pContext)
{
    if (!pContext->pPage || !pContext->pLine)
        return;

    ImageElement* pElement = GetImageElement();
    if (!pElement)
        return;

    // Ԫ��ʣ����
    int nImageWidth = pElement->GetWidth();
    // ��ǰ��ʣ����
    int nLineRemain = pContext->nPageContentWidth - pContext->xLineOffset;  

    bool bCanFillInThisLine = false;

    // �����ܹ����¸�Ԫ��
    if (nImageWidth <= nLineRemain)
    {
        bCanFillInThisLine = true;
    }
    else
    {
        // ���Ѿ���һ�����У�ǿ�Ʒ�
        if (!pContext->pLine->HasRun())
        {
            bCanFillInThisLine = true;
        }
        else
        {
            // �Ų��£�����һ��
            pContext->AppendNewLine();
        }
    }

    Run* pRun = pContext->pLine->AppendRun(pElement, nImageWidth, pElement->GetHeight());
    pRun->SetRange(0, 0);
    pRun->SetPosInLine(pContext->xLineOffset);

    // ���з���
    pContext->xLineOffset += nImageWidth;
    if (pContext->xLineOffset >= pContext->nPageContentWidth)
        pContext->SetLineFinish();

    pContext->SetElementLayoutFinish();
}
