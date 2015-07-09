#pragma once
#include "..\oleelement.h"
#include "..\..\reoleapi.h"
#include "UISDK\Kernel\Inc\Util\iimage.h"

namespace UI
{

class GifOleElement : 
		public OleElement,
		public IREOleTimerCallback
{
public:
    enum {TYPE = 141381050};

    GifOleElement();
    ~GifOleElement();

    static GifOleElement* CreateInstance();

public:
    bool  LoadGif(LPCTSTR  szPath);
    void  LoadOuterGifRes(Gdiplus::Bitmap* p);
    SIZE  GetGifSize();

public:
    virtual long  GetType() override;
    virtual void  Draw(HDC hDC, int xOffset, int yOffset) override;

	virtual void  OnREOleTimer() override;

private:
    void  UnloadImage();
    void  InitGif();
    void  NextFrame();
	UINT  GetCurrentFrameDelay();

private:
    Gdiplus::Bitmap*  m_pImage;  // ����ͼƬ��Դ�е�ͼƬ
    bool  m_bNeedDeleteImage;  // ͼƬ���Լ�new�����Ļ����ⲿ�������

    // gif����
    UINT  m_nFrameCount;
    UINT  m_nCurrentFrame;
	UINT  m_nPrevFrameTick;

    vector<UINT>  m_vecFrameDelay;
};

}