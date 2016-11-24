#include "stdafx.h"
#include "imageelement.h"
#include "..\layout_context.h"
#include "..\line.h"

using namespace UI;
using namespace UI::RT;

UI::RT::ImageElement::ImageElement()
{
    m_sizeDraw.cx = m_sizeDraw.cy = 0;
}

bool  ImageElement::Load(LPCTSTR szPath)
{
	m_image.Destroy();
	
	if (!szPath)
		return false;

	m_image.Load(szPath);
	return !m_image.IsNull();
}

uint  ImageElement::GetImageWidth()
{
    if (m_image.IsNull())
        return 0;
    return m_image.GetWidth();
}
uint  ImageElement::GetImageHeight()
{
    if (m_image.IsNull())
        return 0;
    return m_image.GetHeight();
}

uint  ImageElement::GetWidth()
{
    return m_sizeDraw.cx;
}

uint  ImageElement::GetHeight()
{
    return m_sizeDraw.cy;
}

void  ImageElement::Draw(HDC hDC, Run* run, RECT* prcRun)
{
    if (m_image.IsNull())
        return;

    int x = prcRun->left;
    int y = prcRun->bottom - m_sizeDraw.cy;

    m_image.Draw(hDC,
        x, y, m_sizeDraw.cx, m_sizeDraw.cy,
        0, 0, 
        GetImageWidth(),
        GetImageHeight());
}

void ImageElement::LayoutMultiLine(MultiLineLayoutContext* context)
{
    SIZE sPage = { context->GetLineContentWidth(), context->nPageContentHeight };
    SIZE s = GetLayoutSize(
        sPage, 
        context->getLineRemainWidth()
        );
    context->BeginLayoutElement(this);
        context->PushChar(0, s.cx);
    context->EndLayoutElement(this);
}



SIZE  ImageElement::GetLayoutSize(SIZE pageContentSize, int lineRemain)
{
    int imageWidth = GetImageWidth();
    int imageHeight = GetImageHeight();
    m_sizeDraw.cx = imageWidth;
    m_sizeDraw.cy = imageHeight;

    // 本行足够显示
    if (m_sizeDraw.cx <= lineRemain)
        return m_sizeDraw;

    // 决定本行是否能够通过缩小图片来显示下，先定义一个图片允许的最小显示尺寸
#define MIN_IMAGE_DRAW_WIDTH 16

    int nMinImageWidth = min(MIN_IMAGE_DRAW_WIDTH, imageWidth);

    bool bNeedNewLine = false;
    if (lineRemain < nMinImageWidth)
    {
        bNeedNewLine = true;
    }

    int nRealSize = bNeedNewLine ? min(pageContentSize.cx, imageWidth) : lineRemain;
    if (nRealSize >= imageWidth)
        return m_sizeDraw;

    m_sizeDraw.cx = nRealSize;

    if (imageWidth != 0)
    {
        m_sizeDraw.cy = 
            (uint)((float)imageHeight / (float)imageWidth * (float)nRealSize);
    }

    return m_sizeDraw;
}
