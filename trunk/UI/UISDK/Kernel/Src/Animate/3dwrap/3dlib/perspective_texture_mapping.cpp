#include "stdafx.h"
#include "perspective_texture_mapping.h"

namespace UI
{

PerspectiveTextureMapping::PerspectiveTextureMapping()
{

}

PerspectiveTextureMapping::~PerspectiveTextureMapping()
{

}

void  PerspectiveTextureMapping::Do()
{
    // �ֽ�Ϊ����������: ��ABCD --> ��ABD + ��BCD
    DoTriangle(&m_point[0], &m_point[1], &m_point[3]);
    DoTriangle(&m_point[1], &m_point[2], &m_point[3]);
}

void  PerspectiveTextureMapping::DoTriangle(TexturePoint* V0, TexturePoint* V1, TexturePoint* V2)
{
    // ��������κϷ���
    if (V0->xt == V1->xt && V1->xt == V2->xt)
        return;
    if (V0->yt == V1->yt && V1->yt == V2->yt)
        return;

    // �����ζ�������
    TexturePoint* A = V0;
    TexturePoint* B = V1;
    TexturePoint* C = V2; 

#pragma region // �ж����������ͣ���yֵС������ǰ��

    TexturePoint* temp = NULL;

    if (B->yt < A->yt)
    {
        temp = B; B = A; A = temp;
    }
    if (C->yt < A->yt)
    {
        temp = C; C = A; A = temp;
    }
    if (C->yt < B->yt)
    {
        temp = C; C = B; B = temp;
    }


    TRIANGLE_TYPE  eTriangleType = TRIANGLE_GENERAL;
    if (A->yt == B->yt)
    {
        eTriangleType = TRIANGLE_FLAT_TOP;

        if (B->xt < A->xt)
        {
            temp = B; B = A; A = temp;
        }
    }
    else if (B->yt == C->yt)
    {
        eTriangleType = TRIANGLE_FLAT_BOTTOM;

        if (C->xt < B->xt)
        {
            temp = C; C = B; B = temp;
        }
    }

#pragma endregion
    switch (eTriangleType)
    {
    case TRIANGLE_FLAT_TOP:
        {
            DoFlatTopTriangle(A, B, C);
        }
        break;

    case TRIANGLE_FLAT_BOTTOM:
        {
            DoFlatBottomTriangle(A, B, C);
        }
        break;

    case TRIANGLE_GENERAL:
        {
            DoGeneralTriangle(A, B, C);
        }
        break;
    }

}

//
//  A     B
//
//    C
//
void  PerspectiveTextureMapping::DoFlatTopTriangle(TexturePoint* A, TexturePoint* B, TexturePoint* C)
{
    if (A->z <= 0 || B->z <= 0 || C->z <= 0) 
        return;

    MappingParam  param = {0};

    param.yStart = A->yt;
    param.yEnd = C->yt;

    param.xStart = A->xt << FIXP16_SHIFT;
    param.xEnd = B->xt << FIXP16_SHIFT;

    int  nAz = (int)(A->z + 0.5);
    int  nBz = (int)(B->z + 0.5);
    int  nCz = (int)(C->z + 0.5);
    int  Az = (1<<FIXP28_SHIFT) / nAz;
    int  Bz = (1<<FIXP28_SHIFT) / nBz;
    int  Cz = (1<<FIXP28_SHIFT) / nCz;

    int   Au = (A->u << FIXP20_SHIFT) / nAz; 
    int   Av = (A->v << FIXP20_SHIFT) / nAz; 
    int   Bu = (B->u << FIXP20_SHIFT) / nBz; 
    int   Bv = (B->v << FIXP20_SHIFT) / nBz; 
    int   Cu = (C->u << FIXP20_SHIFT) / nCz; 
    int   Cv = (C->v << FIXP20_SHIFT) / nCz; 

    // ����ͼƬ̫���������FIXP20_SHIFTֻ�ܱ�֤2048�Ĵ�С
    assert(Au > 0 && Av > 0 && Bu > 0 && Bv > 0 && Cu > 0 && Cv > 0);  

    int  nHeight = (C->yt - A->yt);  // A-B�ȸ�

    param.kxLeft  = ((C->xt - A->xt) << FIXP16_SHIFT) / nHeight; 
    param.kxRight = ((C->xt - B->xt) << FIXP16_SHIFT) / nHeight; 

    param.uStart = Au;
    param.vStart = Av;
    param.uEnd = Bu;
    param.vEnd = Bv;

    param.zStart = Az;  
    param.zEnd = Bz;

    param.kuLeft  = (Cu - Au) / nHeight;
    param.kvLeft  = (Cv - Av) / nHeight;
    param.kuRight = (Cu - Bu) / nHeight;
    param.kvRight = (Cv - Bv) / nHeight;

    param.kzLeft  = (Cz - Az) / nHeight;
    param.kzRight = (Cz - Bz) / nHeight;

    _do_triangle(&param);
}

//
//   A
//
// B    C
//
void  PerspectiveTextureMapping::DoFlatBottomTriangle(TexturePoint* A, TexturePoint* B, TexturePoint* C)
{
    if (A->z <= 0 || B->z <= 0 || C->z <= 0)
        return;

    MappingParam  param = {0};

    param.yStart = A->yt;
    param.yEnd = C->yt;

    param.xStart = A->xt << FIXP16_SHIFT;
    param.xEnd = param.xStart;

    int  nAz = (int)(A->z + 0.5);
    int  nBz = (int)(B->z + 0.5);
    int  nCz = (int)(C->z + 0.5);
    int  Az = (1<<FIXP28_SHIFT) / nAz;
    int  Bz = (1<<FIXP28_SHIFT) / nBz;
    int  Cz = (1<<FIXP28_SHIFT) / nCz;

    int  Au = (A->u << FIXP20_SHIFT) / nAz; 
    int  Av = (A->v << FIXP20_SHIFT) / nAz; 
    int  Bu = (B->u << FIXP20_SHIFT) / nBz; 
    int  Bv = (B->v << FIXP20_SHIFT) / nBz; 
    int  Cu = (C->u << FIXP20_SHIFT) / nCz; 
    int  Cv = (C->v << FIXP20_SHIFT) / nCz; 

    // ����ͼƬ̫���������FIXP20_SHIFTֻ�ܱ�֤2048�Ĵ�С
    assert(Au > 0 && Av > 0 && Bu > 0 && Bv > 0 && Cu > 0 && Cv > 0);  

    int  nHeight = (C->yt - A->yt);  // B-C�ȸ�

    param.kxLeft = ((B->xt - A->xt) << FIXP16_SHIFT) / nHeight;
    param.kxRight = ((C->xt - A->xt) << FIXP16_SHIFT) / nHeight;

    param.uStart = Au;
    param.vStart = Av;
    param.uEnd = Au;
    param.vEnd = Av;

    param.zStart = Az;
    param.zEnd = param.zStart;

    param.kuLeft  = (Bu - Au) / nHeight;
    param.kvLeft  = (Bv - Av) / nHeight;
    param.kuRight = (Cu - Au) / nHeight;
    param.kvRight = (Cv - Av) / nHeight;

    param.kzLeft  = (Bz - Az) / nHeight;
    param.kzRight = (Cz - Az) / nHeight;

    _do_triangle(&param);
}

//
//   A                   A                     A             A
//  
// B-----     ������    -----B   ������   B--       ������      B---
//
//       C             C                   C                            C
//     
void  PerspectiveTextureMapping::DoGeneralTriangle(TexturePoint* A, TexturePoint* B, TexturePoint* C)
{
    if (A->z <= 0 || B->z <= 0 || C->z <= 0)  // ����������ƣ���ֹ��������г���z=0�����³�0����
        return;

    MappingParam  param = {0};

    param.yStart = 0;
    param.yEnd = 0;

    param.kxLeft = 0;  // ����ֱ�ߵ�б�ʵĵ���
    param.kxRight = 0;   

    param.xStart = A->xt << FIXP16_SHIFT;  // ɨ����x���귶Χ
    param.xEnd = param.xStart;

    // a. 1/z �� x��y �ǳ����Թ�ϵ��
    // b. u/z �� x��y �ǳ����Թ�ϵ��

    int  nAz = (int)(A->z + 0.5);
    int  nBz = (int)(B->z + 0.5);
    int  nCz = (int)(C->z + 0.5);
    int  Az = (1<<FIXP28_SHIFT) / nAz;
    int  Bz = (1<<FIXP28_SHIFT) / nBz;
    int  Cz = (1<<FIXP28_SHIFT) / nCz;

    int   Au = (A->u << FIXP20_SHIFT) / nAz;   // ��u,vת��Ϊu/z,v/z�󣬲��ܲ������Բ�ֵ�ķ���
    int   Av = (A->v << FIXP20_SHIFT) / nAz;
    int   Bu = (B->u << FIXP20_SHIFT) / nBz;
    int   Bv = (B->v << FIXP20_SHIFT) / nBz;
    int   Cu = (C->u << FIXP20_SHIFT) / nCz;
    int   Cv = (C->v << FIXP20_SHIFT) / nCz;

    // ����ͼƬ̫���������FIXP20_SHIFTֻ�ܱ�֤2048�Ĵ�С
    assert(Au > 0 && Av > 0 && Bu > 0 && Bv > 0 && Cu > 0 && Cv > 0);  

    param.uStart = Au;   // ɨ��������Ӧ���������귶Χ
    param.vStart = Av;
    param.uEnd = param.uStart;
    param.vEnd = param.vStart;

    param.zStart = Az;  
    param.zEnd = param.zStart;

    int  nHeightAB = (B->yt - A->yt);
    int  nHeightAC = (C->yt - A->yt);
    int  nHeightBC = (C->yt - B->yt);

    int  kxAB = ((B->xt - A->xt) << FIXP16_SHIFT) / nHeightAB; 
    int  kxAC = ((C->xt - A->xt) << FIXP16_SHIFT) / nHeightAC;  // 2. �����A,ֵԽСԽ���������C��ֵԽ��Խ����
    int  kxBC = ((C->xt - B->xt) << FIXP16_SHIFT) / nHeightBC;  

    int   kuAB = (Bu - Au) / nHeightAB;   // ʹ��u/z��������u
    int   kvAB = (Bv - Av) / nHeightAB;
    int   kuAC = (Cu - Au) / nHeightAC;
    int   kvAC = (Cv - Av) / nHeightAC;
    int   kuBC = (Cu - Bu) / nHeightBC;
    int   kvBC = (Cv - Bv) / nHeightBC;

    int   kzAB = (Bz - Az) / nHeightAB;
    int   kzAC = (Cz - Az) / nHeightAC;
    int   kzBC = (Cz - Bz) / nHeightBC;

    // ��Ϊ����������
    {
        param.yStart = A->yt;
        param.yEnd = B->yt;

        if (kxAC > kxAB)
        {
            param.kxLeft = kxAB;
            param.kxRight = kxAC;

            param.kuLeft  = kuAB;
            param.kvLeft  = kvAB;
            param.kuRight = kuAC;
            param.kvRight = kvAC;

            param.kzLeft = kzAB;
            param.kzRight = kzAC;
        }
        else
        {
            param.kxLeft = kxAC;
            param.kxRight = kxAB;

            param.kuLeft  = kuAC;
            param.kvLeft  = kvAC;
            param.kuRight = kuAB;
            param.kvRight = kvAB;

            param.kzLeft = kzAC;
            param.kzRight = kzAB;
        }

        _do_triangle(&param);
    }
    //---------------------------------------------\\ 
    {
        param.yStart = param.yEnd;
        param.yEnd = C->yt;

        if (kxAC > kxBC)
        {
            param.kxLeft = kxAC;
            param.kxRight = kxBC;

            param.kuLeft  = kuAC;
            param.kvLeft  = kvAC;
            param.kuRight = kuBC;
            param.kvRight = kvBC;

            param.kzLeft = kzAC;
            param.kzRight = kzBC;
        }
        else
        {
            param.kxLeft = kxBC;
            param.kxRight = kxAC;

            param.kuLeft  = kuBC;
            param.kvLeft  = kvBC;
            param.kuRight = kuAC;
            param.kvRight = kvAC;

            param.kzLeft = kzBC;
            param.kzRight = kzAC;
        }

        _do_triangle(&param);
    }
}

#define GetPixelValue(pBits, nPitch, x, y) \
    (((DWORD*)(pBits + nPitch*(y)))[(x)])
#define GetPixelValue2(pBits, nPitchOffset, x, y) \
    (((DWORD*)(pBits + (y<<nPitchOffset)))[(x)])
#define GetPixelAddr(pBits, nPitch, x, y) \
    (((DWORD*)(pBits + nPitch*(y))) + x)

void  PerspectiveTextureMapping::_do_triangle(MappingParam* pParam)
{
	_do_triangle_subdivided_affine(pParam);
	return;

    byte* pSrcBit = (byte*)m_pTexture->GetBits();
    byte* pDstBit = (byte*)m_pDestBuffer->GetBits();
    
    int nSrcPitch = m_pTexture->GetPitch();
    int nDstPitch = m_pDestBuffer->GetPitch();

    int  nDstWidth = (m_pDestBuffer->GetWidth() - 1);
    int  nDstHeight = (m_pDestBuffer->GetHeight() - 1);

    // �涨������pitch������2��n�η����Լӿ춨λ -- ò��û��ʲô���ˣ��˷�Ҳ�ܿ�
    int nPitchOffset = 0;
    int nTemp = nSrcPitch;
    while (nTemp) { nTemp >>= 1; nPitchOffset++; }
    nPitchOffset--;
    UIASSERT( (1 << (nPitchOffset)) == nSrcPitch);

	// ��ʱ����
	int nx = 0;
	int ny = 0;

#if 0
    // ����һ����������������ַ
    int nHeight = m_pTexture->GetHeight();
    LPDWORD* pdwTextureYIndex = new LPDWORD[nHeight];
    {
        byte* pBits = (byte*)m_pTexture->GetBits(); 

        pdwTextureYIndex[0] = (LPDWORD)pBits;
        for (int i = 1; i < nHeight; i++)
        {
            pBits += nSrcPitch;
            pdwTextureYIndex[i] = (LPDWORD)pBits;
        }
    }
    //    SAFE_ARRAY_DELETE(pdwTextureYIndex);
#endif
    
    int yStart = max(0, pParam->yStart);
    int yEnd = min(nDstHeight, pParam->yEnd);
    int xStart = 0;
    int xEnd = 0;
    int xWidth = 0;
    for (int y = yStart; y < yEnd; y++)
    {
        xStart = (pParam->xStart + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
        xEnd = (pParam->xEnd + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
        xWidth = xEnd - xStart;

        int  kuScanline = 0;
        int  kvScanline = 0;
        int  kzScanline = 0;

        if (xWidth > 0)
        {
            // u,v,z�����x�����Թ�ϵ����ʵ��(u/z,v/z,1/z)
            kuScanline = (pParam->uEnd - pParam->uStart) / xWidth;
            kvScanline = (pParam->vEnd - pParam->vStart) / xWidth; 
            kzScanline = (pParam->zEnd - pParam->zStart) / xWidth;
        }
        else
        {
            kuScanline = (pParam->uEnd - pParam->uStart);
            kvScanline = (pParam->vEnd - pParam->vStart);
            kzScanline = (pParam->zEnd - pParam->zStart);
        }

        int   nu = pParam->uStart;  
        int   nv = pParam->vStart;
        int   nz = pParam->zStart;

        DWORD* pDestBuf = GetPixelAddr(pDstBit, nDstPitch, xStart, y);

        if (xStart < 0)
        {
            int d = -xStart;

            nu += d* kuScanline;  
            nv += d* kvScanline;
            nz += d* kzScanline;

            pDestBuf += d;
            xStart = 0;
        }

        if (xEnd > nDstWidth)
        {
            xEnd = nDstWidth;
        }

        for (int x = xStart; x <= xEnd; x++)
        {
			// [ע]�����γ�������Ч��̫����

			// v/zҪ��ԭΪv����Ҫ�ٳ˻�z. ������ nz = 1/z����� ny = nv/nz

#if 1
			__asm
			{
				// ny = (nv << FIXP8_SHIFT) / nz;
				mov    eax, nv
				shl    eax, 8 
				cdq         
				idiv   nz
				mov    edi, eax  ;  // ny <- edi

				// nx = (nu << FIXP8_SHIFT) / nz;
				mov    eax, nu     
				shl    eax, 8 
				cdq         
				idiv   nz 
				mov    edx, eax  ;  // nx <- edx 

				mov    ecx, nPitchOffset
				shl    edi, cl 
				add    edi, pSrcBit
				mov    eax, dword ptr [edi+edx*4] 
				mov    ecx, dword ptr [pDestBuf]
				mov    dword ptr[ecx], eax
			}

#else
			nx = (nu << FIXP8_SHIFT) / nz;
			ny = (nv << FIXP8_SHIFT) / nz;

//             if (nx < 0 || ny < 0/* || nx >= uvRange.right || ny >= uvRange.bottom*/)
//             {
// 
//             }
//             else
            {
                // *pDestBuf =  GetPixelValue(pSrcBit, nSrcPitch, nx, ny);
                // *pDestBuf =  *((pdwTextureYIndex[ny])+nx);
                *pDestBuf = GetPixelValue2(pSrcBit, nPitchOffset, nx, ny);
            }
#endif
            nu += kuScanline;
            nv += kvScanline;
            nz += kzScanline;

            pDestBuf++;
        }

        pParam->xStart += pParam->kxLeft;
        pParam->xEnd += pParam->kxRight;
        pParam->uStart += pParam->kuLeft;
        pParam->vStart += pParam->kvLeft;
        pParam->uEnd += pParam->kuRight;
        pParam->vEnd += pParam->kvRight;
        pParam->zStart += pParam->kzLeft;
        pParam->zEnd += pParam->kzRight;
    }
}

// ֱ��ʹ�ó���Ч��̫�ͣ�����������Էֶη�ʽ��
// ���Էֶ�����ӳ��ÿ��һ���������16-32���أ��������ص�׼ȷ
// �������꣬Ȼ������Щ����֮����з�����������ӳ�䡣
// ͨ�����ַ�ʽ���ɱ���ÿ�����ض�ִ�г������㣬����ÿ��n������
// ִ����Щ�������У�Ȼ��ͨ�����Բ�ֵ������Щ����֮����������ꡣ
void  PerspectiveTextureMapping::_do_triangle_subdivided_affine(MappingParam* pParam)
{
	byte* pSrcBit = (byte*)m_pTexture->GetBits();
	byte* pDstBit = (byte*)m_pDestBuffer->GetBits();

	int nSrcPitch = m_pTexture->GetPitch();
	int nDstPitch = m_pDestBuffer->GetPitch();

	int  nDstWidth = (m_pDestBuffer->GetWidth() - 1);
	int  nDstHeight = (m_pDestBuffer->GetHeight() - 1);

	// �涨������pitch������2��n�η����Լӿ춨λ -- ò��û��ʲô���ˣ��˷�Ҳ�ܿ�
	int nPitchOffset = 0;
	int nTemp = nSrcPitch;
	while (nTemp) { nTemp >>= 1; nPitchOffset++; }
	nPitchOffset--;
	UIASSERT( (1 << (nPitchOffset)) == nSrcPitch);

	// ��ʱ����
	int nx = 0;
	int ny = 0;

	int yStart = max(0, pParam->yStart);
	int yEnd = min(nDstHeight, pParam->yEnd);
	int xStart = 0;
	int xEnd = 0;
	int xWidth = 0;
	int uStart = 0;
	int uEnd = 0;
	int vStart = 0;
	int vEnd = 0;

	for (int y = yStart; y < yEnd; y++)
	{
		xStart = (pParam->xStart + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
		xEnd = (pParam->xEnd + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
		xWidth = xEnd - xStart;

		uStart = ((pParam->uStart << FIXP8_SHIFT) / (pParam->zStart >> FIXP8_SHIFT)) << 12;  // uStart����FIXP20_SHIFT����˻���Ҫ<<12: (20+8 - (28-8))+12 = 20
		vStart = ((pParam->vStart << FIXP8_SHIFT) / (pParam->zStart >> FIXP8_SHIFT)) << 12;
		uEnd   = ((pParam->uEnd << FIXP8_SHIFT) / (pParam->zEnd >> FIXP8_SHIFT)) << 12;
		vEnd   = ((pParam->vEnd << FIXP8_SHIFT) / (pParam->zEnd >> FIXP8_SHIFT)) << 12;
		
		int  kuScanline = 0;
		int  kvScanline = 0;

		if (xWidth > 0)
		{
			// u,v,z�����x�����Թ�ϵ����ʵ��(u/z,v/z,1/z)
			kuScanline = (uEnd - uStart) / xWidth;
			kvScanline = (vEnd - vStart) / xWidth; 
		}
		else
		{
			kuScanline = (uEnd - uStart);
			kvScanline = (vEnd - vStart);
		}

		int  nu = uStart;  
		int  nv = vStart;

		DWORD* pDestBuf = GetPixelAddr(pDstBit, nDstPitch, xStart, y);

		if (xStart < 0)
		{
			int d = -xStart;

			nu += d* kuScanline;  
			nv += d* kvScanline;

			pDestBuf += d;
			xStart = 0;
		}

		if (xEnd > nDstWidth)
		{
			xEnd = nDstWidth;
		}

		for (int x = xStart; x <= xEnd; x++)
		{
    	    *pDestBuf = GetPixelValue2(pSrcBit, nPitchOffset, (nu>>FIXP20_SHIFT), (nv>>FIXP20_SHIFT));

			nu += kuScanline;
			nv += kvScanline;
			pDestBuf++;
		}

		pParam->xStart += pParam->kxLeft;
		pParam->xEnd += pParam->kxRight;
		pParam->uStart += pParam->kuLeft;
		pParam->vStart += pParam->kvLeft;
		pParam->uEnd += pParam->kuRight;
		pParam->vEnd += pParam->kvRight;
		pParam->zStart += pParam->kzLeft;
		pParam->zEnd += pParam->kzRight;
	}
}
}