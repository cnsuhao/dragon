#include "stdafx.h"
#include "renderchain.h"

#include "UISDK\Kernel\Inc\Interface\iobject.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Inc\Interface\imapattr.h"
#include "UISDK\Kernel\Inc\Interface\ixmlwrap.h"
#include "UISDK\Kernel\Src\RenderLayer\bufferrenderlayer.h"
#include "UISDK\Kernel\Src\RenderLayer\directrenderlayer.h"
#include "UISDK\Kernel\Src\Helper\layout\canvaslayout.h"
#include "UISDK\Kernel\Src\RenderLayer\3dlayer.h"

// layout.xml RenderChain��ǩ����
HRESULT  RenderChain::LoadRenderChain(IUIElement* pUIElement, IUIApplication*, IObject* pObjParent, IObject**)
{
    if (NULL == pUIElement || NULL == pObjParent || OBJ_WINDOW != pObjParent->GetObjectType())
        return E_INVALIDARG;

    IWindowBase*  pWindow = (IWindowBase*)pObjParent;
    RenderChain*  pRenderChain = pWindow->GetImpl()->GetRenderChain();
    if (NULL == pRenderChain)
        return E_FAIL;

    IUIChildNodeEnum*  pChildEnum = pUIElement->EnumChild();
    if (NULL == pChildEnum)
        return E_FAIL;
    
    IUIElement*  pChildElement = NULL;
    while (pChildElement = pChildEnum->NextElement())
    {
        pRenderChain->OnNewChildElement(pChildElement);
        SAFE_RELEASE(pChildElement);
    }
    SAFE_RELEASE(pChildEnum);
    return true;
}
HRESULT  RenderChain::OnNewChildElement(IUIElement* pChildElement)
{
    CComBSTR  bstrTagName;
    pChildElement->GetTagName(&bstrTagName);

    RenderLayer*  pRenderLayer = NULL;
    if (bstrTagName == DirectRenderLayer::GetXmlName())
    {
        IDirectRenderLayer* pi = NULL;
        IDirectRenderLayer::CreateInstance(m_pWindow->GetUIApplication(), &pi);
        if (NULL == pi)
            return false;

        pRenderLayer = pi->GetImpl();
        pRenderLayer->SetWindowPtr(m_pWindow);  // ��ǰ����SetWindowPtr����֤�Ӷ����ܹ���������GetWindowObject
        pRenderLayer->SetRenderChainPtr(this);

        IMapAttribute*  pMapAttrib = NULL;
        pChildElement->GetAttribList(&pMapAttrib);

		SERIALIZEDATA data = {0};
		data.nFlag = SERIALIZEFLAG_LOAD;
		data.pMapAttrib = pMapAttrib;
		UISendMessage(pi, UI_WM_SERIALIZE, (WPARAM)&data);
//        UISendMessage(pi, UI_WM_SETATTRIBUTE, (WPARAM)pMapAttrib, (LPARAM)false);

        SAFE_RELEASE(pMapAttrib);
    }
    else if (bstrTagName == ControlRenderLayer::GetXmlName())
    {
        IControlRenderLayer*  pILayer = NULL;
        IControlRenderLayer::CreateInstance(m_pWindow->GetUIApplication(), &pILayer);
        pRenderLayer = pILayer->GetImpl();

        pRenderLayer->SetWindowPtr(m_pWindow);   // ��ǰ����SetWindowPtr����֤�Ӷ����ܹ���������GetWindowObject
        pRenderLayer->SetRenderChainPtr(this);

        ILayoutManager*  pLayoutMgr = m_pWindow->GetUIApplication()->GetActiveSkinLayoutMgr();
        if (pLayoutMgr)
            pLayoutMgr->LoadObjAttrAndChild(static_cast<IObject*>(pILayer), pChildElement);
    }

    if (pRenderLayer)
    {
        this->InsertLayer(pRenderLayer);
        return S_OK;
    }

    return E_FAIL;
}

RenderChain::RenderChain(IRenderChain* pIRenderChain)
{
    m_pIRenderChain = pIRenderChain;

    m_pWindow = NULL;
	m_hOldBitmap = NULL;
//	m_hMemBitmap = NULL;
	m_hMultiLayerMemDC = NULL;
	m_nLayerCount = 0;
	m_pWindowLayer = NULL;
    m_pFirstLayer = NULL;
    m_p3DLayer = NULL;
    m_lRefCanCommit = 0;

    m_bFirstTimePaintWindow = true;
    SetCanCommit(false);

}
RenderChain::~RenderChain()
{
	RenderLayer* pLayer = m_pFirstLayer;
	while (pLayer)
	{
		RenderLayer* pLayerTemp = pLayer->GetNextLayer();
        if (pLayer != m_pWindowLayer)  // m_pWindowLayer��ʵ����һ��Window�������ⲿ�������٣������ﲻ�ܹ�
        {
            pLayer->GetIRenderLayer()->delete_this();
        }
		pLayer = pLayerTemp;
	}
	m_nLayerCount = 0;
	this->DestroyDoubleBuffer();
}

void  RenderChain::Init(WindowBase* pWindow)
{
    m_pWindow = pWindow;

    m_pWindowLayer = static_cast<WindowRenderLayer*>(pWindow);
    m_pFirstLayer = m_pWindowLayer;
    m_pWindowLayer->SetWindowPtr(pWindow);
    m_pWindowLayer->SetRenderChainPtr(this);
    m_nLayerCount++;
}

HDC  RenderChain::GetMemoryDC()
{
	if (NULL == m_hMultiLayerMemDC && NULL != m_pWindowLayer)
	{
        return m_pWindowLayer->GetMemoryLayerDC();
	}
	else
	{
		return m_hMultiLayerMemDC;
	}
	return NULL;
}
Image*  RenderChain::GetMemoryBuffer()
{
    if (NULL == m_hMultiLayerMemDC && NULL != m_pWindowLayer)
        return m_pWindowLayer->GetMemoryLayerBuffer();
    else
        return &m_MultiLayerBuffer;
}
int  RenderChain::GetLayerCount()
{
	return m_nLayerCount;
}
RenderLayer* RenderChain::GetLastLayer()
{
	RenderLayer* p = m_pFirstLayer;
	if (NULL == p)
		return NULL;

	while (NULL != p->GetNextLayer())
	{
		p = p->GetNextLayer();
	}

	return p;
}

IRenderLayer*  RenderChain::FindLayer(const TCHAR*  szLyerID)
{
	if (NULL == szLyerID)
		return NULL;

	RenderLayer* p = m_pFirstLayer;
	while (p)
	{
		if (0 == wcscmp(szLyerID, p->GetId()))
			return p->GetIRenderLayer();

		p = p->GetNextLayer();
	}

	return NULL;
}

bool RenderChain::InsertLayer(RenderLayer*  p)
{
    if (NULL == p)
        return false;

	if (NULL == m_pFirstLayer)
	{
		m_pFirstLayer = p;
	}
	else
	{
		RenderLayer* pLayer = m_pFirstLayer;
		while (1)
		{
			if (p->GetZorder() >= pLayer->GetZorder())  // ZԽ��Խ�����棬����Խ��
			{
				if (NULL == pLayer->GetNextLayer())     //  ���������
				{
					pLayer->SetNextLayer(p);
					p->SetPrevLayer(pLayer);
					break;
				}
				pLayer = pLayer->GetNextLayer();
				continue;
			}

			// ����pLayerǰ��
			if (pLayer->GetPrevLayer())
			{
				pLayer->GetPrevLayer()->SetNextLayer(p);
				p->SetPrevLayer(pLayer->GetPrevLayer());
			}
			p->SetNextLayer(pLayer);
			pLayer->SetPrevLayer(p);

			if (m_pFirstLayer == pLayer)
				m_pFirstLayer = p;

			break;
		}
	}

	m_nLayerCount++;

	if (NULL == m_hMultiLayerMemDC && m_nLayerCount > 1)  // �����ڶ����ʱ���͵ô���һ������������ϸ�����
	{
		CRect rc;
		::GetClientRect(m_pWindow->GetHWND(), &rc );
		CreateDoubleBuffer(rc.Width(), rc.Height());
	}
	return true;
}

void  RenderChain::RemoveLayer(RenderLayer*  pRenderLayer)
{
    if (!pRenderLayer)
        return;

    if (pRenderLayer == static_cast<RenderLayer*>(m_pWindowLayer))
        return;


    RenderLayer* pLayer = m_pFirstLayer;
    while (pLayer)
    {
        RenderLayer* pLayerNext = pLayer->GetNextLayer();
        if (pLayer == pRenderLayer)
        {
            RenderLayer* pLayerPrev = pLayer->GetPrevLayer();
            if (pLayerNext)
                pLayerNext->SetPrevLayer(pLayerPrev);
            if (pLayerPrev)
                pLayerPrev->SetNextLayer(pLayerNext);

            if (m_pFirstLayer == pRenderLayer)
                m_pFirstLayer = pLayerNext;

            if (pLayer == static_cast<RenderLayer*>(m_p3DLayer))
                m_p3DLayer = NULL;

            pLayer->GetIRenderLayer()->delete_this();
            m_nLayerCount --;
            if (1 == m_nLayerCount)
            {
                this->DestroyDoubleBuffer();
            }
            break;
        }
        pLayer = pLayerNext;
    }
}

void  RenderChain::Commit(RECT* prc)
{
	CombineAllLayer(NULL);
	if (CanCommit())
	{
		m_pWindow->CommitDoubleBuffet2Window(NULL, prc, prc?1:0);
	}
}

void RenderChain::CombineAllLayer(HRGN hRgn)
{
	if (hRgn)
	{
		::SelectClipRgn(m_hMultiLayerMemDC, hRgn);
	}

    if (m_hMultiLayerMemDC)
    {
	    CRect rc(0,0, m_pWindow->GetWidth(), m_pWindow->GetHeight());
	    ::FillRect(m_hMultiLayerMemDC, &rc, (HBRUSH)::GetStockObject(BLACK_BRUSH));
    }

	RenderLayer* pLayer = m_pFirstLayer;
	while (pLayer)
	{
		if (pLayer->IsLayerEnable())
		{
			pLayer->Commit(m_hMultiLayerMemDC);  // ��m_hMemDCΪ��ʱ������ֻ��һ��control layer����ʱ����Ҫ��ϸ���layer
		}
		pLayer = pLayer->GetNextLayer();
	}

	if (hRgn)
	{
		::SelectClipRgn(m_hMultiLayerMemDC, NULL);
	}
}
void RenderChain::OnWindowPaint(HDC hDC)
{
    if (m_bFirstTimePaintWindow)
    {
        m_bFirstTimePaintWindow = false;
        SetCanCommit(true);
    }

    if (m_hMultiLayerMemDC || m_pWindow->IsTransparent())  // ˢ�»��棬����alpha����
    {
	    CRect rc(0,0, m_pWindow->GetWidth(), m_pWindow->GetHeight());
	    ::FillRect(GetMemoryDC(), &rc, (HBRUSH)::GetStockObject(BLACK_BRUSH));
    }

	RenderLayer* pLayer = m_pFirstLayer;
	while (pLayer)
	{
		if (pLayer->IsLayerEnable())
		{
			pLayer->Draw();  // ��m_hMemDCΪ��ʱ������ֻ��һ��control layer����ʱ����Ҫ��ϸ���layer
			pLayer->Commit(m_hMultiLayerMemDC);
		}
		pLayer = pLayer->GetNextLayer();
	}

    if (CanCommit())
        m_pWindow->CommitDoubleBuffet2Window(hDC, NULL);
}

void RenderChain::OnWindowResize(int nType, int nWidth, int nHeight)
{
	if (m_hMultiLayerMemDC)   // ������OnPaint���𴴽��Ի��壬�����ڷֲ㴰�ڸմ�����OnSize��ȥRedrawObject����ʱ��û����ӦOnInitWindow
	{
		this->CreateDoubleBuffer(nWidth, nHeight);
	}

    // window layer��ʵ���Ǵ��ڣ�����ʱռ���������ڡ�����layer������Զ���λ�úʹ�С
    // ����layer���ڲ���window���Ӷ�����Щ��Ҫ�������ֶ����֣�ֻ֧��canvas���֣�

	RenderLayer* pLayer = m_pFirstLayer;
	while (pLayer)
	{
        if (pLayer == m_pWindowLayer)
        {
            UISendMessage(pLayer, WM_SIZE, (WPARAM)nType, MAKELPARAM(nWidth, nHeight));
            pLayer->UpdateLayout(false);
        }
        else
        {
            CanvasLayout::ArrangeObject(pLayer, nWidth, nHeight);
        }

		pLayer = pLayer->GetNextLayer();
	}
}

void RenderChain::UpdateObject(Object* pObj, bool bOnlyRedrawBkgnd, bool bUpdateNow)
{
    if (NULL == pObj)
        return;

    // �������������������ص���Ŀǰ���ػ�����޷���֤��ʾz�����ֱ��ˢ�¸������Ա�֤z index
    if (pObj->TestStyle(OBJECT_STYLE_ZINDEX_OVERLAP) && pObj->GetParentObject())
    {
        UpdateObject(pObj->GetParentObject(), bOnlyRedrawBkgnd, bUpdateNow);
        return;
    }
//    m_MultiLayerBuffer.Save(L"C:\\bbb.png", Gdiplus::ImageFormatPNG);

    // ����֮����ȥ����window���͵��ж�����Ϊ����һ�ֳ�����
    // �ҵ���pWindow->UpdateObject(false)��ˢ������All Layer�������ύ��window��ȥ��ʹ��Invalidate����������
//     int nObjType = pObj->GetObjectType();
//     if (OBJ_HWNDHOST == nObjType)
//     {
//         // ϵͳ�ؼ���ˢ�������Լ�������
//         return;
//     }
//     else if (OBJ_WINDOW == nObjType)
//     {
//         HWND hWnd = pObj->GetHWND();
//         if (NULL == hWnd)
//             return;
// 
//         ::InvalidateRect(hWnd, NULL, TRUE);
//         if (bUpdateNow)
//             UpdateWindow(hWnd);
// 
//         return;
//     }

    RenderLayer*    pLayer = NULL;
    IRenderTarget*  pRenderTarget = NULL;

    pLayer = pObj->GetRenderLayer();
    if (!pLayer)
        return;

    if (m_p3DLayer && pLayer == m_p3DLayer)
    {
        m_p3DLayer->SetDirty(true);
        if (bUpdateNow)
        {
            m_p3DLayer->Draw();

            CombineAllLayer(NULL);
            if (CanCommit())
                m_pWindow->CommitDoubleBuffet2Window(NULL, NULL);
        }
        return;
    }

    if (!pLayer->IsLayerEnable() || !pLayer->CanRedraw())
        return;

    pRenderTarget = pLayer->GetRenderTarget();
    if (!pRenderTarget)
        return;

    // ��ȡ�����ڲ��ϵĿɼ�����
     CRect rcObjVisible;
     if (false == pObj->GetObjectVisibleRect(&rcObjVisible, false))   
         return;  // �ö����ڴ����ϲ��ɼ���������

    // ����Ҫά�ּ���������rendertarget begindrawһ���Ը㶨����
    RenderContext roc(&rcObjVisible, false);  

    // ��ֹ�ֲ㴰�ڵ�alpha�ص����
    bool bNeedClear = m_pWindow->IsTransparent() && pObj->IsTransparent();    

    // ������rcObjVisible���� ����Ѿ����ڻ��ƣ����ύ����
    bool bRet = pRenderTarget->BeginDraw(&rcObjVisible, 1, bNeedClear);
    bool bSelfTransparent = pObj->IsTransparent();
    if (bOnlyRedrawBkgnd)
    {
        // ���ֻˢ�����Ļ�����ô���뽫�Լ���ʶΪ͸���ĲŻ����parent
        bSelfTransparent = true;  
    }

    pObj->DrawObjectTransparentBkgnd(pRenderTarget, roc, bSelfTransparent);

    if (!bOnlyRedrawBkgnd)
    {
        // �ڲ�����ʱ��Ȼ��Ҫ�ü�
        roc.m_bUpdateClip = true;           
        pObj->DrawObject(pRenderTarget, roc);
    }

    if (bRet)
    {
        pRenderTarget->EndDraw();

//         if (m_p3DLayer)
//         {
//             CombineAllLayer(NULL);
//             if (bUpdateNow && CanCommit())
//                 m_pWindow->CommitDoubleBuffet2Window(NULL, NULL);
//         }
//         else
        {
            // ת�ɴ����ϵĿɼ�����
            rcObjVisible.OffsetRect(pLayer->GetParentRectL(), pLayer->GetParentRectT());

            HRGN hRgn = ::CreateRectRgnIndirect(&rcObjVisible);
            CombineAllLayer(hRgn);
            SAFE_DELETE_GDIOBJECT(hRgn);

            // ��ʾ
            if (bUpdateNow && CanCommit())
                m_pWindow->CommitDoubleBuffet2Window(NULL, &rcObjVisible);
        }
    }
}



//
//	������LISTBOX ReDrawItem��ֻˢ��һ���֣�����������object
//
//  ����pbIsDrawing�������ߵ����ߣ���������������ˢ�£������߲��õ���EndRedrawObjectPart
//
IRenderTarget* RenderChain::BeginRedrawObjectPart(Object* pRedrawObj, RECT* prcArray, int nCount, bool* pbIsDrawing, RenderContext* pOutRenderContext)
{
    if (NULL == pRedrawObj)
        return NULL;

    RenderLayer* pRenderLayer = pRedrawObj->GetRenderLayer();
    if (NULL == pRenderLayer)
        return NULL;

    IRenderTarget*  pRenderTarget  =pRenderLayer->GetRenderTarget();
    if (NULL == pRenderTarget)
        return NULL;

    CRect rcClip;
    if (0 == nCount)  // ���� ListCtrlBase::RedrawItemByInnerCtrlʱ�����ٲü����Ѿ�����һ�����ƹ�������
    {
        ::GetClientRect(m_pWindow->m_hWnd, &rcClip);
    }
    else
    {
        for (int i = 0; i < nCount; i++)
            rcClip.UnionRect(&rcClip, &prcArray[i]);
    }
    RenderContext roc(&rcClip, false); // BeginDraw�Ѿ������˼������� �����rcClipֻ�����ڻ��ƹ������ж�һ�������Ƿ������������
    
    bool bNeedClear = m_pWindow->IsTransparent() && pRedrawObj->IsTransparent();
    bool bRet = pRenderTarget->BeginDraw(prcArray, nCount, bNeedClear);  // PS:20130110 ȥ���˷���ֵ���ж�.��Ϊ�б�ؼ��ڲ��ؼ���ˢ��ʱ������ROOTPANEL������redraw listitem���������ﷵ��FALSE����������ʧ�ܡ�����ȷſ��������

    if (pbIsDrawing)  // ��ʹ�Ѿ��ڻ������������ˣ�����Ҳǿ�ƻ���
        *pbIsDrawing = !bRet;

    pRedrawObj->DrawObjectTransparentBkgnd(pRenderTarget, roc, /*true*/pRedrawObj->IsTransparent());

    UISendMessage(pRedrawObj, WM_ERASEBKGND, (WPARAM)pRenderTarget, (LPARAM)&roc);

    roc.DrawClient(pRedrawObj->GetIObject());
    roc.Scroll(pRedrawObj->GetIObject());
    roc.Update(pRenderTarget);

	if (pOutRenderContext)
		*pOutRenderContext = roc;

    return pRenderTarget;
}

//
//	Ҫ�ύ�������ϵ��������BeginDrawObjectʹ��
//  ����Ҫ�ύ���Rectʱ���Ƚ�bFinish����Ϊfalse�����һ������Ϊtrue�ͷ���Դ
//
void  RenderChain::EndRedrawObjectPart(IRenderTarget* pRenderTarget, RECT* prcArray, int nCount)
{
    if (pRenderTarget) 
        pRenderTarget->EndDraw();

    this->CombineAllLayer(NULL);

    if (CanCommit())
        m_pWindow->CommitDoubleBuffet2Window(NULL, prcArray, nCount);
}

//
//	����˫��������
//
void  RenderChain::CreateDoubleBuffer(int nWidth, int nHeight)
{
    if (m_hMultiLayerMemDC)
    {
	    ::SelectObject(m_hMultiLayerMemDC, m_hOldBitmap);
        m_MultiLayerBuffer.Destroy();
    }
    else
    {
    	m_hMultiLayerMemDC = ::CreateCompatibleDC(NULL);
	    ::SetBkMode(m_hMultiLayerMemDC, TRANSPARENT);
	    ::SetStretchBltMode(m_hMultiLayerMemDC, HALFTONE);
    }

	if (nWidth <= 0 || nHeight <=0)  // ע��ֻ�д�����m_hMultiLayerMemDC����OnWindowResize�вŻ��������multi layer buffer
		return;

	m_MultiLayerBuffer.Create(nWidth, nHeight, 32, Image::createAlphaChannel);
	m_hOldBitmap = (HBITMAP)::SelectObject(m_hMultiLayerMemDC, (HBITMAP)m_MultiLayerBuffer);
}
//
//	�ͷ�˫��������
//
void  RenderChain::DestroyDoubleBuffer()
{
	if (m_hMultiLayerMemDC)
	{
		::SelectObject(m_hMultiLayerMemDC, m_hOldBitmap);
		::DeleteDC(m_hMultiLayerMemDC);
		m_hMultiLayerMemDC = NULL;
	}
    m_MultiLayerBuffer.Destroy();
}


void  RenderChain::SetCanCommit(bool b)
{
    if (b)
        m_lRefCanCommit --;
    else
        m_lRefCanCommit ++;
}

bool  RenderChain::CanCommit() 
{ 
    return 0 == m_lRefCanCommit; 
}

Layer3d*  RenderChain::Get3DLayer() 
{
    return m_p3DLayer; 
}
void  RenderChain::On3dObjectBegin()
{   
    if (!m_pWindow)
        return;

    if (!m_p3DLayer)
    {
        ILayer3d* p3DLayer = NULL;
        ILayer3d::CreateInstance(m_pWindow->GetUIApplication(), &p3DLayer);
        if (p3DLayer)
        {
            m_p3DLayer = p3DLayer->GetImpl();
            m_p3DLayer->SetWindowPtr(m_pWindow); 
            m_p3DLayer->SetRenderChainPtr(this);
            m_p3DLayer->SetConfigLeft(0);
            m_p3DLayer->SetConfigRight(0);
            m_p3DLayer->SetConfigTop(0);
            m_p3DLayer->SetConfigBottom(0);

            InsertLayer(m_p3DLayer);

            int nWidth = m_pWindow->GetWidth();
            int nHeight = m_pWindow->GetHeight();

            if (!m_hMultiLayerMemDC || m_MultiLayerBuffer.IsNull())
            {
                CreateDoubleBuffer(nWidth, nHeight);
            }

            m_p3DLayer->CreateBuffer(nWidth, nHeight);   // ���ʱ���п��ܻ�û�д���multi layer buffer
            CanvasLayout::ArrangeObject(m_p3DLayer, nWidth, nHeight);
        }
    }

    if (m_p3DLayer)
    {
        m_p3DLayer->AddRef();
    }
}
void  RenderChain::On3dObjectEnd()
{
    if (m_p3DLayer)
    {
        if (0 == m_p3DLayer->Release())
        {
            this->RemoveLayer(m_p3DLayer);  // m_p3DLayer�����ÿ�
        }
    }
}