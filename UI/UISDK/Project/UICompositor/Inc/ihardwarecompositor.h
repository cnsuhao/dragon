#ifndef IHARDWARE_COMPOSITION_H_LEEIHCY_201407152226
#define IHARDWARE_COMPOSITION_H_LEEIHCY_201407152226
#include "UISDK\Kernel\Inc\Util\math\matrix.h"
#include "UISDK/Kernel/Inc/Interface/irenderlayer.h"

#ifdef UICOMPOSITOR_EXPORTS
#define UICOMPOSITOR_API __declspec(dllexport)
#else
#define UICOMPOSITOR_API __declspec(dllimport)
#endif

namespace UI
{
interface IGpuLayerTexture;
class HardwareComposition;
interface UICOMPOSITOR_API IHardwareComposition
{
	IHardwareComposition();
	void  Release();
    void  SetImpl(HardwareComposition*);

    IGpuLayerTexture*  CreateLayerTexture();
    void  SetRootLayerSurface(IGpuLayerTexture* p);

    bool  BeginCommit();
    void  EndCommit();
    void  Resize(UINT nWidth, UINT nHeight);

private:
	HardwareComposition*  m_pImpl;
};

class UICOMPOSITOR_API GpuLayerCommitContext
{
public:
	GpuLayerCommitContext();
	~GpuLayerCommitContext();

	void  OffsetBy(int x, int y);
	void  SetOffset(int x, int y);
	void  MultiMatrix(float* matrix16);

public:
	int  m_xOffset;
	int  m_yOffset;

	bool   m_bTransformValid;
	float  m_matrixTransform[4][4];
};

class GpuLayerTexture;
interface UICOMPOSITOR_API IGpuLayerTexture
{
    IGpuLayerTexture();
    void  SetImpml(GpuLayerTexture* p);
    GpuLayerTexture*  GetImpl();
    void  Release();

#if 0
	void  Translation(float xPos, float yPos, float zPos);
	void  TranslationBy(float xPos, float yPos, float zPos);
    void  Rotate(float xRotate, float yRotate, float zRotate);
    void  RotateBy(float xRotate, float yRotate, float zRotate);
#endif
    void  Compositor(GpuLayerCommitContext* pContext, IRenderLayerTransform2* pTransform);
    void  UploadHBITMAP(HBITMAP hBitmap, LPRECT prcArray, UINT nCount, bool bTopdown, bool bHasAlphaChannel);
    void  Resize(UINT nWidth, UINT nHeight);

    void  ClearChildren();
    void  AddChild(IGpuLayerTexture* p);

private:
    GpuLayerTexture*  m_pImpl;
};

extern "C" UICOMPOSITOR_API IHardwareComposition*  CreateHardwareComposition(HWND hWnd);

// return:
//     0  代表成功
//     -1 代表失败
extern "C" UICOMPOSITOR_API long  StartupGpuCompositor();

// return:
//     无意义
extern "C" UICOMPOSITOR_API long  ShutdownGpuCompositor();

extern "C" UICOMPOSITOR_API IRenderLayerTransform2*  CreateHard3DTransform();
}

#endif // IHARDWARE_COMPOSITION_H_LEEIHCY_201407152226