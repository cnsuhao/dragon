#pragma once

namespace UI
{
class Stage3D;
class DxArcBall
{
public:
	DxArcBall(Stage3D*  pStage);
	~DxArcBall();

public:
	void  SetHWND(HWND hWnd);

	LRESULT  ProcessMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void  GetRotateMatrix(D3DXMATRIX* pMatrix);
	bool  IsDraging() 
			{ return m_bDrag; }

private:
	void  ScreenToVector(__in POINT* pt, __out D3DXVECTOR3* pVec);
	void  CalcQuaternion(D3DXQUATERNION* pQuat);

	void  BeginMove(POINT* pPt);
	void  EndMove();
	void  Move(POINT* pPt);
private:
	bool  m_bDrag;
	HWND  m_hWnd;

	D3DXVECTOR3  m_vecMoveFrom;
	D3DXVECTOR3  m_vecMoveTo;
	D3DXQUATERNION  m_quatRotate;
	
	Stage3D*  m_pStage;
};

class DxCamera
{
public:
	DxCamera(Stage3D*  pStage);
	~DxCamera();

public:
	void  Init(IDirect3DDevice9* pDevice, HWND hWnd);

	LRESULT  ProcessMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void  SetViewParam(D3DXVECTOR3* pvEye/*, D3DXVECTOR3* pvLookat*/);
	void  SetDeviceView(IDirect3DDevice9* pDevice);

	D3DXMATRIX*  GetViewMatrix() { return &m_matView; }

protected:
	void  UpdateViewMatrix();

private:
	DxArcBall  m_ViewArcBall;

	D3DXMATRIX  m_matView;
	D3DXVECTOR3  m_vecEye;
	D3DXVECTOR3  m_vecLookAt;

	float  m_fRadius;

	Stage3D*  m_pStage;
};

}