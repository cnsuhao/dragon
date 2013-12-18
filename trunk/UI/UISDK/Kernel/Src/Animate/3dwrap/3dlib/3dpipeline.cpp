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
    // 注：这里的坐标轴，y轴是从下到上，与屏幕的相反
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

#pragma region // .局部坐标->世界坐标
    {
        // .以当前值作为角度进行旋转
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

        // 旋转
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

        // 平移
        for (int i = 0; i < 4; i++)
        {
            pt3DWorld[i].x += m_nOffsetX;
            pt3DWorld[i].y += m_nOffsetY;
            pt3DWorld[i].z += m_nOffsetZ;
        }
    }
#pragma endregion

    // 相机位置(这里默认将视平面放在与矩形所在面的同一位置，这样透视出来的坐标直接就可以
    // 当成屏幕坐标来用了，省了一步操作。
    // 但是需要注意的是，如果nCameraPos过小的话看到的图像就会缩小，
    CAMERA camerpos = {0};
    float  fCameraPos = 2000.0f;  // 相机位置
    float  d = fCameraPos;      // 相机与视平面的距离。将两值设成一样，避免了一次到屏幕坐标的转换

    VECTOR4D_INITXYZ(&camerpos.WorldPos, 0,0, -fCameraPos);

#pragma region // 世界坐标转换为相机坐标
    {
        // 平移矩阵
        MATRIX_4_4  matCameraTrans = {0};
        MAT_IDENTITY_4_4(&matCameraTrans);
        matCameraTrans.M30 = -camerpos.WorldPos.x;
        matCameraTrans.M31 = -camerpos.WorldPos.y;
        matCameraTrans.M32 = -camerpos.WorldPos.z;

        // 相机角度为0，不旋转

        for (int i = 0; i < 4; i++)
        {
            Mat_Mul_VECTOR3D_4X4(&pt3DWorld[i], &matCameraTrans, &pt3DCamera[i]);
        }
    }
#pragma endregion

#pragma region // 相机坐标转换为透视坐标
    POINT3D pt3DPerspectivePos[4];
    for (int i = 0; i < 4; i++)
    {
        float z = pt3DCamera[i].z;  // 这里的z是用于和d相比的距离，不是坐标. 当d值取的比较小时，会导致z为负
        float i_z = 0;
        if (0 != z)
            i_z = 1/z;

        if (pt3DCamera[i].z > 0)    // 在视平面之内才能采用相似三角形进行计算
        {
            pt3DPerspectivePos[i].x = d * pt3DCamera[i].x * i_z;  
            pt3DPerspectivePos[i].y = d * pt3DCamera[i].y * i_z; 
        }
    }
#pragma endregion

    // 转换到屏幕坐标系上（屏幕坐标的y轴是从上自下，与世界坐标相反）
    for (int i = 0; i < 4; ++i)
    {
        pt3DPerspectivePos[i].x += m_ptOrigin.x;
        pt3DPerspectivePos[i].y = -pt3DPerspectivePos[i].y;
        pt3DPerspectivePos[i].y += m_ptOrigin.y;
    }

    // 赋值给返回值
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