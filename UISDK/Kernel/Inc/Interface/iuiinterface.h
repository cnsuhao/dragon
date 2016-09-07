#ifndef IUIINTERFACE_H_201504131212
#define IUIINTERFACE_H_201504131212

// IUIApplication�ӿ�
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

	// �������
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
	// ������ʾ�����Ƶĳ�����	
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

	interface IPreTranslateMessage  // IMessageFilter ��ϵͳ�����ط��Ѿ��������
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
    // ���ڼ���ÿ�δ��ڵ���CommitDoubleBuffet2Window(�����bitblt)ʱ����ýӿڶ�����pre/post֪ͨ
    // ԭ��������ʵ�� Caret/DrawFocusRect���ֻ�ֱ�ӻ��ڴ������棬���ǲ����������ƵĶ�������
    // ��֮ǰѡ���أ�����ǰ֮������ʾһ�Σ������ύ���ݵ��µ�˳�����
    //
    // �Ժ��������չʵʱ��ȡ���ݴ��ڱ仯�Ľӿ�
    //
    interface ICommitWindowBufferListener
    {
        virtual void  PreCommitWindowBuffer(HDC hDC, IRenderTarget* pRenderTarget, IWindowBase* pWindow, RECT* prcCommit, int nRectCount) = 0;
        virtual void  PostCommitWindowBuffer(HDC hDC, IRenderTarget* pRenderTarget, IWindowBase* pWindow, RECT* prcCommit, int nRectCount) = 0;

        // ���ڱ�����listenerֻһ����Ч���ڼ����һ�κ�ͽ���listenerɾ��
        virtual bool  OnlyListenOnce() = 0; 
    };
}

#endif // IUIINTERFACE_H_201504131212