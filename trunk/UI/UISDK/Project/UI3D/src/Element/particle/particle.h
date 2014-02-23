#pragma once
#include "UISDK\Project\UI3D\src\Element\element.h"
#include "UISDK\Project\UI3D\src\D3D\Wrap\texture.h"

namespace UI
{
interface IParticleElement;

// 粒子，未实现公告板技术
class Particle
{
public:
    Particle();
    ~Particle();

public:
    void OnPaint();
	
	void  Init(DxTexture*);
	void  OnTick();
	bool  IsAlive() { return m_bAlive; }
	void  SetAlive(bool b) { m_bAlive = b; }

protected:
	//void  DrawTextureRect();

private:
	bool  m_bAlive;  // 是否存活
	
	DxTexture*  m_pTexture;  // 使用ParticleSystem中的哪个纹理进行渲染

	// 由外部存储的渲染数据
	void*  pData1;
	void*  pData2;
	void*  pData3;
	void*  pData4;
};

class ParticleElement : public Element
{
public:
	ParticleElement();
	~ParticleElement();

	IParticleElement*  GetIParticleElement();

public:
	virtual void  OnPaint();

	// 先调用SetTextureCount，再调用SetTexture
	bool  SetTextureCount(unsigned int nTextureCount);
	bool  SetTexture(unsigned int nIndex, const TCHAR* szFile);
	void  InitParticles(unsigned int nCacheCount, unsigned int nAliveCount);
	void  Clear();

	void  OnTick();

private:
	IParticleElement*  m_pIParticleElement;

	unsigned int  m_nParticleCount;
	unsigned int  m_nParticleMaxAliveCount;
	Particle*  m_pParticleCache;

	unsigned int  m_nTextureCacheCount;
	DxTexture*  m_pTextureCache;

    UINT_PTR  m_nTimerID;
};

}