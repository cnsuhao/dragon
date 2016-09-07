#pragma once

// ole timer��ʱ��
// ע��ֻ��һ��TIMER��Ȼ��������ж���

namespace UI
{
interface  IREOleTimerCallback;
struct  REOleTimerItem
{
	IREOleTimerCallback*  pCallback;
};

class REOleTimer
{
public:
	typedef  list<REOleTimerItem>  _MyList;
	typedef  _MyList::iterator     _MyIter;

    REOleTimer();
    ~REOleTimer();

    void  StartTimer();
    void  KillTimer();
    void  OnTimer();

	bool  Register(IREOleTimerCallback*  pCallback);
	bool  UnRegister(IREOleTimerCallback*  pCallback);

private:
	REOleTimerItem*  FindItem(IREOleTimerCallback*  pCallback);
	bool  FindItem(IREOleTimerCallback*  pCallback, __out _MyIter& iter);

private:
    UINT_PTR  m_nTimerId;
	_MyList   m_items;

};

}