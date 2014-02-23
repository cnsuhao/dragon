#include "stdafx.h"
#include "stage3d.h"
#include "UISDK\Project\UI3D\src\Element\element.h"
#include "UISDK\Project\UI3D\src\D3D\d3dapp.h"
#include "UISDK\Project\UI3D\src\Element\mesh\meshelem.h"
#include "UISDK\Project\UI3D\src\Element\particle\particle.h"
#include "UISDK\Project\UI3D\src\D3D\Wrap\sprite.h"

namespace UI
{

Stage3D::Stage3D() : m_camera(this)
{
    m_dwBkgndColor = 0x00000000;
    m_pBkgndImage = NULL;
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
    SAFE_DELETE(m_pBkgndImage);
}


void  Stage3D::OnObjectLoaded()
{
	m_camera.Init(GetDevice(), m_pIStage3D->GetHWND());
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

void  Stage3D::SetBkgndTexture(const TCHAR* szPath)
{
    if (m_pBkgndImage)
        SAFE_DELETE(m_pBkgndImage);
    
    if (szPath)
    {
        m_pBkgndImage = new DxTexture;
        m_pBkgndImage->LoadFile(szPath, GetDevice());
    }
}

void  Stage3D::OnPaint(IRenderTarget* pRenderTarget)
{
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
}

void  Stage3D::Screen2World(int xScreen, int yScreen, float zWorld, float* pxWorld, float* pyWorld)
{
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