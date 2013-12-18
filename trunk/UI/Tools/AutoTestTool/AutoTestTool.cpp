// AutoTestTool.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "AutoTestTool.h"
#include "MainDlg.h"

CAppModule  _Module;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
    _Module.Init(NULL, hInstance);

    CMainDlg dlg; 
    dlg.DoModal();
    _Module.Term();

    return 0;
}