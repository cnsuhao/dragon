#include "stdafx.h"
#include "translation.h"

namespace UI
{
DxTranslation::DxTranslation()
{
	m_xPos = m_yPos = m_zPos = 0.0f;
	m_xRotation = m_yRotation = m_zRotation = 0.0f;
	m_xScale = m_yScale = m_zScale = 0.0f;

	m_pmatCombine1 = m_pmatCombine2 = NULL;
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matScale);
	D3DXMatrixIdentity(&m_matRotation);
	D3DXMatrixIdentity(&m_matTranslation);
}
DxTranslation::~DxTranslation()
{
	
}

void DxTranslation::Move( float xPos, float yPos, float zPos )
{
	m_xPos = xPos;
	m_yPos = yPos;
	m_zPos = zPos;

	D3DXMatrixTranslation(&m_matTranslation, m_xPos, m_yPos, m_zPos);
}

void DxTranslation::MoveBy( float xPos, float yPos, float zPos )
{
	m_xPos += xPos;
	m_yPos += yPos;
	m_zPos += zPos;

	D3DXMatrixTranslation(&m_matTranslation, m_xPos, m_yPos, m_zPos);
}

//
// D3DXMatrixRotationYawPitchRoll
//
// 参数：
// 
//	pOut 
//		[in, out] 指向D3DXMATRIX 结构的操作结果矩阵。 
// 
//	Yaw 
//		[in] 绕着Y轴旋转角度（单位弧度）。 
// 
//	Pitch 
//		[in] 绕着X轴旋转角度（单位弧度）。 
// 
//	Roll 
//		[in] 绕着Z轴旋转角度（单位弧度）。
//
//
void DxTranslation::Rotate( float xRotate, float yRotate, float zRotate )
{
	m_xRotation = xRotate;
	m_yRotation = yRotate;
	m_zRotation = zRotate;

	D3DXMatrixRotationYawPitchRoll(&m_matRotation, m_yRotation, m_xRotation, m_zRotation);
}

void DxTranslation::RotateBy( float xRotate, float yRotate, float zRotate )
{
	m_xRotation += xRotate;
	m_yRotation += yRotate;
	m_zRotation += zRotate;

	D3DXMatrixRotationYawPitchRoll(&m_matRotation, m_yRotation, m_xRotation, m_zRotation);
}

void  DxTranslation::RotateY(float yRotate)
{
	m_yRotation = yRotate;
	D3DXMatrixRotationY(&m_matRotation, m_yRotation);
}
void  DxTranslation::RotateYBy(float yRotate)
{
	m_yRotation += yRotate;
	D3DXMatrixRotationY(&m_matRotation, m_yRotation);
}

void DxTranslation::Scale( float xScale, float yScale, float zScale )
{
	m_xScale = xScale;
	m_yScale = yScale;
	m_zScale = zScale;

	D3DXMatrixScaling(&m_matScale, m_xScale, m_yScale, m_zScale);
}

void DxTranslation::ScaleBy( float xScale, float yScale, float zScale )
{
	m_xScale += xScale;
	m_yScale += yScale;
	m_zScale += zScale;

	D3DXMatrixScaling(&m_matScale, m_xScale, m_yScale, m_zScale);
}

void  DxTranslation::Update()
{
	D3DXMATRIX  matView, matTransposed;
	D3DXMatrixMultiply(&m_matWorld, &m_matScale, &m_matRotation);
	D3DXMatrixMultiply(&m_matWorld, &m_matWorld, &m_matTranslation);

	if (m_pmatCombine1)
		D3DXMatrixMultiply(&m_matWorld, &m_matWorld, m_pmatCombine1);
	if (m_pmatCombine2)
		D3DXMatrixMultiply(&m_matWorld, &m_matWorld, m_pmatCombine2);
}

D3DXMATRIX*  DxTranslation::GetMatrix()
{
	return &m_matWorld;
}

// void  DxTranslation::ApplyDevice(IDirect3DDevice9* pDevice)
// {
// 	if (!pDevice)
// 		return;
// 
// 	Update();
// 	pDevice->SetTransform(D3DTS_WORLD, &m_matWorld);
// }

void  DxTranslation::GetPos(float* px, float* py, float* pz)
{
    if (px)
        *px = m_xPos;
    if (py)
        *py = m_yPos;
    if (pz)
        *pz = m_zPos;
}
void  DxTranslation::GetRotate(float* px, float* py, float* pz)
{
    if (px)
        *px = m_xRotation;
    if (py)
        *py = m_yRotation;
    if (pz)
        *pz = m_zRotation;
}
void  DxTranslation::GetScale(float* px, float* py, float* pz)
{
    if (px)
        *px = m_xScale;
    if (py)
        *py = m_yScale;
    if (pz)
        *pz = m_zScale;
}

}