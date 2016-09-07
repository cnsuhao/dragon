#pragma once

void  MsgBox(const TCHAR* szText, const TCHAR* szTitle = NULL);
void  GetUIEditorConfigFilePath(TCHAR* pszPath);

bool  WaitForEnter2Confirm(HWND hWnd);
bool  SaveTemplateResFileTo(const TCHAR* szDstPath, UINT nId, const TCHAR* szResType);

void  ParsePropertyCommand(const TCHAR*  szCmdLine, String& strCommand, String& strArgs);
void  ParsePropertyCommandArg(const TCHAR*  szArgs, String& strKey, String& strValue);

void  GetObjectTypeBitmap(OBJ_TYPE eType, UINT nControlType, IRenderBitmap** ppRenderBitma);

void  TrimStringLeft(String&  str, const TCHAR c = _T(' '));
void  TrimStringRight(String&  str, const TCHAR c = _T(' '));
void  TrimString(String&  str, const TCHAR c = _T(' '));