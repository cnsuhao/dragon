#pragma once
#include "animatemgr.h"
namespace UI
{

// ʲôҲ������ֻ��Ϊ��ά�ֶ�����ʱ��
class NoneAnimateTimeline : public ITimelineImpl<INoneTimeline>
{
public:
    virtual void  GetCurrentValue(void* ppOut){}
    virtual void  Init() {};
    virtual bool  OnTick(int nTimeElapse) { return false; }
    virtual void  OnReverse() {}
};

}