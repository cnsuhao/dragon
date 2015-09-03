#ifndef IANIMATE_H_D4A2D364_BE5D_4a61_985C_C09506B05ACB
#define IANIMATE_H_D4A2D364_BE5D_4a61_985C_C09506B05ACB

#include "ianimatedef.h"

namespace UI
{
	//////////////////////////////////////////////////////////////////////////
	//
	//     动画接口
	//
	//////////////////////////////////////////////////////////////////////////

	interface ITimeline : public IRootInterface
	{
		virtual void     Destroy() = 0;
		virtual void     SetRepeateCount(int n) = 0;
        virtual void     SetTimeType(TIMELINE_TIME_TYPE eType) = 0;
        virtual void     SetAutoReverse(bool b) = 0;
		virtual bool     IsFinish() = 0;
        virtual void     SetFinish() = 0;
		virtual void     GetCurrentValue(void* ppOut) = 0;
		virtual void     SetId(int nID) = 0;
		virtual int      GetId() = 0;
		virtual void     SetUserData(LPARAM lParam) = 0;
		virtual LPARAM   GetUserData() = 0;
        virtual void     SetCustomBezierArgs(double, double, double, double) = 0;

		

		// 只能内部使用的接口，外面不要调用
		virtual void     x_OnAnimateStart() = 0;  
		virtual bool     x_OnTick() = 0;
	};

    interface IMoveAlgorithm 
    {
        virtual ~IMoveAlgorithm() = 0 {};

        // 反向动画前，将动画参数进行反转
        virtual void  Reverse() = 0;

		// t表示从动画开始到现在经过的时间(frame/ms/s)
		virtual bool OnTick(int t, float* pCurrentValue) = 0;
    };

    // S = VT
//     interface ILinearMove : public IMoveAlgorithm
//     {
//         virtual void SetParam1(float from, float to, float t) = 0;
//         virtual void SetParam2(float from, float to, float v) = 0;
//         virtual void SetParam3(float from, float v, float t) = 0;
//     };
//     // Vt^2 - Vo^2 = 2as
//     // s = Vot + 1/2at^2
//     // 匀加速或匀减速运动
//     interface IAccelerateMove : public IMoveAlgorithm
//     {
//         virtual void SetParam1(float from, float to, float t, float Vo) = 0;
//     };
//     interface IEasingMove : public IMoveAlgorithm
//     {
//         virtual void SetParam(float from, float to, float t, EaseType eType) = 0;
//     };

	interface IIntTimeline : public ITimeline
	{
		virtual void   SetOutRef(int* pRef) = 0;
		virtual void   SetEaseParam(int from, int to, int t, EaseType eType) = 0;
		virtual void   SetLinerParam1(int from, int to, int t) = 0;
		virtual void   SetAccelerateParam1(int from, int to, int t, float Vo) = 0;
		
	};
	interface IFloatTimeline : public ITimeline
	{
		virtual void   SetOutRef(float* pRef) = 0;
		virtual void   SetEaseParam(float from, float to, float t, EaseType eType) = 0;
		virtual void   SetLinerParam1(float from, float to, float t) = 0;
		virtual void   SetAccelerateParam1(float from, float to, float t, float Vo) = 0;
	};
    interface INoneTimeline : public ITimeline
    {
    };
	interface IRectTimeline : public ITimeline
	{
	};


    class Storyboard;
    interface UISDKAPI IStoryboard : public IRootInterface
    {
        UI_DECLARE_Ixxx_INTERFACE_Construct2(IStoryboard, Storyboard);
        UI_DECLARE_Ixxx_INTERFACE_CreateImpl(IStoryboard, Storyboard);

        IMessage*   GetNotifyObj();
        void        SetNotifyObj(IMessage* pNotify);
        void        SetId(int nID);
        int         GetId();
        void        SetWParam(WPARAM wParam);
        WPARAM      GetWParam();
        void        SetLParam(LPARAM lParam);
        LPARAM      GetLParam();

		IIntTimeline*    CreateIntTimeline(int nTimelineId);
		IFloatTimeline*  CreateFloatTimeline(int nTimelineId);
		INoneTimeline*   CreateNoneTimeline(int nTimelineId);

        ITimeline*  GetTimeline(unsigned int nIndex);
        ITimeline*  FindTimeline(int nTimelineId);

        bool  IsFinish();
        void  Begin();
        void  BeginBlock();
        void  BeginDelay(long lElapse);
    };

    class WindowAnimateBase;
    interface UISDKAPI IWindowAnimateBase : public IStoryboard
    {
        UI_DECLARE_Ixxx_INTERFACE_Construct2(IWindowAnimateBase, WindowAnimateBase);
        UI_DECLARE_Ixxx_INTERFACE_CreateImpl(IWindowAnimateBase, WindowAnimateBase);

        void        UpdateWindowSourceImage();
    };

    // 窗口3D旋转动画
    class Window3DAnimate;
	interface UISDKAPI IWindow3DAnimate : public IWindowAnimateBase
	{
        UI_DECLARE_Ixxx_INTERFACE_Construct2(IWindow3DAnimate, Window3DAnimate);
        UI_DECLARE_Ixxx_INTERFACE_CreateImpl(IWindow3DAnimate, Window3DAnimate);

		float*   GetRotateXPtr();
		float*   GetRotateYPtr();
		float*   GetRotateZPtr();

		int*   GetOffsetXPtr();
		int*   GetOffsetYPtr();
		int*   GetOffsetZPtr();
	};

    // 窗口从上到下渐变显示/隐藏动画
    class WindowUpDownAlphaShowAnimate;
    interface UISDKAPI IWindowUpDownAlphaShowAnimate : public IWindowAnimateBase
    {
        UI_DECLARE_Ixxx_INTERFACE_Construct2(IWindowUpDownAlphaShowAnimate, WindowUpDownAlphaShowAnimate);
        UI_DECLARE_Ixxx_INTERFACE_CreateImpl(IWindowUpDownAlphaShowAnimate, WindowUpDownAlphaShowAnimate);

        bool  Show(int nId, int nDuration);
        bool  Hide(int nId, int nDuration);
    };

    // 窗口渐显渐隐动画 

    class WindowAlphaShowAnimate;
    interface UISDKAPI IWindowAlphaShowAnimate : public IWindowAnimateBase
    {
        UI_DECLARE_Ixxx_INTERFACE_Construct2(IWindowAlphaShowAnimate, WindowAlphaShowAnimate);
        UI_DECLARE_Ixxx_INTERFACE_CreateImpl(IWindowAlphaShowAnimate, WindowAlphaShowAnimate);

        bool  Show(int nId, int nDuration);
        bool  Hide(int nId, int nDuration);
    };


    class ControlAnimateBase;
    interface UISDKAPI IControlAnimateBase : public IStoryboard
    {
        UI_DECLARE_Ixxx_INTERFACE_Construct2(IControlAnimateBase, ControlAnimateBase);
        UI_DECLARE_Ixxx_INTERFACE_CreateImpl(IControlAnimateBase, ControlAnimateBase);
    };


    // 两个控件滑动
    class SlideAnimate;
    interface UISDKAPI ISlideAnimate : public IControlAnimateBase
    {
        UI_DECLARE_Ixxx_INTERFACE_Construct2(ISlideAnimate, SlideAnimate);
        UI_DECLARE_Ixxx_INTERFACE_CreateImpl(ISlideAnimate, SlideAnimate);

        bool  Slide(IObject* pObj1Left, IObject* pObj2Right, CRect* prcCommitInWindow, int nFlags);
    };

    class AnimateManager;
	interface UISDKAPI IAnimateManager : public IRootInterface
	{
        IAnimateManager(AnimateManager* p);
        AnimateManager*  GetImpl();

        int   SetFps(int n);

        // void  AddStoryboard(IStoryboard* p); -- 不提供该接口，应该是由Storyboard::Begin内部调用

        void  ClearStoryboardOfNotify(IMessage* pMsg);
        void  RemoveStoryboard(IStoryboard* p);
        void  RemoveStoryboard(IMessage* pMsg, int nId);

        IStoryboard*  CreateStoryboard(IMessage* pNotify, int nId = 0, WPARAM wParam = 0, LPARAM lParam = 0);
        IWindowAnimateBase*  CreateWindowAnimateInstance(E_WINDOW_ANIMATE_TYPE eType, IWindowBase* pWindow);
        IControlAnimateBase*  CreateControlAnimateInstance(E_CONTROL_ANIMATE_TYPE eType);

    private:
        AnimateManager*  m_pImpl;
	}; 
}

#endif // IANIMATE_H_D4A2D364_BE5D_4a61_985C_C09506B05ACB