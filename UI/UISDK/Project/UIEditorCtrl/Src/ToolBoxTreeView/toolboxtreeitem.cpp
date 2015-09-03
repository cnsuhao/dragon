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

            // DoDrag�ὫWM_LBUTOTNUP��Ϣ�Ե��������ֶ���һ��
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
    // SAFE_DELETE_GDIOBJECT(hDragBitmap); -- �Լ��ֶ�ɾ����ʧ�ܡ��ǲ����Զ����ͷ��ˣ�

    return;
}


// ע�����ֱ�ӽ�Item���ڻ����(0,0)���������קʱ������������ؾ�Ȼ��ʾ���ұ�ȥ�ˣ�
//     ����Ҳ��֪��windows�ڲ�����ô����ġ���˽�����ͼƬ����g_rcDragImgPadding��
//     ������קʱ��ʾ��ͼƬ����������
HBITMAP  ToolBoxTreeItem::CreateDragBitmap(int* pWidth, int* pHeight)
{
    CRect  rcParent;
    m_pIToolBoxTreeItem->GetParentRect(&rcParent);

    int nHeight = rcParent.Height() + g_rcDragImgPadding.left + g_rcDragImgPadding.right;
    int nWidth = rcParent.Width() + g_rcDragImgPadding.top + g_rcDragImgPadding.bottom;

    // ��Win7�����������£���N����֤�õ��������������С����������קʱ�޷���ʾͼƬ
    if (nWidth < 295)
        nWidth = 295;
    if (nHeight < 86)
        nHeight = 86;

    // ����λͼ
    UI::ImageWrap  image;
    image.Create(nWidth, nHeight, 32, UI::ImageWrap::createAlphaChannel);
    HDC hDC = image.GetDC();

    // ����rendertarget
    HWND hWnd = m_pIToolBoxTreeItem->GetIListCtrlBase()->GetHWND();
    UI::IRenderTarget*  pRenderTarget = UI::UICreateRenderTarget(NULL, GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS, true);
    pRenderTarget->BindHDC(hDC);

    // ����
    pRenderTarget->BeginDraw();
    pRenderTarget->SetViewportOrgEx(
        -rcParent.left+g_rcDragImgPadding.left, 
        -rcParent.top+g_rcDragImgPadding.top);  // ����DrawItemInnerControl�е�ƫ��

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