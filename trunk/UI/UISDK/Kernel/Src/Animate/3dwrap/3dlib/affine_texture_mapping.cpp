#include "stdafx.h"
#include "affine_texture_mapping.h"

namespace UI
{

AffineTextureMapping::AffineTextureMapping()
{
   
}

AffineTextureMapping::~AffineTextureMapping()
{

}

void  AffineTextureMapping::Do()
{
    // �ֽ�Ϊ����������: ��ABCD --> ��ABD + ��BCD
    DoTriangle(&m_point[0], &m_point[1], &m_point[3]);
    DoTriangle(&m_point[1], &m_point[2], &m_point[3]);
}

void  AffineTextureMapping::DoTriangle(TexturePoint* V0, TexturePoint* V1, TexturePoint* V2)
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
void  AffineTextureMapping::DoFlatTopTriangle(TexturePoint* A, TexturePoint* B, TexturePoint* C)
{
    MappingParam  param = {0};

    param.yStart = A->yt;
    param.yEnd = C->yt;

    param.xStart = (float)A->xt;
    param.xEnd = (float)B->xt;

    float fHeight = (float)(C->yt - A->yt);  // A-B�ȸ�
    float _fHeight = 1/fHeight;

    param.kLeft = (float)(C->xt - A->xt) * _fHeight; 
    param.kRight = (float)(C->xt - B->xt) * _fHeight;  

    param.uStart = (float)A->u;
    param.vStart = (float)A->v;
    param.uEnd = (float)B->u;
    param.vEnd = (float)B->v;

    param.kuLeft  = float(C->u - A->u) * _fHeight;
    param.kvLeft  = float(C->v - A->v) * _fHeight;
    param.kuRight = float(C->u - B->u) * _fHeight;
    param.kvRight = float(C->v - B->v) * _fHeight;

    _do_triangle(&param);
}

//
//   A
//
// B    C
//
void  AffineTextureMapping::DoFlatBottomTriangle(TexturePoint* A, TexturePoint* B, TexturePoint* C)
{
    MappingParam  param = {0};

    param.yStart = A->yt;
    param.yEnd = C->yt;

    param.xStart = (float)A->xt;
    param.xEnd = (float)A->xt;

    float fHeight = (float)(C->yt - A->yt);  // B-C�ȸ�
    float _fHeight = 1/fHeight;

    param.kLeft = (float)(B->xt - A->xt) * _fHeight; 
    param.kRight = (float)(C->xt - A->xt) * _fHeight;  

    param.uStart = (float)A->u;
    param.vStart = (float)A->v;
    param.uEnd = (float)A->u;
    param.vEnd = (float)A->v;

    param.kuLeft  = float(B->u - A->u) * _fHeight;
    param.kvLeft  = float(B->v - A->v) * _fHeight;
    param.kuRight = float(C->u - A->u) * _fHeight;
    param.kvRight = float(C->v - A->v) * _fHeight;

    _do_triangle(&param);
}

//
//   A                   A                     A             A
//  
// B-----     ������    -----B   ������   B--       ������      B---
//
//       C             C                   C                            C
//     
void  AffineTextureMapping::DoGeneralTriangle(TexturePoint* A, TexturePoint* B, TexturePoint* C)
{
    MappingParam  param = {0};

    param.yStart = 0;
    param.yEnd = 0;

    param.kLeft = 0.0f;  // ����ֱ�ߵ�б�ʵĵ���
    param.kRight = 0.0f;   

    param.xStart = (float)A->xt;  // ɨ����x���귶Χ
    param.xEnd = param.xStart;

    param.uStart = (float)A->u;   // ɨ��������Ӧ���������귶Χ
    param.vStart = (float)A->v;
    param.uEnd = (float)A->u;
    param.vEnd = (float)A->v;

    float fHeightAB = (float)(B->yt - A->yt);
    float fHeightAC = (float)(C->yt - A->yt);
    float fHeightBC = (float)(C->yt - B->yt);
    float _fHeightAB = 1/fHeightAB;
    float _fHeightAC = 1/fHeightAC;
    float _fHeightBC = 1/fHeightBC;

    float kAB = (float)(B->xt - A->xt) * _fHeightAB; 
    float kAC = (float)(C->xt - A->xt) * _fHeightAC;  // 2. �����A,ֵԽСԽ���������C��ֵԽ��Խ����
    float kBC = (float)(C->xt - B->xt) * _fHeightBC;  

    float kuAB = float(B->u - A->u) * _fHeightAB;
    float kvAB = float(B->v - A->v) * _fHeightAB;
    float kuAC = float(C->u - A->u) * _fHeightAC;
    float kvAC = float(C->v - A->v) * _fHeightAC;
    float kuBC = float(C->u - B->u) * _fHeightBC;
    float kvBC = float(C->v - B->v) * _fHeightBC;

    // ��Ϊ����������
    {
        param.yStart = A->yt;
        param.yEnd = B->yt;

        if (kAC > kAB)
        {
            param.kLeft = kAB;
            param.kRight = kAC;

            param.kuLeft  = kuAB;
            param.kvLeft  = kvAB;
            param.kuRight = kuAC;
            param.kvRight = kvAC;
        }
        else
        {
            param.kLeft = kAC;
            param.kRight = kAB;

            param.kuLeft  = kuAC;
            param.kvLeft  = kvAC;
            param.kuRight = kuAB;
            param.kvRight = kvAB;
        }

        _do_triangle(&param);
    }
    {
        param.yStart = param.yEnd;
        param.yEnd = C->yt;

        if (kAC > kBC)
        {
            param.kLeft = kAC;
            param.kRight = kBC;

            param.kuLeft  = kuAC;
            param.kvLeft  = kvAC;
            param.kuRight = kuBC;
            param.kvRight = kvBC;
        }
        else
        {
            param.kLeft = kBC;
            param.kRight = kAC;

            param.kuLeft  = kuBC;
            param.kvLeft  = kvBC;
            param.kuRight = kuAC;
            param.kvRight = kvAC;
        }
        
        _do_triangle(&param);
    }
}

#define GetPixelValue(pBits, nPitch, x, y) \
    (((DWORD*)(pBits + nPitch*(y)))[(x)])
#define GetPixelAddr(pBits, nPitch, x, y) \
    (((DWORD*)(pBits + nPitch*(y))) + x)

void  AffineTextureMapping::_do_triangle(MappingParam* pParam)
{
    byte* pSrcBit = (byte*)m_pTexture->GetBits();
    byte* pDstBit = (byte*)m_pDestBuffer->GetBits();

    int nSrcPitch = m_pTexture->GetPitch();
    int nDstPitch = m_pDestBuffer->GetPitch();

    for (int y = pParam->yStart; y < pParam->yEnd; y++)
    {
        float fWidth = pParam->xEnd - pParam->xStart;
        if (fWidth > EPSILON_E6 || fWidth < -EPSILON_E6)  // != 0
        {
            float _fWidth = 1/fWidth;

            // u, v �� x �����Թ�ϵ
            float kuScanline = (pParam->uEnd - pParam->uStart) * _fWidth;
            float kvScanline = (pParam->vEnd -pParam->vStart) * _fWidth; 

            float fu = pParam->uStart;
            float fv = pParam->vStart;

            int nStart = (int)(pParam->xStart+0.5);
            DWORD* pDestBuf = GetPixelAddr(pDstBit, nDstPitch, nStart, y);

            int nLength = (int)(fWidth+1);
            for (int x = 0 ; x < nLength; x++)
            {
                *pDestBuf = GetPixelValue(pSrcBit, nSrcPitch, (int)fu, (int)fv);

                fu += kuScanline;
                fv += kvScanline;
                pDestBuf++;
            }
        }
        pParam->xStart += pParam->kLeft;
        pParam->xEnd += pParam->kRight;
        pParam->uStart += pParam->kuLeft;
        pParam->vStart += pParam->kvLeft;
        pParam->uEnd += pParam->kuRight;
        pParam->vEnd += pParam->kvRight;
    }
}

}