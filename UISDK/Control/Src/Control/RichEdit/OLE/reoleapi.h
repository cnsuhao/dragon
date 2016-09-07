#pragma once
#include "UISDK\Control\Inc\Interface\iricheditole.h"

// REOLE �е�һЩȫ�ֺ���

namespace UI
{
	// ��̬��Դ 
	void  SetREOleResUIApplication(IUIApplication* pUIApp);
	

	// ��ʱ��
	interface IREOleTimerCallback
	{
		virtual void  OnREOleTimer() PURE;
	};
	bool  RegisterREOleTimer(IREOleTimerCallback*  pCallback);
	bool  UnRegisterREOleTimer(IREOleTimerCallback*  pCallback);


	// ��Դ����
	interface IREOleResUpdateCallback
	{
		virtual void  OnREOleResUpdate(REOleLoadStatus e, LPCTSTR szPath, long lData) PURE;
	};

	bool  RegisterREOleResUpdate(LPCTSTR  szMD5, IREOleResUpdateCallback* pCallback, long lData);
	bool  UnRegisterREOleResUpdate(LPCTSTR  szMD5, IREOleResUpdateCallback* pCallback);
}