#pragma once
#include "UISDK/Kernel/Src/Base/Message/message.h"

//////////////////////////////////////////////////////////////////////////
//              ���Թ���
//
//  �ɵĹ����߼����ѷ�����
//   1. ��m_nScroll_V0��Ϊ���ٶ��������˶��������ٶȵ���0ֹͣ
//   2. ÿһ��MOUSEWHEELʱ�����ٶ�����Ϊm_nScroll_V0
//   3. ������������Ϊ·��
//   4. ÿһ��MOUSEWHEELʱ���ۼ�����Ҫ��·��
//   5. ʱ�䲻��Ҫ��2as=v^2��û��ʱ�����
//
//  �µĹ����߼���
//   1. ���ü��ٵĻ����㷨
//   2. ÿ�ι���ʱ��̶�
//   3. ��������������ٴδ���mousewheel������¶�����from/to/duration���������������ٴ�����
//
//////////////////////////////////////////////////////////////////////////

// TODO: ��ios�߽絯�Թ������� m_bAnimateOverflow
//  2014/8/27 
//  . �ڶ��������е������������ֹͣ��ǰ��������δ���
//  . ����жϵ�������������
//  . ����ϸ��

// bouncy edge

namespace UI
{

interface  IStoryboard;

enum SmoothScrollResult
{
    INERTIA_SCROLL_STOP = 0,      // ��������
    INERTIA_SCROLL_CONTINUE = 1,  // ��������
    INERTIA_SCROLL_OVERFLOW = 2,  // �����
};

interface ISmoothScrollCallback
{
    virtual  void SmoothScroll_Start() = 0;
    virtual  void SmoothScroll_Stop() = 0;
    virtual  SmoothScrollResult SmoothScroll_Scroll(
				int nDeltaPos, bool bAllowOverflow) = 0;
};

class SmoothScroll : public Message
{
public:
    SmoothScroll();
    ~SmoothScroll();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_ANIMATE_TICK(OnAnimateTick)
    UI_END_MSG_MAP

public:
    void  SetUIApplication(IUIApplication* p);
    void  SetCallback(ISmoothScrollCallback* pCallback);
    bool  IsEnable();
    void  SetEnable(bool b);
    bool  IsScrolling();
    void  AddPower(int zDelta, int nViewPage);
    void  StopScroll();

protected:
    void  OnAnimateTick(int nCount, IStoryboard** ppArray);
	void  OnScrollStop();
    void  CreateAnimate();
    void  DestroyAnimate();

private:
    IUIApplication*  m_pUIApplication;
    ISmoothScrollCallback*  m_pCallback;

	// �������¹��������Ǵ�������
	enum MOUSEWHEEL_DIR { DOWN, UP};
	MOUSEWHEEL_DIR  m_eMouseWheelDir;

    bool  m_bEnable;
    IStoryboard*  m_pAnimateStoryboard;

	int  m_nStep;         // ÿ��mousewheel��������
	int  m_nDuration;     // ÿ�ι���ʱ��
    int  m_nScrollDistance;     // ���ι�������
	int  m_nPrevScrollDistance; // ��¼��һ��ontick֪ͨ��s���ȣ�����һ�ε�s����������ó�������Ҫ�ƶ��ľ���
};

}