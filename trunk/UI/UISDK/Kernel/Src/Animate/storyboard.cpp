#include "stdafx.h"
#include "storyboard.h"
#include "UISDK\Kernel\Inc\Interface\ianimate.h"
#include "UISDK\Kernel\Src\Animate\animatemgr.h"
#include "timeline.h"
#include "noneanimatetimeline.h"

Storyboard::Storyboard()
{
    m_pAnimateMgr = NULL;
    m_pIStoryboard = NULL;
    m_pNotify = NULL;
    m_nID = 0;
    m_pWParam = m_pLParam = 0;
}
Storyboard::~Storyboard()
{
    DestroyAllTimeline();
}

void  Storyboard::SetAnimateMgr(AnimateManager*  p)
{
    m_pAnimateMgr = p;
}

bool  Storyboard::AddTimeline(ITimeline* p)
{
    if (NULL == p)
        return false;

    if (NULL != FindTimeline(p->GetId()))
    {
        UIASSERT(0);
        return false;
    }

    m_listTimeline.push_back(p);
    return true;
}

bool  Storyboard::DestroyTimeline(int nTimelineId)
{
    TimelineIter iter = m_listTimeline.begin();
    for (; iter != m_listTimeline.end(); iter++)
    {
        ITimeline*  p = *iter;
        if (p->GetId() == nTimelineId)
        {
            m_listTimeline.erase(iter);
            SAFE_DELETE(p);
            return true;
        }
    }
    return false;
}

ITimeline*  Storyboard::CreateTimeline(TIMELINE_VALUE_TYPE eType, int nTimelineId, int nMoveAlgo, IMoveAlgorithm** ppMoveAlgo)
{
    ITimeline* pRet = NULL;
    switch (eType)
    {
    case TV_INT:
        {
            IntTimeline* p = new IntTimeline();
            p->SetAnimateMgrPtr(m_pAnimateMgr);
            pRet = static_cast<ITimeline*>(p);

            IIntMoveAlgorithm* pAlgo = p->CreateMoveAlgorithm((TIMELINE_MOVE_ALGORITHM)nMoveAlgo);
            if (ppMoveAlgo && pAlgo)
                *ppMoveAlgo = static_cast<IMoveAlgorithm*>(pAlgo);
        }
        break;

    case TV_FLOAT:
        {
            FloatTimeline* p = new FloatTimeline();
            p->SetAnimateMgrPtr(m_pAnimateMgr);
            pRet = static_cast<ITimeline*>(p);

            IFloatMoveAlgorithm* pAlgo = p->CreateMoveAlgorithm((TIMELINE_MOVE_ALGORITHM)nMoveAlgo);
            if (ppMoveAlgo && pAlgo)
                *ppMoveAlgo = static_cast<IMoveAlgorithm*>(pAlgo);
        }
        break;

    case TV_NONE:
        {
            NoneAnimateTimeline* p = new NoneAnimateTimeline();
            p->SetAnimateMgrPtr(m_pAnimateMgr);
            pRet = static_cast<ITimeline*>(p);
        }
        break;

    case TV_RECT:
        {
        }
        break;
    }

    if (pRet)
    {
        pRet->SetId(nTimelineId);
        if (false == AddTimeline(pRet))
        {
            SAFE_DELETE(pRet);
        }
    }

    return pRet;
}

void  Storyboard::DestroyAllTimeline()
{
    TimelineIter iter = m_listTimeline.begin();
    for (; iter != m_listTimeline.end(); iter++)
    {
        ITimeline*  p = *iter;
        SAFE_DELETE(p);
    }
    m_listTimeline.clear();
}

ITimeline*  Storyboard::FindTimeline(int nTimelineId)
{
    TimelineIter iter = m_listTimeline.begin();
    for (; iter != m_listTimeline.end(); iter++)
    {
        if ((*iter)->GetId() == nTimelineId)
            return *iter;
    }

    return NULL;
}

void  Storyboard::Begin()
{
    m_pAnimateMgr->AddStoryboard(m_pIStoryboard);
}
void  Storyboard::End()
{

}

IMessage*  Storyboard::GetNotifyObj()
{
    return m_pNotify;
}
void  Storyboard::SetNotifyObj(IMessage* pNotify)
{
    m_pNotify = pNotify;
}


// TODO: 优化
void  Storyboard::OnAnimateStart()
{
    TimelineIter iter = m_listTimeline.begin();
    for (; iter != m_listTimeline.end(); iter++)
    {
        (*iter)->x_OnAnimateStart();
    }
}

// TODO: 优化
void  Storyboard::SetFinishFlag()
{
    TimelineIter iter = m_listTimeline.begin();
    for (; iter != m_listTimeline.end(); iter++)
    {
        (*iter)->x_SetFinishFlag();
    }
}

bool  Storyboard::IsFinish()
{   
    TimelineIter iter = m_listTimeline.begin();
    for (; iter != m_listTimeline.end(); iter++)
    {
        if (false == (*iter)->IsFinish())
            return false;
    }
    return true;
}

void  Storyboard::OnTick(bool* pbFinish)
{
    *pbFinish = true; // 不判断指针有效性了，外部去保证

    TimelineIter iter = m_listTimeline.begin();
    for (; iter != m_listTimeline.end(); iter++)
    {
        ((*iter)->x_OnTick());

        if (!(*iter)->IsFinish())
            *pbFinish = false;
    }
}