#ifndef IUIINTERFACE_H_201504131212
#define IUIINTERFACE_H_201504131212

// IUIApplication接口
namespace UI
{
	interface  IToolTipUI;
	interface  IWindowBase;
	interface  ISkinManager;
	interface  ISkinRes;
	struct     TOOLTIPITEM;
	interface  IRenderBitmap;
	interface  IRenderFont;
	interface  ISkinConfigRes;
	interface  ISkinConfigManager;
	interface  ITopWindowManager;
	interface  IAnimateManager;
	interface  IWaitForHandlesMgr;
	interface  IRenderTarget;
	interface  ITrayIcon;
	interface  IMessageFilterMgr;
    interface  IMapAttribute;
    interface  IObject;
    interface  IUIDocument;

	// 句柄定义
	interface IUIApplication;

    interface ILayoutWindowNodeList  
    {
        virtual void  Release() PURE;
        virtual UINT  GetCount() PURE;
        virtual LPCTSTR  GetWindowName(UINT index) PURE;
        virtual LPCTSTR  GetWindowId(UINT index) PURE;
        virtual LPCTSTR  GetWindowPath(UINT index) PURE;
    };
    
    class TopWindowManager;
    interface UISDKAPI ITopWindowManager : public IRootInterface
    {
        ITopWindowManager(TopWindowManager* p);
        TopWindowManager* GetImpl();

        HRESULT  AddTopWindowObject(IWindowBase*);
        HRESULT  RemoveTopWindowObject(IWindowBase*);
        void     ChangeSkin(ISkinRes* pNewSkinRes);

        bool  UpdateAllWindow();
        void  SendMessage2AllWnd(UIMSG*  pMsg);
        void  PostMessage2AllWnd(UIMSG* pMsg);
        void  ForwardMessage2AllObj(UIMSG*  pMsg);

    private:
        TopWindowManager*  m_pImpl;
    };

    
    //////////////////////////////////////////////////////////////////////////

	//
	// 各种提示条绘制的抽象类	
	//
	interface IToolTipUI
	{
	public:
		virtual ~IToolTipUI() = 0 {};
		virtual bool  Create()  = 0;
		virtual bool  Destroy() = 0;
		virtual bool  SetText(LPCTSTR szText) = 0;
		virtual bool  SetTitle(LPCTSTR szText) = 0;
		virtual bool  Show(HWND hWndParent) = 0;
		virtual bool  Hide() = 0;
		virtual bool  SetUIApplication(IUIApplication* p) = 0;
		virtual void  OnSerialize(SERIALIZEDATA* pData) = 0;
	};

	interface IWaitForHandleCallback
	{
		virtual void OnWaitForHandleObjectCallback(HANDLE, LPARAM) = 0;
	};
	interface IWaitForHandlesMgr : public IRootInterface
	{
		virtual bool  AddHandle(HANDLE h, IWaitForHandleCallback* pCB, LPARAM l) = 0;
		virtual bool  RemoveHandle(HANDLE h) = 0;
	};

	interface IFlashInvalidateListener
	{
		virtual void OnInvalidateRect(LPCRECT, BOOL) = 0;
	};

	interface IPreTranslateMessage  // IMessageFilter 被系统其它地方已经定义过了
	{
	public:
		virtual BOOL PreTranslateMessage(MSG* pMsg) = 0;
	};

	interface IMessageFilterMgr : public IRootInterface
	{
		virtual void  AddMessageFilter(IPreTranslateMessage* p) = 0;
		virtual void  RemoveMessageFilter(IPreTranslateMessage* p) = 0;
	};

    //
    // 用于监听每次窗口调用CommitDoubleBuffet2Window(里面的bitblt)时，向该接口对象发送pre/post通知
    // 原本是用于实现 Caret/DrawFocusRect这种会直接画在窗口上面，又是采用异或方向绘制的对象，在提
    // 交之前选隐藏，在提前之后再显示一次，避免提交数据导致的顺序错乱
    //
    // 以后可用于扩展实时获取数据窗口变化的接口
    //
    interface ICommitWindowBufferListener
    {
        virtual void  PreCommitWindowBuffer(HDC hDC, IRenderTarget* pRenderTarget, IWindowBase* pWindow, RECT* prcCommit, int nRectCount) = 0;
        virtual void  PostCommitWindowBuffer(HDC hDC, IRenderTarget* pRenderTarget, IWindowBase* pWindow, RECT* prcCommit, int nRectCount) = 0;

        // 用于标明该listener只一次有效。在检测完一次后就将该listener删除
        virtual bool  OnlyListenOnce() = 0; 
    };
}

#endif // IUIINTERFACE_H_201504131212