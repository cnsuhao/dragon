#ifndef CACHEBITMAP_H_3DABC025_800B_46ed_AC6F_0D3854DD6257
#define CACHEBITMAP_H_3DABC025_800B_46ed_AC6F_0D3854DD6257

#include "UISDK\Kernel\Src\Atl\image.h"
namespace UI
{
// 2013.7.30
// 用于解决一些特效计算过程中需要临时的内存图片，避免不断的创建内存位图
// 在这里使用全局缓存图片。
class CacheBitmap
{
public:
    CacheBitmap();
    ~CacheBitmap();

    static CacheBitmap*  GetInstance()
    {
        static CacheBitmap s;
        return &s;
    }

    HBITMAP  Create(int nWidth, int nHeight);
    Image*  CreateEx(int nWidth, int nHeight);

    void  Destroy();
    void  Clear(DWORD dwColor, RECT* prc);
   // void  Transparent(DWORD dwTransparent, RECT* prc);

protected:
    void  AdjustWH(int nWidth, int nHeight, int& nLastWidth, int& nLastHeight);

    int  CalcMin2(int n);

public:
    Image  m_buffer;
};
}

#endif // CACHEBITMAP_H_3DABC025_800B_46ed_AC6F_0D3854DD6257