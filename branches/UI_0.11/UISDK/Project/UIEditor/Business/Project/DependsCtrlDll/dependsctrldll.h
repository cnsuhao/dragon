#pragma once

struct DependsCtrlDllItem
{
    String  strRelativePath;  // 相对于uiproj的路径，用于保存的路径
    String  strAbsolutePath;  // 通过strRelativePath计算出来的绝对路径
};

class CDependsCtrlDllMgr
{
public:
    CDependsCtrlDllMgr();
    ~CDependsCtrlDllMgr();

    bool AddCtrlDll(const TCHAR* szPath);
    bool DelCtrlDll(const TCHAR* szPath);

    void  OnLoadItem(const TCHAR* szPath);

public:
    vector<DependsCtrlDllItem*>  m_vStrDlls;
};