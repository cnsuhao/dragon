#include "stdafx.h"
#include "uieditorfunction.h"
#include "Dialog\Framework\MainFrame.h"
#include "UISDK\Kernel\Inc\Interface\ixmlwrap.h"
#include "Dialog\Framework\statusbar.h"
#include "Dialog\Module\Layout\LayoutEditorWnd.h"
#include "Dialog\Framework\propertydialog.h"

bool  UE_AddSkin(const TCHAR* szDir, const TCHAR* szSkinName)
{
    return false;
}
// 
// bool  UE_AddObjectAttribute(ISkinRes* pSkinRes, IObject* pObject, IUIElement* pUIElement, const TCHAR* szKey, const TCHAR* szValue)
// {
//     if (NULL == pSkinRes || 
//         NULL == pObject  ||
//         NULL == pUIElement ||
//         NULL == szKey    ||
//         NULL == szValue)
//         return false;
// 
//     if (pUIElement->HasAttrib(szKey))
//     {
//         String  str = _T("�������Ѵ���: ");
//         str.append(szKey);
//         g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());
//         return false;
//     }
// 
//     if (false == pUIElement->AddAttrib(szKey, szValue))
//     {
//         String  str = _T("�������ʧ��: ");
//         str.append(szKey);
//         str.append(_T(" = \""));
//         str.append(szValue);
//         str.append(_T("\""));
//         g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());
//         return false;
//     }
// 
//     // ����dirty����
//     IUIDocument* pDoc = NULL;
//     pUIElement->GetDocument(&pDoc);
//     pDoc->SetDirty(true);
//     SAFE_RELEASE(pDoc);
// 
//     g_pGlobalData->m_pMainFrame->SetDirty();
// 
//     // ˢ�½���
//     UE_ReloadObjectAttribute(pSkinRes, pObject, pUIElement);
//     UE_RefreshLayoutView(pSkinRes, pObject);
// 
//     return true;
// }

//
//  ɾ��ָ�������ĳ������
//
// bool  UE_RemoveObjectAttribute(ISkinRes* pSkinRes, IObject* pObject, IUIElement* pUIElement, const TCHAR* szAttrib)
// {
//     if (NULL == pSkinRes || 
//         NULL == pObject  ||
//         NULL == pUIElement ||
//         NULL == szAttrib)
//         return false;
// 
//     if (!pUIElement->HasAttrib(szAttrib))
//     {
//         String  str = _T("�����Բ�����: ");
//         str.append(szAttrib);
//         g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());
//         return false;
//     }
// 
//     if (false == pUIElement->RemoveAttrib(szAttrib))
//     {
//         String  str = _T("ɾ�������ֶ�ʧ��: ");
//         str.append(szAttrib);
//         g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());
//         return false;
//     }
// 
//     // ����dirty����
//     IUIDocument* pDoc = NULL;
//     pUIElement->GetDocument(&pDoc);
//     pDoc->SetDirty(true);
//     SAFE_RELEASE(pDoc);
// 
//     g_pGlobalData->m_pMainFrame->SetDirty();
// 
//     // ˢ�½���
//     UE_ReloadObjectAttribute(pSkinRes, pObject, pUIElement);
//     UE_RefreshLayoutView(pSkinRes, pObject);
//     return true;
// }

//
// ��ն�����������
//
// bool  UE_ClearObjectAttribute(ISkinRes* pSkinRes, IObject* pObject, IUIElement* pUIElement)
// {
//     if (NULL == pSkinRes ||
//         NULL == pObject  ||
//         NULL == pUIElement)
//         return false;
// 
//     if (false == pUIElement->ClearAttrib())
//     {
//         String  str = _T("��ն�������ʧ��");
//         g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());
//         return false;
//     }
// 
//     // ����dirty����
//     IUIDocument* pDoc = NULL;
//     pUIElement->GetDocument(&pDoc);
//     pDoc->SetDirty(true);
//     SAFE_RELEASE(pDoc);
// 
//     g_pGlobalData->m_pMainFrame->SetDirty();
// 
//     // ˢ�½���
//     UE_ReloadObjectAttribute(pSkinRes, pObject, pUIElement);
//     UE_RefreshLayoutView(pSkinRes, pObject);
//     return true;
// }

//
// �޸Ķ�������
//
// bool  UE_ModifyObjectAttribute(ISkinRes* pSkinRes, IObject* pObject, IUIElement* pUIElement, const TCHAR* szKey, const TCHAR* szValue)
// {
//     if (NULL == pSkinRes || 
//         NULL == pObject  ||
//         NULL == pUIElement ||
//         NULL == szKey    ||
//         NULL == szValue)
//         return false;
// 
//     if (!pUIElement->HasAttrib(szKey))
//     {
//         String  str = _T("�����Բ�����: ");
//         str.append(szKey);
//         g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());
//         return false;
//     }
// 
//     if (false == pUIElement->ModifyAttrib(szKey, szValue))
//     {
//         String  str = _T("�޸������ֶ�ʧ��: ");
//         str.append(szKey);
//         str.append(_T(" = \""));
//         str.append(szValue);
//         str.append(_T("\""));
//         g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());
//         return false;
//     }
// 
//     // ����dirty����
//     IUIDocument* pDoc = NULL;
//     pUIElement->GetDocument(&pDoc);
//     pDoc->SetDirty(true);
//     SAFE_RELEASE(pDoc);
// 
//     g_pGlobalData->m_pMainFrame->SetDirty();
// 
//     // ˢ�½���
//     UE_ReloadObjectAttribute(pSkinRes, pObject, pUIElement);
//     UE_RefreshLayoutView(pSkinRes, pObject);
//     return true;
// }

// 
//  ���¼��ض������ԣ���ˢ��
//
bool  UE_ReloadObjectAttribute(ISkinRes* pSkinRes, IObject* pObject, IUIElement* pUIElement)
{
	IPropertyHandler* pHandler = g_pGlobalData->m_pPropertyDialog->GetHandler();
	if (!pHandler)
		return false;

	if (pHandler->GetHandlerType() != LayoutPropertyHandler::PROPERTY_HANDLER_TYPE)
		return false;

	static_cast<LayoutPropertyHandler*>(pHandler)->ReloadObjectAttribute(pObject);
	return true;
}

// ����������������ĳһЩ��ֵΪpObject������ֵ
void  UE_UpdateAttributeInPropertyCtrl(IObject* pObject, LPCTSTR szKey)
{
	IPropertyHandler* pHandler = g_pGlobalData->m_pPropertyDialog->GetHandler();
	if (!pHandler)
		return;

	if (pHandler->GetHandlerType() != LayoutPropertyHandler::PROPERTY_HANDLER_TYPE)
		return;

	static_cast<LayoutPropertyHandler*>(pHandler)->UpdateObjectAttributeInCtrl(pObject, szKey);

// 	if (g_pGlobalData->m_pPropertyDialog)
// 		g_pGlobalData->m_pPropertyDialog->UpdateObjectAttributeInCtrl(pObject, szKey);
}

// ˢ�²�����
void  UE_RefreshProjectTree()
{
    g_pGlobalData->m_pProjectTreeDialog->m_pTreeLayout->LayoutAllItemsAndRedraw();
}

// ����ˢ��������ʾ
void  UE_RefreshPropertyDlg(IObject* pObject)
{
	IPropertyHandler* pHandler = g_pGlobalData->m_pPropertyDialog->GetHandler();
	if (!pHandler)
		return;

	if (pHandler->GetHandlerType() != LayoutPropertyHandler::PROPERTY_HANDLER_TYPE)
		return;

	static_cast<LayoutPropertyHandler*>(pHandler)->ReloadObjectAttribute(pObject);
}

// ˢ��UI����
void  UE_RefreshUIWindow(IObject* pObject)
{
    if (!pObject)
        return;

    IWindowBase*  pWindowBase = pObject->GetWindowObject();
    if (!pWindowBase)
        return;

    pWindowBase->UpdateMyLayout(false);

    // ǿ��ˢ��
    pWindowBase->UpdateWindow(NULL, NULL);
}

//
//  ˢ�´���Ԥ����ͼ��ͬʱ���´��ڴ�С���Խ���޸Ĵ��ڴ�Сʱlayoutview::m_rcWindowDraw
//
bool  UE_RefreshLayoutView(IObject* pObject)
{
    HWND hWnd = g_pGlobalData->m_pMainFrame->GetLayoutHWND(pObject);
    if (hWnd)
    {
        ::SendMessage(hWnd, CLayoutEditorDlg::WM_USER_REFRESH, 0, 0);
        return true;
    }

    return false;
}


HRESULT  UE_ExecuteCommand(ICommand* p, bool bDelay)
{
	if (bDelay)
	{
		g_pGlobalData->m_pCmdHistroyMgr->PostExecute(p);
		return 0;
	}
	else
	{
		return g_pGlobalData->m_pCmdHistroyMgr->Execute(p);
	}
}