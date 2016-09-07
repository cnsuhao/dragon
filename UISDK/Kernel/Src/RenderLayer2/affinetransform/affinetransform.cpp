#include "stdafx.h"
#include "affinetransform.h"
#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\3dlib.h"

using namespace UI;


AffineTransform::AffineTransform()
{
	Init_Sin_Cos_Tables();

	identity();
}

void AffineTransform::identity()
{
	m_bInvalidate = true;

	m_eOriginTypeX = TRANSFORM_ROTATE_ORIGIN_CENTER;
	m_eOriginTypeY = TRANSFORM_ROTATE_ORIGIN_CENTER;
	m_fOriginOffsetX = 0;
	m_fOriginOffsetY = 0;
	m_fOriginOffsetZ = 0;
	m_fWidth = 0;
	m_fHeight = 0;

	m_xPos = 0;
	m_yPos = 0;
	m_rotate = 0;
	m_xScale = 0;
	m_yScale = 0;

	m_matrix.Identity();
	m_matrixInverse.Identity();
}

AffineTransform::~AffineTransform()
{

}

AffineTransform*  AffineTransform::CreateInstance()
{
	return new AffineTransform();
}
void  AffineTransform::Release()
{
	delete this;
}
TRANSFORM_TYPE  AffineTransform::get_type()
{
	return TRANSFORM_2D;
}

void  AffineTransform::set_transform_rotate_origin(
		TRANSFORM_ROTATE_ORIGIN eX, float fX,
		TRANSFORM_ROTATE_ORIGIN eY, float fY,
		float fZ)
{
	m_eOriginTypeX = eX;
	m_eOriginTypeY = eY;
	m_fOriginOffsetX = fX;
	m_fOriginOffsetY = fY;
	m_fOriginOffsetZ = fZ;
}

void  AffineTransform::set_pos(int x, int y)
{

}

void  AffineTransform::set_size(int w, int h)
{
	m_fWidth = (float)w;
	m_fHeight = (float)h;

	Invalidate();
}

void  AffineTransform::get_matrix(LPMATRIX33 pMatrix)
{
	if (!pMatrix)
		return;
	
	if (m_bInvalidate)
		Update();

	m_matrix.CopyTo(pMatrix);
}

void  AffineTransform::mappoint_layer_2_view(__inout POINT* ptInLayer)
{
	m_matrix.MapPoint(ptInLayer, ptInLayer);
}

void  AffineTransform::mappoint_view_2_layer(__inout POINT* ptInLayer)
{
	m_matrixInverse.MapPoint(ptInLayer, ptInLayer);
	//m_matrix.UnMapPoint(ptInLayer, ptInLayer);
}

void  AffineTransform::maprect_layer_2_view(__in RECT* rcInLayer, __out QUAD* pqInView)
{
	POINT& ptA = pqInView->ptA;
	POINT& ptB = pqInView->ptB;
	POINT& ptC = pqInView->ptC;
	POINT& ptD = pqInView->ptD;

	ptA.x = rcInLayer->left;
	ptA.y = rcInLayer->top;
	ptB.x = rcInLayer->right;
	ptB.y = rcInLayer->top;
	ptC.x = rcInLayer->right;
	ptC.y = rcInLayer->bottom;
	ptD.x = rcInLayer->left;
	ptD.y = rcInLayer->bottom;

	mappoint_layer_2_view(&ptA);
	mappoint_layer_2_view(&ptB);
	mappoint_layer_2_view(&ptC);
	mappoint_layer_2_view(&ptD);
}

void  AffineTransform::Invalidate()
{
	m_bInvalidate = true;
}

void  AffineTransform::Update()
{
	if (!m_bInvalidate)
		return;

	m_bInvalidate = false;
	m_matrix.Identity();
	
	// Ðý×ª
	float fx = 0;
	float fy = 0;
	GetRotateOrigin(&fx, &fy);
	m_matrix.Translate(fx, fy);
	m_matrix.Rotate(m_rotate);
	m_matrix.Translate(-fx, -fy);

	// Æ«ÒÆ
	m_matrix.Translate(m_xPos, m_yPos);

	m_matrix.Inverse(&m_matrixInverse);
}

void  AffineTransform::GetRotateOrigin(float* pfx, float* pfy)
{
	if (pfx)
	{
		switch (m_eOriginTypeX)
		{
		case TRANSFORM_ROTATE_ORIGIN_ABSOLUTE:
		case TRANSFORM_ROTATE_ORIGIN_LEFT:
			*pfx = m_fOriginOffsetX;
			break;

		case TRANSFORM_ROTATE_ORIGIN_RIGHT:
			*pfx = m_fWidth - m_fOriginOffsetX;
			break;

		case TRANSFORM_ROTATE_ORIGIN_CENTER:
			*pfx = m_fWidth/2 + m_fOriginOffsetX;
			break;
		}
	}
	if (pfy)
	{
		switch (m_eOriginTypeY)
		{
		case TRANSFORM_ROTATE_ORIGIN_ABSOLUTE:
		case TRANSFORM_ROTATE_ORIGIN_TOP:
			*pfy = m_fOriginOffsetY;
			break;

		case TRANSFORM_ROTATE_ORIGIN_BOTTOM:
			*pfy = m_fHeight - m_fOriginOffsetY;
			break;

		case TRANSFORM_ROTATE_ORIGIN_CENTER:
			*pfy = m_fHeight/2 + m_fOriginOffsetY;
			break;
		}
	}
}

void  AffineTransform::rotate(float rotation)
{
	m_rotate = rotation;

	Invalidate();
}
void  AffineTransform::rotateby(float rotation)
{
	m_rotate += rotation;

	Invalidate();
}

void  AffineTransform::translate(float xPos, float yPos)
{
	m_xPos = xPos;
	m_yPos = yPos;

	Invalidate();
}
void  AffineTransform::translateby(float xPos, float yPos)
{
	m_xPos += xPos;
	m_yPos += yPos;

	Invalidate();
}
void  AffineTransform::scale(float xScale, float yScale)
{
	m_xScale = xScale;
	m_yScale = yScale;

	Invalidate();
}
void  AffineTransform::scaleby(float xScale, float yScale)
{
	m_xScale += xScale;
	m_yScale += yScale;

	Invalidate();
}