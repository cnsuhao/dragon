#pragma once

namespace UI
{
#if 0  // -- 过期，没什么用了
class WindowBase;
class CommitWindowBufferListenerMgr
{
public:
    typedef list<ICommitWindowBufferListener*> _MyList;
	typedef list<ICommitWindowBufferListener*>::iterator _MyIter;

	CommitWindowBufferListenerMgr();
	~CommitWindowBufferListenerMgr();

	void  DoPre(HDC hDC, IRenderTarget* pRenderTarget, RECT* prc, int nRectCount);
	void  DoPost(HDC hDC, IRenderTarget* pRenderTarget, RECT* prc, int nRectCount);
	void  SetWindowPtr(WindowBase* p){ m_pWindow = p;}

	void  AddListener(ICommitWindowBufferListener* p);
	void  RemoveListener(ICommitWindowBufferListener* p);
	void  ClearListener();
	int   GetListenerCount() { return (int)m_list.size(); }

protected:
	list<ICommitWindowBufferListener*>  m_list;
	WindowBase*   m_pWindow;
};
#endif
}