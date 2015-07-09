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
    ImageWrap*  m_pImage;  // ����ͼƬ��Դ�е�ͼƬ
    bool  m_bNeedDeleteImage;  // ͼƬ���Լ�new�����Ļ����ⲿ�������
};

}