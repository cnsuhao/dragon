#include "stdafx.h"
#include "particle.h"
#include "UISDK\Project\UI3D\inc\ielement.h"


namespace UI
{
Particle::Particle()
{
	m_pTexture = NULL;
	m_bAlive = false;
}

Particle::~Particle()
{

}	

void Particle::OnPaint()
{
	//DrawTextureRect(); 
}

void  Particle::Init(DxTexture* pTexture)
{
	m_pTexture = pTexture;
}

void  Particle::OnTick()
{

}

//////////////////////////////////////////////////////////////////////////

ParticleElement::ParticleElement()
{
	m_nParticleCount = 0;
	m_pParticleCache = NULL;
	m_nTextureCacheCount = 0;
	m_pTextureCache = NULL;
	m_pIParticleElement = NULL;
    m_nTimerID = 0;

	srand(GetTickCount());
}

ParticleElement::~ParticleElement()
{
    if (m_nTimerID && g_pUIApplication)
    {
        g_pUIApplication->KillTimer(m_nTimerID);
    }
	SAFE_DELETE(m_pIParticleElement);
	Clear();
}

IParticleElement*  ParticleElement::GetIParticleElement()
{
	if (m_pIParticleElement)
		return m_pIParticleElement;

	m_pIParticleElement = new IParticleElement(this);
	return m_pIParticleElement;
}

void  ParticleElement::Clear()
{
	if (m_pParticleCache)
	{
// 	for (int i = 0; i < m_nCacheCount; i++)
// 	{
// 		delete m_pElemCache[i];
// 	}
		delete[] m_pParticleCache;
		m_pParticleCache = NULL;
		m_nParticleCount = 0;
	}

	if (m_pTextureCache)
	{
		delete [] m_pTextureCache;
		m_pTextureCache = NULL;
		m_nTextureCacheCount = 0;
	}
}

bool  ParticleElement::SetTextureCount(unsigned int nTextureCount)
{
	if (m_pTextureCache)
		return false;

	m_nTextureCacheCount = nTextureCount;
	if (m_nTextureCacheCount <= 0)
		return false;

	m_pTextureCache = new DxTexture[nTextureCount];
	return true;
}

bool  ParticleElement::SetTexture(unsigned int nIndex, const TCHAR* szFile)
{
	if (nIndex >= m_nTextureCacheCount)
		return false;
	
	m_pTextureCache[nIndex].LoadFile(szFile, GetDevice());
	return true;
}

void  ParticleTimerProc(UINT_PTR nTimerID, TimerItem* pTimerItem)
{
    ParticleElement*  pThis = (ParticleElement*)pTimerItem->wParam;
    pThis->OnTick();
}

void  ParticleElement::InitParticles(unsigned int nCacheCount, unsigned int nAliveCount)
{
	if (nAliveCount > nCacheCount || nCacheCount <= 0)
		return;

	if (m_pParticleCache)
	{
		delete[] m_pParticleCache;
		m_pParticleCache = NULL;
		m_nParticleCount = 0;
		m_nParticleMaxAliveCount = 0;
	}

	m_nParticleCount = nCacheCount;
	m_nParticleMaxAliveCount = nAliveCount;

	m_pParticleCache = new Particle[nCacheCount];
	for (unsigned int i = 0; i < m_nParticleCount; i++)
	{
		DxTexture* pTexture = NULL;
		if (m_nTextureCacheCount)
			pTexture = &m_pTextureCache[rand()%m_nTextureCacheCount];
		
		m_pParticleCache[i].Init(pTexture);
	}
	for (unsigned int j = 0; j < m_nParticleMaxAliveCount; j++)
	{
		m_pParticleCache[j].SetAlive(true);
	}

    // 开始计时器
    if (g_pUIApplication)
    {
        UI::TimerItem  ti;
        ti.pProc = ParticleTimerProc;
        ti.wParam = (WPARAM)this;
        m_nTimerID = g_pUIApplication->SetTimer(33, &ti);
    }
}

void  ParticleElement::OnPaint()
{
	m_matWorld.ApplyDevice(GetDevice());

	for (unsigned int i = 0; i < m_nParticleCount; i++)
	{
		if (m_pParticleCache[i].IsAlive())
		{
			m_pParticleCache[i].OnPaint();
		}
	}
}

void  ParticleElement::OnTick()
{
	for (unsigned int i = 0; i < m_nParticleCount; i++)
	{
		if (m_pParticleCache[i].IsAlive())
		{
			m_pParticleCache[i].OnTick();
		}
	}
}

}