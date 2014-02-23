#pragma once
#include "animatemgr.h"

// 2014.2.20  TODO: ����repeat delay���ܡ�ÿһ��ѭ��ʱ������ʱһ��ʱ�䣬��Ҫ������Ļ������ü�һ������״̬���ܣ��Ժ��ټ�

namespace UI
{
#define ANIMATE_TIME_LINE_FLAG_FINISH       0x0004   // ָʾһ��timeline�Ƿ��Ѿ����ڽ�����
#define ANIMATE_TIME_LINE_FLAG_NEED_RESET   0x0008   // ��ʾһ��timelineһ��ѭ���������´ο�ʼǰ��Ҫ����
#define ANIMATE_TIME_LIEN_FLAG_REVERSING    0x0010   // ��ʾ���ڷ�����ж���

    template <class T>
    class ITimelineImpl : public T //public ITimeline
    {
    protected:
        ITimelineImpl()
        {
            m_nBeginTime = 0;
            m_nFrameElapse = 0;
            m_eTimeType = TT_BY_MS;  // Ĭ���Ժ���Ϊ��λ

            m_nRepeatTimes = 1;  // Ĭ��ֻ����һ��
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

        // virtual void GetCurrentValue(void** ppOut) = 0;  // �ɸ�������ȥʵ��

        // TODO: �ú�������Ӧ����Ϊ�ӿڱ�¶���ⲿ�ġ�����������AnimateManager��Ҳֻ���õ�ITimelineָ�롣
        // ����������취�ܷ���ģ������ʵ�֣�ͬʱ���ܱ���IIntTimeline�̳���ITimeline
        virtual void x_OnAnimateStart()
        {
            this->Init();
            m_nBeginTime = GetTickCount();
        }
        virtual void x_SetFinishFlag() 
        { 
            m_nFlags |= ANIMATE_TIME_LINE_FLAG_FINISH;
        }


        // ע��GetTickCount�ľ��Ⱥܵͣ�ֻ��15/16ms
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
                    m_nFlags |= ANIMATE_TIME_LINE_FLAG_NEED_RESET;  // �´ο�ʼǰ�����ö�������

                    // �����ݽ��з���
                    OnReverse();

                    // ����Ľ���������һ�β��Ž���
                    if (m_nFlags&ANIMATE_TIME_LIEN_FLAG_REVERSING)
                    {
                        if (m_nRepeatTimes > 0 && 0 == --m_nRepeatTimes)
                        {
                            x_SetFinishFlag();
                        }
                        m_nFlags &= ~ANIMATE_TIME_LIEN_FLAG_REVERSING;
                    }
                    // ����Ľ��������뷴��
                    else
                    {
                        m_nFlags |= ANIMATE_TIME_LIEN_FLAG_REVERSING;
                    }
                }
                else
                {
                    // һ�β��Ž���
                    if (m_nRepeatTimes > 0 && 0 == --m_nRepeatTimes)
                    {
                        x_SetFinishFlag();
                    }
                    else
                    {
                        m_nFlags |= ANIMATE_TIME_LINE_FLAG_NEED_RESET;  // �´ο�ʼǰ�����ö�������
                    }
                }
            }
        }


    public:
        // ��������麯��
        virtual void  Init() = 0;
        virtual bool  OnTick(int nTimeElapse) = 0; // ���ض����Ƿ����(һ�Σ�����reverse��repeate)
        virtual void  OnReverse() = 0;

        void  SetAnimateMgrPtr(AnimateManager* p)
        { m_pAnimateMgr = p; }

    protected:
        AnimateManager*  m_pAnimateMgr;

        int      m_nBeginTime;   // ��¼������ʼʱ��time tick����ʱ�䵽��m_nBeginTime+m_nDuretionʱ������������
        int      m_nFrameElapse; // ������֡��
        int      m_nRepeatTimes; // 1��ʾ����һ�Σ�С�ڵ���0��ʾһֱ���ţ���--m_nRepeatTimes==0ʱֹͣ. 
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
