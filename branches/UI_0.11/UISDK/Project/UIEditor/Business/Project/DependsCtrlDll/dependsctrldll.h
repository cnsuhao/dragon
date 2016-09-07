#pragma once

struct DependsCtrlDllItem
{
    String  strRelativePath;  // �����uiproj��·�������ڱ����·��
    String  strAbsolutePath;  // ͨ��strRelativePath��������ľ���·��
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