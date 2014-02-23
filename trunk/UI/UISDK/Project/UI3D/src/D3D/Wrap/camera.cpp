#include "stdafx.h"
#include "camera.h"
#include "UISDK\Project\UI3D\src\D3D\stage3d.h"

namespace UI
{

DxArcBall::DxArcBall(Stage3D*  pStage)
{
	m_bDrag = false;
	m_hWnd = NULL;
	m_pStage = pStage;

	D3DXQuaternionIdentity(&m_quatRotate);
	memset(m_vecMoveFrom, 0, sizeof(m_vecMoveFrom));
	memset(m_vecMoveTo, 0, sizeof(m_vecMoveTo));
}

DxArcBall::~DxArcBall()
{

}

void  DxArcBall::SetHWND(HWND hWnd)
{
	m_hWnd = hWnd;
}

void  DxArcBall::ScreenToVector(__in POINT* pt, __out D3DXVECTOR3* pVec)
{
	CRect rcStage;
	if (m_pStage)
	{
		m_pStage->GetIStage3D()->GetWindowRect(&rcStage);
	}
	else
	{
		::GetClientRect(m_hWnd, &rcStage);
	}

	// 归一，原点在屏幕中央
	POINT ptStage = {pt->x - rcStage.left, pt->y - rcStage.top};
	int nHalfWidth = (rcStage.right-rcStage.left)>>1;
	int nHalfHeight = (rcStage.bottom-rcStage.top)>>1;
	if (0 == nHalfHeight || 0 == nHalfWidth)
		return;

	float x = (float) (-(ptStage.x - nHalfWidth) *1.0f / nHalfWidth);  
	float y = (float) ((ptStage.y - nHalfHeight) *1.0f / nHalfHeight);


	// 将x,y映射到球体上， x^2 + y^2 + z^2 = 1;
	float z = 0.0f;

	float temp = x*x + y*y;
	if (temp > 1.0f)  // 出现这种情况的场合：鼠标点击拖拽时，拖出窗口范围
	{
		float scale = 1.0f / sqrtf(temp);
		x *= scale;
		y *= scale;
	}
	else
	{
		z = sqrtf(1-temp);
	}

	pVec->x = x;
	pVec->y = y;
	pVec->z = z;
}


// 由两个向量计算四元数，坐标系 + 旋转度数
void  DxArcBall::CalcQuaternion(D3DXQUATERNION* pQuat)
{
	float fCos = D3DXVec3Dot(&m_vecMoveFrom, &m_vecMoveTo);

	D3DXVECTOR3 vecCross;
	D3DXVec3Cross(&vecCross, &m_vecMoveFrom, &m_vecMoveTo);

	pQuat->w = fCos;
	pQuat->x = vecCross.x;
	pQuat->y = vecCross.y;
	pQuat->z = vecCross.z;
}

LRESULT  DxArcBall::ProcessMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	switch (message)
	{
	case WM_LBUTTONDOWN:
		{
			bHandled = TRUE;
		
			SetCapture(m_hWnd);
			POINT pt = {(int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)};
			BeginMove(&pt);
		}
		break;

	case WM_LBUTTONUP:
		{
			bHandled = TRUE;
			if (m_bDrag)
			{
				EndMove();
				ReleaseCapture();
			}
		}
		break;

	case WM_MOUSEMOVE:
		{
			if (m_bDrag)
			{
				bHandled = TRUE;

				POINT pt = {(int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)};
				this->Move(&pt);
			}
		}
		break;

	case WM_CANCELMODE:
		{
			if (m_bDrag)
			{
				EndMove();
			}
		}
		break;

	default:
		break;
	}

	return 0;
}

void  DxArcBall::BeginMove(POINT* pPt)
{
	m_bDrag = true;
	ScreenToVector(pPt, &m_vecMoveFrom);
}

void  DxArcBall::EndMove()
{
	m_bDrag = false;
}

void  DxArcBall::Move(POINT* pPt)
{
	if (!m_bDrag)
		return;

	ScreenToVector(pPt, &m_vecMoveTo);

	D3DXQUATERNION quat;
	CalcQuaternion(&quat);
	m_vecMoveFrom = m_vecMoveTo;

	m_quatRotate *= quat;
}

void DxArcBall::GetRotateMatrix(D3DXMATRIX* pMatrix)
{
	D3DXMatrixRotationQuaternion(pMatrix, &m_quatRotate);
}

DxCamera::DxCamera(Stage3D* pStage) : m_ViewArcBall(pStage)
{
	m_pStage = pStage;
	D3DXMatrixIdentity(&m_matView);

	m_vecLookAt = D3DXVECTOR3(0, 0, 0);
	D3DXVECTOR3 vecEye = D3DXVECTOR3(0, 0, -5);
	SetViewParam(&vecEye);
}

DxCamera::~DxCamera()
{

}

void  DxCamera::Init(IDirect3DDevice9* pDevice, HWND hWnd)
{
	m_ViewArcBall.SetHWND(hWnd);
}

void  DxCamera::UpdateViewMatrix()
{
	D3DXMATRIX  matrix;
	m_ViewArcBall.GetRotateMatrix(&matrix);

	D3DXMatrixInverse(&matrix, NULL, &matrix);
	D3DXVECTOR3 vWorldUp;
	D3DXVECTOR3 vLocalUp = D3DXVECTOR3(0, 1, 0);
	D3DXVec3TransformCoord(&vWorldUp, &vLocalUp, &matrix);
	
	D3DXVECTOR3 vWorldAhead;
	D3DXVECTOR3 vLocalAhead = D3DXVECTOR3(0, 0, 1);
	D3DXVec3TransformCoord(&vWorldAhead, &vLocalAhead, &matrix);
	
	// Update the eye point based on a radius away from the lookAt position
	m_vecEye = m_vecLookAt - vWorldAhead * m_fRadius;
	D3DXMatrixLookAtLH( &m_matView, &m_vecEye, &m_vecLookAt, &vWorldUp );

	m_pStage->OnViewMatrixChanged();
}

LRESULT  DxCamera::ProcessMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	LRESULT lret = m_ViewArcBall.ProcessMessage(hWnd, message, wParam, lParam, bHandled);
	if (bHandled)
	{
		if (m_ViewArcBall.IsDraging())
		{
			UpdateViewMatrix();
		}
		return lret;
	}

	switch (message)
	{
	case WM_MOUSEWHEEL:
		{
			int nDelta = (int)(short)HIWORD(wParam);
			{
				m_fRadius -= (nDelta / WHEEL_DELTA) * 0.4f;
				UpdateViewMatrix();
			}
		}
		break;
	}
	return 0;
}

void DxCamera::SetViewParam( D3DXVECTOR3* pvEye/*, D3DXVECTOR3* pvLookat */)
{
	m_vecEye = *pvEye;
	D3DXVECTOR3 vUp( 0,1,0 );
	D3DXMatrixLookAtLH( &m_matView, pvEye, &m_vecLookAt, &vUp );

	m_fRadius = D3DXVec3Length(pvEye);
}

void  DxCamera::SetDeviceView(IDirect3DDevice9* pDevice)
{
	if (!pDevice)
		return;

	pDevice->SetTransform(D3DTS_VIEW, &m_matView);
}


}