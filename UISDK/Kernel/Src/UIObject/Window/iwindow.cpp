#include "stdafx.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Src\UIObject\Window\window.h"
#include "UISDK\Kernel\Src\RenderLayer2\layer\windowrender.h"
#include "UISDK\Kernel\Src\Resource\skinres.h"

using namespace UI;

UI_IMPLEMENT_Ixxx_INTERFACE(IWindowBase, WindowBase, Panel)
UI_IMPLEMENT_Ixxx_INTERFACE(IWindow, Window, WindowBase)

HWND  IWindowBase::GetHWND()                                              
{ 
	return m_pWindowBaseImpl->GetHWND(); 
}
BOOL  IWindowBase::IsChildWindow()                                        
{
	return m_pWindowBaseImpl->IsChildWindow(); 
}
BOOL  IWindowBase::IsWindowVisible()
{
	return m_pWindowBaseImpl->IsWindowVisible(); 
}
void  IWindowBase::ShowWindow()                                           
{
	m_pWindowBaseImpl->ShowWindow();
}
void  IWindowBase::HideWindow()                                            
{ 
	m_pWindowBaseImpl->HideWindow(); 
}
void  IWindowBase::CenterWindow(HWND hWndCenter)                           
{
	m_pWindowBaseImpl->CenterWindow(hWndCenter);
}
void  IWindowBase::CenterWindow(HMONITOR hMonitor)                         
{
	m_pWindowBaseImpl->CenterWindow(hMonitor);
}
bool  IWindowBase::IsDoModal()                                             
{ 
	return m_pWindowBaseImpl->IsDoModal(); 
}
IWindowRender*  IWindowBase::GetIWindowRender()                            
{ 
	return m_pWindowBaseImpl->GetWindowRender()->GetIWindowRender();
}
ISkinRes*  IWindowBase::GetSkinRes()                                      
{ 
	return m_pWindowBaseImpl->GetSkinRes()->GetISkinRes(); 
}
void   IWindowBase::SetFocusObject(IObject* pObj)
{
    Object*  p = NULL;
    if (pObj)
        p = pObj->GetImpl();

    m_pWindowBaseImpl->SetFocusObject(p);
}

void  IWindowBase::SetPressObject(IObject* pObj)
{
	Object*  p = NULL;
	if (pObj)
		p = pObj->GetImpl();

	m_pWindowBaseImpl->SetPressObject(p);
}

IObject*  IWindowBase::GetHoverObject()
{
    Object* p = m_pWindowBaseImpl->GetHoverObject();
    if (p)
        return p->GetIObject();
    return NULL;
}
IObject*  IWindowBase::GetPressObject()
{
    Object* p = m_pWindowBaseImpl->GetPressObject();
    if (p)
        return p->GetIObject();
    return NULL;
}
IObject*  IWindowBase::GetObjectByPos(IObject* pObjParent, POINT* pt, bool bSkinBuilderInvoke)
{ 
    if (NULL == pObjParent)
        return NULL;

    Object* pRet = m_pWindowBaseImpl->GetObjectByPos(pObjParent->GetImpl(), pt, bSkinBuilderInvoke); 
    if (pRet)
        return pRet->GetIObject();

    return NULL;
}
bool  IWindowBase::Create(const TCHAR* szID, HWND hWndParent, RECT* prc) 
{
	return m_pWindowBaseImpl->Create(szID, hWndParent, prc);
}
void  IWindowBase::DestroyWindow()
{
	return m_pWindowBaseImpl->DestroyWindow();
}
bool  IWindowBase::Attach(HWND hWnd, const TCHAR* szID) 
{ 
	return m_pWindowBaseImpl->Attach(hWnd, szID);
}
void  IWindowBase::Detach()
{
	m_pWindowBaseImpl->Detach(); 
}
long  IWindowBase::DoModal(const TCHAR* szID, HWND hWndParent, bool canResize ) 
{
	return m_pWindowBaseImpl->DoModal(szID, hWndParent, canResize);
}
long  IWindowBase::DoModal(HINSTANCE hResInst, UINT nResID, const TCHAR* szID, HWND hWndParent)
{
	return m_pWindowBaseImpl->DoModal(hResInst, nResID, szID, hWndParent); 
}
HWND  IWindowBase::DoModeless(const TCHAR* szID, HWND hWndParent, bool canResize )
{
	return m_pWindowBaseImpl->DoModeless(szID, hWndParent, canResize);
}
HWND  IWindowBase::DoModeless(HINSTANCE hResInst, UINT nResID, const TCHAR* szID, HWND hWndParent) 
{ 
	return m_pWindowBaseImpl->DoModeless(hResInst, nResID, szID, hWndParent);
}
void  IWindowBase::EndDialog(INT_PTR n)
{ 
	return m_pWindowBaseImpl->EndDialog(n);
}

void  IWindowBase::CalcWindowSizeByClientSize( SIZE sizeClient, SIZE* pSizeWindow )
{
	m_pWindowBaseImpl->CalcWindowSizeByClientSize(sizeClient, pSizeWindow); 
}
void  IWindowBase::CalcClientRectByWindowRect( RECT* rcWindow, RECT* rcClient )
{
	m_pWindowBaseImpl->CalcClientRectByWindowRect(rcWindow, rcClient);
}
void  IWindowBase::UpdateWindow(HDC hDC, RECT* prcInvalid)
{
    m_pWindowBaseImpl->UpdateWindow(hDC, prcInvalid);
}
void  IWindowBase::SaveMemBitmap(TCHAR* szFile) 
{
	m_pWindowBaseImpl->SaveMemBitmap(szFile);
}
void  IWindowBase::DrawMemBitmap(HDC hDC, RECT* prc, bool bAlphaBlend)
{
	m_pWindowBaseImpl->DrawMemBitmap(hDC, prc, bAlphaBlend); 
}
void  IWindowBase::EnableDwmTransition(bool b) 
{ 
	m_pWindowBaseImpl->EnableDwmTransition(b); 
}

bool  IWindowBase::AddAnchorItem(const SyncWindowData& data)
{
	return m_pWindowBaseImpl->AddAnchorItem(data);
}
void  IWindowBase::HideAllAnchorItem() 
{
	m_pWindowBaseImpl->HideAllAnchorItem(); 
}

HRESULT  IWindowBase::SetDroppable(bool b) 
{
	return m_pWindowBaseImpl->SetCanDrop(b);
}
void  IWindowBase::SetHardComposite(bool b)
{
	return m_pWindowBaseImpl->SetHardComposite(b);
}
bool  IWindowBase::IsHardComposite()
{
	return m_pWindowBaseImpl->IsHardComposite();
}
void  IWindowBase::DirectComposite()
{
	m_pWindowBaseImpl->DirectComposite();
}