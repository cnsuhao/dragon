#pragma once
#include "UISDK\Kernel\Inc\Interface\irenderlayer.h"

namespace UI
{

class AffineTransform : public IRenderLayerTransform2D
{
public:
	AffineTransform();
	~AffineTransform();

public:
	static AffineTransform*  CreateInstance();

	virtual void  Release() override; 
	virtual TRANSFORM_TYPE  get_type() override;
	virtual void  get_matrix(LPMATRIX33 pMatrix) override;

	virtual void  set_transform_rotate_origin(
			TRANSFORM_ROTATE_ORIGIN eX, float fX,
			TRANSFORM_ROTATE_ORIGIN eY, float fY,
			float fZ) override;
	virtual void  set_pos(int x, int y) override;
	virtual void  set_size(int w, int h) override;

	virtual void  mappoint_layer_2_view(__inout POINT* ptInLayer) override;
	virtual void  mappoint_view_2_layer(__inout POINT* ptInLayer) override;
	virtual void  maprect_layer_2_view(__in RECT* rcInLayer, __out QUAD* pqInView) override;

public:
	void  identity();
	void  translate(float xPos, float yPos);
	void  translateby(float xPos, float yPos);
	void  rotate(float rotation);
	void  rotateby(float rotation);
	void  scale(float xScale, float yScale);
	void  scaleby(float xScale, float yScale);

protected:
	void  Invalidate();
	void  Update();
	void  GetRotateOrigin(float* pfx, float* pfy);

private:
	Matrix33  m_matrix;
	Matrix33  m_matrixInverse;
	bool   m_bInvalidate;

	// 旋转中心相关数据
	TRANSFORM_ROTATE_ORIGIN  m_eOriginTypeX;
	TRANSFORM_ROTATE_ORIGIN  m_eOriginTypeY;
	float  m_fOriginOffsetX;
	float  m_fOriginOffsetY;
	float  m_fOriginOffsetZ;
	float  m_fWidth;
	float  m_fHeight;

	float  m_xPos;
	float  m_yPos;
	float  m_rotate;
	float  m_xScale;
	float  m_yScale;
};

}