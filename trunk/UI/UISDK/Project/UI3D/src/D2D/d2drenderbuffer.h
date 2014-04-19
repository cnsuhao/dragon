#pragma once

// Bitmaps can also be created from in-memory data that was set up through other means.
// After a bitmap has been created, it can be drawn by the render target DrawBitmap method
// or with a bitmap brush.
//
// Because creating bitmap resources on hardware render targets is often an expensive
// operation, Direct2D can update the contents of a bitmap (or portion of the bitmap)
// by using the CopyFromBitmap, CopyFromRenderTarget, and CopyFromMemory methods. Using 
// these methods can potentially save the costs associated with additional GPU texture
// allocations.
//
namespace UI
{
// ������IWICBitmapת����Share ID2D1Bitmapֻ��ʹ���������ģʽ
#if 0
class Direct2DRenderBuffer
{
public:
    Direct2DRenderBuffer();
    ~Direct2DRenderBuffer();

    void   Resize(unsigned int width, unsigned int height);
    void   Clear(DWORD dwColor, RECT* prc);
    void   Destroy();
    void   Dump();
    void   GetImageData(ImageData*  pData);

	bool   LockFull(IWICBitmapLock**);

public:
    IWICBitmap*   m_pWICBitmap;  // TODO: ʹ��IWICBitmap��Ϊ���棬����D2Dֻ��ʹ��software��ʧȥ��Ӳ�����ٵ����ԣ�������ôд��

    unsigned int  m_nWidth;
    unsigned int  m_nHeight;
};
#endif
}