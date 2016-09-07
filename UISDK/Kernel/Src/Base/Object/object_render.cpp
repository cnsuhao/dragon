#include "stdafx.h"
#include "object.h"
#include "UISDK\Kernel\Src\RenderLayer2\layer\renderlayer.h"
#include "UISDK\Kernel\Src\RenderLayer2\layer\windowrender.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Src\RenderLayer2\soft3d\renderlayersoft3d.h"

using namespace UI;

// �ػ��������(һ��������������£�״̬�仯��ʱ����ػ�)
// ��layerΪ��λ���������Ͻ��и���
//
void  Object::UpdateObject(bool bUpdateNow)
{
    CRect  rcObjDamage;
    ::SetRect(&rcObjDamage, 0, 0, GetWidth(), GetHeight());
    UpdateObjectEx(&rcObjDamage, 1, bUpdateNow);
}

// ����������/�ƶ���ʱ��ˢ�±���
void Object::UpdateObjectBkgnd(bool bUpdateNow)
{
    CRect  rcObjDamage(0, 0, GetWidth(), GetHeight());

    bool bOldTrans = IsTransparent();
    SetTransparent(true);  // ǿ�ƻ������󱳾�
    SetCanRedraw(false);   // ǿ�Ʋ������Լ�
    {
        RealUpdateObjectEx(&rcObjDamage, 1, bUpdateNow);
    }
    SetCanRedraw(true);
    SetTransparent(bOldTrans);

}

//
// 1. ֧�ָ���һ������Ķ���ֲ���������ListCtrl��ˢ��.
// 2. Ϊ��ʵ��ĳЩ�������жϿɼ���ɻ�����һ���ִ���Ų��
//    RealUpdateObjectEx�У�����UpdateObjectBkgnd
// 3. ע������û���� prcObjArray �� obj.rect �Ľ����������prcObjArray�����ؼ���Χ�������Ƴ��硣
//    �����汣֤��������ȷ��
//
void  Object::UpdateObjectEx(RECT* prcObjArray, int nCount, bool bUpdateNow)
{
    if (!prcObjArray || !nCount)
        return;

#ifdef TRACE_DRAW_PROCESS
    UI_LOG_DEBUG(_T("UpdateObjectEx: %s -----------------------"), m_strId.c_str());
#endif

    //////////////////////////////////////////////////////////////////////////
    // 1. ״̬�ж�
    //////////////////////////////////////////////////////////////////////////

    if (!IsVisible() || !CanRedraw())
        return;

#if 1 // ���µ���ˢ�·�ʽ(����Ч)

    RealUpdateObjectEx(prcObjArray, nCount, bUpdateNow);

#else // ���ϵ���ˢ�·�ʽ(����),���Էֲ㴰����Ч���ֲ㴰��BeginPaint֮���õ���ps.rcPaintΪ��

    HWND  hWnd = GetHWND();
    if (!hWnd)
        return;

    for (int i = 0; i < nCount; i++)
    {
        RECT rcWnd = {0};
        ObjectRect2WindowRect(this, &prcObjArray[i], &rcWnd);
        ::InvalidateRect(hWnd, &rcWnd, FALSE);
    }
    if (bUpdateNow)
    {
        ::UpdateWindow(hWnd);
    }

#endif
}

void  Object::RealUpdateObjectEx(
	    RECT* prcObjArray, 
	    int nCount,
	    bool bUpdateNow
)
{
    //////////////////////////////////////////////////////////////////////////
    // 2. ���ϱ���ˢ��
    //////////////////////////////////////////////////////////////////////////

    // ÿ��layer�ϱ�ˢ�µĶ���
    Object*  pObjDamage = this;   

    // �ö��������������nCountΪ1����ListCtrl�п���Ϊ���
    RectArray   prcObjDamage;     // �������������
    RectArray   prcDamageInLayer; // �����ڸò��ϵ�������

	prcObjDamage.CopyFromArray(prcObjArray, nCount);
	prcDamageInLayer.SetSize(nCount);

    RenderLayer*  pLayer = this->GetRenderLayer2();
    while (pLayer)
    {
        // ����ö���Ŀɼ�����Ϊ�գ�����Ҫ�ټ�����ȥ��
        if (false == pLayer->UpdateDescendantObject(
                __in  pObjDamage, 
                __in  prcObjDamage.m_prcArray, 
                __in  nCount, 
                __out prcDamageInLayer.m_prcArray))
        {
            return;
        }

#ifdef _DEBUG
        static bool bDebug = false;
        if (bDebug)
		    pLayer->GetRenderTarget()->Save(0);
#endif

        // �ϲ�layer
        RenderLayer*  pParentLayer = pLayer->GetParentRenderLayer();
        if (!pParentLayer)
            break;

// 		CRect rcLayerDirty(0, 0, 0, 0);
//         if (pLayer->IsTransformed() && pLayer->m_pTransform)
//         {
//             // ���ڲ����ת��������һЩ��Ч����Ҳ��ӽ���
//             pLayer->m_pTransform->GetInvalidRect(&rcLayerDirty);
//         }
// 
// 		if (rcLayerDirty.IsRectEmpty())
		{
			// ���ص�rcDamageInLayer��������ʾpObjDamage��pLayer�е�������
			prcObjDamage.CopyFrom(&prcDamageInLayer);
		}
// 		else
// 		{
// 			nCount += 1;
// 			prcObjDamage.Destroy();
// 			prcObjDamage.SetSize(nCount);
// 			
// 			memcpy(prcObjDamage.m_prcArray, prcDamageInLayer.m_prcArray, sizeof(RECT)*(nCount-1));
// 			prcObjDamage.SetAt(nCount-1, rcLayerDirty);
// 
// 			prcDamageInLayer.SetSize(nCount);
// 		}

        pObjDamage = pLayer->GetCreateObject();
        pLayer = pParentLayer;

    }

    //////////////////////////////////////////////////////////////////////////
    // 3. �ύ������
    //////////////////////////////////////////////////////////////////////////

    if (bUpdateNow)
    {
        WindowRender* pWndRender = GetWindowRender();
		if (pWndRender)
	        pWndRender->Commit(NULL, prcDamageInLayer.m_prcArray, nCount);
    }

    return;
}

//  ��������Ļ��ƣ�����z order�������⣩��˳��
//  1. ���ؼ�����
//  2. ���Լ�zorderС��sibling����
//  3. �Լ��Լ��Լ����ӿؼ�
//  4. ���Լ�zorder���sibling����
//
//  pObjDamage
//    [in] ������Ҫ��ˢ�µ�obj��Ҳ���ǵ���UpdateObject�Ķ���
// 
//  pRenderTarget
//    [in] ����Ŀ��
//
//  context
//    [in] pRenderTarget�Ļ��������ģ�ָ����this��������
//
//  stackContext
//    [in] pObjDamage�������б�����Ʋ���
//
void  Object::HandleDamageStackContext(Object* pObjDamage, 
                                       IRenderTarget*  pRenderTarget,
                                       RenderContext context,
                                       deque<AncestorDamageInfo>& stackContext)
{
    //////////////////////////////////////////////////////////////////////////
    // 1. ȡ����
    //////////////////////////////////////////////////////////////////////////

    AncestorDamageInfo  data = stackContext.back();
    stackContext.pop_back();

    Object*  pChildObj = NULL;
    if (stackContext.size())
        pChildObj = stackContext.back().pAncestor;
    else
        pChildObj = pObjDamage;
    
    RenderContext  contextSave = context;  // ר������z order����

    //////////////////////////////////////////////////////////////////////////
    // 2. ���Լ��ı��� + less zorder
    //////////////////////////////////////////////////////////////////////////

    if (data.bDrawBkgnd)
    {
		context.m_bUpdateClip = this->NeedClip();
		context.Update(pRenderTarget);
		context.m_bUpdateClip = true;
        
        UISendMessage(m_pIObject, WM_ERASEBKGND, (WPARAM)pRenderTarget, 
            (LPARAM)&context);

#ifdef TRACE_DRAW_PROCESS
        UI_LOG_DEBUG(_T("EraseBkgnd: %s"), m_strId.c_str());
#endif

        // ��������ı���������Ҫ�������WM_PAINT����
        ObjStyle s = {0};
        s.float_on_parent_content = 1;
        if (pChildObj->TestObjectStyle(s))
        {
            RenderContext cDrawContent = context;
            cDrawContent.DrawClient(m_pIObject);
            cDrawContent.Update(pRenderTarget);
            ::UISendMessage(m_pIObject, WM_PAINT, (WPARAM)pRenderTarget, 
                (LPARAM)&cDrawContent);    
        }

        // ���Ʊ�pChildObj zOrderС�Ķ���
        TestZorderOverlapLess(pChildObj, pRenderTarget, &data.rcDamage, &contextSave);
    }

    //////////////////////////////////////////////////////////////////////////
    // 3. �ݹ黭�Ӷ���
    //////////////////////////////////////////////////////////////////////////

    if (!pChildObj->IsNcObject())
        context.DrawClient(m_pIObject);
    context.DrawChild(pChildObj->m_pIObject);

    if (stackContext.size())
    {
        pChildObj->HandleDamageStackContext(pObjDamage, pRenderTarget, context, stackContext);
    }
    else  // ������ɣ��ֵ����Լ���
    {
		context.m_bUpdateClip = pChildObj->NeedClip();
		context.Update(pRenderTarget);
		context.m_bUpdateClip = true;

		pChildObj->DrawObject(pRenderTarget, context); 

        // ע��DrawObject�������ڻ��ӿؼ�ʱ���ᵼ�¼��������ĸ�С
        // ��˷���֮��������ٻ�great zorder����Ȼ�Ȼ�ԭ��������
    }

    //////////////////////////////////////////////////////////////////////////
    // 4. �ж�great zorder
    //////////////////////////////////////////////////////////////////////////

    TestZorderOverlapGreat(pChildObj, pRenderTarget, &data.rcDamage, &contextSave);

    if (m_objStyle.post_paint)
    {
        DoPostPaint(pRenderTarget, contextSave);
    }
}


// ���Ʊ�pChildObj zOrder��С�Ķ���
void  Object::TestZorderOverlapLess(
        Object* pChildObj, 
        IRenderTarget* pRenderTarget, 
        RECT* prcClip, 
        const RenderContext* pContext)
{
    Object* pTest = NULL;
    if (pChildObj->IsNcObject())
        pTest = m_pNcChild;
    else
        pTest = m_pChild;

    RECT  rcIntersect = {0};
    int   nZorder = pChildObj->GetZOrder();

    while (pTest)   // child���ź����
    {
        if (pTest == pChildObj)
            break;
        if (pTest->GetZOrder() == nZorder)
            break;

        if (IntersectRect(&rcIntersect, &pTest->m_rcParent, prcClip))
        {
            RenderContext c = *pContext;

            if (!pTest->IsNcObject())
                 c.DrawClient(m_pIObject);
            c.DrawChild(pTest->m_pIObject);
            c.m_bUpdateClip = true; 
            c.Update(pRenderTarget);
            pTest->DrawObject(pRenderTarget, c);
        }

        pTest = pTest->m_pNext;
    }
}

// ���Ʊ�pChildObj zOrder����Ķ���
void  Object::TestZorderOverlapGreat(
        Object* pChildObj, 
        IRenderTarget* pRenderTarget, 
        RECT* prcClip, 
        const RenderContext* pContext)
{
    int  nZorder = pChildObj->GetZOrder();
    RECT rcIntersect = {0};

    Object* pTest = pChildObj->m_pNext;
    while (pTest)   
    {
        if (pTest->GetZOrder() == nZorder)
        {
            pTest = pTest->m_pNext;
            continue;
        }

        if (IntersectRect(&rcIntersect, &pTest->m_rcParent, prcClip))
        {
            RenderContext c = *pContext;

            if (!pTest->IsNcObject())
                c.DrawClient(m_pIObject);
            c.DrawChild(pTest->m_pIObject);
            c.m_bUpdateClip = true;  // ����Ҫ��䡣��Ϊ��ǰ�ļ�������һ����init clip region
            c.Update(pRenderTarget);

            pTest->DrawObject(pRenderTarget, c);
        }

        pTest = pTest->m_pNext;
    }
}




//
// �ݹ�. ���ö������Ӷ�����Ƶ�pRenderTarget����
// ��Ϊ�ӿؼ��ķ�Χ����Ҫ��parent����������������ȡ�Ӻ͸��Ľ���clip rgn���л���  
//

bool Object::DrawObject(IRenderTarget* pRenderTarget, RenderContext context)
{
	if (!this->IsSelfVisible() || !this->CanRedraw())
        return false;

    if (m_pRenderLayer)
    {
		if (m_pRenderLayer->NeedUpdate())
		{
			m_pRenderLayer->UpdateLayer(NULL);
		}
		else if (context.m_lFlags & RENDERCONTEXT_FLAG_RENDER_MODE_TOPDOWN)
		{
			m_pRenderLayer->UpdateLayer(NULL);
		}

        m_pRenderLayer->Commit(pRenderTarget);  // pRenderTarget�п���ΪNULL
    }
    else
    {
        RealDrawObject(pRenderTarget, context);
    }
    return true;
}
// �ܶ�������治��Ҫ�ж��Լ��Ŀɼ��ԣ���˽����ƴ����DrawObject�����
// 2014.8.26 update. �ǿͻ����ؼ�Ҳ������WM_PAINT֮���ٻ��ơ�����Ʈ���ӿؼ�
//
void  Object::RealDrawObject(IRenderTarget* pRenderTarget, RenderContext context)
{
#ifdef TRACE_DRAW_PROCESS
    UI_LOG_DEBUG(_T("RealDrawObject: %s"), m_strId.c_str());
#endif

    ::UISendMessage(this, WM_ERASEBKGND, (WPARAM)pRenderTarget, (LPARAM)&context); 
	this->virtualOnPostDrawObjectErasebkgnd();

    if (m_pNcChild)
    {
        RenderContext nonclient = context;

        context.DrawClient(m_pIObject);
        context.Update(pRenderTarget);
        ::UISendMessage(this, WM_PAINT, (WPARAM)pRenderTarget, (LPARAM)&context);    
        this->DrawChildObject(pRenderTarget, context);

        nonclient.Update(pRenderTarget);
        this->drawNcChildObject(pRenderTarget, nonclient);
    }
    else
    {
        context.DrawClient(m_pIObject);
        context.Update(pRenderTarget);
        ::UISendMessage(this, WM_PAINT, (WPARAM)pRenderTarget, (LPARAM)&context);    

        this->DrawChildObject(pRenderTarget, context);
    }
    if (m_objStyle.post_paint)
    {
        this->DoPostPaint(pRenderTarget, context);
    }

#ifdef _DEBUG
	static bool bDebug = false;
	if (bDebug)
		pRenderTarget->Save(0);
#endif
}


void Object::DrawChildObject(IRenderTarget* pRenderTarget, RenderContext& context)
{
	Object*  pChild = NULL;
	while (pChild = EnumChildObject(pChild))
	{
		RenderContext contextChild = context;

        // �����������Ҫ���ã���ô�Լ�Ҳ������
		bool bChildNeedClip = pChild->NeedClip();
		contextChild.m_bUpdateClip = bChildNeedClip;

		bool bNeedDrawChild = contextChild.DrawChild(pChild->GetIObject());
		if (bNeedDrawChild)
		{
			// ����Ӷ�����һ���㣬�����б任���򲻸��¼������򣬶����л���������ļ�������
			// (ע��ǰһ���ھӶ���pRenderTarget�ļ��������޸��ˣ����Ҫ�ֶ�����һ��UpdateClipRgn)
			if (!bChildNeedClip)
			{
				if (pChild->GetPrevObject())
					context.UpdateClipRgn(pRenderTarget);

				contextChild.Update(pRenderTarget);
				contextChild.m_bUpdateClip = true;
			}
			else
			{
				contextChild.Update(pRenderTarget);
			}
			pChild->DrawObject(pRenderTarget, contextChild);
		}
		else
		{
		}
	}
}


void Object::drawNcChildObject(IRenderTarget* pRenderTarget, RenderContext& content)
{
	Object*  pChild = NULL;
	while (pChild = EnumNcChildObject(pChild))
	{
		RenderContext contextChild = content;
		if (contextChild.DrawChild(pChild->GetIObject()))
		{
			contextChild.Update(pRenderTarget);
			pChild->DrawObject(pRenderTarget, contextChild);
		}
	}
}


#if 0 //����
// �����͸�������Ļ�����Ҫ��ȡ�������еı���
// ����ʹ�ǲ�͸����object��Ҳ��Ҫһ������������ȡ���ļ�������
//
//   Ϊ����Ӧ�ڶ������ص�ʱ���ܹ�ˢ���Լ��ĸ����󱳾�����Ҫ����һ�������������Լ���͸������
//   �������bSelfTransparent������ͨˢ�µ�ʱ�򴫵�this->IsTransparen()������ʱ��ֱ�Ӵ���false

void Object::DrawBkgndInLayer(IRenderTarget* pRTLayer, RenderContext& roc, bool bSelfTransparent)
{
	if (m_pRenderLayer)  // �Լ�����layer�ĵ�һ�����󣬲���Ҫ��
		return;  

	Object*  pObjFirstParentToDrawBk = NULL;
	pObjFirstParentToDrawBk = GetFirstOpacityAncestor(bSelfTransparent);

	Object* pObjParent = GetRenderLayerCreateObject();
	while (pObjParent)
	{
		OBJ_TYPE eType = pObjParent->GetObjectType();

		if (!pObjParent->IsNcObject())
		{
			Object* pParent = pObjParent->GetParentObject();
			if (pParent)
			{
				roc.DrawClient(pParent->GetIObject());
			}
		}

		// ��pObjFirstParentToDrawBk��ʼ����
		bool  bEraseBkgnd = true;
		if (pObjFirstParentToDrawBk)  
		{
			if (pObjParent == pObjFirstParentToDrawBk)  
			{
				// ������
				pObjFirstParentToDrawBk = NULL;   
			}
			else
			{
				bEraseBkgnd = false;
			}
		}

		if (bEraseBkgnd)
		{
			roc.Update(pRTLayer);
			::UISendMessage(pObjParent, WM_ERASEBKGND, (WPARAM)pRTLayer, (LPARAM)&roc);
		}

		pObjParent = this->REnumParentObject(pObjParent);

		if (pObjParent)
			roc.DrawChild(pObjParent->GetIObject());
	}

	if (!this->IsNcObject())
	{
		Object* pParent = this->GetParentObject();
		if (pParent)
		{
			roc.DrawClient(pParent->GetIObject());
		}
	}
	roc.DrawChild(m_pIObject);
	roc.Update(pRTLayer);
}
#endif

HBITMAP  Object::TakeSnapshot()
{

    Image image;
    image.Create(GetWidth(), GetHeight(), 32, Image::createAlphaChannel);
    HDC hDC = image.GetDC();
    SetBkMode(hDC, TRANSPARENT);
    SetStretchBltMode(hDC, HALFTONE);
    
    IRenderTarget*  pRenderTarget = NULL;
    GetWindowRender()->CreateRenderTarget(&pRenderTarget);
    pRenderTarget->BindHDC(hDC);
    pRenderTarget->BeginDraw();
    {
        CRect rcRenderRegion(0, 0, GetWidth(), GetHeight());
        RenderContext  context(&rcRenderRegion, &rcRenderRegion, true);
        this->RealDrawObject(pRenderTarget, context);
    }
    pRenderTarget->EndDraw();

    pRenderTarget->Release();
    image.ReleaseDC();
#ifdef _DEBUGx
    image.Save(L"C:\\aaa.png", Gdiplus::ImageFormatPNG);
#endif
    return image.Detach();
}


// ��֧��zorder�ص���ֱ��ȡ���ȵı���
HBITMAP  Object::TakeBkgndSnapshot()
{
    if (!m_pParent)
        return NULL;

    Image image;
    image.Create(m_rcParent.Width(), m_rcParent.Height(), 32, Image::createAlphaChannel);
    HDC hDC = image.GetDC();
    SetBkMode(hDC, TRANSPARENT);
    SetStretchBltMode(hDC, HALFTONE);

	IRenderTarget*  pRenderTarget = NULL;
    GetWindowRender()->CreateRenderTarget(&pRenderTarget);

    pRenderTarget->BindHDC(hDC);
    pRenderTarget->BeginDraw();
    {
        // ����游��������û�в�͸���ģ��Ӹø�������
        Object*  pObjAncestorFirst2DrawBkgnd = NULL;
        Object*  pChild = this;
        CRect    rcInAncestor(0, 0, GetWidth(), GetHeight());
        while (pObjAncestorFirst2DrawBkgnd = this->EnumParentObject(pObjAncestorFirst2DrawBkgnd))
        {
            ChildRect2ParentRect(pChild, &rcInAncestor, &rcInAncestor);

            // �ҵ���͸��������߲����
            if (!pObjAncestorFirst2DrawBkgnd->IsTransparent())
                break;

            pChild = pObjAncestorFirst2DrawBkgnd;
        }
        if (NULL == pObjAncestorFirst2DrawBkgnd)
            pObjAncestorFirst2DrawBkgnd = GetWindowObject();

        CRect  rcClip(0, 0, GetWidth(), GetHeight());
        CRect  rcCurrentClip(&rcInAncestor);
        rcCurrentClip.OffsetRect(-rcInAncestor.left*2, -rcInAncestor.top*2);
        RenderContext  context(&rcClip, &rcCurrentClip, false);
        context.m_ptOffset.x = rcCurrentClip.left;
        context.m_ptOffset.y = rcCurrentClip.top;

        Object*  pParent = pObjAncestorFirst2DrawBkgnd;
        context.Update(pRenderTarget);
        ::UISendMessage(pParent, WM_ERASEBKGND, (WPARAM)pRenderTarget, (LPARAM)&context);
        while (pChild = REnumParentObject(pParent))
        {
            if (!pChild->IsNcObject())
                context.DrawClient(pParent->m_pIObject);
            context.DrawChild(pChild->m_pIObject);
            context.Update(pRenderTarget);

            ::UISendMessage(pParent, WM_ERASEBKGND, (WPARAM)pRenderTarget, (LPARAM)&context);
            pParent = pChild;
        }
    }
    pRenderTarget->EndDraw();
    pRenderTarget->Release();
    image.ReleaseDC();

#ifdef _DEBUGx
    image.Save(L"C:\\aaa.png", Gdiplus::ImageFormatPNG);
#endif
    return image.Detach();
}



void Object::OnEraseBkgnd(IRenderTarget* pRenderTarget)
{
    if (m_pBkgndRender)
    {
        CRect rc(0,0, this->GetWidth(), this->GetHeight());
        m_pBkgndRender->DrawState(pRenderTarget, &rc, 0);
    }
}

void Object::DoPostPaint(IRenderTarget* pRenderTarget, RenderContext context)
{
    if (m_objStyle.post_paint)
    {
        // �ӿؼ����������˼�������������Ȼָ��ٻ�postpaint
        context.m_bUpdateClip = true;
        context.Update(pRenderTarget);
        UI::UISendMessage(m_pIObject, UI_WM_POSTPAINT, (WPARAM)pRenderTarget);
    }
}
