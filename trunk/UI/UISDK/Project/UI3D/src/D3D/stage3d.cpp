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

	m_pIStage3D->ModifyStyle(CONTROL_STYLE_TABSTOP, OBJECT_STYLE_REJECT_MOUSE_MSG_SELF, 0);  // 1. ���ܽ��㣬֧��MOUSEWHEEL 2. ���������Ϣ��֧�ֵ����ק
	m_pIStage3D->SetTransparent(false); // Ŀǰ����֧�ֽ�3d͸����������һЩ������alphablend��ʧ�ܣ�ֻ������bitblt
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

    // ��ȡ����
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

 	// ת��ΪͶӰ����
	// [ע��D3D����ƽ����ǽ�ƽ�棬����zֱ��ȡnearֵ1.0f]
	float xProj = ((xScreen*2.0f)/nWidth - 1.0f) / matProj._11;
	float yProj = (1.0f - (2.0f*yScreen)/nHeight) / matProj._22;
	float zProj = 1.0f;

	// �������ˮ�ߵķ����򣬽����껹ԭ������������
	D3DXMATRIX  matWorld;
	GetDevice()->GetTransform(D3DTS_WORLD, &matWorld);
	D3DXMATRIX inverseMatWorldView;
	D3DXMATRIX matWorldView = matWorld * (*m_camera.GetViewMatrix());
	D3DXMatrixInverse(&inverseMatWorldView, NULL, &matWorldView);        

	// ���͸�����������������е�λ��
	D3DXVECTOR3 vNear;
	D3DXVECTOR3 v(xProj, yProj, zProj);
	D3DXVec3TransformCoord(&vNear, &v, &inverseMatWorldView);

	float fOrigX = inverseMatWorldView._41;
	float fOrigY = inverseMatWorldView._42;
	float fOrigZ = inverseMatWorldView._43;

	// �����z��ȵ�x,y

//	(vNear.z-fOrigZ)/(z-fOrigZ) = (vNear.x-fOrigX)/(fx - fOrigX)
	if (pxWorld)
		*pxWorld = (vNear.x-fOrigX)*(zWorld-fOrigZ)/(vNear.z-fOrigZ) + fOrigX;
	if (pyWorld)
		*pyWorld = (vNear.y-fOrigY)*(zWorld-fOrigZ)/(vNear.z-fOrigZ) + fOrigY;

#if 0
	// ע��������δ���ֻ����������ߵķ��򣬲�����һ�����꣡��
	if (px)
		*px = xProj*inverseMatWorldView._11 + yProj*inverseMatWorldView._21 + zProj*inverseMatWorldView._31;
	if (py)
		*py = xProj*inverseMatWorldView._12 + yProj*inverseMatWorldView._22 + zProj*inverseMatWorldView._32; 
	if (pz)
		*pz = xProj*inverseMatWorldView._13 + yProj*inverseMatWorldView._23 + zProj*inverseMatWorldView._33; 
#endif
}

// ������λ��z���ϣ�����һ��RECT���ⲿ��ָʾ��ǰstage�ɼ���������귶Χ
// ��δ��������нǶȵ������
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