#include "stdafx.h"
#include "UISDK/Project/UICompositor/inc/ihardwarecompositor.h"
#include "hardwarecompositor.h"
#include "GpuLayerTexture.h"
#include "d3d10\d3dapp.h"
#include "hard3dtransform.h"
using namespace UI;

namespace UI
{
    D3D10App*  g_pD3D10App = NULL;
    long  g_lD3D10AppRef = 0;
}

IHardwareComposition::IHardwareComposition()
{
	m_pImpl = NULL;
}
void  IHardwareComposition::SetImpl(HardwareComposition* p)
{
    m_pImpl = p;
}
void  IHardwareComposition::Release()
{
	delete m_pImpl;
}

bool  IHardwareComposition::BeginCommit()
{
    return m_pImpl->BeginCommit();
}
void  IHardwareComposition::EndCommit()
{
    m_pImpl->EndCommit();
}
void  IHardwareComposition::Resize(UINT nWidth, UINT nHeight)
{
    m_pImpl->Resize(nWidth, nHeight);
}

IGpuLayerTexture*  IHardwareComposition::CreateLayerTexture()
{
    GpuLayerTexture* p = m_pImpl->CreateLayerTexture();
    if (p)
        return p->GetIGpuLayerTexture();

    return NULL;
}
void  IHardwareComposition::SetRootLayerSurface(IGpuLayerTexture* p)
{
    m_pImpl->SetRootLayerTexture(p?p->GetImpl():NULL);
}

//////////////////////////////////////////////////////////////////////////

IGpuLayerTexture::IGpuLayerTexture()
{
    m_pImpl = NULL;
}
void  IGpuLayerTexture::SetImpml(GpuLayerTexture* p)
{
    m_pImpl = p;
}
GpuLayerTexture*  IGpuLayerTexture::GetImpl()
{
    return m_pImpl;
}
void  IGpuLayerTexture::Release()
{
    m_pImpl->Release();
}

void  IGpuLayerTexture::UploadHBITMAP(HBITMAP hBitmap, LPRECT prcArray, UINT nCount, bool bTopdown, bool bHasAlphaChannel)
{
    m_pImpl->UploadHBITMAP(hBitmap, prcArray, nCount, bTopdown, bHasAlphaChannel);
}

void  IGpuLayerTexture::Resize(UINT nWidth, UINT nHeight)
{
    m_pImpl->Resize(nWidth, nHeight);
}

void  IGpuLayerTexture::ClearChildren()
{
    m_pImpl->ClearChildren();
}
void  IGpuLayerTexture::AddChild(IGpuLayerTexture* p)
{
    if (!p)
        return;
    m_pImpl->AddChild(p->GetImpl());
}


void  IGpuLayerTexture::Compositor(GpuLayerCommitContext* pContext, IRenderLayerTransform2* pTransform)
{
    m_pImpl->Compositor(pContext, pTransform);
}
#if 0
void  IGpuLayerTexture::Translation(float xPos, float yPos, float zPos)
{
	m_pImpl->Translation(xPos, yPos, zPos);
}
void  IGpuLayerTexture::TranslationBy(float xPos, float yPos, float zPos)
{
	m_pImpl->TranslationBy(xPos, yPos, zPos);
}
void  IGpuLayerTexture::Rotate(float xRotate, float yRotate, float zRotate)
{
    m_pImpl->Rotate(xRotate, yRotate, zRotate);
}
void  IGpuLayerTexture::RotateBy(float xRotate, float yRotate, float zRotate)
{
    m_pImpl->RotateBy(xRotate, yRotate, zRotate);
}
#endif
//////////////////////////////////////////////////////////////////////////

GpuLayerCommitContext::GpuLayerCommitContext()
{
	m_xOffset = 0;
	m_yOffset = 0;

	m_bTransformValid = false;
	memset(&m_matrixTransform, 0, sizeof(m_matrixTransform));
	m_matrixTransform[0][0]
		= m_matrixTransform[1][1]
		= m_matrixTransform[2][2]
		= m_matrixTransform[3][3] 
		= 1;
}

GpuLayerCommitContext::~GpuLayerCommitContext()
{

}

void  GpuLayerCommitContext::OffsetBy(int x, int y)
{
	m_xOffset += x;
	m_yOffset += y;
}

void  GpuLayerCommitContext::SetOffset(int x, int y)
{
	m_xOffset = x;
	m_yOffset = y;
}

void  GpuLayerCommitContext::MultiMatrix(float* matrix16)
{
	D3DXMATRIX  mat1(matrix16);
	if (D3DXMatrixIsIdentity(&mat1))
		return;
	
	D3DXMATRIX  mat2((float*)m_matrixTransform);

	mat1 *= mat2;
	memcpy(&m_matrixTransform, &mat1, sizeof(mat1));
	
	m_bTransformValid = true;
}

//////////////////////////////////////////////////////////////////////////

extern "C" 
{
    UICOMPOSITOR_API IHardwareComposition*  CreateHardwareComposition(
        HWND hWnd)
    {
	    // ¼ì²âÓ²¼þ£º
	    HardwareComposition* p = new HardwareComposition(hWnd);
	    return &p->m_IHardwareComposition;
    }
    UICOMPOSITOR_API long  StartupGpuCompositor()
    {
        if (NULL == g_pD3D10App)
        {
            UIASSERT(0 == g_lD3D10AppRef);
            g_pD3D10App = new D3D10App;
            if (!g_pD3D10App->Init())
            {
                SAFE_DELETE(g_pD3D10App);
                return -1;
            }
        }
        g_lD3D10AppRef ++;

        return 0;
    }

    UICOMPOSITOR_API long  ShutdownGpuCompositor()
    {
        g_lD3D10AppRef--;
        if (0 == g_lD3D10AppRef)
        {
            UIASSERT(g_pD3D10App);
            if (g_pD3D10App)
            {
                SAFE_DELETE(g_pD3D10App);
            }
        }

        return 0;
    }

	UICOMPOSITOR_API IRenderLayerTransform2*  CreateHard3DTransform()
	{
		return Hard3DTransform::CreateInstance();
	}
}
