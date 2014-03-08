#pragma once

namespace UI
{
class DxTranslation
{
public:
	DxTranslation();
	~DxTranslation();

public:
	void  Move(float xPos, float yPos, float zPos);
	void  MoveBy(float xPos, float yPos, float zPos);

	void  Rotate(float xRotate, float yRotate, float zRotate);
	void  RotateBy(float xRotate, float yRotate, float zRotate);
	void  RotateY(float yRotate);
	void  RotateYBy(float yRotate);

	void  Scale(float xScale, float yScale, float zScale);
	void  ScaleBy(float xScale, float yScale, float zScale);

	void  Update();
	D3DXMATRIX*  GetMatrix();
	//void  ApplyDevice(IDirect3DDevice9* pDevice);

    void  GetPos(float* px, float* py, float* pz);
    void  GetRotate(float* px, float* py, float* pz);
    void  GetScale(float* px, float* py, float* pz);

protected:
	float  m_xPos;
	float  m_yPos;
	float  m_zPos;

	float  m_xRotation;
	float  m_yRotation;
	float  m_zRotation;

	float  m_xScale;
	float  m_yScale;
	float  m_zScale;

	D3DXMATRIX  m_matWorld;  // Result

	D3DXMATRIX  m_matScale;
	D3DXMATRIX  m_matRotation;
	D3DXMATRIX  m_matTranslation;
	D3DXMATRIX*  m_pmatCombine1;
	D3DXMATRIX*  m_pmatCombine2;
};
}