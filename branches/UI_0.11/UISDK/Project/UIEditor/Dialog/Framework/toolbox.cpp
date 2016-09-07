#include "stdafx.h"
#include "toolbox.h"
#include "UISDK\Kernel\Inc\Interface\iimagerender.h"
#include "UISDK\Project\UIEditorCtrl\Inc\itoolboxtreeview.h"

CToolBox::CToolBox()
{
    m_pTreeView = NULL;
}
CToolBox::~CToolBox()
{

}

void  CToolBox::OnInitialize()
{
    m_pTreeView = (ITreeView*)FindChildObject(_T("tree"));
    if (m_pTreeView)
    {
        GetEditUIApplication()->LoadUIObjectListToToolBox();
        m_pTreeView->ModifyStyleEx(TREE_STYLE_HASLINE, 0, 0);
    }
}

BOOL  CToolBox::PreCreateWindow(CREATESTRUCT* pcs)
{
    pcs->dwExStyle = WS_EX_TOOLWINDOW;
    return TRUE;
}

void  CToolBox::InsertObject(const TCHAR* szName, const TCHAR* szCategory, UINT nObjType)
{
    if (!m_pTreeView)
        return;

    OBJ_TYPE nMajorType = ::GetObjectMajorType(nObjType);
    if (nMajorType == OBJ_WINDOW)
        return;

    if (NULL == szCategory)
        szCategory = _T("Default");

    IListItemBase* pParent = NULL;

    Util::ISplitStringEnum* pEnum = NULL;
    int nSize = Util::SplitString(szCategory, XML_PATH_SEPARATOR, &pEnum);
    for (int i = 0; i < nSize; i++)
    {
        IListItemBase* pFind = m_pTreeView->FindChildItemByText(pEnum->GetText(i), pParent, NULL);
        if (pFind)
            pParent = pFind;
        else
            pParent = m_pTreeView->InsertNormalItem(pEnum->GetText(i), pParent?pParent:UITVI_ROOT);
    }
    SAFE_RELEASE(pEnum);

    IToolBoxTreeItem* pTreeItem = NULL;
    IToolBoxTreeItem::CreateInstance(g_pGlobalData->m_pMyUIApp->GetDefaultSkinRes(), &pTreeItem);
//    ITreeItem* pTreeItem = m_pTreeView->InsertNormalItem(szName, pParent);
    m_pTreeView->InsertItem(pTreeItem, pParent, UITVI_LAST);
    pTreeItem->SetText(szName);

    // 设置图片
    IRenderBitmap*  pIcon = NULL;
    GetObjectTypeBitmap(nMajorType, ::GetObjectExtentType(nObjType), &pIcon);
    if (pIcon)
    {
        IImageRender* pImageRender = NULL;
        IImageRender::CreateInstance(g_pGlobalData->m_pMyUIApp->GetDefaultSkinRes(), &pImageRender);  // 没有调用addref，因此不用释放
        pImageRender->SetImageDrawType(DRAW_BITMAP_CENTER);
        pImageRender->SetRenderBitmap(pIcon);
        SAFE_RELEASE(pIcon);
        pTreeItem->SetIconRender(pImageRender);
    }
}
