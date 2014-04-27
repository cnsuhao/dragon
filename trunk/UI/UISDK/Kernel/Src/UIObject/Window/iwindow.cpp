#include "stdafx.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Src\UIObject\Window\window.h"
#include "UISDK\Kernel\Src\RenderLayer2\layer\windowrender.h"
#include "UISDK\Kernel\Src\Resource\skinres.h"

namespace UI
{
UI_IMPLEMENT_Ixxx_INTERFACE(IWindowBase, WindowBase, Panel)
UI_IMPLEMENT_Ixxx_INTERFACE(IWindow, Window, WindowBase)

HWND  IWindowBase::GetHWND()                                               { return m_pWindowBaseImpl->GetHWND(); }
BOOL  IWindowBase::IsChildWindow()                                         { return m_pWindowBaseImpl->IsChildWindow(); }
void  IWindowBase::ShowWindow()                                            { m_pWindowBaseImpl->ShowWindow(); }
void  IWindowBase::HideWindow()                                            { m_pWindowBaseImpl->HideWindow(); }
void  IWindowBase::CenterWindow(HWND hWndCenter)                           { m_pWindowBaseImpl->CenterWindow(hWndCenter); }
void  IWindowBase::UpdateDesktopLayout()                                   { m_pWindowBaseImpl->UpdateDesktopLayout(); }
bool  IWindowBase::IsDoModal()                                             { return m_pWindowBaseImpl->IsDoModal(); }
IWindowRender*  IWindowBase::GetIWindowRender()                            { return m_pWindowBaseImpl->GetWindowRender()->GetIWindowRender(); }
ISkinRes*  IWindowBase::GetSkinRes()                                       { return m_pWindowBaseImpl->GetSkinRes()->GetISkinRes(); }

void    IWindowBase::OnObjectDeleteInd(IObject* p) 
{ 
    if (p)
        m_pWindowBaseImpl->OnObjectDeleteInd(p->GetImpl()); 
}
void  IWindowBase::OnObjectHideInd(IObject* p)   
{ 
    if (p)
        m_pWindowBaseImpl->OnObjectHideInd(p->GetImpl()); 
}

void   IWindowBase::SetFocusObject(IObject* pObj)
{
    Object*  p = NULL;
    if (pObj)
        p = pObj->GetImpl();

    m_pWindowBaseImpl->SetFocusObject(p);
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
bool  IWindowBase::Create(const TCHAR* szID, HWND hWndParent, RECT* prc) {return m_pWindowBaseImpl->Create(szID, hWndParent, prc);}
void  IWindowBase::Attach(HWND hWnd, const TCHAR* szID) { m_pWindowBaseImpl->Attach(hWnd, szID); }
void  IWindowBase::Detach() { m_pWindowBaseImpl->Detach(); }
long  IWindowBase::DoModal(const TCHAR* szID, HWND hWndParent, bool canResize ) { return m_pWindowBaseImpl->DoModal(szID, hWndParent, canResize); }
long  IWindowBase::DoModal(HINSTANCE hResInst, UINT nResID, const TCHAR* szID, HWND hWndParent) { return m_pWindowBaseImpl->DoModal(hResInst, nResID, szID, hWndParent); }
HWND  IWindowBase::DoModeless(const TCHAR* szID, HWND hWndParent, bool canResize ) { return m_pWindowBaseImpl->DoModeless(szID, hWndParent, canResize); }
HWND  IWindowBase::DoModeless(HINSTANCE hResInst, UINT nResID, const TCHAR* szID, HWND hWndParent) { return m_pWindowBaseImpl->DoModeless(hResInst, nResID, szID, hWndParent); }
void  IWindowBase::EndDialog(INT_PTR n) { return m_pWindowBaseImpl->EndDialog(n); }

void  IWindowBase::CommitDoubleBuffet2Window(HDC hDCWnd, RECT* prcCommit, int nRectCount) { m_pWindowBaseImpl->CommitDoubleBuffet2Window(hDCWnd, prcCommit, nRectCount); }
void  IWindowBase::CalcWindowSizeByClientSize( SIZE sizeClient, SIZE* pSizeWindow ) { m_pWindowBaseImpl->CalcWindowSizeByClientSize(sizeClient, pSizeWindow); }
void  IWindowBase::CalcClientRectByWindowRect( RECT* rcWindow, RECT* rcClient ) { m_pWindowBaseImpl->CalcClientRectByWindowRect(rcWindow, rcClient); }
void  IWindowBase::SaveMemBitmap(TCHAR* szFile) { m_pWindowBaseImpl->SaveMemBitmap(szFile); }
void  IWindowBase::DrawMemBitmap(HDC hDC, RECT* prc, bool bAlphaBlend) { m_pWindowBaseImpl->DrawMemBitmap(hDC, prc, bAlphaBlend); }
// void  IWindowBase::AddCommitWindowBufferListener(ICommitWindowBufferListener* p) { m_pWindowBaseImpl->AddCommitWindowBufferListener(p); }
// void  IWindowBase::RemoveCommitWindowBufferListener(ICommitWindowBufferListener* p) { m_pWindowBaseImpl->RemoveCommitWindowBufferListener(p); }

bool  IWindowBase::AddAnchorItem(const SyncWindowData& data){ return m_pWindowBaseImpl->AddAnchorItem(data); }
void  IWindowBase::HideAllAnchorItem() { m_pWindowBaseImpl->HideAllAnchorItem(); }

HRESULT  IWindowBase::SetCanDrop(bool b) { return m_pWindowBaseImpl->SetCanDrop(b); }
}