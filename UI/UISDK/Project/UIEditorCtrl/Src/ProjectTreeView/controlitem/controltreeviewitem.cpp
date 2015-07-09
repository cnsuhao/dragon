#include "stdafx.h"
#include "ControlTreeViewItem.h"
#include "UISDK\Kernel\Inc\Util\iimage.h"

namespace UI
{
UINT ControlTreeViewItem::s_nControlDragCF = 0;

ControlTreeViewItem::ControlTreeViewItem()
{
    m_pIControlTreeViewItem = NULL;
    m_ptLastLButtonDown.x = m_ptLastLButtonDown.y = 0;

    if (0 == s_nControlDragCF)
    {
        s_nControlDragCF = ::RegisterClipboardFormat(_T("UIEDITOR_PROJECTTREEVIEW_CONTROL_CF"));
    }
}
ControlTreeViewItem::~ControlTreeViewItem()
{

}

void  ControlTreeViewItem::OnMouseMove(UINT nFlags, POINT point)
{
    if (nFlags & MK_LBUTTON)
    {
        if (point.x != m_ptLastLButtonDown.x || 
            point.y != m_ptLastLButtonDown.y)
        {
            DoDrag(point);

            // DoDrag�ὫWM_LBUTOTNUP��Ϣ�Ե��������ֶ���һ��
            PostMessage(m_pIControlTreeViewItem->GetIListCtrlBase()->GetHWND(),
                WM_LBUTTONUP, 0, MAKELPARAM(point.x, point.y));
        }
    }
    SetMsgHandled(FALSE);
}
void  ControlTreeViewItem::OnLButtonDown(UINT nFlags, POINT point)
{
    m_ptLastLButtonDown.x = point.x;
    m_ptLastLButtonDown.y = point.y;

    SetMsgHandled(FALSE);  // SELECTED/FOCUS
}


void  ControlTreeViewItem::DoDrag(POINT point)
{
    // �Ƚ��Լ��۵��ϣ���������ק���Լ����ӽ��
    bool  bOldExpand = false;
    if (m_pIControlTreeViewItem->HasChild() && m_pIControlTreeViewItem->IsExpand())
        bOldExpand = true;
    if (bOldExpand)
    {
        m_pIControlTreeViewItem->SetExpand(false);
        m_pIControlTreeViewItem->GetIListCtrlBase()->LayoutItem(m_pIControlTreeViewItem, true);
    }


    IDataObject*  pDataObject = NULL;
    IDropSource*  pDropSource = NULL;
    IDragSourceHelper2*  pDragSourceHelper = NULL;

    UI::CreateDataObjectInstance(&pDataObject, NULL);
    UI::CreateDropSourceInstance(&pDropSource);
    ::CoCreateInstance(CLSID_DragDropHelper, NULL, CLSCTX_INPROC, IID_IDragSourceHelper2, (void**)&pDragSourceHelper);

#pragma region
    FORMATETC  format = {0};
    format.dwAspect = DVASPECT_CONTENT;
    format.cfFormat = s_nControlDragCF;
    format.tymed = TYMED_HGLOBAL;

    const TCHAR* szText = m_pIControlTreeViewItem->GetText();    
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
        m_pIControlTreeViewItem->GetParentRect(&rcParent);
        m_pIControlTreeViewItem->GetIListCtrlBase()->ItemRect2WindowRect(&rcParent, &rcWindow);

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

    if (bOldExpand)
    {
        m_pIControlTreeViewItem->SetExpand(true);
        m_pIControlTreeViewItem->GetIListCtrlBase()->LayoutItem(m_pIControlTreeViewItem, true);
    }
    return;
}


// ע�����ֱ�ӽ�Item���ڻ����(0,0)���������קʱ������������ؾ�Ȼ��ʾ���ұ�ȥ�ˣ�
//     ����Ҳ��֪��windows�ڲ�����ô����ġ���˽�����ͼƬ����g_rcDragImgPadding��
//     ������קʱ��ʾ��ͼƬ����������
HBITMAP  ControlTreeViewItem::CreateDragBitmap(int* pWidth, int* pHeight)
{
    CRect  rcParent;
    m_pIControlTreeViewItem->GetParentRect(&rcParent);
    IListCtrlBase*  pListCtrl = m_pIControlTreeViewItem->GetIListCtrlBase();

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
    {
        ITextRenderBase*  pTextRender = m_pIControlTreeViewItem->GetIListCtrlBase()->GetTextRender();
        IRenderFont*  pRenderFont = pTextRender->GetRenderFont();
        LOGFONT  lf = {0};
        pRenderFont->GetLogFont(&lf);

        // ��������λ��
        Gdiplus::Graphics  g(hDC);
        Gdiplus::RectF  rcLayout(
            (Gdiplus::REAL)g_rcDragImgPadding.left, 
            (Gdiplus::REAL)g_rcDragImgPadding.top, 
            (Gdiplus::REAL)rcParent.Width(), 
            (Gdiplus::REAL)rcParent.Height());

        INormalTreeItemShareData*  pShareData = (INormalTreeItemShareData*)pListCtrl->GetItemTypeShareData(LISTITEM_TYPE_TREEITEM_NORMAL);
        int nOffset = pListCtrl->GetChildNodeIndent()* m_pIControlTreeViewItem->CalcDepth();
        nOffset += pShareData->GetImageWidth() + pShareData->GetExpandIconWidth();
        rcLayout.Offset((Gdiplus::REAL)nOffset, 0);
        
        Gdiplus::Font  font(hDC, &lf);
        Gdiplus::SolidBrush brush(Gdiplus::Color(0xFEFF0000));

        Gdiplus::StringFormat  sf;
        sf.SetAlignment(Gdiplus::StringAlignmentNear);
        sf.SetLineAlignment(Gdiplus::StringAlignmentCenter);

        g.DrawString(m_pIControlTreeViewItem->GetText(), -1, &font, rcLayout, &sf, &brush);
    }
    image.ReleaseDC();

    if (pWidth)
        *pWidth = image.GetWidth();
    if (pHeight)
        *pHeight = image.GetHeight();

    return image.Detach();
}

}