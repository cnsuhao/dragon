#include "stdafx.h"
#include "topwindowmanager.h"

#include "uisdk\Kernel\Inc\Interface\iobject.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Kernel\Src\Helper\layout\layout.h"
#include "UISDK\Kernel\Src\Resource\skinres.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Src\Base\Application\uiapplication.h"

TopWindowManager::TopWindowManager(UIApplication* p)
{
    m_pITopWindowManager = NULL;
    m_pUIApplication = p;
}
TopWindowManager::~TopWindowManager()
{
    SAFE_DELETE(m_pITopWindowManager);
}
ITopWindowManager*  TopWindowManager::GetITopWindowManager()
{
    if (NULL == m_pITopWindowManager)
        m_pITopWindowManager = new ITopWindowManager(this);

    return m_pITopWindowManager;
}

HRESULT TopWindowManager::AddTopWindowObject(WindowBase* pObj)
{
	if (NULL == pObj)
		return E_INVALIDARG;

	_MyIter  iter = m_lTopWindowObject.begin();
	_MyIter  iterEnd = m_lTopWindowObject.end();
	for (; iter!=iterEnd; iter++)
	{
		if (pObj == *iter)
			return S_FALSE;
	}

	this->m_lTopWindowObject.push_back( pObj );

	UI_LOG_DEBUG( _T("TopWindowManager::AddTopWindowObject, ID=%s"), pObj->GetId());
	return S_OK;
}

HRESULT TopWindowManager::RemoveTopWindowObject(WindowBase* pObj)
{
	if (NULL == pObj)
		return E_INVALIDARG;

	_MyIter  iter = m_lTopWindowObject.begin();
	_MyIter  iterEnd = m_lTopWindowObject.end();
	for(; iter!=iterEnd; iter++)
	{
		if (pObj == *iter)
		{
			m_lTopWindowObject.erase( iter );

			UI_LOG_DEBUG( _T("TopWindowManager::RemoveTopWindowObject, ID=%s"), pObj->GetId());
			return S_OK;
		}
	}
	return E_FAIL;
}

void TopWindowManager::ClearTopWindowObject( )
{
	this->m_lTopWindowObject.clear();

	UI_LOG_DEBUG( _T("TopWindowManager::ClearTopWindowObject ") );
}

IWindowBase* TopWindowManager::GetWindowBase(HWND hWnd)
{
	_MyIter  iter = m_lTopWindowObject.begin();
	_MyIter  iterEnd = m_lTopWindowObject.end();
	for( ; iter!=iterEnd; iter++ )
	{
		WindowBase* p = *iter;
		if( p->GetHWND() == hWnd )
			return p->GetIWindowBase();
	}

	return NULL;
}

//
//	修改皮肤，让当前所有窗口重新加载
//
void TopWindowManager::ChangeSkin(SkinRes* pNewSkinRes)
{
	if( NULL == pNewSkinRes )
		return;

	// ps: ??? 不换肤的窗口应该不存在吧...至少只要它还使用皮肤资源的话，就要换肤 （2012.12.17）
	vector<WindowBase*>  vecNotJoinChangeSkinWindows;  // 将不参与换肤的窗口放在该列表中，这样只需要发送一次UI_WM_SKINCHANGING消息

	_MyIter  iter = m_lTopWindowObject.begin();
	for( ; iter!=m_lTopWindowObject.end(); iter++ )
	{
		WindowBase* pWindow = *iter;
		if (NULL == pWindow)
			continue;

		BOOL bChangeSkin = TRUE;
		UISendMessage(pWindow, UI_WM_SKINCHANGING, (WPARAM)&bChangeSkin, 0);
		if (FALSE == bChangeSkin)
		{
			vecNotJoinChangeSkinWindows.push_back(pWindow);
			continue;
		}

		//////////////////////////////////////////////////////////////////////////
		//
		// 1. 将所有的子元素重列，用于后面重新布局

		list<Object*>  listAllChildObjects;
		this->GetAllChildIntoList((*iter), listAllChildObjects);

		// 清除所有对象的组织关系
		list<Object*>::iterator iter2 = listAllChildObjects.begin();
		for (; iter2 != listAllChildObjects.end(); iter2++)
		{
			(*iter2)->ClearMyTreeRelationOnly();
		}
		pWindow->ClearMyTreeRelationOnly();
        UISendMessage(pWindow, UI_WM_RESETATTRIBUTE);  // 这里多调用一次，用于防止ReLoadLayout失败，子对象全部被删除，而窗口的属性又没有被重置。
		pNewSkinRes->ReLoadLayout(pWindow, listAllChildObjects);

		// 将剩余没有再被使用的object释放掉
		iter2 = listAllChildObjects.begin();
		for ( ; iter2 != listAllChildObjects.end(); iter2++ )
		{
            Object* pObj = *iter2;
            pWindow->GetMouseMgr()->OnObjectRemoveInd(pObj);   // <-- 由于pObj已经没不到window对象了，因此在这里单独调用
            
			pObj->GetIObject()->delete_this();
			pObj = NULL;
		}
		listAllChildObjects.clear();
	}
	
	//
	// 刷新窗口，放在后面统一刷新，因为ChangeSkin操作比较慢，会导致各个窗口之后有时间差
	//
	// 注：对于分层窗口，如果SetWindowPos前后SIZE未变的话，则分层窗口收不到WM_SIZE消息，
	//	   这也将导致分层窗口得不到刷新，因此在这里需要针对该情况单独处理。
	//	   TODO: 有没有直接无视大小相同也发送WM_SIZE的方法？
	//
	for (iter = m_lTopWindowObject.begin(); iter!=m_lTopWindowObject.end(); iter++)
	{
		WindowBase* pWindow = *iter;
		if (NULL == pWindow)
			continue;

		// 该窗口不参与换肤
		int nSize = (int)vecNotJoinChangeSkinWindows.size();
		for (int i = 0; i < nSize; i++)
		{
			if (vecNotJoinChangeSkinWindows[i] == pWindow)
				continue;
		}

// 		int nOldWidth = pWindow->GetWidth();
// 		int nOldHeight = pWindow->GetHeight();

		DesktopLayout dl;
		dl.Arrange(pWindow);

        // 通知
        UISendMessage(pWindow, UI_WM_SKINCHANGED, 0, 0);  // <-- 放在arrange后面，是为了能够在响应中正确拿到控件位置
                                                          // <-- 放在Update前面，是为了能够在重绘之前重新获取控件指针，避免崩溃

//		::SetWindowPos( pWindow->m_hWnd, NULL, pWindow->GetParentRectL(), pWindow->GetParentRectT(), 
//			pWindow->GetWidth(), pWindow->GetHeight(), SWP_NOZORDER|SWP_NOACTIVATE );  // 在这里由于资源都已经销毁了，应该防止窗口再刷新

// 		if (nOldWidth == pWindow->GetWidth() && nOldHeight == pWindow->GetHeight())
 		{
// 			if (WS_EX_LAYERED & ::GetWindowLong(pWindow->m_hWnd,GWL_EXSTYLE))
// 			{
				pWindow->UpdateObject();
// 			}
 		}
	}
}

//
//	改变当前皮肤的色调
//
bool TopWindowManager::UpdateAllWindow( )
{
	_MyIter  iter = m_lTopWindowObject.begin();
	_MyIter  iterEnd = m_lTopWindowObject.end();

	// 刷新窗口
	for( iter = m_lTopWindowObject.begin(); iter!=iterEnd; iter++ )
	{
		WindowBase* pWindow = *iter;
        if (NULL == pWindow)
            continue;

		pWindow->UpdateObject();
	}
	return true;
}

//
//	向所有的顶层窗口发送消息
//
void TopWindowManager::SendMessage2AllWnd(UIMSG* pMsg)
{
	_MyIter  iter = m_lTopWindowObject.begin();
	_MyIter  iterEnd = m_lTopWindowObject.end();
	for ( ; iter!=iterEnd; iter++)
	{
		 WindowBase* pObj = *iter;
         if (NULL == pObj)
             continue;

		 pMsg->pMsgTo = pObj->GetIMessage();
		 ::UISendMessage(pMsg);
	}
}
void  TopWindowManager::PostMessage2AllWnd(UIMSG* pMsg)
{
    _MyIter  iter = m_lTopWindowObject.begin();
    for (; iter != m_lTopWindowObject.end(); iter++)
    {
        WindowBase* pObj = *iter;
        if (NULL == pObj)
            continue;

        pMsg->pMsgTo = pObj->GetIMessage();
        ::UIPostMessage(m_pUIApplication->GetIUIApplication(), pMsg);
    }
}
void  TopWindowManager::ForwardMessage2AllObj(UIMSG*  pMsg)
{
    _MyIter  iter = m_lTopWindowObject.begin();
    for (; iter != m_lTopWindowObject.end(); iter++)
    {
        WindowBase* pObj = *iter;
        if (NULL == pObj)
            continue;

        pMsg->pMsgTo = pObj->GetIMessage();
        UISendMessage(pMsg);

        pObj->ForwardMessageToChildObject(pObj, pMsg);
    }
}

//
//	将一个对象的子对象打散，全放在一个list中用于重新布局，对于一个OBJ_CONTROL类型则不再拆分，它的child和本身属性一个整体
//
void TopWindowManager::GetAllChildIntoList(Object* pParent, list<Object*>& listObjects)
{
	if (NULL == pParent || OBJ_CONTROL == pParent->GetObjectType())
		return;

	Object*  pChild = NULL;
	while (pChild = pParent->EnumChildObject(pChild))
	{
		listObjects.push_back(pChild);

		if (pChild->GetObjectType() != OBJ_CONTROL)
			this->GetAllChildIntoList(pChild, listObjects);
	}
}


