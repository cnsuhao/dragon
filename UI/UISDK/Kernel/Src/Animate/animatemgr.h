#ifndef ANIMATEMGR_H_D4894A37_DD24_41d4_9699_E31C1CE1421E
#define ANIMATEMGR_H_D4894A37_DD24_41d4_9699_E31C1CE1421E

#include "UISDK\Kernel/Inc/Interface/ianimate.h"

namespace UI
{
	// http://msdn.microsoft.com/zh-cn/magazine/cc163397.aspx  扩展 WPF 动画类
	// http://www.cnblogs.com/Jax/archive/2008/03/26/1123778.html 《Programming WPF》翻译 第8章 动画

	// http://msdn.microsoft.com/en-us/library/system.windows.media.animation.doubleanimationbase_members(v=vs.85).aspx  DoubleAnimationBase Members

	// 了解什么动画可以或不可以实现的关键是，理解它的聚焦属性的天性
	
// 	Inheritance Hierarchy (WPF 类继承列表)
// 		--------------------------------------------------------------------------------
// 
// 		System.Object
// 		System.Windows.Threading.DispatcherObject
// 		System.Windows.DependencyObject
// 		System.Windows.Freezable
// 		System.Windows.Media.Animation.Animatable
// 		System.Windows.Media.Animation.Timeline
// 		System.Windows.Media.Animation.AnimationTimeline
// 		System.Windows.Media.Animation.DoubleAnimationBase
// 		System.Windows.Media.Animation.DoubleAnimation
// 		System.Windows.Media.Animation.DoubleAnimationUsingKeyFrames
// 		System.Windows.Media.Animation.DoubleAnimationUsingPath

	// 所谓的Delegate其是就回回调接口。在MVC中的 M->C 的通讯方式就是这么实现的
	// M就会调用回调接口，然后C必须实现这个回调接口，这样MC就能通讯了，而且M也不
	// 用了解C的任何信息.
	// Delegate是一对一的，而Notification是一对多的广播消息，Target/Action是将硬件消息
	// （鼠标点击，键盘按钮之类）转换成程序消息。
	
	// Target-Action?

	// http://gamebabyrocksun.blog.163.com/blog/static/571534632010219544464/  游戏引擎循环机制
	
	// 仿WPF动画原理
    // 一个对象可能同时添加多个Storyboard。为了实现给该对象一次tick只发送一个通知，将该对象所有的Storyboard串起来管理
    // PS: 如果去掉ObjectStoryboard这一层逻辑会简单很多……一个对象同时添加多个Storyboard概率很多吗？


	/*  motion blur<<运动模糊>>

	● 游戏中为什么需要模糊处理？清晰一点不是更好吗？

		为什么每秒24帧的速度对于电影来说已经足以获得很流畅的视觉效果，而
		对于电脑游戏来说却会显得磕磕碰碰呢？原因很简单，摄像机在工作的时
		候并非一帧一帧静止的拍摄，它所摄下的每一帧已经包含了1/24秒以内的
		所有视觉信息，包括物体的位移。如果在看录像的时候按下暂停键，我们
		得所到的并不是一幅清晰的静止画面，而是一张模糊的图像，原因就在于
		此。电脑做不到这一点，游戏里的每一帧就是一幅静止画面，如果你在运
		动的过程中抓一张图片下来，得到的肯定是一幅清晰的静态图。运动模糊
		技术的目的就在于增强快速移动场景的真实感，这一技术并不是在两帧之
		间插入更多的位移信息，而是将当前帧同前一帧混合在一起所获得的一种
		效果。
	*/

	class ObjectStoryboard  
	{
	public:
		ObjectStoryboard(IMessage* p);
		~ObjectStoryboard();

		void  AddStoryboard(IStoryboard* p);
		void  RemoveStoryboard(IStoryboard* p);
        void  RemoveStroyboard(int nId);
		void  CheckFinishFlag();
		int   FindStoryboard(int nId);
        IStoryboard*   FindStoryboard2(int nId);
        void  SetNeedCheckFinishFlag(bool b)
                { m_bNeedCheckFinish = b; }

		IMessage*      m_pNotifyObject;  // 有可能为NULL
		IStoryboard**  m_pStoryboardList;
		int            m_nCount;

    private:
        bool   m_bNeedCheckFinish;         // 如果有一个storyboard结束了，将设置该标识。在OnWaitForHandleObjectCallback中将检查该标识
	};

	typedef list<ObjectStoryboard*>  ObjectStoryboardList;
	typedef list<ObjectStoryboard*>::iterator ObjectStoryboardIter;
    class AnimateManager : public IWaitForHandleCallback
	{
	public:
		AnimateManager();
		~AnimateManager();

		void  AddStoryboard(IStoryboard* p);
        void  AddStoryboardBlock(IStoryboard* p);
		void  ClearStoryboardOfNotify(IMessage* pMsg);
        void  RemoveStoryboard(IMessage* pMsg, int nId);
		void  RemoveStoryboard(IStoryboard* p);
        ObjectStoryboard*  FindObjectStoryboard(IMessage* pNotify);
		
        IStoryboard*  CreateStoryboard(IMessage* pNotify, int nId = 0, WPARAM wParam = 0, LPARAM lParam = 0);
		IWindowAnimateBase*  CreateWindowAnimateInstance(E_WINDOW_ANIMATE_TYPE eType, IWindowBase* pWindow);
        IControlAnimateBase* CreateControlAnimateInstance(E_CONTROL_ANIMATE_TYPE eType);

        IAnimateManager*  GetIAnimateManager();
	public:
		void    SetUIApplication(UIApplication* p) 
        {
            m_pUIApplication = p; 
        }
		int     GetFps() { return m_nFps; }
		int     SetFps(int n);

        virtual void OnWaitForHandleObjectCallback(HANDLE, LPARAM);
		void    OnTime();
	protected:
        void    Destroy();

		void    StartAnimate();
		void    EndAnimate();

		void    SetTimer();
		void    KillTimer();

		ObjectStoryboardIter  FindObjectStoryboardIter(IMessage* pNotify);

	protected:
        IAnimateManager*  m_pIAnimateManager;
		LARGE_INTEGER     m_liPerFreq;     // 用于帧数计算

		HANDLE m_hTimer;
		int    m_nFps;
		bool   m_bHandlingTimerCallback;   // 避免在遍历stl过程中外部又删除一个对象，导致崩溃

		ObjectStoryboardList  m_listObjStoryboard;
		UIApplication*  m_pUIApplication;
	};
}

#endif  // ANIMATEMGR_H_D4894A37_DD24_41d4_9699_E31C1CE1421E