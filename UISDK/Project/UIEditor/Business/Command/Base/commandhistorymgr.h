#pragma once
interface ICommand;

// 2014.4.12
// һ�������У�ÿһ�����ڽ���Ӧһ����ʷ��¼��������ֻ��һ����ʷ��¼
// �����ڹر�ʱ���ô��ڶ�Ӧ����ʷ��¼����ɾ��������vs����ʷ��¼��
//

class CCommandHistroy
{
public:
	CCommandHistroy();
	~CCommandHistroy();

	void  Clear();

	HRESULT  Execute(ICommand* p);
	void  Undo();
	void  Redo();

protected:
	void  NotifyUndoRedoCountChanged();
    void  clearRedo();

public:
	deque<ICommand*>   m_stack_undo;
	deque<ICommand*>   m_stack_redo;

	int     m_nMaxHistorySize;
	long*   m_pKey;
};



class CCommandHistroyMgr : public IMessage
{
public:
#define POST_EXECUTE_MSG  (WM_USER+1)


	CCommandHistroyMgr();
	~CCommandHistroyMgr();

	UI_BEGIN_MSG_MAP_Ixxx(CCommandHistroyMgr)
		UIMESSAGE_HANDLER_EX(POST_EXECUTE_MSG, OnPostExecute)
	UI_END_MSG_MAP

	HRESULT  Execute(ICommand* p);
	void  PostExecute(ICommand* p);  
	void  Undo(long* pKey);
	void  Redo(long* pKey);

	bool  CloseElem(long* pKey);
	void  GetUndoRedoCount(long* pKey, int* pnUndo, int* pnRedo);

	CCommandHistroy*  GetElemByKey(long*  pKey);

protected:
	LRESULT  OnPostExecute(UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	typedef  map<long*, CCommandHistroy*>  _MyMap;
	typedef  _MyMap::iterator  _MyIter;

	map<long*, CCommandHistroy*>  m_mapElement;
};