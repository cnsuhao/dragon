#pragma once
#include "animatemgr.h"

// 2014.2.20  TODO: 增加repeat delay功能。每一次循环时可以延时一段时间，但要加这个的话，还得加一个动画状态功能，以后再加

namespace UI
{
#define ANIMATE_TIME_LINE_FLAG_FINISH       0x0004   // 指示一个timeline是否已经到期结束了
#define ANIMATE_TIME_LINE_FLAG_NEED_RESET   0x0008   // 表示一个timeline一次循环结束，下次开始前需要重置
#define ANIMATE_TIME_LIEN_FLAG_REVERSING    0x0010   // 表示正在反向进行动画

    template <class T>
    class ITimelineImpl : public T //public ITimeline
    {
    protected:
        ITimelineImpl()
        {
            m_nBeginTime = 0;
            m_nFrameElapse = 0;
            m_eTimeType = TT_BY_MS;  // 默认以毫秒为单位

            m_nRepeatTimes = 1;  // 默认只播放一次
            m_bAutoReverse = false;

            m_nFlags = 0;
            m_nID = 0;
            m_pParam = NULL;
        }

    public:
        virtual ~ITimelineImpl(){};
        virtual void  Destroy() 
        { 
            delete this; 
        }
        virtual void  SetRepeateCount(int n) 
        { 
            m_nRepeatTimes = n; 
        }
        virtual bool  IsFinish() 
        { 
            return m_nFlags & ANIMATE_TIME_LINE_FLAG_FINISH ? true:false; 
        }
        virtual void  SetUserData(LPARAM lParam)
        { 
            m_pParam = lParam;
        }
        virtual LPARAM  GetUserData() 
        { 
            return m_pParam; 
        }
        virtual void  SetId(int nID) 
        { 
            m_nID = nID; 
        }
        virtual int  GetId() 
        { 
            return m_nID; 
        }
        virtual void  SetTimeType(TIMELINE_TIME_TYPE eType) 
        {
            m_eTimeType = eType; 
        }
        virtual void  SetAutoReverse(bool b)
        {
            m_bAutoReverse = b;
        }

        // virtual void GetCurrentValue(void** ppOut) = 0;  // 由各个子类去实现

        // TODO: 该函数本不应该作为接口暴露给外部的。但是无奈在AnimateManager中也只能拿到ITimeline指针。
        // 后面再想想办法能否不用模块类来实现，同时还能保存IIntTimeline继承于ITimeline
        virtual void x_OnAnimateStart()
        {
            this->Init();
            m_nBeginTime = GetTickCount();
        }
        virtual void x_SetFinishFlag() 
        { 
            m_nFlags |= ANIMATE_TIME_LINE_FLAG_FINISH;
        }


        // 注：GetTickCount的精度很低，只有15/16ms
        virtual void x_OnTick()
        {
            if (m_nFlags & ANIMATE_TIME_LINE_FLAG_NEED_RESET)
            {
                m_nFlags &= ~ANIMATE_TIME_LINE_FLAG_NEED_RESET;

                m_nFrameElapse = 0;
                m_nBeginTime = GetTickCount();
            }
            else if (IsFinish())
            {
                return;
            }

            m_nFrameElapse ++;

            BOOL bTimelineFinish = FALSE;
            int nTimeElapse = 0;
            switch (m_eTimeType)
            {
            case TT_BY_FRAME:
                nTimeElapse = m_nFrameElapse;
                break;

            case TT_BY_MS:
                nTimeElapse = GetTickCount() - m_nBeginTime;
                break;

            case TT_BY_SECOND:
                nTimeElapse = (GetTickCount() - m_nBeginTime)/1000;
            }

            bTimelineFinish = OnTick(nTimeElapse);

            if (bTimelineFinish)
            {
                if (m_bAutoReverse)
                {
                    m_nFlags |= ANIMATE_TIME_LINE_FLAG_NEED_RESET;  // 下次开始前先重置动画参数

                    // 将数据进行反向
                    OnReverse();

                    // 反向的结束，代表一次播放结束
                    if (m_nFlags&ANIMATE_TIME_LIEN_FLAG_REVERSING)
                    {
                        if (m_nRepeatTimes > 0 && 0 == --m_nRepeatTimes)
                        {
                            x_SetFinishFlag();
                        }
                        m_nFlags &= ~ANIMATE_TIME_LIEN_FLAG_REVERSING;
                    }
                    // 正向的结束，进入反向
                    else
                    {
                        m_nFlags |= ANIMATE_TIME_LIEN_FLAG_REVERSING;
                    }
                }
                else
                {
                    // 一次播放结束
                    if (m_nRepeatTimes > 0 && 0 == --m_nRepeatTimes)
                    {
                        x_SetFinishFlag();
                    }
                    else
                    {
                        m_nFlags |= ANIMATE_TIME_LINE_FLAG_NEED_RESET;  // 下次开始前先重置动画参数
                    }
                }
            }
        }


    public:
        // 给子类的虚函数
        virtual void  Init() = 0;
        virtual bool  OnTick(int nTimeElapse) = 0; // 返回动画是否结束(一次，不管reverse和repeate)
        virtual void  OnReverse() = 0;

        void  SetAnimateMgrPtr(AnimateManager* p)
        { m_pAnimateMgr = p; }

    protected:
        AnimateManager*  m_pAnimateMgr;

        int      m_nBeginTime;   // 记录动画开始时的time tick，当时间到达m_nBeginTime+m_nDuretion时，动画结束。
        int      m_nFrameElapse; // 经历的帧数
        int      m_nRepeatTimes; // 1表示播放一次，小于等于0表示一直播放，当--m_nRepeatTimes==0时停止. 
        bool     m_bAutoReverse;

        int      m_nFlags;
        TIMELINE_TIME_TYPE  m_eTimeType;

        int      m_nID;
        LPARAM   m_pParam;
    };


	class IntTimeline : public ITimelineImpl<IIntTimeline>
	{
	public:
		IntTimeline();
		~IntTimeline();

        IIntMoveAlgorithm*  CreateMoveAlgorithm(TIMELINE_MOVE_ALGORITHM eType);

        virtual void  GetCurrentValue(void* pOut);
        virtual void  SetOutRef(int* pRef);

	public:
		virtual void  Init();
        virtual bool  OnTick(int nTimeElapse);
        virtual void  OnReverse();

	protected:
        IIntMoveAlgorithm*  m_pMoveAlgorithm;
		int     m_nCurValue;
		int*    m_pOutRef;
	};

    class FloatTimeline : public ITimelineImpl<IFloatTimeline>
    {
    public:
        FloatTimeline();
        ~FloatTimeline();

        IFloatMoveAlgorithm*  CreateMoveAlgorithm(TIMELINE_MOVE_ALGORITHM eType);

        virtual void  GetCurrentValue(void* pOut);
        virtual void  SetOutRef(float* pRef);

    public:
        virtual void  Init();
        virtual bool  OnTick(int nTimeElapse);
        virtual void  OnReverse();

    protected:
        IFloatMoveAlgorithm*  m_pMoveAlgorithm;
        float     m_fCurValue;
        float*    m_pOutRef;
    };

}
