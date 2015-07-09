#include "stdafx.h"
#include "UISDK\Kernel\Inc\Interface\iobject.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Src\RenderLayer2\layer\renderlayer.h"

using namespace UI;

UI_IMPLEMENT_Ixxx_INTERFACE(IObject, Object, Message)

//////////////////////////////////////////////////////////////////////////

TCHAR*  IObject::GetObjectName()                          
{ 
	return m_pObjectImpl->GetObjectName();
}
OBJ_TYPE  IObject::GetObjectType()                        
{ 
	return m_pObjectImpl->GetObjectType(); 
}
CONTROL_TYPE  IObject::GetObjectExtentType()             
{
	return m_pObjectImpl->GetObjectExtentType();
}

LPCTSTR  IObject::GetId()                          
{
	return m_pObjectImpl->GetId();
}
void  IObject::SetId(LPCTSTR szText)             
{
	m_pObjectImpl->SetId(szText); 
}

IWindowBase*  IObject::GetWindowObject() 
{ 
    WindowBase* p = m_pObjectImpl->GetWindowObject();
    if (p)
        return p->GetIWindowBase();
    else
        return NULL;
}

IRenderLayer*   IObject::GetRenderLayer2()
{
	RenderLayer* p = m_pObjectImpl->GetRenderLayer2();
	if (p)
		return p->GetIRenderLayer2();
	else
		return NULL;
}
IRenderLayer*   IObject::GetSelfRenderLayer2()
{
    RenderLayer* p = m_pObjectImpl->GetSelfRenderLayer2();
    if (p)
        return p->GetIRenderLayer2();
    else
        return NULL;
}

HWND  IObject::GetHWND()                                   
{
	return m_pObjectImpl->GetHWND(); 
}
IUIApplication* IObject::GetUIApplication()                
{
	return m_pObjectImpl->GetIUIApplication();
}
void  IObject::SetOutRef(void** ppOutRef)                  
{
	m_pObjectImpl->SetOutRef(ppOutRef); 
}

void  IObject::InitDefaultAttrib()                          
{
	m_pObjectImpl->InitDefaultAttrib(); 
}
void  IObject::SetObjectPos(int x, int y, int cx, int cy, int nFlag)
{ 
	m_pObjectImpl->SetObjectPos(x, y, cx, cy, nFlag);
}
void  IObject::SetObjectPos(RECT* prc, int nFlag)          
{
	m_pObjectImpl->SetObjectPos(prc, nFlag);
}
void  IObject::SetUserData(LPVOID p)                        
{ 
	m_pObjectImpl->SetUserData(p); 
}
LPVOID  IObject::GetUserData()                             
{ 
	return m_pObjectImpl->GetUserData(); 
}
void  IObject::ModifyObjectStyle(ObjStyle* add, ObjStyle* remove)
{
	m_pObjectImpl->ModifyObjectStyle(add, remove);
}
bool  IObject::TestObjectStyle(const ObjStyle& test)
{
	return m_pObjectImpl->TestObjectStyle(test);
}
void  IObject::ModifyStyleEx( UINT add, UINT remove, bool bNotify) 
{
	m_pObjectImpl->ModifyStyleEx(add, remove, bNotify); 
}
bool  IObject::TestStyleEx(UINT nStyleEx)                  
{
	return m_pObjectImpl->TestStyleEx(nStyleEx); 
}
UINT  IObject::GetStyleEx()                                
{
	return m_pObjectImpl->GetStyleEx(); 
}
void  IObject::SetStyleEx(UINT n)                           
{ 
	return m_pObjectImpl->SetStyleEx(n);
}
bool  IObject::testStateBit(UINT bit)                       
{
	return m_pObjectImpl->testStateBit(bit);
}
void  IObject::setStateBit(UINT bit)                        
{
	m_pObjectImpl->setStateBit(bit); 
}
void  IObject::clearStateBit(UINT bit)                      
{
	m_pObjectImpl->clearStateBit(bit); 
}
void  IObject::SetCanRedraw(bool bReDraw)                   
{
	m_pObjectImpl->SetCanRedraw(bReDraw);
}
bool  IObject::CanRedraw()                                  
{
	return m_pObjectImpl->CanRedraw(); 
}
IRenderFont*  IObject::GetRenderFont()                      
{
	return m_pObjectImpl->GetRenderFont(); 
}
void  IObject::SetRenderLayer(bool b)                       
{
	return m_pObjectImpl->SetRenderLayer(b);
}

int   IObject::GetStateBit()                                
{ 
	return m_pObjectImpl->GetStateBit(); 
}
bool  IObject::IsFocus()                                   
{ 
	return m_pObjectImpl->IsFocus(); 
}
bool  IObject::SetFocusInWindow()
{
	return m_pObjectImpl->SetFocusInWindow();
}
bool  IObject::IsTabstop()         
{ 
	return m_pObjectImpl->IsTabstop(); 
}
bool  IObject::CanTabstop()      
{
	return m_pObjectImpl->CanTabstop();
}
void  IObject::SetTabstop(bool b)  
{
	m_pObjectImpl->SetTabstop(b);
}
bool  IObject::IsVisible()        
{ 
	return m_pObjectImpl->IsVisible(); 
}
bool  IObject::IsCollapsed()    
{ 
	return m_pObjectImpl->IsCollapsed(); 
}
bool  IObject::IsSelfVisible()  
{
	return m_pObjectImpl->IsSelfVisible();
}
bool  IObject::IsEnable()        
{ 
	return m_pObjectImpl->IsEnable(); 
}
void  IObject::SetVisible(bool b, bool bRedraw, bool bUpdateLayout) 
{
	m_pObjectImpl->SetVisible(b, bRedraw, bUpdateLayout); 
}
void  IObject::SetEnable(bool b, bool bUpdateNow) 
{
	m_pObjectImpl->SetEnable(b, bUpdateNow); }
bool  IObject::IsDefault()                                 
{ 
	return m_pObjectImpl->IsDefault(); }
bool  IObject::IsHover()                                  
{ 
	return m_pObjectImpl->IsHover(); }
bool  IObject::IsPress()                                  
{ 
	return m_pObjectImpl->IsPress(); }
bool  IObject::IsForcePress()                           
{ 
	return m_pObjectImpl->IsForcePress(); 
}
bool  IObject::IsForceHover()                            
{
	return m_pObjectImpl->IsForceHover(); 
}
bool  IObject::IsSelected()                              
{ 
	return m_pObjectImpl->IsSelected(); 
}
void  IObject::SetDefault(bool b, bool bNotify)        
{
	m_pObjectImpl->SetDefault(b, bNotify);
}
void  IObject::SetForceHover(bool b, bool bNotify)        
{
	m_pObjectImpl->SetForceHover(b, bNotify); 
}
void  IObject::SetForcePress(bool b, bool bNotify)         
{ 
	m_pObjectImpl->SetForcePress(b, bNotify);
}
void  IObject::SetHover(bool b, bool bNotify)              
{ 
	m_pObjectImpl->SetHover(b, bNotify); 
}
void  IObject::SetPress(bool b, bool bNotify)              
{
	m_pObjectImpl->SetPress(b, bNotify);
}
void  IObject::SetSelected(bool b, bool bNotify)          
{
	m_pObjectImpl->SetSelected(b, bNotify); 
}

bool  IObject::IsTransparent()                            
{
	return m_pObjectImpl->IsTransparent();
}
void  IObject::SetTransparent(bool b)                      
{ 
	m_pObjectImpl->SetTransparent(b); 
}
bool  IObject::IsNcObject()                                
{
	return m_pObjectImpl->IsNcObject();
}
void  IObject::SetAsNcObject(bool b)                       
{ 
	m_pObjectImpl->SetAsNcObject(b); 
}
bool  IObject::IsRejectMouseMsgAll()                       
{
	return m_pObjectImpl->IsRejectMouseMsgAll(); 
}
bool  IObject::IsRejectMouseMsgSelf()                      
{ 
	return m_pObjectImpl->IsRejectMouseMsgSelf(); 
}
void  IObject::SetZorderDirect(int lz)                      
{ 
	return m_pObjectImpl->SetZorderDirect(lz);
}
int   IObject::GetZOrder()                                 
{ 
	return m_pObjectImpl->GetZOrder(); 
}

IObject*  IObject::FindChildObject(LPCTSTR szObjId) 
{
    Object* p = m_pObjectImpl->FindChildObject(szObjId); 
    if (p)
        return p->GetIObject();
    return NULL;
}
IObject*  IObject::FindNcChildObject(LPCTSTR szObjId)
{
	Object* p = m_pObjectImpl->FindNcChildObject(szObjId); 
	if (p)
		return p->GetIObject();
	return NULL;
}
IObject*  IObject::GetParentObject()  
{
    Object* p = m_pObjectImpl->GetParentObject(); 
    if (p)
        return p->GetIObject();
    return NULL;
}
IObject*  IObject::GetChildObject() 
{ 
    Object* p = m_pObjectImpl->GetChildObject(); 
    if (p)
        return p->GetIObject();
    return NULL;
}
IObject*  IObject::GetNcChildObject() 
{ 
    Object* p = m_pObjectImpl->GetNcChildObject(); 
    if (p)
        return p->GetIObject();
    return NULL;
}
IObject*  IObject::GetLastChildObject()
{ 
    Object* p = m_pObjectImpl->GetLastChildObject(); 
    if (p)
        return p->GetIObject();
    return NULL;
}
IObject*  IObject::GetNextObject()
{ 
    Object* p = m_pObjectImpl->GetNextObject(); 
    if (p)
        return p->GetIObject();
    return NULL;
}
IObject*  IObject::GetPrevObject() 
{
    Object* p = m_pObjectImpl->GetPrevObject();
    if (p)
        return p->GetIObject();
    return NULL;
}
IObject*  IObject::GetRootObject()
{ 
    Object* p = m_pObjectImpl->GetRootObject(); 
    if (p)
        return p->GetIObject();
    return NULL;
}

// IObject*  IObject::GetChildObjectByIndex(unsigned long lIndex)
// {
// 	Object* pObject = m_pObjectImpl->GetChildObjectIndex(lIndex);
// 	if (!pObject)
// 		return NULL;
// 
// 	return pObject->GetIObject();
// }

IObject*  IObject::EnumChildObject(IObject* p)  
{
    Object* pObject = NULL;
    if (p)
        pObject = p->GetImpl();

    Object* pRet = m_pObjectImpl->EnumChildObject(pObject); 
    if (pRet)
        return pRet->GetIObject();

    return NULL;
}
IObject*  IObject::REnumChildObject(IObject* p) 
{ 
    Object* pObject = NULL;
    if (p)
        pObject = p->GetImpl();

    Object* pRet = m_pObjectImpl->REnumChildObject(pObject); 
    if (pRet)
        return pRet->GetIObject();

    return NULL;
}
IObject*  IObject::EnumNcChildObject(IObject* p) 
{
    Object* pObject = NULL;
    if (p)
        pObject = p->GetImpl();

    Object* pRet = m_pObjectImpl->EnumNcChildObject(pObject); 
    if (pRet)
        return pRet->GetIObject();

    return NULL;
}

IObject*  IObject::EnumAllChildObject(IObject* p) 
{ 
    Object* pObject = NULL;
    if (p)
        pObject = p->GetImpl();

    Object* pRet = m_pObjectImpl->EnumAllChildObject(pObject);
    if (NULL == pRet)
        return NULL;

    return pRet->GetIObject();
}
IObject*  IObject::EnumParentObject(IObject* p)
{ 
    Object* pObject = NULL;
    if (p)
        pObject = p->GetImpl();

    Object* pRet = m_pObjectImpl->EnumParentObject(pObject); 
    if (NULL == pRet)
        return NULL;

    return pRet->GetIObject();
}
IObject*  IObject::REnumParentObject(IObject* p)
{ 
    Object* pObject = NULL;
    if (p)
        pObject = p->GetImpl();

    Object* pRet = m_pObjectImpl->REnumParentObject(pObject); 
    if (NULL == pRet)
        return NULL;

    return pRet->GetIObject();
}

IObject*  IObject::GetNextTreeItemObject()
{ 
    Object* p = m_pObjectImpl->GetNextTreeItemObject(); 
    if (NULL == p)
        return NULL;

    return p->GetIObject();
}
IObject*  IObject::GetPrevTreeItemObject()
{ 
    Object* p = m_pObjectImpl->GetPrevTreeItemObject(); 
    if (NULL == p)
        return NULL;

    return p->GetIObject();
}
IObject*  IObject::GetNextTreeTabstopItemObject() 
{ 
    Object* p = m_pObjectImpl->GetNextTreeTabstopItemObject(); 
    if (NULL == p)
        return NULL;

    return p->GetIObject();
}
IObject*  IObject::GetPrevTreeTabstopItemObject() 
{ 
    Object* p =  m_pObjectImpl->GetPrevTreeTabstopItemObject(); 
    if (NULL == p)
        return NULL;

    return p->GetIObject();
}
IObject*  IObject::GetNextTabObject()
{ 
    Object* p = m_pObjectImpl->GetNextTabObject(); 
    if (NULL == p)
        return NULL;

    return p->GetIObject();
}
IObject*  IObject::GetPrevTabObject()
{
    Object* p = m_pObjectImpl->GetPrevTabObject(); 
    if (NULL == p)
        return NULL;

    return p->GetIObject();
}

void  IObject::SetParentObjectDirect(IObject* p) 
{ 
    if (p)
        m_pObjectImpl->SetParentObjectDirect(p->GetImpl()); 
    else
        m_pObjectImpl->SetParentObjectDirect(NULL); 

}
void  IObject::SetChildObjectDirect(IObject* p)
{
    if (p)
        m_pObjectImpl->SetChildObjectDirect(p->GetImpl()); 
    else
        m_pObjectImpl->SetChildObjectDirect(NULL); 

}
void  IObject::SetNcChildObjectDirect(IObject* p)
{
    if (p)
        m_pObjectImpl->SetNcChildObjectDirect(p->GetImpl()); 
    else
        m_pObjectImpl->SetNcChildObjectDirect(NULL); 

}
void  IObject::SetNextObjectDirect(IObject* p) 
{
    if (p)
        m_pObjectImpl->SetNextObjectDirect(p->GetImpl()); 
    else
        m_pObjectImpl->SetNextObjectDirect(NULL); 

}
void  IObject::SetPrevObjectDirect(IObject* p) 
{
    if (p)
        m_pObjectImpl->SetPrevObjectDirect(p->GetImpl()); 
    else
        m_pObjectImpl->SetPrevObjectDirect(NULL); 
}

void  IObject::AddChild(IObject* p)
{ 
    if (p)
        m_pObjectImpl->AddChild(p->GetImpl()); 
}
void  IObject::InsertChild(IObject* pObj, IObject* pInsertAfter)
{
    Object*  pInsertAfterImpl = NULL;
    if (pInsertAfter)
        pInsertAfterImpl = pInsertAfter->GetImpl();

    if (pObj)
        m_pObjectImpl->InsertChild(pObj->GetImpl(), pInsertAfterImpl);
}
void  IObject::AddNcChild(IObject*p) 
{
    if (p)
        m_pObjectImpl->AddNcChild(p->GetImpl()); 
}
void  IObject::InsertAfter(IObject* pInsertAfter)
{
    Object* p = NULL;

    if (pInsertAfter)
        p = pInsertAfter->GetImpl();

    m_pObjectImpl->InsertAfter(p);
}
void  IObject::InsertBefore(IObject* pInsertBefore)
{
    Object* p = NULL;

    if (pInsertBefore)
        p = pInsertBefore->GetImpl();

    m_pObjectImpl->InsertBefore(p);
}
bool  IObject::IsMyChild(IObject* pChild, bool bFindInGrand) 
{
    if (NULL == pChild)
        return false;

    return m_pObjectImpl->IsMyChild(pChild->GetImpl(), bFindInGrand); 
}
bool  IObject::RemoveChildInTree(IObject* pChild) 
{ 
    if (NULL == pChild)
        return false;

    return m_pObjectImpl->RemoveChildInTree(pChild->GetImpl()); 
}
void  IObject::RemoveMeInTheTree() 
{ 
    m_pObjectImpl->RemoveMeInTheTree(); 
}

void  IObject::UpdateObjectNonClientRegion()
{
    return m_pObjectImpl->UpdateObjectNonClientRegion();
}
void  IObject::Standalone() { m_pObjectImpl->ClearMyTreeRelationOnly(); }
void  IObject::MoveToAsFirstChild() { m_pObjectImpl->MoveToAsFirstChild(); }
void  IObject::MoveToAsLastChild() { m_pObjectImpl->MoveToAsLastChild(); }
bool  IObject::SwapObject(IObject* pObj1, IObject* pObj2)
{
    Object* p1 = NULL;
    Object* p2 = NULL;

    if (pObj1)
        p1 = pObj1->GetImpl();
    if (pObj2)
        p2 = pObj2->GetImpl();

    return m_pObjectImpl->SwapObject(p1, p2);
}

void  IObject::UpdateObject(bool bUpdateNow)
{ 
    m_pObjectImpl->UpdateObject(bUpdateNow); 
}
void  IObject::UpdateObjectEx(RECT* prcObjArray, int nCount, bool bUpdateNow) 
{ 
    m_pObjectImpl->UpdateObjectEx(prcObjArray, nCount, bUpdateNow); 
}
void  IObject::UpdateObjectBkgnd(bool bUpdateNow) 
{
    m_pObjectImpl->UpdateObjectBkgnd(bUpdateNow);
}
void  IObject::UpdateLayout(bool bUpdate) 
{ 
    m_pObjectImpl->UpdateLayout(bUpdate);
}
void  IObject::UpdateMyLayout(bool bUpdate)
{ 
    m_pObjectImpl->UpdateMyLayout(bUpdate);
}

void  IObject::GetNonClientRegion(REGION4* prc) 
{
    m_pObjectImpl->GetNonClientRegion(prc); 
}
void  IObject::SetExtNonClientRegion(REGION4* prc)
{
    m_pObjectImpl->SetExtNonClientRegion(prc); 
}
void  IObject::GetExtNonClientRegion(REGION4* prc)
{
    m_pObjectImpl->GetExtNonClientRegion(prc);
}

void  IObject::GetBorderRegion(REGION4* prc){ m_pObjectImpl->GetBorderRegion(prc); }
void  IObject::SetBorderRegion(REGION4* prc) { m_pObjectImpl->SetBorderRegion(prc); }
void  IObject::GetPaddingRegion(REGION4* prc){ m_pObjectImpl->GetPaddingRegion(prc); }
void  IObject::SetPaddingRegion(REGION4* prc){ m_pObjectImpl->SetPaddingRegion(prc); }
void  IObject::SetMarginRegion(REGION4* prc) { m_pObjectImpl->SetMarginRegion(prc); }
void  IObject::GetMarginRegion(REGION4* prc) { m_pObjectImpl->GetMarginRegion(prc); }
void  IObject::GetClientRectInObject(RECT* prc)        
{ 
	m_pObjectImpl->GetClientRectInObject(prc); 
}
void  IObject::GetObjectClientRect(RECT* prc) 
{
	m_pObjectImpl->GetObjectClientRect(prc); 
}
void  IObject::GetClientRectInWindow(RECT* prc)
{ 
	m_pObjectImpl->GetClientRectInWindow(prc); 
}
void  IObject::GetParentRect(RECT* prc)       
{ 
	m_pObjectImpl->GetParentRect(prc); 
}

POINT  IObject::GetRealPosInWindow()                                              
{ 
    return m_pObjectImpl->GetWindowPoint(); 
}
void  IObject::GetWindowRect(RECT* prc)                                          
{ 
    m_pObjectImpl->GetWindowRect(prc);
}
void  IObject::WindowRect2ObjectRect(const RECT* rcWindow, RECT* rcObj)
{
    m_pObjectImpl->WindowRect2ObjectRect(rcWindow, rcObj); 
}
void  IObject::WindowPoint2ObjectPoint(const POINT* ptWindow, POINT* ptObj, bool bCalcTransform)      
{
    m_pObjectImpl->WindowPoint2ObjectPoint(ptWindow, ptObj, bCalcTransform); 
}
void  IObject::WindowPoint2ObjectClientPoint(const POINT* ptWindow, POINT* ptObj, bool bCalcTransform)
{ 
    m_pObjectImpl->WindowPoint2ObjectClientPoint(ptWindow, ptObj, bCalcTransform); 
}
void  IObject::ObjectPoint2ObjectClientPoint(const POINT* ptWindow, POINT* ptObj) 
{
    Object::ObjectPoint2ObjectClientPoint(m_pObjectImpl, ptWindow, ptObj); 
}
void  IObject::ClientRect2ObjectRect(const RECT* rcClient, RECT* rcObj)           
{
    Object::ObjectClientRect2ObjectRect(m_pObjectImpl, rcClient, rcObj); 
}
void  IObject::ObjectRect2ClientRect(const RECT* rcObj, RECT* rcClient)
{
    Object::ObjectRect2ObjectClientRect(m_pObjectImpl, rcObj, rcClient);
}
void  IObject::ClientRect2WindowRect(const RECT* rcClient, RECT* rcWnd)
{
	Object::ObjectClientRect2WindowRect(m_pObjectImpl, rcClient, rcWnd);
}

bool  IObject::GetScrollOffset(int* pxOffset, int* pyOffset)
{ 
	return m_pObjectImpl->GetScrollOffset(pxOffset, pyOffset);
}
bool  IObject::GetScrollRange(int* pxRange, int* pyRange)   
{
	return m_pObjectImpl->GetScrollRange(pxRange, pyRange); 
}
bool  IObject::GetRectInWindow(RECT* prc, bool bVisiblePart)      
{
	return m_pObjectImpl->GetRectInWindow(prc, bVisiblePart);
}
bool  IObject::GetRectInLayer(RECT* prc, bool bVisiblePart)        
{ 
	return m_pObjectImpl->GetRectInLayer(prc, bVisiblePart); 
}
bool  IObject::GetVisibleClientRectInLayer(RECT* prc)
{ 
	return m_pObjectImpl->GetVisibleClientRectInLayer(prc);
}

int  IObject::GetWidth()            
{ 
	return m_pObjectImpl->GetWidth(); 
}
int  IObject::GetHeight()            
{ 
	return m_pObjectImpl->GetHeight(); 
}
int  IObject::GetWidthWithMargins()  
{
	return m_pObjectImpl->GetWidthWithMargins();
}
int  IObject::GetHeightWithMargins()
{ 
	return m_pObjectImpl->GetHeightWithMargins();
}

ILayoutParam*  IObject::GetLayoutParam() 
{ 
	return m_pObjectImpl->GetLayoutParam();
}
void  IObject::CreateLayoutParam()
{
	m_pObjectImpl->CreateLayoutParam();
}
int   IObject::GetConfigWidth()        
{ 
	return m_pObjectImpl->GetConfigWidth();
}
int   IObject::GetConfigHeight()       
{ 
	return m_pObjectImpl->GetConfigHeight();
}
int   IObject::GetConfigLayoutFlags()  
{ 
	return m_pObjectImpl->GetConfigLayoutFlags();
}
int   IObject::GetConfigLeft()         
{
	return m_pObjectImpl->GetConfigLeft(); 
}
int   IObject::GetConfigRight()       
{ 
	return m_pObjectImpl->GetConfigRight();
}
int   IObject::GetConfigTop()         
{ 
	return m_pObjectImpl->GetConfigTop(); 
}
int   IObject::GetConfigBottom()       
{ 
	return m_pObjectImpl->GetConfigBottom(); 
}
void  IObject::SetConfigWidth(int n)   
{ 
	m_pObjectImpl->SetConfigWidth(n); 
}
void  IObject::SetConfigHeight(int n)  
{ 
	m_pObjectImpl->SetConfigHeight(n); 
}
void  IObject::SetConfigLayoutFlags(int n) 
{ 
	m_pObjectImpl->SetConfigLayoutFlags(n); 
}
void  IObject::SetConfigLeft(int n)    
{ 
	m_pObjectImpl->SetConfigLeft(n); 
}
void  IObject::SetConfigRight(int n)   
{ 
	m_pObjectImpl->SetConfigRight(n);
}
void  IObject::SetConfigTop(int n)     
{ 
	m_pObjectImpl->SetConfigTop(n); 
}
void  IObject::SetConfigBottom(int n) 
{
	m_pObjectImpl->SetConfigBottom(n);
}

ITextRenderBase*  IObject::GetTextRender() 
{ 
	return m_pObjectImpl->GetTextRender();
}
IRenderBase*  IObject::GetBkRender()       
{ 
	return m_pObjectImpl->GetBkRender();
}
IRenderBase*  IObject::GetForeRender()     
{
	return m_pObjectImpl->GetForeRender();
}
void  IObject::SetBkgndRender(IRenderBase* p)   
{
	m_pObjectImpl->SetBkgndRender(p); 
}
void  IObject::SetForegndRender(IRenderBase* p) 
{ 
	m_pObjectImpl->SetForegndRender(p);
}
void  IObject::SetTextRender(ITextRenderBase* p)
{
	m_pObjectImpl->SetTextRender(p);
}

void  IObject::SetAttributeByPrefix(LPCTSTR szPrefix, IMapAttribute* pMatAttrib, bool bReload, bool bErase) 
{ 
	m_pObjectImpl->SetAttributeByPrefix(szPrefix, pMatAttrib, bReload, bErase); 
}
void  IObject::ParseStyleAndSetAttribute(IMapAttribute* pMatAttrib, bool bReload) 
{
	m_pObjectImpl->ParseStyleAndLoadAttribute(pMatAttrib, bReload); 
}
void  IObject::LoadAttributeFromXml(IUIElement* pXmlElement, bool bReload)
{
	m_pObjectImpl->LoadAttributeFromXml(pXmlElement, bReload);
}
LPCTSTR  IObject::GetAttribute(LPCTSTR szKey, bool bErase) 
{ 
	return m_pObjectImpl->GetAttribute(szKey, bErase);
}
void  IObject::AddAttribute(LPCTSTR szKey, LPCTSTR  szValue)
{ 
	m_pObjectImpl->AddAttribute(szKey, szValue); 
}
void  IObject::GetMapAttribute(IMapAttribute** ppMapAttribute)
{ 
	m_pObjectImpl->GetMapAttribute(ppMapAttribute); 
}
SIZE  IObject::GetDesiredSize()
{ 
	return m_pObjectImpl->GetDesiredSize(); 
}

HBITMAP  IObject::TakeSnapshot() 
{
	return m_pObjectImpl->TakeSnapshot();
}
HBITMAP  IObject::TakeBkgndSnapshot()
{ 
	return m_pObjectImpl->TakeBkgndSnapshot(); 
}
GRAPHICS_RENDER_LIBRARY_TYPE  IObject::GetGraphicsRenderLibraryType() 
{ 
	return m_pObjectImpl->GetGraphicsRenderLibraryType(); 
}
bool  IObject::SetCursorId(LPCTSTR szCursorId)
{
	return m_pObjectImpl->SetCursorId(szCursorId); 
}
LPCTSTR  IObject::GetToolTipText()
{
    return m_pObjectImpl->GetToolTipText();
}
void  IObject::SetToolTipText(LPCTSTR szText)
{
    m_pObjectImpl->SetToolTipText(szText);
}
bool  IObject::SetMouseCapture(int nNotifyMsgId)
{ 
	return m_pObjectImpl->SetMouseCapture(nNotifyMsgId);
}
bool  IObject::ReleaseMouseCapture()
{ 
	return m_pObjectImpl->ReleaseMouseCapture(); 
}
bool  IObject::SetKeyboardCapture(int nNotifyMsgId)
{ 
	return m_pObjectImpl->SetKeyboardCapture(nNotifyMsgId);
}
bool  IObject::ReleaseKeyboardCapture()
{
	return m_pObjectImpl->ReleaseKeyboardCapture(); 
}

void  IObject::LoadColor(LPCTSTR szColor, Color*& pColorRef)
{
    return m_pObjectImpl->LoadColor(szColor, pColorRef);
}
LPCTSTR  IObject::SaveColor(Color*& pColorRef)
{
    return m_pObjectImpl->SaveColor(pColorRef);
}
void  IObject::LoadTextRender(LPCTSTR szName)
{
	m_pObjectImpl->LoadTextRender(szName);
}
LPCTSTR  IObject::SaveTextRender()
{
	return m_pObjectImpl->SaveTextRender();
}