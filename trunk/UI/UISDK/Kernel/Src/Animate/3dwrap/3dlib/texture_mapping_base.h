#pragma once
#include "UISDK\Kernel\Src\Atl\image.h"
#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\3dlib.h"

namespace UI
{

enum TRIANGLE_TYPE
{
	TRIANGLE_GENERAL,    
	TRIANGLE_FLAT_TOP,    // 平顶
	TRIANGLE_FLAT_BOTTOM, // 平底
};

class TextureMappingBase
{
public:
    TextureMappingBase()
    {
        m_pDestBuffer = NULL;
        m_pTexture = NULL;
        memset(&m_quad, 0, sizeof(Quad));
        memset(m_point, 0, sizeof(TexturePoint)*4);
    }
    virtual ~TextureMappingBase()
    {

    }

    virtual void Do()
	{

	}
    virtual void SetQuad(Quad* pQuad, RECT* prc) 
	{
		if (pQuad)
		{
			memcpy(&m_quad, pQuad, sizeof(Quad));
			m_point[0].xt = pQuad->Ax;
			m_point[0].yt = pQuad->Ay;
            m_point[0].z  = pQuad->Az;

			m_point[1].xt = pQuad->Bx;
			m_point[1].yt = pQuad->By;
            m_point[1].z  = pQuad->Bz;

			m_point[2].xt = pQuad->Cx;
			m_point[2].yt = pQuad->Cy;
            m_point[2].z  = pQuad->Cz;

			m_point[3].xt = pQuad->Dx;
			m_point[3].yt = pQuad->Dy;
            m_point[3].z  = pQuad->Dz;
		}
		if (prc)
			m_rect.CopyRect(prc);
	}
    
    void SetSrcBuffer(Image* pBuffer)
    {
        m_pTexture = pBuffer;
    }
    void SetDstBuffer(Image* pBuffer)
    {
        m_pDestBuffer = pBuffer;
    }
    void SetTexturePointA(int x, int y, int u, int v)
    {
        SetTexturePoint(0, x, y, u, v);
    }
    void SetTexturePointB(int x, int y, int u, int v)
    {
        SetTexturePoint(1, x, y, u, v);
    }
    void SetTexturePointC(int x, int y, int u, int v)
    {
        SetTexturePoint(2, x, y, u, v);
    }
    void SetTexturePointD(int x, int y, int u, int v)
    {
        SetTexturePoint(3, x, y, u, v);
    }

private:
    void SetTexturePoint(int nIndex, int x, int y, int u, int v)
    {
        if (nIndex < 0 || nIndex > 3)
            return;

        m_point[nIndex].x = x;
        m_point[nIndex].y = y;
        m_point[nIndex].u = u;
        m_point[nIndex].v = v;
    }

public:
    Image*  m_pDestBuffer;
    Image*  m_pTexture;

	CRect  m_rect;             // 控件的窗口坐标
    Quad   m_quad;             // 映射后的结果坐标
    TexturePoint  m_point[4];  // 源数据坐标及纹理坐标
};

}