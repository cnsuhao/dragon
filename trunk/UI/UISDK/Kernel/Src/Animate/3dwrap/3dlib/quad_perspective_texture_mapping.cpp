#include "stdafx.h"
#include "quad_perspective_texture_mapping.h"

namespace UI
{

void QuadPerspectiveTextureMapping::SetQuad(Quad* pQuad, RECT* prc) 
{
    if (!pQuad || !prc)
        return;
	__super::SetQuad(pQuad, prc);

    RECT rc = *prc;

    // 计算矩形->任意四边形的对应关系
    rc.right--;   // src bitmap如果是0-400,那么只能对应图片索引的0-399，400已经越界了。但是Quad不一样，它是用于计算的，完全可以包含最右值
    rc.bottom--;

    m_perspective.SetQuad(pQuad, &rc);
    //m_perspective.ChangeToFixedPoint();
}

#define GetPixelValue(pBits, nPitch, x, y) \
	(((DWORD*)(pBits + nPitch*(y)))[(x)])

void  QuadPerspectiveTextureMapping::Do()
{
    if (!m_pTexture || !m_pDestBuffer)
        return;

	BYTE* pbSrcBits = NULL;
	BYTE* pDstBits = (BYTE*)m_pDestBuffer->GetBits();
	int   nDstPitch = m_pDestBuffer->GetPitch();
    int   nWidth = m_pDestBuffer->GetWidth();
    int   nHeight = m_pDestBuffer->GetHeight();

    // 如果源图是400的宽度，则在计算插值时，则允许的范围是 (-0.9999 ~ 399.99999)
    CRect rcSrcRange;
    rcSrcRange.SetRect(m_rect.left-1, m_rect.top-1, m_rect.right, m_rect.bottom);

    int   nSrcPitch = m_pTexture->GetPitch();
    BYTE* pSrcBits = (BYTE*)m_pTexture->GetBits();

	float fxSrc = 0.0f;
	float fySrc = 0.0f;
	float *pfxSrc = &fxSrc;  // 为了在perspective_transform中使用指针
	float *pfySrc = &fySrc;

	Color Color0, Color1, Color2, Color3, ColorResult;  // 二次线性插值数据

    // 清空buffer2
    // m_bufferDst.Clear();

    // 	for (int X = 0, Y = 0; Y < nSrcBmpHeight; Y++)
    // 	{
    // 		for (X = 0; X < nSrcBmpWidth; X++)
    // 		{
    // 在大循环之前剔除掉一些空白区域
	int nMinX = max(0, min(min(min(m_quad.Ax,m_quad.Bx),m_quad.Cx),m_quad.Dx));
	int nMinY = max(0, min(min(min(m_quad.Ay,m_quad.By),m_quad.Cy),m_quad.Dy));
	int nMaxX = min(nWidth-1,  max(max(max(m_quad.Ax,m_quad.Bx),m_quad.Cx),m_quad.Dx));  // nWidth超出了索引
	int nMaxY = min(nHeight-1, max(max(max(m_quad.Ay,m_quad.By),m_quad.Cy),m_quad.Dy));

	pDstBits = (BYTE*)m_pDestBuffer->GetBits() + (nMinY*nDstPitch);
	for (int X = 0, Y = nMinY; Y <= nMaxY; Y++)  // quad的边界值也要计算
	{
		for (X = nMinX; X <= nMaxX; X++)
		{
			//m_perspective.transform(X, Y, &xSrc, &ySrc); // (TODO: 需要优化，该函数内部需要8次乘法，一次除法...了解下3D引擎是怎么实现的)
			perspective_transform/*_fp*/(m_perspective, X, Y, pfxSrc, pfySrc);  // <-- 窗口坐标

			int nxSrc = 0; //fx;   // 注: 直接赋值 int i = floatvalue; 会产生一个 _ftol()调用
			int nySrc = 0; //fy;   //     导致效率降低。这里使用内嵌汇编指令。(3D游戏编程大师技巧 P918)

            FLOAT_TO_INT(fxSrc, nxSrc);
            FLOAT_TO_INT(fySrc, nySrc);

			// ceil的效率非常非常低!千万别在这用
			// 但是有一个问题如果height为300，ySrc=299.99999999时，转成(int)得到的结果是300，
			// 但在这里判断的话是成功的，导致最后崩溃，因此当ySrc>299时即退出
			if (nxSrc < rcSrcRange.left ||    
                nxSrc >= rcSrcRange.right || 
                nySrc < rcSrcRange.top || 
                nySrc >= rcSrcRange.bottom)
			{
				continue;
			}
         
//             nxSrc ++; // 加上左边界
//             nySrc ++; // 加上上边界
//             fxSrc += 1.0f;
//             fySrc += 1.0f;

			float u = (float)fxSrc - nxSrc;  // 用于二次线性插值计算
			float v = (float)fySrc - nySrc;

			nxSrc -= rcSrcRange.left;   // 修改为相对于源图片的坐标 nxSrc = nxSrc - (rcSrcRange
			nySrc -= rcSrcRange.top;

			// 目标与源图像素对应
#if 0
			fxSrc -= rcSrcRange.left;
			fySrc -= rcSrcRange.top;
			//////////////////////////////////////////////////////////////////////////
			// 邻近取样, 速度快，但有锯齿
			((DWORD*)pDstBits)[X] = ((DWORD*)(pSrcBits + nSrcPitch*(int)fySrc))[(int)fxSrc];
#else
			//////////////////////////////////////////////////////////////////////////
			// 二次线性插值，能有效去掉锯齿，但速度非常慢
			//((DWORD*)pDstBits)[X] = Bilinear(m_pSrcBits, m_nSrcPitch, nxSrc, nySrc, (float)fxSrc, (float)fySrc);

			//
			// 二次线性插值公式推导
			// http://blog.csdn.net/dakistudio/article/details/1767100 二次线性插值公式推导
            // http://blog.csdn.net/housisong/article/details/1452249  优化方法
			//
			// x为fx的向下取整，y为fy的向下取整
			//
			//inline DWORD& Bilinear(BYTE* pBits, const int& nPitch, const int& x, const int& y, const float& fx, const float& fy)
			{
				// 已将原始图片的right/bottom扩大1px，这样在获取 x+1, y+1时达到数组边缘也不会崩溃
				// 取附近的四个像素的颜色值(x,y) (x+1, y) (x, y+1) (x+1, y+1)
				// PS: 这四个调用也非常耗CPU，但是不知道怎么优化。越优化CPU越高了

 			    pbSrcBits = pSrcBits + (nySrc*nSrcPitch + (nxSrc<<2));
 			    Color0.m_col = *((DWORD*)(pbSrcBits));
 			    Color2.m_col = (((DWORD*)(pbSrcBits))[1]);
 			    pbSrcBits += nSrcPitch;
				Color1.m_col = *((DWORD*)(pbSrcBits));
				Color3.m_col = (((DWORD*)(pbSrcBits))[1]);

//  			Color0.m_col = GetPixelValue(pSrcBits, nSrcPitch, nxSrc,  nySrc);
//  			Color2.m_col = GetPixelValue(pSrcBits, nSrcPitch, nxSrc+1,nySrc);
//  			Color1.m_col = GetPixelValue(pSrcBits, nSrcPitch, nxSrc,  nySrc+1);
//  			Color3.m_col = GetPixelValue(pSrcBits, nSrcPitch, nxSrc+1,nySrc+1);

				// 将浮点数乘法转为定点数乘法。同时优化浮点数转成整数。
				float fpm3 = FLOAT_TO_FIXP16(u*v);
				float fpm2 = FLOAT_TO_FIXP16(u*(1.0f-v));
				float fpm1 = FLOAT_TO_FIXP16(v*(1.0f-u));
				float fpm0 = FLOAT_TO_FIXP16((1.0f-u)*(1.0f-v));
				int pm3_16 = 0;
				int pm2_16 = 0;
				int pm1_16 = 0;
				int pm0_16 = 0;

                FLOAT_TO_INT(fpm3, pm3_16);
                FLOAT_TO_INT(fpm2, pm2_16);
                FLOAT_TO_INT(fpm1, pm1_16);
                FLOAT_TO_INT(fpm0, pm0_16);

				ColorResult.a = (byte)((pm0_16*Color0.a + pm1_16*Color1.a + pm2_16*Color2.a + pm3_16*Color3.a) >> FIXP16_SHIFT);
				ColorResult.r = (byte)((pm0_16*Color0.r + pm1_16*Color1.r + pm2_16*Color2.r + pm3_16*Color3.r) >> FIXP16_SHIFT);
				ColorResult.g = (byte)((pm0_16*Color0.g + pm1_16*Color1.g + pm2_16*Color2.g + pm3_16*Color3.g) >> FIXP16_SHIFT);
				ColorResult.b = (byte)((pm0_16*Color0.b + pm1_16*Color1.b + pm2_16*Color2.b + pm3_16*Color3.b) >> FIXP16_SHIFT);
				((DWORD*)pDstBits)[X] = ColorResult.m_col;

// 				float u = (float)fxSrc - nxSrc;
// 				float v = (float)fySrc - nySrc;
// 				float pm3 = u*v;
// 				float pm2 = u*(1.0f-v);
// 				float pm1 = v*(1.0f-u);
// 				float pm0 = (1.0f-u)*(1.0f-v);
// 
// 				ColorResult.a = (byte)(pm0*Color0.a + pm1*Color1.a + pm2*Color2.a + pm3*Color3.a);
// 				ColorResult.r = (byte)(pm0*Color0.r + pm1*Color1.r + pm2*Color2.r + pm3*Color3.r);
// 				ColorResult.g = (byte)(pm0*Color0.g + pm1*Color1.g + pm2*Color2.g + pm3*Color3.g);
// 				ColorResult.b = (byte)(pm0*Color0.b + pm1*Color1.b + pm2*Color2.b + pm3*Color3.b);
// 				((DWORD*)pDstBits)[X] = ColorResult.m_col;
			}
#endif
		}
		pDstBits += nDstPitch;
	}
}

}