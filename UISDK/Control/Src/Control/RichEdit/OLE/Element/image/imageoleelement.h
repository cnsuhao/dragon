#pragma once
#include "..\oleelement.h"
#include "UISDK\Kernel\Inc\Util\iimage.h"

namespace UI
{

class ImageOleElement : public OleElement
{
public:
    enum {TYPE = 140281055};

    ImageOleElement();
    ~ImageOleElement();

    static ImageOleElement* CreateInstance();

public:
    bool  LoadImage(LPCTSTR  szPath);
    void  LoadOuterImageRes(ImageWrap* p);

    ImageWrap*  GetImage() const;
    SIZE  GetImageSize() const;
    
public:
    virtual long  GetType() override;
    virtual void  Draw(HDC hDC, int xOffset, int yOffset) override;

private:
    void  UnloadImage();
    void  DrawImage(ImageWrap* pImage, HDC hDC, int xOffset, int yOffset);

private:
    ImageWrap*  m_pImage;  // 引用图片资源中的图片
    bool  m_bNeedDeleteImage;  // 图片是自己new出来的还是外部设进来的
};

}