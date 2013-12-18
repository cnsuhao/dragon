#include "stdafx.h"
#include "3dpipeline.h"

namespace UI
{

_3dPipeline::_3dPipeline()
{
    m_nRotateX = 0;
    m_nRotateY = 0;
    m_nRotateZ = 0;
    m_nOffsetX = 0;
    m_nOffsetY = 0;
    m_nOffsetZ = 0;
    m_ptOrigin.x = m_ptOrigin.y = 0;
    memset(&m_quad, 0, sizeof(Quad));
    memset(m_ptLocale, 0, sizeof(m_ptLocale));
}


void  _3dPipeline::update_locale_pos(RECT* prc)
{
    // ע������������ᣬy���Ǵ��µ��ϣ�����Ļ���෴
    // A
    m_ptLocale[0].x = (float)prc->left - m_ptOrigin.x;
    m_ptLocale[0].y = (float)m_ptOrigin.y - prc->top;
    m_ptLocale[0].z = 0.0f;
    // B
    m_ptLocale[1].x = (float)(prc->right - m_ptOrigin.x);
    m_ptLocale[1].y = m_ptLocale[0].y;
    m_ptLocale[1].z = 0.0f;
    // C
    m_ptLocale[2].x = m_ptLocale[1].x;
    m_ptLocale[2].y = (float)-(prc->bottom - m_ptOrigin.y);
    m_ptLocale[2].z = 0.0f;
    // D
    m_ptLocale[3].x = m_ptLocale[0].x;
    m_ptLocale[3].y = m_ptLocale[2].y;
    m_ptLocale[3].z = 0.0f;
}

void _3dPipeline::Do()
{
    POINT3D pt3DWorld[4] = {0};
    POINT3D pt3DCamera[4] = {0};

#pragma region // .�ֲ�����->��������
    {
        // .�Ե�ǰֵ��Ϊ�ǶȽ�����ת
        float fDegreeX = (float)m_nRotateX;
        float fDegreeY = (float)m_nRotateY;
        float fDegreeZ = (float)m_nRotateZ;

        MATRIX_4_4  matTemp1, matTemp2;
        MATRIX_4_4  matRotateY, matRotateX, matRotateZ;

        MATRIX_4_4_PTR pLeftArg = NULL;
        if (0 != fDegreeY)
        {
            // 			matRotateY = {
            // 				Fast_Cos(fDegreeY),  0, -Fast_Sin(fDegreeY), 0,
            // 				0, 1, 0, 0,
            // 				Fast_Sin(fDegreeY),  0,  Fast_Cos(fDegreeY), 0,
            // 				0, 0, 0, 1
            //			};
            MAT_IDENTITY_4_4(&matRotateY);
            matRotateY.M00 = Fast_Cos(fDegreeY);
            matRotateY.M02 = -Fast_Sin(fDegreeY);
            matRotateY.M20 = Fast_Sin(fDegreeY);
            matRotateY.M22 = Fast_Cos(fDegreeY);

            pLeftArg = &matRotateY;
        }

        if (0 != fDegreeX)
        {
            // 			MATRIX_4_4 matRotateX= {
            // 				1, 0, 0, 0,
            // 				0,  Fast_Cos(fDegreeX), Fast_Sin(fDegreeX), 0,
            // 				0, -Fast_Sin(fDegreeX), Fast_Cos(fDegreeX), 0,
            // 				0, 0, 0, 1
            // 			};

            MAT_IDENTITY_4_4(&matRotateX);
            matRotateX.M11 = Fast_Cos(fDegreeX);
            matRotateX.M12 = Fast_Sin(fDegreeX);
            matRotateX.M21 = -Fast_Sin(fDegreeX);
            matRotateX.M22 = Fast_Cos(fDegreeX);

            if (NULL == pLeftArg)
            {
                pLeftArg = &matRotateX;
            }
            else
            {
                Mat_Mul_4X4(pLeftArg, &matRotateX, &matTemp1);
                pLeftArg = &matTemp1;
            }
        }

        if (0 != fDegreeZ)
        {
            // 			MATRIX_4_4 matRotateZ= {
            //  			Fast_Cos(fDegreeZ), Fast_Sin(fDegreeZ), 0, 0
            // 				-Fast_Sin(fDegreeZ), Fast_Cos(fDegreeZ), 1, 0,
            // 				0, 0, 1, 0,
            // 				0, 0, 0, 1
            // 			};

            MAT_IDENTITY_4_4(&matRotateZ);
            matRotateZ.M00 = Fast_Cos(fDegreeZ);
            matRotateZ.M01 = Fast_Sin(fDegreeZ);
            matRotateZ.M10 = -Fast_Sin(fDegreeZ);
            matRotateZ.M11 = Fast_Cos(fDegreeZ);

            if (NULL == pLeftArg)
            {
                pLeftArg = &matRotateZ;
            }
            else
            {
                Mat_Mul_4X4(pLeftArg, &matRotateZ, &matTemp2);
                pLeftArg = &matTemp2;
            }
        }

        // ��ת
        if (pLeftArg)
        {
            for (int i = 0; i < 4; i++)
                Mat_Mul_VECTOR3D_4X4(&m_ptLocale[i], pLeftArg, &pt3DWorld[i]);
        }
        else
        {
            for (int i = 0; i < 4; i++)
            {
                pt3DWorld[i].x = m_ptLocale[i].x;
                pt3DWorld[i].y = m_ptLocale[i].y;
            }
        }

        // ƽ��
        for (int i = 0; i < 4; i++)
        {
            pt3DWorld[i].x += m_nOffsetX;
            pt3DWorld[i].y += m_nOffsetY;
            pt3DWorld[i].z += m_nOffsetZ;
        }
    }
#pragma endregion

    // ���λ��(����Ĭ�Ͻ���ƽ�����������������ͬһλ�ã�����͸�ӳ���������ֱ�ӾͿ���
    // ������Ļ���������ˣ�ʡ��һ��������
    // ������Ҫע����ǣ����nCameraPos��С�Ļ�������ͼ��ͻ���С��
    CAMERA camerpos = {0};
    float  fCameraPos = 2000.0f;  // ���λ��
    float  d = fCameraPos;      // �������ƽ��ľ��롣����ֵ���һ����������һ�ε���Ļ�����ת��

    VECTOR4D_INITXYZ(&camerpos.WorldPos, 0,0, -fCameraPos);

#pragma region // ��������ת��Ϊ�������
    {
        // ƽ�ƾ���
        MATRIX_4_4  matCameraTrans = {0};
        MAT_IDENTITY_4_4(&matCameraTrans);
        matCameraTrans.M30 = -camerpos.WorldPos.x;
        matCameraTrans.M31 = -camerpos.WorldPos.y;
        matCameraTrans.M32 = -camerpos.WorldPos.z;

        // ����Ƕ�Ϊ0������ת

        for (int i = 0; i < 4; i++)
        {
            Mat_Mul_VECTOR3D_4X4(&pt3DWorld[i], &matCameraTrans, &pt3DCamera[i]);
        }
    }
#pragma endregion

#pragma region // �������ת��Ϊ͸������
    POINT3D pt3DPerspectivePos[4];
    for (int i = 0; i < 4; i++)
    {
        float z = pt3DCamera[i].z;  // �����z�����ں�d��ȵľ��룬��������. ��dֵȡ�ıȽ�Сʱ���ᵼ��zΪ��
        float i_z = 0;
        if (0 != z)
            i_z = 1/z;

        if (pt3DCamera[i].z > 0)    // ����ƽ��֮�ڲ��ܲ������������ν��м���
        {
            pt3DPerspectivePos[i].x = d * pt3DCamera[i].x * i_z;  
            pt3DPerspectivePos[i].y = d * pt3DCamera[i].y * i_z; 
        }
    }
#pragma endregion

    // ת������Ļ����ϵ�ϣ���Ļ�����y���Ǵ������£������������෴��
    for (int i = 0; i < 4; ++i)
    {
        pt3DPerspectivePos[i].x += m_ptOrigin.x;
        pt3DPerspectivePos[i].y = -pt3DPerspectivePos[i].y;
        pt3DPerspectivePos[i].y += m_ptOrigin.y;
    }

    // ��ֵ������ֵ
    for (int i = 0; i < 4; i++)
    {
        m_quad.pos[2*i]   = (int)(pt3DPerspectivePos[i].x + 0.5);
        m_quad.pos[2*i+1] = (int)(pt3DPerspectivePos[i].y + 0.5);
    }
    m_quad.Az = pt3DCamera[0].z;
    m_quad.Bz = pt3DCamera[1].z;
    m_quad.Cz = pt3DCamera[2].z;
    m_quad.Dz = pt3DCamera[3].z;
}

}