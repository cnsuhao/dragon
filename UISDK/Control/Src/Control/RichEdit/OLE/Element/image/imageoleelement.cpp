#include "stdafx.h"
#include "ImageOleElement.h"

using namespace UI;

ImageOleElement::ImageOleElement()
{
    m_pImage = NULL;
    m_bNeedDeleteImage = false;
    m_style.bEnable = false; // 不接收鼠标消息
}
ImageOleElement::~ImageOleElement()
{
    UnloadImage();
}
void  ImageOleElement::UnloadImage()
{
    if (m_bNeedDeleteImage && m_pImage)
    {
        SAFE_DELETE(m_pImage);
        m_bNeedDeleteImage = false;
        return;
    }

    m_pImage = NULL;
}

ImageOleElement*  ImageOleElement::CreateInstance()
{
    return new ImageOleElement;
}

long  ImageOleElement::GetType() 
{
    return ImageOleElement::TYPE;
}

void  ImageOleElement::Draw(HDC hDC, int xOffset, int yOffset)
{
    DrawImage(m_pImage, hDC, xOffset, yOffset);
}

void  ImageOleElement::DrawImage(ImageWrap* pImage, HDC hDC, int xOffset, int yOffset)
{
    if (!pImage || pImage->IsNull())
        return;

    pImage->Draw(hDC, 
        m_rcInOle.left + xOffset,
        m_rcInOle.top + yOffset, 
        m_rcInOle.Width(),
        m_rcInOle.Height(),
        0, 0, 
        pImage->GetWidth(),
        pImage->GetHeight());
}

ImageWrap*  ImageOleElement::GetImage() const
{
    return m_pImage;
}

SIZE  ImageOleElement::GetImageSize() const
{
    SIZE s = {0};
    if (!m_pImage || m_pImage->IsNull())
    {
        return s;
    }

    s.cx = m_pImage->GetWidth();
    s.cy = m_pImage->GetHeight();
    return s;
}

bool  ImageOleElement::LoadImage(LPCTSTR  szPath)
{
    UnloadImage();

    if (!szPath)
        return false;

    m_pImage = new ImageWrap;
    m_bNeedDeleteImage = true;

    if (szPath)
    {
        m_pImage->Load(szPath, true);
    }
    if (m_pImage->IsNull())
    {
        UnloadImage();
    }

    return true;
}

void  ImageOleElement::LoadOuterImageRes(ImageWrap* p)
{
    UnloadImage();
    m_pImage = p;
    m_bNeedDeleteImage = false;
}