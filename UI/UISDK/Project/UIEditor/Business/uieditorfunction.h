#pragma once

bool  UE_AddSkin(const TCHAR* szDir, const TCHAR* szSkinName);

// bool  UE_AddObjectAttribute(   ISkinRes* pSkinRes, IObject* pObject, IUIElement* pUIElement, const TCHAR* szKey, const TCHAR* szValue);
// bool  UE_RemoveObjectAttribute(ISkinRes* pSkinRes, IObject* pObject, IUIElement* pUIElement, const TCHAR* szAttrib);
// bool  UE_ClearObjectAttribute( ISkinRes* pSkinRes, IObject* pObject, IUIElement* pUIElement);
// bool  UE_ModifyObjectAttribute(ISkinRes* pSkinRes, IObject* pObject, IUIElement* pUIElement, const TCHAR* szKey, const TCHAR* szValue);



HRESULT  UE_ExecuteCommand(ICommand* p, bool bDelay = false);
void  UE_RefreshProjectTree();
void  UE_RefreshUIWindow(IObject* pObject);
bool  UE_RefreshLayoutView(IObject* pObject);
void  UE_UpdateAttributeInPropertyCtrl(IObject* pObject, LPCTSTR szKey);

void  UE_RefreshPropertyDlg(IObject* pObject);
bool  UE_ReloadObjectAttribute(ISkinRes* pSkinRes, IObject* pObject, IUIElement* pUIElement);
