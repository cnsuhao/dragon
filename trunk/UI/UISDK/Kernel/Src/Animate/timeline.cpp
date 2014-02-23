#include "stdafx.h"
#include "timeline.h"
#include "movealgo.h"

IntTimeline::IntTimeline()
{
	m_nCurValue = 0;
	m_pOutRef = NULL;
    m_pMoveAlgorithm = NULL;
}

IntTimeline::~IntTimeline()
{
    SAFE_DELETE(m_pMoveAlgorithm);
}

void IntTimeline::Init()
{

}

bool  IntTimeline::OnTick(int nTimeElapse)
{
    bool  bTimelineFinish = true;
    if (m_pMoveAlgorithm)
    {
        bTimelineFinish = m_pMoveAlgorithm->OnTick(nTimeElapse, &m_nCurValue);
    }

    if (m_pOutRef)
    {
        *m_pOutRef = m_nCurValue;
    }

    return bTimelineFinish;
}

void  IntTimeline::OnReverse()
{
    // 将数据进行反向
    m_pMoveAlgorithm->Reverse();

}

IIntMoveAlgorithm*  IntTimeline::CreateMoveAlgorithm(TIMELINE_MOVE_ALGORITHM eType)
{
    SAFE_DELETE(m_pMoveAlgorithm);
    switch (eType)
    {
    case TMA_Linear:
        {
            m_pMoveAlgorithm = new LinearMove<int, IIntLinearMove>; 
        }
        break;

    case TMA_Accelerate:
        {
            m_pMoveAlgorithm = new AccelerateMove<int, IIntAccelerateMove>;
        }
        break;

    case TMA_Easing:
        {
            m_pMoveAlgorithm = new EasingMove<int, IIntEasingMove>;
        }
        break;
    }

    return static_cast<IIntMoveAlgorithm*>(m_pMoveAlgorithm);
}

void IntTimeline::GetCurrentValue(void* pOut)
{
	if (NULL == pOut)
		return;

	*(int*)pOut = m_nCurValue;
}

void IntTimeline::SetOutRef(int* pRef)
{
	m_pOutRef = pRef;
}

//////////////////////////////////////////////////////////////////////////


FloatTimeline::FloatTimeline()
{
    m_fCurValue = 0;
    m_pOutRef = NULL;
    m_pMoveAlgorithm = NULL;
}

FloatTimeline::~FloatTimeline()
{
    SAFE_DELETE(m_pMoveAlgorithm);
}

void FloatTimeline::Init()
{

}

bool  FloatTimeline::OnTick(int nTimeElapse)
{
    bool  bTimelineFinish = true;
    if (m_pMoveAlgorithm)
    {
        bTimelineFinish = m_pMoveAlgorithm->OnTick(nTimeElapse, &m_fCurValue);
    }

    if (m_pOutRef)
    {
        *m_pOutRef = m_fCurValue;
    }

    return bTimelineFinish;
}

void  FloatTimeline::OnReverse()
{
    // 将数据进行反向
    m_pMoveAlgorithm->Reverse();

}

IFloatMoveAlgorithm*  FloatTimeline::CreateMoveAlgorithm(TIMELINE_MOVE_ALGORITHM eType)
{
    SAFE_DELETE(m_pMoveAlgorithm);
    switch (eType)
    {
    case TMA_Linear:
        {
            m_pMoveAlgorithm = new LinearMove<float, IFloatLinearMove>; 
        }
        break;

    case TMA_Accelerate:
        {
            m_pMoveAlgorithm = new AccelerateMove<float, IFloatAccelerateMove>;
        }
        break;

    case TMA_Easing:
        {
            m_pMoveAlgorithm = new EasingMove<float, IFloatEasingMove>;
        }
        break;
    }

    return static_cast<IFloatMoveAlgorithm*>(m_pMoveAlgorithm);
}

void FloatTimeline::GetCurrentValue(void* pOut)
{
    if (NULL == pOut)
        return;

    *(float*)pOut = m_fCurValue;
}

void FloatTimeline::SetOutRef(float* pRef)
{
    m_pOutRef = pRef;
}