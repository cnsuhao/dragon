#ifndef IRENDERLAYER_H_8500D1A2_903C_4c76_B3B0_5248E65E95C3
#define IRENDERLAYER_H_8500D1A2_903C_4c76_B3B0_5248E65E95C3
#include "UISDK\Kernel\Inc\Util\math\matrix.h"
#include "UISDK\Kernel\Inc\Base\xmldefine.h"

namespace UI
{
interface IRenderTarget;
class RenderLayer;
class WindowRender;


enum TRANSFORM_TYPE
{
	TRANSFORM_2D,
	TRANSFORM_SOFT3D,
	TRANSFORM_HARD3D,
};

// 旋转中心位置
// 默认值：(CENTER, CENTER, 0)
enum TRANSFORM_ROTATE_ORIGIN
{
	TRANSFORM_ROTATE_ORIGIN_ABSOLUTE = 0,
    TRANSFORM_ROTATE_ORIGIN_TOP      = 1,
	TRANSFORM_ROTATE_ORIGIN_LEFT     = 1,
    TRANSFORM_ROTATE_ORIGIN_CENTER   = 2,
	TRANSFORM_ROTATE_ORIGIN_RIGHT    = 3,
	TRANSFORM_ROTATE_ORIGIN_BOTTOM   = 3,
	TRANSFORM_ROTATE_ORIGIN_PERCENT  = 4,
};
typedef TRANSFORM_ROTATE_ORIGIN ROTATE_CENTER_TYPE;

interface IRenderLayerTransform2
{
	virtual void  Release() PURE;
	virtual TRANSFORM_TYPE  get_type() PURE;

	// anchorPoint
	virtual void  set_transform_rotate_origin(
			TRANSFORM_ROTATE_ORIGIN eX, float fX,
			TRANSFORM_ROTATE_ORIGIN eY, float fY,
			float fZ) PURE;
	virtual void  set_size(int w, int h) PURE;
    virtual void  set_pos(int x, int y) PURE;

	virtual void  mappoint_layer_2_view(__inout POINT* ptInLayer) PURE;
	virtual void  mappoint_view_2_layer(__inout POINT* ptInLayer) PURE;
	virtual void  maprect_layer_2_view(
			__in RECT* rcInLayer, __out QUAD* pqInView) PURE;
};

interface IRenderLayerTransform2D : public IRenderLayerTransform2
{
	virtual void  get_matrix(LPMATRIX33 pMatrix) PURE;

	virtual void  translate(float x, float y) PURE;
	virtual void  scale(float x, float y) PURE;
	virtual void  rotate(float angle) PURE;
};

interface IRenderLayerTransform3D : public IRenderLayerTransform2
{
    virtual void  set_backface_visibility(bool b) PURE;
    virtual bool  get_backface_visibility() PURE;
    virtual bool  is_visible() PURE;

	virtual void  get_matrix(MATRIX44* pMatrix) PURE;

	// Defines a 3D translation
	virtual void  translate3d(float x, float y, float z) PURE;

	// Defines a 3D translation, using only the value for the X-axis
	virtual void  translateX(float x) PURE;

	// Defines a 3D translation, using only the value for the Y-axis
	virtual void  translateY(float y) PURE;

	// Defines a 3D translation, using only the value for the Z-axis
	virtual void  translateZ(float z) PURE;

	// Defines a 3D scale transformation
	virtual void  scale3d(float x, float y, float z) PURE;

	// Defines a 3D scale transformation by giving a value for the X-axis
	virtual void  scaleX(float x) PURE;

	// Defines a 3D scale transformation by giving a value for the Y-axis
	virtual void  scaleY(float y) PURE;

	// Defines a 3D scale transformation by giving a value for the Z-axis
	virtual void  scaleZ(float z) PURE;

	// Defines a 3D rotation
	virtual void  rotate3d(float x, float y, float z) PURE;

	// Defines a 3D rotation along the X-axis
	virtual void  rotateX(float angle) PURE;

	// Defines a 3D rotation along the Y-axis
	virtual void  rotateY(float angle) PURE;

	// Defines a 3D rotation along the Z-axis
	virtual void  rotateZ(float angle) PURE;

	// Defines a perspective view for a 3D transformed element
	virtual void  perspective(float n) PURE;

};

interface UISDKAPI IRenderLayer
{
public:
	IRenderLayer(RenderLayer* p);
	~IRenderLayer();

	RenderLayer*  GetImpl();
    void  SetAlwaysUpdate(bool b);
	void  SetOpacity(byte b);
	byte  GetOpacity();
    void  DirectCommit(bool bUpdate);
    IRenderTarget*  GetRenderTarget();
    
	void  CreateTransform(TRANSFORM_TYPE eType);
	IRenderLayerTransform2D*  GetTransform2D(bool bForceCreate);
	IRenderLayerTransform3D*  GetTransform3D(bool bForceCreate);

public:
	RenderLayer*  m_pRenderLayer2Impl;
};

interface UISDKAPI IWindowRender
{
public:
    IWindowRender(WindowRender* p);
    ~IWindowRender();

    WindowRender*  GetImpl();
    void  SetCanCommit(bool b);
    void  Commit(HDC hDC, RECT* prc, int nCount=1);

    GRAPHICS_RENDER_LIBRARY_TYPE  GetGraphicsRenderType();
	void  SetGraphicsRenderType(GRAPHICS_RENDER_LIBRARY_TYPE  eTpye);
    bool  GetRequireAlphaChannel();
    
private:
    WindowRender*  m_pWindowRenderImpl;
};

}

#endif // IRENDERLAYER_H_8500D1A2_903C_4c76_B3B0_5248E65E95C3