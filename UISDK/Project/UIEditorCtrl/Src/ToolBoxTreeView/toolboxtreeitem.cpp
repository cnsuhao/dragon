#include "stdafx.h"
#include "toolboxtreeitem.h"
#include "UISDK\Kernel\Inc\Util\iimage.h"

const RECT g_rcDragImgPadding = {10,10,10,10};
namespace UI
{
UINT ToolBoxTreeItem::s_nLayoutItemDragCF = 0;

ToolBoxTreeItem::ToolBoxTreeItem()
{
    m_pIToolBoxTreeItem = NULL;
    m_ptLastLButtonDown.x = m_ptLastLButtonDown.y = 0;

    if (0 == s_nLayoutItemDragCF)
    {
        s_nLayoutItemDragCF = ::RegisterClipboardFormat(_T("UIEDITOR_LAYOUTVIEW_OBJECT_CF"));
    }
}

void  ToolBoxTreeItem::OnLButtonUp(UINT nFlags, POINT point)
{
    SetMsgHandled(FALSE);
}
void  ToolBoxTreeItem::OnMouseMove(UINT nFlags, POINT point)
{
    if (nFlags & MK_LBUTTON)
    {
        if (point.x != m_ptLastLButtonDown.x || 
            point.y != m_ptLastLButtonDown.y)
        {
            DoDrag(point);

            // DoDrag会将WM_LBUTOTNUP消息吃掉，这里手动发一个
            PostMessage(m_pIToolBoxTreeItem->GetIListCtrlBase()->GetHWND(),
                WM_LBUTTONUP, 0, MAKELPARAM(point.x, point.y));
        }
    }
    SetMsgHandled(FALSE);
}
void  ToolBoxTreeItem::OnLButtonDown(UINT nFlags, POINT point)
{
    m_ptLastLButtonDown.x = point.x;
    m_ptLastLButtonDown.y = point.y;

    SetMsgHandled(FALSE);  // SELECTED/FOCUS
}


void  ToolBoxTreeItem::DoDrag(POINT point)
{
    IDataObject*  pDataObject = NULL;
    IDropSource*  pDropSource = NULL;
    IDragSourceHelper2*  pDragSourceHelper = NULL;

    UI::CreateDataObjectInstance(&pDataObject);
    UI::CreateDropSourceInstance(&pDropSource);
    ::CoCreateInstance(CLSID_DragDropHelper, NULL, CLSCTX_INPROC, IID_IDragSourceHelper2, (void**)&pDragSourceHelper);

#pragma region
    FORMATETC  format = {0};
    format.dwAspect = DVASPECT_CONTENT;
    format.cfFormat = s_nLayoutItemDragCF;
    format.tymed = TYMED_HGLOBAL;

    const TCHAR* szText = m_pIToolBoxTreeItem->GetText();    
    HGLOBAL hGlobal = GlobalAlloc(0, sizeof(TCHAR)*(_tcslen(szText)+1));
    TCHAR* lpDest = (TCHAR*)::GlobalLock(hGlobal);
    _tcscpy(lpDest, szText);
    ::GlobalUnlock(hGlobal);

    STGMEDIUM medium = {0};
    medium.tymed = TYMED_HGLOBAL;
    medium.hGlobal = hGlobal;
    pDataObject->SetData(&format, &medium, TRUE);
#pragma endregion

    HBITMAP hDragBitmap = NULL;
    if (pDragSourceHelper)
    {
        SHDRAGIMAGE image = {0};
        hDragBitmap = CreateDragBitmap((int*)&image.sizeDragImage.cx, (int*)&image.sizeDragImage.cy);
        image.hbmpDragImage = hDragBitmap;

        CRect rcParent;
        CRect rcWindow;
        m_pIToolBoxTreeItem->GetParentRect(&rcParent);
        m_pIToolBoxTreeItem->GetIListCtrlBase()->ItemRect2WindowRect(&rcParent, &rcWindow);

        image.ptOffset.x = point.x - rcWindow.left + g_rcDragImgPadding.left;
        image.ptOffset.y = point.y - rcWindow.top + g_rcDragImgPadding.top;

        HRESULT hr = pDragSourceHelper->InitializeFromBitmap(&image, pDataObject);
        UIASSERT(SUCCEEDED(hr));
    }

    DWORD dwEffect = 0;
    HRESULT hr = SHDoDragDrop(NULL, pDataObject, pDropSource, DROPEFFECT_COPY|DROPEFFECT_MOVE, &dwEffect);
    SAFE_RELEASE(pDataObject);
    SAFE_RELEASE(pDropSource);
    SAFE_RELEASE(pDragSourceHelper);
    // SAFE_DELETE_GDIOBJECT(hDragBitmap); -- 自己手动删除会失败。是不是自动被释放了？

    return;
}


// 注：如果直接将Item画在缓存的(0,0)处，结果拖拽时左侧有若干像素居然显示到右边去了，
//     具体也不知道windows内部是怎么处理的。因此将缓存图片增大g_rcDragImgPadding，
//     这样拖拽时显示的图片就能正常了
HBITMAP  ToolBoxTreeItem::CreateDragBitmap(int* pWidth, int* pHeight)
{
    CRect  rcParent;
    m_pIToolBoxTreeItem->GetParentRect(&rcParent);

    int nHeight = rcParent.Height() + g_rcDragImgPadding.left + g_rcDragImgPadding.right;
    int nWidth = rcParent.Width() + g_rcDragImgPadding.top + g_rcDragImgPadding.bottom;

    // 在Win7无主题的情况下，经N次验证得到这两个数。如果小于它，则拖拽时无法显示图片
    if (nWidth < 295)
        nWidth = 295;
    if (nHeight < 86)
        nHeight = 86;

    // 创建位图
    UI::ImageWrap  image;
    image.Create(nWidth, nHeight, 32, UI::ImageWrap::createAlphaChannel);
    HDC hDC = image.GetDC();

    // 创建rendertarget
    HWND hWnd = m_pIToolBoxTreeItem->GetIListCtrlBase()->GetHWND();
    UI::IRenderTarget*  pRenderTarget = UI::UICreateRenderTarget(NULL, GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS, true);
    pRenderTarget->BindHDC(hDC);

    // 绘制
    pRenderTarget->BeginDraw();
    pRenderTarget->SetViewportOrgEx(
        -rcParent.left+g_rcDragImgPadding.left, 
        -rcParent.top+g_rcDragImgPadding.top);  // 抵消DrawItemInnerControl中的偏移

    UISendMessage(m_pIToolBoxTreeItem, WM_PAINT, (WPARAM)pRenderTarget, 0);
    pRenderTarget->EndDraw();

    SAFE_RELEASE(pRenderTarget);
    image.ReleaseDC();

    if (pWidth)
        *pWidth = image.GetWidth();
    if (pHeight)
        *pHeight = image.GetHeight();

    return image.Detach();
}

}