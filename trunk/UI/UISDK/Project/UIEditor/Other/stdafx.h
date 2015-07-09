// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�:
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <vector>
#include <stack>
#include <deque>
#include <map>
#include <algorithm>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��
// _vswprintf': This function or variable may be unsafe. Consider using vswprintf_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#pragma warning(disable:4996)

#include <atltypes.h>
#include <atlbase.h>
#include <atlstr.h>
#include <atlwin.h>
#include <atlapp.h>
#include <atlcrack.h>
#include <atldlgs.h>
#include <atlctrls.h>
#include <atlframe.h>
#include <Shellapi.h>

#ifdef IDC_STATIC
#undef IDC_STATIC
#endif

// ��������
#define  WINDOW_PADDING             10   // �����ڿؼ����봰�ڱ�Ե�Ĵ�С
#define  LEFT_TREE_WINDOW_WIDTH    300   // ���Ĵ��ڿ��

#define  WIDTH_IMAGEBUILD_LISTCTRL 300
#define  HEIGHT_TOOLBAR_BTN         22
#define  WIDTH_TOOLBAR_BTN          50
#define  HEIGHT_TAB                 20  // TAB�ؼ��ĸ߶�
#define  STATUSBAR_HEIGHT           22
#define  TOOLBAR_HEIGHT             26

#define  UIEDITOR_CONFIG_FILE_NAME _T("uieditor.xml")   // �����ļ�����

// ������
#include "UISDK\Project\UIUtil\h\util.h"
#pragma comment(lib,"uiutil.lib")

#include "UISDK\Kernel\Inc\Base\inc.h"
#include "UISDK\Control\Inc\Base\inc.h"
#pragma comment(lib,"uisdk.lib")
#pragma comment(lib,"uictrls.lib")

#include "UISDK\Project\UIEditorCtrl\Inc\inc.h"
#pragma comment(lib, "uieditorctrl.lib")

using namespace UI;

#include "Util/util.h"
#include "Business\GlobalData.h"
#include "Business\Command\Base\commandhistorymgr.h"
#include "UISDK\Kernel\Inc\Base\leakdetect.h"

extern CAppModule      _Module;
extern CGlobalData*    g_pGlobalData;

IUIApplication*  GetEditUIApplication();
ISkinManager*    GetEditSkinManager();
CProjectData*    GetProjectData();
CDependsCtrlDllMgr*  GetProjDependsCtrlDllMgr();
CUIEditor*  GetUIEditor();
CPropertyDialog*  GetPropertyDialog();
namespace UI
{
	interface IPropertyCtrl;
}

UI::IPropertyCtrl*   GetPropertyCtrl();


extern const GUID IID_IMDIClietnView;
extern const GUID IID_IExplorerMDIClientView;
extern const GUID IID_ILogMDIClientView;
extern const GUID IID_ILayoutMDIClientView;
extern const GUID IID_IMenuMDIClientView;

void UIEDITOR_LOG_DEBUG( TCHAR* szContent, ... );
void UIEDITOR_LOG_INFO( TCHAR* szContent, ... );
void UIEDITOR_LOG_WARN( TCHAR* szContent, ... );
void UIEDITOR_LOG_ERROR( TCHAR* szContent, ... );
void UIEDITOR_LOG_FATAL( TCHAR* szContent, ...);

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' "\
	"version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// Ԥ������
#define MDIVIEW_MESSAGE_PREVIEW  (WM_USER+1000)