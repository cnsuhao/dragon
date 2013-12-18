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

    // �������->�����ı��εĶ�Ӧ��ϵ
    rc.right--;   // src bitmap�����0-400,��ôֻ�ܶ�ӦͼƬ������0-399��400�Ѿ�Խ���ˡ�����Quad��һ�����������ڼ���ģ���ȫ���԰�������ֵ
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

    // ���Դͼ��400�Ŀ�ȣ����ڼ����ֵʱ��������ķ�Χ�� (-0.9999 ~ 399.99999)
    CRect rcSrcRange;
    rcSrcRange.SetRect(m_rect.left-1, m_rect.top-1, m_rect.right, m_rect.bottom);

    int   nSrcPitch = m_pTexture->GetPitch();
    BYTE* pSrcBits = (BYTE*)m_pTexture->GetBits();

	float fxSrc = 0.0f;
	float fySrc = 0.0f;
	float *pfxSrc = &fxSrc;  // Ϊ����perspective_transform��ʹ��ָ��
	float *pfySrc = &fySrc;

	Color Color0, Color1, Color2, Color3, ColorResult;  // �������Բ�ֵ����

    // ���buffer2
    // m_bufferDst.Clear();

    // 	for (int X = 0, Y = 0; Y < nSrcBmpHeight; Y++)
    // 	{
    // 		for (X = 0; X < nSrcBmpWidth; X++)
    // 		{
    // �ڴ�ѭ��֮ǰ�޳���һЩ�հ�����
	int nMinX = max(0, min(min(min(m_quad.Ax,m_quad.Bx),m_quad.Cx),m_quad.Dx));
	int nMinY = max(0, min(min(min(m_quad.Ay,m_quad.By),m_quad.Cy),m_quad.Dy));
	int nMaxX = min(nWidth-1,  max(max(max(m_quad.Ax,m_quad.Bx),m_quad.Cx),m_quad.Dx));  // nWidth����������
	int nMaxY = min(nHeight-1, max(max(max(m_quad.Ay,m_quad.By),m_quad.Cy),m_quad.Dy));

	pDstBits = (BYTE*)m_pDestBuffer->GetBits() + (nMinY*nDstPitch);
	for (int X = 0, Y = nMinY; Y <= nMaxY; Y++)  // quad�ı߽�ֵҲҪ����
	{
		for (X = nMinX; X <= nMaxX; X++)
		{
			//m_perspective.transform(X, Y, &xSrc, &ySrc); // (TODO: ��Ҫ�Ż����ú����ڲ���Ҫ8�γ˷���һ�γ���...�˽���3D��������ôʵ�ֵ�)
			perspective_transform/*_fp*/(m_perspective, X, Y, pfxSrc, pfySrc);  // <-- ��������

			int nxSrc = 0; //fx;   // ע: ֱ�Ӹ�ֵ int i = floatvalue; �����һ�� _ftol()����
			int nySrc = 0; //fy;   //     ����Ч�ʽ��͡�����ʹ����Ƕ���ָ�(3D��Ϸ��̴�ʦ���� P918)

            FLOAT_TO_INT(fxSrc, nxSrc);
            FLOAT_TO_INT(fySrc, nySrc);

			// ceil��Ч�ʷǳ��ǳ���!ǧ���������
			// ������һ���������heightΪ300��ySrc=299.99999999ʱ��ת��(int)�õ��Ľ����300��
			// ���������жϵĻ��ǳɹ��ģ���������������˵�ySrc>299ʱ���˳�
			if (nxSrc < rcSrcRange.left ||    
                nxSrc >= rcSrcRange.right || 
                nySrc < rcSrcRange.top || 
                nySrc >= rcSrcRange.bottom)
			{
				continue;
			}
         
//             nxSrc ++; // ������߽�
//             nySrc ++; // �����ϱ߽�
//             fxSrc += 1.0f;
//             fySrc += 1.0f;

			float u = (float)fxSrc - nxSrc;  // ���ڶ������Բ�ֵ����
			float v = (float)fySrc - nySrc;

			nxSrc -= rcSrcRange.left;   // �޸�Ϊ�����ԴͼƬ������ nxSrc = nxSrc - (rcSrcRange
			nySrc -= rcSrcRange.top;

			// Ŀ����Դͼ���ض�Ӧ
#if 0
			fxSrc -= rcSrcRange.left;
			fySrc -= rcSrcRange.top;
			//////////////////////////////////////////////////////////////////////////
			// �ڽ�ȡ��, �ٶȿ죬���о��
			((DWORD*)pDstBits)[X] = ((DWORD*)(pSrcBits + nSrcPitch*(int)fySrc))[(int)fxSrc];
#else
			//////////////////////////////////////////////////////////////////////////
			// �������Բ�ֵ������Чȥ����ݣ����ٶȷǳ���
			//((DWORD*)pDstBits)[X] = Bilinear(m_pSrcBits, m_nSrcPitch, nxSrc, nySrc, (float)fxSrc, (float)fySrc);

			//
			// �������Բ�ֵ��ʽ�Ƶ�
			// http://blog.csdn.net/dakistudio/article/details/1767100 �������Բ�ֵ��ʽ�Ƶ�
            // http://blog.csdn.net/housisong/article/details/1452249  �Ż�����
			//
			// xΪfx������ȡ����yΪfy������ȡ��
			//
			//inline DWORD& Bilinear(BYTE* pBits, const int& nPitch, const int& x, const int& y, const float& fx, const float& fy)
			{
				// �ѽ�ԭʼͼƬ��right/bottom����1px�������ڻ�ȡ x+1, y+1ʱ�ﵽ�����ԵҲ�������
				// ȡ�������ĸ����ص���ɫֵ(x,y) (x+1, y) (x, y+1) (x+1, y+1)
				// PS: ���ĸ�����Ҳ�ǳ���CPU�����ǲ�֪����ô�Ż���Խ�Ż�CPUԽ����

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

				// ���������˷�תΪ�������˷���ͬʱ�Ż�������ת��������
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