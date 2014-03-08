#include "stdafx.h"
#include "stage3d.h"
#include "UISDK\Project\UI3D\src\D3D\element\element.h"
#include "UISDK\Project\UI3D\src\D3D\d3dapp.h"

#if 0
#include "UISDK\Project\UI3D\src\Element\mesh\meshelem.h"
#include "UISDK\Project\UI3D\src\Element\particle\particle.h"
#include "UISDK\Project\UI3D\src\D3D\Wrap\sprite.h"
#endif

namespace UI
{

Stage3D::Stage3D() : m_camera(this)
{
    for (int i = 0; i < 4; i++)
        m_fBkgndColor[i] = 0;
    m_pIStage3D = NULL;
    m_sizeStage.cx = m_sizeStage.cy = 0;

	g_pD3DApp->AddStage(this);
	m_bEnableCamera = true;
}

Stage3D::~Stage3D()
{
    ClearElement();

	g_pD3DApp->RemoveStage(this);
}

bool  Stage3D::AddElement(Element* pElem)
{
	if (!pElem)
		return false;

	list<Element*>::iterator iter = std::find(m_listElement.begin(), m_listElement.end(), pElem);
	if (iter != m_listElement.end())
		return false;

	m_listElement.push_back(pElem);
	return true;
}

bool  Stage3D::RemoveElement(Element* pElem)
{
	if (!pElem)
		return false;

	list<Element*>::iterator iter = std::find(m_listElement.begin(), m_listElement.end(), pElem);
	if (iter == m_listElement.end())
		return false;

	m_listElement.erase(iter);
	return true;
}

void  Stage3D::ClearElement()
{
    list<Element*>::iterator iter = m_listElement.begin();
    for (; iter != m_listElement.end(); iter++)
    {
        Element* p = *iter;
        SAFE_DELETE(p);
    }
    m_listElement.clear();
}


void  Stage3D::OnObjectLoaded()
{
	m_camera.Init(m_pIStage3D->GetHWND());
}

void Stage3D::ResetAttribute()
{
    DO_PARENT_PROCESS(IStage3D, IPanel);

	m_pIStage3D->ModifyStyle(CONTROL_STYLE_TABSTOP, OBJECT_STYLE_REJECT_MOUSE_MSG_SELF, 0);  // 1. 接受焦点，支持MOUSEWHEEL 2. 接受鼠标消息，支持点击拖拽
	m_pIStage3D->SetTransparent(false); // 目前还不支持将3d透明背景。在一些机器上alphablend上失败，只能先用bitblt
}
void Stage3D::SetAttribute(IMapAttribute* pMapAttr, bool bReload)
{
    DO_PARENT_PROCESS(IStage3D, IPanel);
//    IUIApplication*  pUIApp = m_pIStage3D->GetUIApplication();
}

void  Stage3D::OnViewMatrixChanged()
{
	m_pIStage3D->UpdateObject();
}
void  Stage3D::OnResetDevice()
{
	
}
void  Stage3D::SetBkgndColor(DWORD dwColor)
{
    m_fBkgndColor[0] = (float)GetRValue(dwColor)/255.0f;
    m_fBkgndColor[1] = GetGValue(dwColor)/255.0f;
    m_fBkgndColor[2] = GetBValue(dwColor)/255.0f;
    m_fBkgndColor[3] = (dwColor>>24)/255.0f;
}
DWORD  Stage3D::GetBkgndColor()
{
    DWORD r = (DWORD)((m_fBkgndColor[0] * 255) + 0.5f);
    DWORD g = (DWORD)((m_fBkgndColor[1] * 255) + 0.5f);
    DWORD b = (DWORD)((m_fBkgndColor[2] * 255) + 0.5f);
    DWORD a = (DWORD)((m_fBkgndColor[3] * 255) + 0.5f);

    if (r>255)
        r = 255;
    if (g>255)
        g = 255;
    if (b>255)
        b = 255;
    if (a>255)
        a = 255;
    return RGBA(r,g,b,a);
}

void  Stage3D::OnSize(UINT nType, int cx, int cy)
{
    m_sizeStage.cx = cx;
    m_sizeStage.cy = cy;
    g_pD3DApp->RequestBackBuffer(cx, cy);
	SetMsgHandled(FALSE);
}

LRESULT  Stage3D::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	
	if (m_bEnableCamera)
	{
		LRESULT lr = m_camera.ProcessMessage(NULL, message, wParam, lParam, bHandled);
		if (bHandled)
			return lr;
	}

	return 0;
}

void  Stage3D::OnPaint(IRenderTarget* pRenderTarget)
{
    // 	Clear the back buffer 
    GetDevice()->ClearRenderTargetView(g_pD3DApp->m_pRenderTargetView, m_fBkgndColor);

    //     FillRectWorld f;
    // 
    //     RECTF rc;
    //     rc.Set(-0.8f, 0.8f, 0.8f, -0.8f);
    //     f.CreateBuffer(&rc, 0);
    // 
    //     f.OnPaint();

    list<Element*>::iterator iter = m_listElement.begin();
    for (; iter != m_listElement.end(); iter++)
    {
        (*iter)->OnPaint();
    }

    // Present the information rendered to the back buffer to the front buffer (the screen)
    g_pD3DApp->m_pSwapChain->Present( 0, 0 );

//     IDXGISurface1* g_pSurface1 = NULL;
//     g_pD3DApp->m_pSwapChain->GetBuffer(0, __uuidof(IDXGISurface1), (void**)&g_pSurface1);
//     HDC hDC = NULL;
//     g_pSurface1->GetDC(FALSE, &hDC);
    
//     BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
//     //if (!AlphaBlend(pRenderTarget->GetBindHDC(), 0, 0, nWidth, nHeight, hDC, 0, 0, nWidth, nHeight, bf))
//     {
//         BitBlt(pRenderTarget->GetBindHDC(), 0, 0, 500, 500, hDC, 0, 0, SRCCOPY);
//     }


//     RECT rcEmpty = {0,0,0,0};
//     g_pSurface1->ReleaseDC(&rcEmpty);
//     g_pSurface1->Release();


#if 0
    GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, m_dwBkgndColor, 1.0f, 0);
    if (FAILED(GetDevice()->BeginScene()))
        return;

	m_camera.SetDeviceView(GetDevice());

    list<Element*>::iterator iter = m_listElement.begin();
    for (; iter != m_listElement.end(); iter++)
    {
        (*iter)->OnPaint();
    }

    GetDevice()->EndScene();

    // 提取数据
    g_pD3DApp->CommitBackBuffer(pRenderTarget, m_sizeStage.cx, m_sizeStage.cy);
 //   GetDevice()->Present(NULL, NULL, NULL, NULL);
#endif
}

void  Stage3D::Screen2World(int xScreen, int yScreen, float zWorld, float* pxWorld, float* pyWorld)
{
#if 0
	D3DXMATRIX  matProj;
	GetDevice()->GetTransform(D3DTS_PROJECTION, &matProj);
	
	D3DVIEWPORT9 vp;
	GetDevice()->GetViewport(&vp);
	int nWidth = vp.Width;
	int nHeight = vp.Height;

 	// 转换为投影坐标
	// [注：D3D的视平面就是近平面，所以z直接取near值1.0f]
	float xProj = ((xScreen*2.0f)/nWidth - 1.0f) / matProj._11;
	float yProj = (1.0f - (2.0f*yScreen)/nHeight) / matProj._22;
	float zProj = 1.0f;

	// 计算出流水线的反方向，将坐标还原到世界坐标中
	D3DXMATRIX  matWorld;
	GetDevice()->GetTransform(D3DTS_WORLD, &matWorld);
	D3DXMATRIX inverseMatWorldView;
	D3DXMATRIX matWorldView = matWorld * (*m_camera.GetViewMatrix());
	D3DXMatrixInverse(&inverseMatWorldView, NULL, &matWorldView);        

	// 求出透视坐标在世界坐标中的位置
	D3DXVECTOR3 vNear;
	D3DXVECTOR3 v(xProj, yProj, zProj);
	D3DXVec3TransformCoord(&vNear, &v, &inverseMatWorldView);

	float fOrigX = inverseMatWorldView._41;
	float fOrigY = inverseMatWorldView._42;
	float fOrigZ = inverseMatWorldView._43;

	// 计算出z深度的x,y

//	(vNear.z-fOrigZ)/(z-fOrigZ) = (vNear.x-fOrigX)/(fx - fOrigX)
	if (pxWorld)
		*pxWorld = (vNear.x-fOrigX)*(zWorld-fOrigZ)/(vNear.z-fOrigZ) + fOrigX;
	if (pyWorld)
		*pyWorld = (vNear.y-fOrigY)*(zWorld-fOrigZ)/(vNear.z-fOrigZ) + fOrigY;

#if 0
	// 注：下面这段代码只是求出了射线的方向，并不是一个坐标！！
	if (px)
		*px = xProj*inverseMatWorldView._11 + yProj*inverseMatWorldView._21 + zProj*inverseMatWorldView._31;
	if (py)
		*py = xProj*inverseMatWorldView._12 + yProj*inverseMatWorldView._22 + zProj*inverseMatWorldView._32; 
	if (pz)
		*pz = xProj*inverseMatWorldView._13 + yProj*inverseMatWorldView._23 + zProj*inverseMatWorldView._33; 
#endif

#endif
}

// 如果相机位于z轴上，返回一个RECT给外部，指示当前stage可见区域的坐标范围
// （未考虑相机有角度的情况）
void  Stage3D::GetFullWorldCoord(float fzWorld, float*  pfWidth, float* pfHeight)
{
	float fx = 0, fy = 0;
	this->Screen2World(m_pIStage3D->GetWidth(), m_pIStage3D->GetHeight(), fzWorld, &fx, &fy);

	if (pfWidth)
	{
		*pfWidth = abs(fx);
	}
	if (pfHeight)
	{
		*pfHeight = abs(fy);
	}
}

}