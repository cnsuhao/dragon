#ifndef ANIMATEMGR_H_D4894A37_DD24_41d4_9699_E31C1CE1421E
#define ANIMATEMGR_H_D4894A37_DD24_41d4_9699_E31C1CE1421E

#include "UISDK\Kernel/Inc/Interface/ianimate.h"

namespace UI
{
	// http://msdn.microsoft.com/zh-cn/magazine/cc163397.aspx  ��չ WPF ������
	// http://www.cnblogs.com/Jax/archive/2008/03/26/1123778.html ��Programming WPF������ ��8�� ����

	// http://msdn.microsoft.com/en-us/library/system.windows.media.animation.doubleanimationbase_members(v=vs.85).aspx  DoubleAnimationBase Members

	// �˽�ʲô�������Ի򲻿���ʵ�ֵĹؼ��ǣ�������ľ۽����Ե�����
	
// 	Inheritance Hierarchy (WPF ��̳��б�)
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

	// ��ν��Delegate���Ǿͻػص��ӿڡ���MVC�е� M->C ��ͨѶ��ʽ������ôʵ�ֵ�
	// M�ͻ���ûص��ӿڣ�Ȼ��C����ʵ������ص��ӿڣ�����MC����ͨѶ�ˣ�����MҲ��
	// ���˽�C���κ���Ϣ.
	// Delegate��һ��һ�ģ���Notification��һ�Զ�Ĺ㲥��Ϣ��Target/Action�ǽ�Ӳ����Ϣ
	// ������������̰�ť֮�ࣩת���ɳ�����Ϣ��
	
	// Target-Action?

	// http://gamebabyrocksun.blog.163.com/blog/static/571534632010219544464/  ��Ϸ����ѭ������
	
	// ��WPF����ԭ��
	class ObjectStoryboard  // һ���������ͬʱ��Ӷ��Storyboard��Ϊ��ʵ�ָ��ö���һ��tickֻ����һ��֪ͨ�����ö������е�Storyboard����������
	{
	public:
		ObjectStoryboard(IMessage* p);
		~ObjectStoryboard();

		void  AddStoryboard(IStoryboard* p);
		void  RemoveStoryboard(IStoryboard* p);
		void  CheckFinishFlag();
		int   FindStoryboard(int nID);

		IMessage*      m_pNotifyObject;  // �п���ΪNULL
		IStoryboard**  m_pStoryboardList;
		int            m_nCount;
	};

	typedef list<ObjectStoryboard*>  ObjectStoryboardList;
	typedef list<ObjectStoryboard*>::iterator ObjectStoryboardIter;
    class AnimateManager : public IWaitForHandleCallback
	{
	public:
		AnimateManager();
		~AnimateManager();

		void  AddStoryboard(IStoryboard* p);
		void  ClearStoryboardOfNotify(IMessage* pMsg);
		void  RemoveStoryboard(IStoryboard* p);
		
        IStoryboard*  CreateStoryboard();
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
	protected:
        void    Destroy();

		void    StartAnimate();
		void    EndAnimate();

		void    SetTimer();
		void    KillTimer();

		ObjectStoryboard*  FindObjectStoryboard(IMessage* pNotify);
		ObjectStoryboardIter  FindObjectStoryboardIter(IMessage* pNotify);

	protected:
        IAnimateManager*  m_pIAnimateManager;

		HANDLE m_hTimer;
		int    m_nFps;
		bool   m_bHandlingTimerCallback;


		ObjectStoryboardList  m_listObjStoryboard;
		UIApplication*  m_pUIApplication;
	};
}

#endif  // ANIMATEMGR_H_D4894A37_DD24_41d4_9699_E31C1CE1421E