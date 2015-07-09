#include "StdAfx.h"
#include "Dialog\Framework\projecttreedialog.h"
#include "Dialog\Framework\MainFrame.h"
#include "UISDK\Kernel\Inc\Interface\iobject.h"
#include "UISDK\Control\Inc\Interface\itabctrl.h"
#include "UISDK\Kernel\Inc\Interface\iimagerender.h"
#include "UISDK\Control\Inc\Interface\imenu.h"
#include "UISDK\Control\Inc\Interface\iradiobutton.h"
#include "Business\Project\ProjectData\projectdata.h"
#include "Business\Command\RemoveObjectCommnd.h"
#include "Business\Command\MoveObjectUpDownCommand.h"


#pragma region // data

LayoutTreeItemData::LayoutTreeItemData()
{
	m_pObject = NULL;
	m_pSkin = NULL;
    m_pListItem = NULL;
    m_eType = TREEITEM_DATA_TYPE_UNKNOWN;
    m_bLazyLoad = false;
}

LayoutTreeItemData::~LayoutTreeItemData()
{
}

ResourceTreeItemData::ResourceTreeItemData()
{
    m_eType = TREEITEM_DATA_TYPE_UNKNOWN;
	m_pListItem = NULL;
	m_pSkin = NULL;
}
ResourceTreeItemData::~ResourceTreeItemData()
{
	
}

ExplorerTreeItemData::ExplorerTreeItemData()
{
	m_hTreeItem = NULL;
	m_bDirty = false;
	m_pSkin = NULL;
}
ExplorerTreeItemData::~ExplorerTreeItemData()
{

}

LogTreeItemData::LogTreeItemData()
{
	m_hTreeItem = NULL;
	m_bDirty = false;
	m_eResType = UI_RESOURCE_LOGMODULE;
	m_lLogCookie = 0;
}
LogTreeItemData::~LogTreeItemData()
{
	
}

SkinResData::SkinResData()
{
    m_pSkinRes = NULL;
    m_pItem_LayoutTree = NULL;
}
#pragma endregion


CProjectTreeDialog::CProjectTreeDialog()
{
	g_pGlobalData->m_pProjectTreeDialog = this;
	
	m_pMainFrame = NULL;
	m_pCurTreeCtrl = NULL;
    m_pTreeLayout = NULL;
    m_pTreeResource = NULL;
    m_pRadioBtnLayout = NULL;
    m_pRadioBtnResource = NULL;
}

CProjectTreeDialog::~CProjectTreeDialog(void)
{
	m_pMainFrame = NULL;

	if (g_pGlobalData)
		g_pGlobalData->m_pProjectTreeDialog = NULL;
}

BOOL  CProjectTreeDialog::PreCreateWindow(CREATESTRUCT* pcs)
{
    pcs->style = WS_CHILD;
    pcs->dwExStyle = 0;
    return TRUE;
}

void CProjectTreeDialog::OnInitialize()
{
    // root 工程名
    int nBufSize = 0;
    String strText = GetProjectData()->GetProjName();


    m_pTreeLayout = (IProjectTreeView*)FindChildObject(_T("treelayout"));
    if (m_pTreeLayout)
    {
        m_pTreeLayout->ModifyStyleEx(TREE_STYLE_HASLINE, 0, 0);

        ITreeItem* pItem = m_pTreeLayout->InsertNormalItem(strText.c_str());
        LayoutTreeItemData* pData = new LayoutTreeItemData;
        pData->m_pListItem = static_cast<IListItemBase*>(pItem);
        pData->m_eType = TREEITEM_DATA_TYPE_LAYOUT_ROOT;
        m_mapLayoutTree[pData->m_pListItem] = pData;
    }

    m_pTreeResource = (IProjectTreeView*)FindChildObject(_T("treeresource"));
    if (m_pTreeResource)
    {
        m_pTreeResource->ModifyStyleEx(TREE_STYLE_HASLINE, 0, 0);
        
        ITreeItem* pItem = m_pTreeResource->InsertNormalItem(strText.c_str());
        ResourceTreeItemData* pData = new ResourceTreeItemData;
        pData->m_pListItem = static_cast<IListItemBase*>(pItem);
        pData->m_eType = TREEITEM_DATA_TYPE_RESOURCE_ROOT;
        m_mapResTree[pData->m_pListItem] = pData;
    }

    m_pRadioBtnLayout = (IRadioButton*)FindChildObject(TEXT("radio_layout"));
    m_pRadioBtnResource = (IRadioButton*)FindChildObject(TEXT("radio_resource"));
    if (m_pRadioBtnLayout)
        m_pRadioBtnLayout->SetCheck(true, false);

	// 皮肤列表
	LONG nCount = g_pGlobalData->m_pProjectData->m_pEditSkinMgr->GetSkinCount();
	for (int i = 0; i < nCount; i++)
	{
		ISkinRes* pSkin = g_pGlobalData->m_pProjectData->m_pEditSkinMgr->GetSkinResByIndex(i);
		if (pSkin)
		    this->InsertSkinItem(pSkin);
	}
}

// 加载日志信息
void CProjectTreeDialog::InitLogItem()
{
	if (NULL == g_pGlobalData->m_pLog)
		return;

	HTREEITEM hTreeRoot = m_treeCtrlLog.InsertItem( _T("Log"), TVI_ROOT, TVI_LAST );
	if (NULL == hTreeRoot)
		return;

	long nCount = 0;
	g_pGlobalData->m_pLog->GetLogItemCount(&nCount);
	for (int i = 0; i<nCount; i++)
	{
		ILogItem* pLogItem = NULL;
		g_pGlobalData->m_pLog->GetLogItemByIndex(i, &pLogItem);
		if (pLogItem)
		{
			CComBSTR bstr;
			pLogItem->GetName(&bstr);

			long lCookie = 0;
			pLogItem->GetCookie(&lCookie);

            ::SendMessage(GetHWND(), WM_INSERT_LOG_ITEM,(WPARAM)(TCHAR*)bstr, (LPARAM)lCookie);
			pLogItem->Release();
		}
	}
}

//
//	释放保存在tree ctrl中的数据
//
void CProjectTreeDialog::OnDestroy( )
{
    SetMsgHandled(FALSE);
	{
		map<IListItemBase*, LayoutTreeItemData*>::iterator iter = m_mapLayoutTree.begin();
		map<IListItemBase*, LayoutTreeItemData*>::iterator iterEnd = m_mapLayoutTree.end();

		for (; iter != iterEnd; iter++)
		{
			LayoutTreeItemData* pData = iter->second;
			SAFE_DELETE(pData);
		}
		m_mapLayoutTree.clear();
	}
	{
		map<IListItemBase*, ResourceTreeItemData*>::iterator iter = m_mapResTree.begin();
		map<IListItemBase*, ResourceTreeItemData*>::iterator iterEnd = m_mapResTree.end();

		for (; iter != iterEnd; iter++)
		{
			ResourceTreeItemData* pData = iter->second;
			SAFE_DELETE(pData);
		}
		m_mapResTree.clear();
	}
	{
		map<HTREEITEM, ExplorerTreeItemData*>::iterator iter = m_mapTreeCtrlExplorer.begin();
		map<HTREEITEM, ExplorerTreeItemData*>::iterator iterEnd = m_mapTreeCtrlExplorer.end();

		for (; iter != iterEnd; iter++)
		{
			ExplorerTreeItemData* pData = iter->second;
			SAFE_DELETE(pData);
		}
		m_mapTreeCtrlExplorer.clear();
	}
	{
		map<HTREEITEM, LogTreeItemData*>::iterator iter = m_mapTreeCtrlLog.begin();
		map<HTREEITEM, LogTreeItemData*>::iterator iterEnd = m_mapTreeCtrlLog.end();

		for (; iter != iterEnd; iter++)
		{
			LogTreeItemData* pData = iter->second;
			SAFE_DELETE(pData);
		}
		m_mapTreeCtrlLog.clear();
	}

    {
        map<ISkinRes*, SkinResData*>::iterator iter = m_mapSkinResData.begin();
        map<ISkinRes*, SkinResData*>::iterator iterEnd = m_mapSkinResData.end();

        for (; iter != iterEnd; iter++)
        {
            SkinResData* pData = iter->second;
            SAFE_DELETE(pData);
        }
        m_mapSkinResData.clear();
    }
}
void CProjectTreeDialog::OnSize(UINT nType, CSize size)
{
	this->CalcLayout( size.cx, size.cy );
}
void CProjectTreeDialog::CalcLayout( int cx, int cy )
{
// 	m_tabCtrl.SetWindowPos(NULL, WINDOW_PADDING, cy-WINDOW_PADDING-HEIGHT_TAB, cx-2*WINDOW_PADDING, HEIGHT_TAB, SWP_NOZORDER );
// 	m_treeCtrlRes.SetWindowPos(NULL, WINDOW_PADDING,WINDOW_PADDING, cx-2*WINDOW_PADDING,cy-2*WINDOW_PADDING-HEIGHT_TAB, SWP_NOZORDER );
// 	m_treeCtrlLog.SetWindowPos(NULL, WINDOW_PADDING,WINDOW_PADDING, cx-2*WINDOW_PADDING,cy-2*WINDOW_PADDING-HEIGHT_TAB, SWP_NOZORDER );
// 	m_treeCtrlLayout.SetWindowPos(NULL, WINDOW_PADDING,WINDOW_PADDING, cx-2*WINDOW_PADDING,cy-2*WINDOW_PADDING-HEIGHT_TAB, SWP_NOZORDER );
// 	m_treeCtrlExplorer.SetWindowPos(NULL, WINDOW_PADDING,WINDOW_PADDING, cx-2*WINDOW_PADDING,cy-2*WINDOW_PADDING-HEIGHT_TAB, SWP_NOZORDER );
}


LRESULT  CProjectTreeDialog::OnResourceTreeDBClick(WPARAM w, LPARAM l)
{
	IListItemBase* pItem = m_pTreeResource->GetFirstSelectItem();
	if (NULL == pItem)
		return 0;

	map<IListItemBase*, ResourceTreeItemData*>::iterator iter = m_mapResTree.find(pItem);
	if (iter == m_mapResTree.end())
		return 0;

	ResourceTreeItemData* pData = iter->second;
	if (NULL == pData)
		return 0;

	switch (pData->m_eType)
	{
    case TREEITEM_DATA_TYPE_RESOURCE_IMAGE:
		m_pMainFrame->SwitchToSkinResPanel(UI_RESOURCE_IMAGE, pData->m_pSkin);
        break;

    case TREEITEM_DATA_TYPE_RESOURCE_COLOR:
        m_pMainFrame->SwitchToSkinResPanel(UI_RESOURCE_COLOR, pData->m_pSkin);
        break;

    case TREEITEM_DATA_TYPE_RESOURCE_FONT:
        m_pMainFrame->SwitchToSkinResPanel(UI_RESOURCE_FONT, pData->m_pSkin);
        break;

    case TREEITEM_DATA_TYPE_RESOURCE_STYLE:
        m_pMainFrame->SwitchToSkinResPanel(UI_RESOURCE_STYLE, pData->m_pSkin);
        break;
	}
	return 0;
}

LRESULT CProjectTreeDialog::OnLogTreeDBClick(LPNMHDR pnmh)
{
	HTREEITEM hItem = m_treeCtrlLog.GetSelectedItem();
	if (NULL == hItem)
		return 0;

	map<HTREEITEM, LogTreeItemData*>::iterator iter = m_mapTreeCtrlLog.find(hItem);
	if (iter == m_mapTreeCtrlLog.end())
		return 0;

	LogTreeItemData* pData = iter->second;
	if (NULL == pData)
		return 0;

	if (hItem != NULL)
	{
		TCHAR szText[MAX_STRING_LEN];
		m_treeCtrlLog.GetItemText(hItem, szText, MAX_STRING_LEN);
		m_pMainFrame->SwitchToLogPanel(pData, szText);
	}

	return 0;
}

LRESULT CProjectTreeDialog::OnExplorerTreeDBClick(LPNMHDR pnmh)
{

	HTREEITEM hItem = m_treeCtrlExplorer.GetSelectedItem();
	if (NULL == hItem)
		return 0;

	map<HTREEITEM, ExplorerTreeItemData*>::iterator iter = m_mapTreeCtrlExplorer.find(hItem);
	if (iter == m_mapTreeCtrlExplorer.end())
		return 0;

	ExplorerTreeItemData* pData = iter->second;
	if (NULL == pData)
		return 0;

	m_pMainFrame->SwitchToExplorerPanel(pData);

	return 0;
}

LRESULT CProjectTreeDialog::OnLogTreeRClick(LPNMHDR pnmh)
{
	HTREEITEM hTreeItem = m_treeCtrlLog.GetSelectedItem();
	if ( NULL == hTreeItem )
		return 0;

	const int MENUID_ADD_LOG_ITEM = 1;

	HMENU hMenu = ::CreatePopupMenu();
	::InsertMenu(hMenu,0,MF_BYPOSITION, MENUID_ADD_LOG_ITEM, _T("Add New Log Module") );

	POINT pt;
	::GetCursorPos(&pt);
	UINT nRet = ::TrackPopupMenuEx(hMenu, TPM_RETURNCMD, pt.x, pt.y, GetHWND(), NULL );
	::DestroyMenu(hMenu);

	switch(nRet)
	{
	case MENUID_ADD_LOG_ITEM:// 增加一个日志模块
		{
			m_pMainFrame->SwitchToLogPanel(NULL, NULL);
		}
		break;
	}
	
	return 0;
}

LRESULT CProjectTreeDialog::OnExplorerTreeRClick(LPNMHDR pnmh)
{
	HTREEITEM hTreeItem = m_treeCtrlExplorer.GetSelectedItem();
	if (NULL == hTreeItem)
		return 0;

	map<HTREEITEM, ExplorerTreeItemData*>::iterator iter = m_mapTreeCtrlExplorer.find(hTreeItem);
	if (iter == m_mapTreeCtrlExplorer.end())
		return 0;

	ExplorerTreeItemData* pData = iter->second;
	if (NULL == pData)
		return 0;

	if (pData->m_strFilePath.empty())
		return 0;

	const int MENUID_FIND_FILE_IN_EXPLORER = 1;

	HMENU hMenu = ::CreatePopupMenu();
	::InsertMenu(hMenu,0,MF_BYPOSITION, MENUID_FIND_FILE_IN_EXPLORER, _T("打开所在文件夹") );

	POINT pt;
	::GetCursorPos(&pt);
	UINT nRet = ::TrackPopupMenuEx(hMenu, TPM_RETURNCMD, pt.x, pt.y, GetHWND(), NULL );
	::DestroyMenu(hMenu);

	switch(nRet)
	{
	case MENUID_FIND_FILE_IN_EXPLORER:// 增加一个日志模块
		{
			Util::OpenFolderAndSelectFile((TCHAR*)(pData->m_strFilePath.c_str()));
		}
		break;
	}

	return 0;
}


//
//	通知：新增日志模块消息
//  wParam : logname
//	lParam : log cookie
//
LRESULT CProjectTreeDialog::OnInsertLogItem(UINT, WPARAM wParam, LPARAM lParam)
{
	TCHAR* szModuleName = (TCHAR*)wParam;
	if ( NULL==szModuleName )
		return 0;

	HTREEITEM hRoot = m_treeCtrlLog.GetRootItem();
	HTREEITEM hTreeItem = m_treeCtrlLog.InsertItem(szModuleName, hRoot, TVI_LAST);
	if (hTreeItem)
	{
		LogTreeItemData* pData = new LogTreeItemData;
		pData->m_hTreeItem = hTreeItem;
		pData->m_eResType = UI_RESOURCE_LOGMODULE;
		pData->m_lLogCookie = lParam;
		m_mapTreeCtrlLog[hTreeItem] = pData;

		m_treeCtrlLog.Expand(hRoot);
	}

	
	return 1;
}

LRESULT CProjectTreeDialog::OnModifyLogItem( UINT, WPARAM wParam, LPARAM lParam )
{
	TCHAR* szModuleName = (TCHAR*)wParam;
	TCHAR* szOldModuleName = (TCHAR*)lParam;
	if (NULL==szModuleName || NULL == szOldModuleName)
		return 0;

	// 查找
	HTREEITEM hRoot = m_treeCtrlLog.GetRootItem();
	HTREEITEM hItem = m_treeCtrlLog.GetChildItem(hRoot);
	while (NULL != hItem)
	{
		TCHAR szName[MAX_STRING_LEN] = _T("");
		m_treeCtrlLog.GetItemText(hItem, szName, MAX_STRING_LEN );
		if (0 == _tcscmp(szName, szOldModuleName))
		{
			m_treeCtrlLog.SetItemText(hItem, szModuleName);
			break;
		}

		hItem = m_treeCtrlLog.GetNextItem(hItem, TVGN_NEXT);
	}

	return 1;
}

//
//	在res tree, layout tree中插入一个皮肤结点
//
//	res tree中
//	皮肤名称结点的 data 为 皮肤句柄 HSKIN
//	资源名称结点的 data 为 是否需要保存 BOOL bDirty
//
void CProjectTreeDialog::InsertSkinItem(ISkinRes* pSkin)
{
	if (NULL == pSkin)
		return;

    String strSkinName;
    TCHAR* pszName = NULL;
    int    nSize = 0;

    strSkinName = pSkin->GetName();
    do 
    {
        // 保存结点信息
        SkinResData*  pSkinResData = new SkinResData;
        pSkinResData->m_pSkinRes = pSkin;
        m_mapSkinResData[pSkin] = pSkinResData;

#pragma region // layout
        {
            IListItemBase* pTreeRootLayout = m_pTreeLayout->GetFirstItem();
            if (NULL == pTreeRootLayout)
                break;

            IListItemBase* pSkinTreeItemLayout = m_pTreeLayout->InsertSkinItem(strSkinName.c_str(), pTreeRootLayout);
            {
                pSkinResData->m_pItem_LayoutTree = pSkinTreeItemLayout;

                LayoutTreeItemData* pTreeItemData = new LayoutTreeItemData;
                pTreeItemData->m_eType = TREEITEM_DATA_TYPE_LAYOUT_SKIN;
                pTreeItemData->m_pSkin = pSkin;
                pTreeItemData->m_strNodeID = strSkinName;
                pTreeItemData->m_pListItem = pSkinTreeItemLayout;
                m_mapLayoutTree[pSkinTreeItemLayout] = pTreeItemData;
            }

            // 加载皮肤下面的窗口数据
            pSkin->Load();

            ILayoutWindowNodeList* pWindowArray = NULL;
            if (pSkin->GetLayoutManager()->LoadWindowNodeList(&pWindowArray))
            {
                UINT nCount = pWindowArray->GetCount();
                for (UINT i = 0; i < nCount; i++)
                {
                    InsertWindowItem(pSkin, 
                        pWindowArray->GetWindowPath(i), 
                        pWindowArray->GetWindowName(i), 
                        pWindowArray->GetWindowId(i), 
                        false);
                }
            }
            SAFE_RELEASE(pWindowArray);
        }
        m_pTreeLayout->LayoutAllItemsAndRedraw();

#pragma endregion
        
#pragma region  // resource
        {
            IListItemBase* pTreeRootResource = m_pTreeResource->GetFirstItem();
            if (NULL == pTreeRootResource)
                break;

            IListItemBase* pSkinTreeItemResource = m_pTreeResource->InsertSkinItem(strSkinName.c_str(), pTreeRootResource);
            
            // 保存结点信息
            pSkinResData->m_pItem_ResourceTree = pSkinTreeItemResource;

            ResourceTreeItemData* pTreeItemData = new ResourceTreeItemData;
            pTreeItemData->m_eType = TREEITEM_DATA_TYPE_RESOURCE_SKIN;
            pTreeItemData->m_pSkin = pSkin;
            pTreeItemData->m_pListItem = pSkinTreeItemResource;
            m_mapResTree[pSkinTreeItemResource] = pTreeItemData;
            
            // 插入资源类型
            IListItemBase* hItem = m_pTreeResource->InsertNormalItem( _T("image"), pSkinTreeItemResource);
            if (hItem)
            {
                ResourceTreeItemData* pData = new ResourceTreeItemData;
                pData->m_pSkin = pSkin;
                pData->m_eType = TREEITEM_DATA_TYPE_RESOURCE_IMAGE;
                pData->m_pListItem = hItem;
                m_mapResTree[hItem] = pData;
            }

            hItem = m_pTreeResource->InsertNormalItem( _T("color"), pSkinTreeItemResource);
            if (hItem)
            {
                ResourceTreeItemData* pData = new ResourceTreeItemData;
                pData->m_pSkin = pSkin;
                pData->m_eType = TREEITEM_DATA_TYPE_RESOURCE_COLOR;
                pData->m_pListItem = hItem;
                m_mapResTree[hItem] = pData;
            }

            hItem = m_pTreeResource->InsertNormalItem( _T("font"), pSkinTreeItemResource);
            if (hItem)
            {
                ResourceTreeItemData* pData = new ResourceTreeItemData;
                pData->m_pSkin = pSkin;
                pData->m_eType = TREEITEM_DATA_TYPE_RESOURCE_FONT;
                pData->m_pListItem = hItem;
                m_mapResTree[hItem] = pData;
            }

            hItem = m_pTreeResource->InsertNormalItem( _T("style"), pSkinTreeItemResource);
            if (hItem)
            {
                ResourceTreeItemData* pData = new ResourceTreeItemData;
                pData->m_pSkin = pSkin;
                pData->m_eType = TREEITEM_DATA_TYPE_RESOURCE_STYLE;
                pData->m_pListItem = hItem;
                m_mapResTree[hItem] = pData;
            }
        }
#pragma endregion
    } 
    while (0);
}

SkinResData*  CProjectTreeDialog::GetSkinResData(ISkinRes* pSkinRes)
{
    map<ISkinRes*, SkinResData*>::iterator iterSkinData = m_mapSkinResData.find(pSkinRes);
    if (iterSkinData == m_mapSkinResData.end())
        return NULL;

    return iterSkinData->second;
}
LayoutTreeItemData*  CProjectTreeDialog::GetLayoutTreeItemData(IListItemBase* pItem)
{
    if (!pItem)
        return NULL;

    map<IListItemBase*, LayoutTreeItemData*>::iterator iter = m_mapLayoutTree.find(pItem);
    if (iter == m_mapLayoutTree.end())
        return NULL;

    return iter->second;
}

IListItemBase*  CProjectTreeDialog::InsertWindowXmlTreeItem(ISkinRes* pSkin, LPCTSTR szXmlPath)
{
    if (!szXmlPath)
        return NULL;

    SkinResData*  pSkinResData = GetSkinResData(pSkin);
    if (!pSkinResData)
        return NULL;

    //  查找
    IListItemBase* pParentTreeItem = pSkinResData->m_pItem_LayoutTree;
    IListItemBase* pChild = pParentTreeItem->GetChildItem();
    while (pChild)
    {
        if (0 == _tcscmp(pChild->GetText(), szXmlPath))
            return pChild;

        pChild = pChild->GetNextItem();
    }

    // 没有，创建
    ITreeItem* pTreeItem = m_pTreeLayout->InsertNormalItem(szXmlPath, pParentTreeItem);

    return pTreeItem;
}

void  CProjectTreeDialog::InsertWindowItem(
        ISkinRes* pSkin, 
        LPCTSTR szPath,
        LPCTSTR szName, 
        LPCTSTR szId,
        bool bRedraw)
{
    IListItemBase*  pParentTreeItem = InsertWindowXmlTreeItem(pSkin, szPath);
    if (!pParentTreeItem)
        return;

    String strNodeName(szId);
    if (szName && szName[0])
    {
        strNodeName.append(_T("["));
        strNodeName.append(szName);
        strNodeName.append(_T("]"));
    }

    ITreeItem*  pTreeItem = m_pTreeLayout->InsertNormalItem(strNodeName.c_str(), 
        pParentTreeItem, UITVI_LAST, LF_NONE);

    if (!pTreeItem)
        return;

    TREEITEM_DATA_TYPE  eDataType = TREEITEM_DATA_TYPE_LAYOUT_WINDOW;
    // 设置图标
    IRenderBitmap*  pIcon = NULL;
//  if (0 == _tcscmp(szName, _T("Menu")))
//  {
//      GetObjectTypeBitmap(OBJ_CONTROL, CONTROL_MENU, &pIcon);
//      eDataType = TREEITEM_DATA_TYPE_LAYOUT_MENU;
//  }
//  else if (0 == _tcscmp(szName, _T("TooltipWindow")))
//  {
//      GetObjectTypeBitmap(OBJ_WINDOW, WINDOW_TOOLTIP, &pIcon);
//  }
//  else
    {
        GetObjectTypeBitmap(OBJ_WINDOW, 0, &pIcon);
    }

    if (pIcon)
    {
        IImageRender* pImageRender = NULL;
        IImageRender::CreateInstance(g_pGlobalData->m_pMyUIApp, &pImageRender);  // 没有调用addref，因此不用释放
        pImageRender->SetImageDrawType(DRAW_BITMAP_CENTER);
        pImageRender->SetRenderBitmap(pIcon);
        SAFE_RELEASE(pIcon);
        pTreeItem->SetIconRender(pImageRender);
    }

    LayoutTreeItemData* pData = new LayoutTreeItemData;
    pData->m_pListItem = pTreeItem;
    pData->m_strNodeID = szId;
    pData->m_strXmlPath = szPath;

    if (szName && szName[0])
        pData->m_strNodeName = szName;
    else
        pData->m_bLazyLoad = true;

    pData->m_pSkin = pSkin;
    pData->m_pObject = NULL;
    pData->m_eType = eDataType;
    m_mapLayoutTree[pTreeItem] = pData;

    if (bRedraw)
        m_pTreeLayout->LayoutItem(pParentTreeItem, true);
}

String CProjectTreeDialog::GetResName( UI_RESOURCE_TYPE e )
{
	switch(e)
	{
	case UI_RESOURCE_IMAGE:
		return _T("image");
		break;
	case UI_RESOURCE_COLOR:
		return _T("color");
		break;
	case UI_RESOURCE_FONT:
		return _T("font");
		break;
	case UI_RESOURCE_STYLE:
		return _T("style");
		break;
	case UI_RESOURCE_LAYOUT:
		return _T("layout");
		break;
	}
	return _T("");
}


HTREEITEM CProjectTreeDialog::FindExplorerItem(ISkinRes* hSkin, TCHAR* szFilePath)
{
	map<HTREEITEM, ExplorerTreeItemData*>::iterator iter = m_mapTreeCtrlExplorer.begin();
	map<HTREEITEM, ExplorerTreeItemData*>::iterator iterEnd = m_mapTreeCtrlExplorer.end();

	for (; iter != iterEnd; iter++)
	{
		ExplorerTreeItemData* pData = iter->second;
		if (NULL == pData)
			continue;
		
		if (hSkin == pData->m_pSkin && 
			_tcsicmp(pData->m_strFilePath.c_str(), szFilePath) == 0)
			return pData->m_hTreeItem;
	}
	return NULL;
}

void CProjectTreeDialog::LoadObjectChildNode(IObject* pParent)
{
    if (NULL == pParent)
        return;

    IListItemBase* pParentTreeItem = g_pGlobalData->m_pProjectData->m_uieditor.GetObjectHTREEITEM(pParent);

    IObject* pChild = NULL;
    while (pChild = pParent->EnumChildObject(pChild))
    {
        InsertLayoutChildObjectNode(pChild, pParentTreeItem);
        //if (pChild->GetObjectType() != OBJ_CONTROL)
        {
            LoadObjectChildNode(pChild);
        }
    }

	pChild = NULL;
	while (pChild = pParent->EnumNcChildObject(pChild))
	{
		InsertLayoutChildObjectNode(pChild, pParentTreeItem);
		//if (pChild->GetObjectType() != OBJ_CONTROL)
		{
			LoadObjectChildNode(pChild);
		}
	}
}

bool CProjectTreeDialog::InsertLayoutChildObjectNode(
    IObject* pChild, 
    IListItemBase* pParentTreeItem, 
    IListItemBase* pInsertAfter)
{
	if (NULL == pChild || NULL == pParentTreeItem)
		return false;

    const TCHAR* szId = pChild->GetId(); 
    const TCHAR* szName = pChild->GetObjectName();

	String strNodeName(szId);
	strNodeName.append(_T("["));
	strNodeName.append(szName);
	strNodeName.append(_T("]"));

    ITreeItem* pTreeItem = m_pTreeLayout->InsertControlItem(
        strNodeName.c_str(), 
        pParentTreeItem, 
        pInsertAfter, 
        LF_NONE);
	if (pTreeItem)
	{
        // 设置图片
        IRenderBitmap*  pIcon = NULL;
        GetObjectTypeBitmap(pChild->GetObjectType(), pChild->GetObjectExtentType(), &pIcon);
        if (pIcon)
        {
            IImageRender* pImageRender = NULL;
            IImageRender::CreateInstance(g_pGlobalData->m_pMyUIApp, &pImageRender);  // 没有调用addref，因此不用释放
            pImageRender->SetImageDrawType(DRAW_BITMAP_CENTER);
            pImageRender->SetRenderBitmap(pIcon);
            SAFE_RELEASE(pIcon);
            pTreeItem->SetIconRender(pImageRender);
        }

        // 关联数据
		LayoutTreeItemData* pData = new LayoutTreeItemData;
		pData->m_pListItem = pTreeItem;
		pData->m_strNodeID = szId;
		pData->m_strNodeName = szName;
		pData->m_pObject = pChild;
        pData->m_eType = TREEITEM_DATA_TYPE_LAYOUT_OBJECT;
		pData->m_pSkin = m_mapLayoutTree[pParentTreeItem]->m_pSkin;
		m_mapLayoutTree[pTreeItem] = pData;

		g_pGlobalData->m_pProjectData->m_uieditor.SetObjectHTREEITEM(pChild, pTreeItem);
        m_pTreeLayout->LayoutItem(pTreeItem, true);
	}
	return false;
}

bool  CProjectTreeDialog::RemoveLayoutObjectNode(IObject* pChild)
{
    IListItemBase* pItem = g_pGlobalData->m_pProjectData->m_uieditor.GetObjectHTREEITEM(pChild);
    if (NULL == pItem)
        return false;

    map<IListItemBase*, LayoutTreeItemData*>::iterator iter = m_mapLayoutTree.find(pItem);
    if (iter != m_mapLayoutTree.end())
    {
        LayoutTreeItemData* pData = iter->second;
        SAFE_DELETE(pData);
        m_mapLayoutTree.erase(iter);
    }
    m_pTreeLayout->RemoveItem(pItem, UI::LF_REDRAW|UI::LF_LAYOUT);
    return true;
}

bool CProjectTreeDialog::SelectLayoutObjectNode(IObject* pChild)
{
	if (NULL == pChild)
		return false;

	IListItemBase* pItem = g_pGlobalData->m_pProjectData->m_uieditor.GetObjectHTREEITEM(pChild);
	if (NULL == pItem)
		return false;

    m_pTreeLayout->SelectItem(pItem, true);
	return true;
}

LRESULT  CProjectTreeDialog::OnLayoutTreeSelChanged(WPARAM w, LPARAM l)
{
    IListItemBase*  pNewItem = m_pTreeLayout->GetFirstSelectItem();
	if (NULL == pNewItem)
		return 0;

    LayoutTreeItemData* pData = GetLayoutTreeItemData(pNewItem);
    if (!pData)
        return 0;

	if (pData->m_eType == TREEITEM_DATA_TYPE_LAYOUT_ROOT ||
		pData->m_eType == TREEITEM_DATA_TYPE_LAYOUT_SKIN)
		return 0;

    // 2015.3.4 menu也走window，不再单独区分
//     if (pData->m_eType == TREEITEM_DATA_TYPE_LAYOUT_MENU)
//     {
//         m_pMainFrame->SwitchToMenuPanel(pData);
//     }
//     else
    {
	    m_pMainFrame->SwitchToLayoutPanel(pData);
    }
	return 0;
}

LRESULT  CProjectTreeDialog::OnLayoutTreeRClick(WPARAM w, LPARAM l)
{
    IListItemBase*  pItem = (IListItemBase*)l;
    if (NULL == pItem)
        return 0;

    POINT pt = { GET_X_LPARAM(w), GET_Y_LPARAM(w) };
    ::MapWindowPoints(GetHWND(), NULL, &pt, 1);

    map<IListItemBase*, LayoutTreeItemData*>::iterator iter = m_mapLayoutTree.find(pItem);
    if (iter == m_mapLayoutTree.end())
        return 0;

    if (TREEITEM_DATA_TYPE_LAYOUT_OBJECT == iter->second->GetDataType())
    {
#define MENU_ID_LAYOUT_OBJECT_DELETE       100
#define MENU_ID_LAYOUT_OBJECT_MOVE_PREV    101
#define MENU_ID_LAYOUT_OBJECT_MOVE_NEXT    102
#define MENU_ID_LAYOUT_OBJECT_MOVE_TOP     103
#define MENU_ID_LAYOUT_OBJECT_MOVE_BOTTOM  104

		IMenu* pMenu = NULL;
		IMenu::CreateInstance(g_pGlobalData->m_pMyUIApp, &pMenu);
		pMenu->InitDefaultAttrib();

		IListItemBase*  pDeleteMenuItem = pMenu->AppendString(_T("删除"), MENU_ID_LAYOUT_OBJECT_DELETE);
        pMenu->AppendSeparator(-1);
        IListItemBase*  pMovePrevMenuItem = pMenu->AppendString(_T("上移"), MENU_ID_LAYOUT_OBJECT_MOVE_PREV);
        IListItemBase*  pMoveNextMenuItem = pMenu->AppendString(_T("下移"), MENU_ID_LAYOUT_OBJECT_MOVE_NEXT);
        IListItemBase*  pMoveTopMenuItem = pMenu->AppendString(_T("移至顶部"), MENU_ID_LAYOUT_OBJECT_MOVE_TOP);
        IListItemBase*  pMoveBottomMenuItem = pMenu->AppendString(_T("移至底部"), MENU_ID_LAYOUT_OBJECT_MOVE_BOTTOM);

        pDeleteMenuItem->SetIconFromImageId(_T("command_delete_red"));
        pMovePrevMenuItem->SetIconFromImageId(_T("command_moveup2"));
        pMoveNextMenuItem->SetIconFromImageId(_T("command_movedown2"));
        pMoveTopMenuItem->SetIconFromImageId(_T("command_moveup"));
        pMoveBottomMenuItem->SetIconFromImageId(_T("command_movedown"));

        IObject*  pObj = GetObjectByHTREEITEM(pItem);
        if (!pObj->GetPrevObject())
        {
            pMovePrevMenuItem->SetDisable(true);
            pMoveTopMenuItem->SetDisable(true);
        }
        if (!pObj->GetNextObject())
        {
            pMoveNextMenuItem->SetDisable(true);
            pMoveBottomMenuItem->SetDisable(true);
        }

		int nRet = pMenu->TrackPopupMenu(TPM_RETURNCMD, pt.x, pt.y, NULL);
		switch (nRet)
		{
		case MENU_ID_LAYOUT_OBJECT_DELETE:
			{
				DeleteObjectItem(pItem);
			}
			break;

        case MENU_ID_LAYOUT_OBJECT_MOVE_NEXT:
            {
                UE_ExecuteCommand(MoveObjectDownCommand::CreateInstance(pObj));
            }
            break;

        case MENU_ID_LAYOUT_OBJECT_MOVE_PREV:
            {
                UE_ExecuteCommand(MoveObjectUpCommand::CreateInstance(pObj));
            }
            break;

        case MENU_ID_LAYOUT_OBJECT_MOVE_BOTTOM:
            {
                UE_ExecuteCommand(MoveObjectBottomCommand::CreateInstance(pObj));
            }
            break;

        case MENU_ID_LAYOUT_OBJECT_MOVE_TOP:
            {
                UE_ExecuteCommand(MoveObjectTopCommand::CreateInstance(pObj));
            }
            break;
		}
		SAFE_DELETE_Ixxx(pMenu);
    }
    else if (TREEITEM_DATA_TYPE_LAYOUT_WINDOW == iter->second->GetDataType())
    {
#define MENU_ID_LAYOUT_WINDOW_DELETE  100
        
//         IMenu* pMenu = NULL;
//         IMenu::CreateInstance(g_pGlobalData->m_pMyUIApp, &pMenu);
//         pMenu->InitDefaultAttrib();
//         pMenu->AppendString(_T("删除窗口"), MENU_ID_LAYOUT_WINDOW_DELETE);
//         int nRet = pMenu->TrackPopupMenu(TPM_RETURNCMD, pt.x, pt.y, NULL);
//         switch (nRet)
//         {
//         case MENU_ID_LAYOUT_WINDOW_DELETE:
//             {
//                 DeleteWindowItem(pItem);
//             }
//             break;
//         }
//         SAFE_DELETE_Ixxx(pMenu);
    }
    return 0;
}

// 拖拽控件，设置控件新位置
LRESULT   CProjectTreeDialog::OnControlDropEvent(WPARAM w, LPARAM l)
{
    ProjectTreeViewControlItemDropData* pData = (ProjectTreeViewControlItemDropData*)w;
    if (NULL == pData->pHitItem)
        return 0;

    if (pData->eType == _DragLeave)
        return 0;

    IListItemBase*  pDragItem = m_pTreeLayout->GetFirstSelectItem();
    IListItemBase*  pDropItem = pData->pHitItem;

    IObject*  pDragObj = GetObjectByHTREEITEM(pDragItem);
    IObject*  pDropObj = GetObjectByHTREEITEM(pDropItem);

    // 同一ITEM
    if (pDragItem == pDropItem)
    {
        * pData->pData->pdwEffect = DROPEFFECT_NONE;
        return 0;
    }

    // 无效obj
    if (!pDragObj || !pDropObj)
    {
        * pData->pData->pdwEffect = DROPEFFECT_NONE;
        return 0;
    }

    // 不是同一个Window
    IWindowBase*  pDragWnd = pDragObj->GetWindowObject();
    IWindowBase*  pDropWnd = pDropObj->GetWindowObject();
    if (pDragWnd != pDropWnd)
    {
        * pData->pData->pdwEffect = DROPEFFECT_NONE;
        return 0;
    }

    // 不能往子对象中拖拽
    if (pDragObj->IsMyChild(pDropObj, true))
    {
        * pData->pData->pdwEffect = DROPEFFECT_NONE;
        return 0;
    }

    // 如果是一个容器对象，则放到该容器下面
    OBJ_TYPE  eDropObjType = pDropObj->GetObjectType();
    if (eDropObjType == OBJ_PANEL || eDropObjType == OBJ_WINDOW)
    {
        // 不能再放到自己的父对象下面
        if (pDropObj->IsMyChild(pDragObj, false))
        {
            * pData->pData->pdwEffect = DROPEFFECT_NONE;
            return 0;
        }

        if (pData->eType == _Drop)
        {
            UE_ExecuteCommand(DragDropObjectCommand::CreateInstance(pDragObj, pDropObj, pDropObj->GetLastChildObject()), true);
        }
    
        * pData->pData->pdwEffect = DROPEFFECT_MOVE;
        return 0;
    }

    if (pData->eType == _Drop)
    {
        // 如果是一个控件，则放到该控件后面
        UE_ExecuteCommand(DragDropObjectCommand::CreateInstance(pDragObj, pDropObj->GetParentObject(), pDropObj), true);
    }
    * pData->pData->pdwEffect = DROPEFFECT_MOVE;
    return 0;
}

// 右击菜单，删除一个窗口
void  CProjectTreeDialog::DeleteWindowItem(IListItemBase*  pItem)
{
    map<IListItemBase*, LayoutTreeItemData*>::iterator iter = m_mapLayoutTree.find(pItem);
    if (iter == m_mapLayoutTree.end())
        return;

    IObject* pOb = iter->second->m_pObject;
}

IObject*  CProjectTreeDialog::GetObjectByHTREEITEM(IListItemBase*  pItem)
{
    map<IListItemBase*, LayoutTreeItemData*>::iterator iter = m_mapLayoutTree.find(pItem);
    if (iter == m_mapLayoutTree.end())
        return NULL;

    IObject* pObj = iter->second->m_pObject;
    return pObj;
}

// 右击菜单，删除一个控件
void   CProjectTreeDialog::DeleteObjectItem(IListItemBase*  pItem)
{
	IObject* pObj = GetObjectByHTREEITEM(pItem);
    if (!pObj)
        return;

	UE_ExecuteCommand(RemoveObjectCommand::CreateInstance(pObj), true);
}

//
// 调用堆栈
// CProjectTreeDialog::OnObjectDeleteInd
// CUIEditor::OnObjectDeleteInd
// Object::FinalRelease
//
void  CProjectTreeDialog::OnObjectDeleteInd(ObjectEditorData* pObjData)
{
	map<IListItemBase*, LayoutTreeItemData*>::iterator iter = m_mapLayoutTree.find(pObjData->pTreeItem);
	if (iter != m_mapLayoutTree.end())
	{
		LayoutTreeItemData* pData = (*iter).second;
		if (pData->m_eType == TREEITEM_DATA_TYPE_LAYOUT_OBJECT)
		{
			m_pTreeLayout->RemoveItem(pObjData->pTreeItem, LF_LAYOUT|LF_REDRAW);
			SAFE_DELETE(pData);
			m_mapLayoutTree.erase(iter);
		}
		else
		{
			pData->m_pObject = NULL;
		}
	}
}


// void  CProjectTreeDialog::OnEraseBkgnd(IRenderTarget* pRenderTarget)
// {
//     CRect  rc;
//     this->GetClientRect(&rc);
// 
//     UI::Color cBack(241,237,237,255);
//     UI::Color cTopLine(145,145,145,255);
//     UI::Color cBorder(0xFFFFFFFF);
// 
//     pRenderTarget->FillRect(&rc, &cBack);
// }


void  CProjectTreeDialog::OnBtnLayout()
{
    if (m_pTreeLayout)
    {
        m_pTreeLayout->SetVisible(true);
    }
}
void  CProjectTreeDialog::OnBtnResource()
{
    if (m_pTreeResource)
    {
        m_pTreeResource->SetVisible(true);
    }
}

// 控件的id被修改后，更新树结点上的名字
void  CProjectTreeDialog::UpdatLayoutObjectTreeItemText(IObject* pObj)
{
    if (!pObj)
        return;

    IListItemBase* pItem = GetLayoutObjectTreeItem(pObj);
    if (!pItem)
        return;

    const TCHAR* szId = pObj->GetId(); 
    const TCHAR* szName = pObj->GetObjectName();

    String strNodeName(szId);
    strNodeName.append(_T("["));
    strNodeName.append(szName);
    strNodeName.append(_T("]"));

    pItem->SetText(strNodeName.c_str());
    m_pTreeLayout->InvalidateItem(pItem);
    m_pTreeLayout->Refresh();
}

// 获取一个控件的树结点
IListItemBase*  CProjectTreeDialog::GetLayoutObjectTreeItem(IObject* pObj)
{
    if (!pObj)
        return NULL;

    ObjectEditorData* pData = GetUIEditor()->GetObjectEditorData(pObj);
    if (pData)
        return pData->pTreeItem;

//     map<IListItemBase*, LayoutTreeItemData*>::iterator iter = 
//         m_mapLayoutTree.begin();
//     for (; iter != m_mapLayoutTree.end(); ++iter)
//     {
//         if (iter->second->m_pObject == pObj)
//             return iter->first;
//     }

    return NULL;
}