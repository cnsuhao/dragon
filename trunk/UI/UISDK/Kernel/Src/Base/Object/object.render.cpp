#include "stdafx.h"
#include "object.h"
#include "UISDK\Kernel\Src\RenderLayer2\layer\renderlayer2.h"
#include "UISDK\Kernel\Src\RenderLayer2\layer\windowrender.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Src\RenderLayer2\soft3d\renderlayersoft3d.h"

namespace UI
{

//
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
//
void  Object::UpdateObjectEx(RECT* prcObjArray, int nCount, bool bUpdateNow)
{
    if (!prcObjArray || !nCount)
        return;

#ifdef TRACE_DRAW_PROCESS
    UI_LOG_DEBUG(_T("UpdateObjectEx id=%s -----------------------"), m_strId.c_str());
#endif

    //////////////////////////////////////////////////////////////////////////
    // 1. ״̬�ж�
    //////////////////////////////////////////////////////////////////////////

    if (!IsVisible() || !CanRedraw())
        return;

    RealUpdateObjectEx(prcObjArray, nCount, bUpdateNow);
}

void  Object::RealUpdateObjectEx(RECT* prcObjArray, int nCount, bool bUpdateNow)
{
    //////////////////////////////////////////////////////////////////////////
    // 2. ���ϱ���ˢ��
    //////////////////////////////////////////////////////////////////////////

    // ÿ��layer�ϱ�ˢ�µĶ���
    Object*  pObjDamage = this;   

    // �ö��������������nCountΪ1����ListCtrl�п���Ϊ���
    CRect*   prcObjDamage = new CRect[nCount];
    CRect*   prcDamageInLayer = new CRect[nCount];

    memcpy(prcObjDamage, prcObjArray, sizeof(RECT)*nCount);
    memset(prcDamageInLayer, 0, sizeof(CRect)*nCount);

    RenderLayer2*  pLayer = this->GetRenderLayer2();
    while (pLayer)
    {
        // ����ö���Ŀɼ�����Ϊ�գ�����Ҫ�ټ�����ȥ��
        if (false == pLayer->UpdateDescendant(pObjDamage, 
                                            __in  prcObjDamage, 
                                            __in  nCount, 
                                            __out prcDamageInLayer))
        {
            SAFE_ARRAY_DELETE(prcObjDamage);
            SAFE_ARRAY_DELETE(prcDamageInLayer);
            return;
        }

#ifdef _DEBUG
        static bool bDebug = false;
        if (bDebug)
		    pLayer->GetRenderTarget()->Save(0);
#endif

        // �ϲ�layer
        RenderLayer2*  pParentLayer = pLayer->GetParentRenderLayer();
        if (!pParentLayer)
            break;

		CRect rcLayerDirty(0, 0, 0, 0);
        if (pLayer->IsTransformed())
        {
            // ���ڲ����ת��������һЩ��Ч����Ҳ��ӽ���
            if (pLayer->m_pTransform)
            {
                RECT  rcInvalid;
                pLayer->m_pTransform->GetInvalidRect(&rcInvalid);
        
                if (!::IsRectEmpty(&rcInvalid))
                {
					rcLayerDirty = rcInvalid;
                }
            }
        }

		if (rcLayerDirty.IsRectEmpty())
		{
			// ���ص�rcDamageInLayer��������ʾpObjDamage��pLayer�е�������
			memcpy(prcObjDamage, prcDamageInLayer, sizeof(CRect)*nCount);
		}
		else
		{
			nCount += 1;
			SAFE_ARRAY_DELETE(prcObjDamage);
			prcObjDamage = new CRect[nCount];
			memcpy(prcObjDamage, prcDamageInLayer, sizeof(CRect)*(nCount-1));
			prcObjDamage[nCount-1] = rcLayerDirty;

			SAFE_ARRAY_DELETE(prcDamageInLayer);
			prcDamageInLayer = new CRect[nCount];
		}

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
	        pWndRender->Commit(NULL, prcDamageInLayer, nCount);
    }

    SAFE_ARRAY_DELETE(prcObjDamage);
    SAFE_ARRAY_DELETE(prcDamageInLayer);
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
void  Object::HandleDamageStackContext(Object* pObjDamage, IRenderTarget*  pRenderTarget, RenderContext context, stack<AncestorDamageInfo>& stackContext)
{
    //////////////////////////////////////////////////////////////////////////
    // 1. ȡ����
    //////////////////////////////////////////////////////////////////////////

    AncestorDamageInfo  data = stackContext.top();
    stackContext.pop();

    Object*  pChildObj = NULL;
    if (stackContext.size())
        pChildObj = stackContext.top().pAncestor;
    else
        pChildObj = pObjDamage;
    
    RenderContext  contextSave = context;  // ר������z order����

    //////////////////////////////////////////////////////////////////////////
    // 2. ���Լ��ı��� + less zorder
    //////////////////////////////////////////////////////////////////////////

    if (data.bDrawBkgnd)
    {
        context.Update(pRenderTarget);
        UISendMessage(m_pIObject, WM_ERASEBKGND, (WPARAM)pRenderTarget, (LPARAM)&context);
#ifdef TRACE_DRAW_PROCESS
        UI_LOG_DEBUG(_T("EraseBkgnd: %s"), m_strId.c_str());
#endif

        // ���Ʊ�pChildObj zOrderС�Ķ���
        TestZorderOverlapLess(pChildObj, pRenderTarget, &data.rcDamage, contextSave);
    }

    //////////////////////////////////////////////////////////////////////////
    // 3. �ݹ黭�Ӷ���
    //////////////////////////////////////////////////////////////////////////

    if (!pChildObj->IsNcObject())
        context.DrawClient(m_pIObject);
    context.DrawChild(pChildObj->m_pIObject);

    Object*  pChild = NULL;
    if (stackContext.size())
    {
        pChildObj->HandleDamageStackContext(pObjDamage, pRenderTarget, context, stackContext);
    }
    else  // ������ɣ��ֵ����Լ���
    {
        context.m_bUpdateClip = true;
        context.Update(pRenderTarget);
        pChildObj->DrawObject(pRenderTarget, context); 

        // ע��DrawObject�������ڻ��ӿؼ�ʱ���ᵼ�¼��������ĸ�С
        // ��˷���֮��������ٻ�great zorder����Ȼ�Ȼ�ԭ��������
    }

    //////////////////////////////////////////////////////////////////////////
    // 4. �ж�great zorder
    //////////////////////////////////////////////////////////////////////////

    TestZorderOverlapGreat(pChildObj, pRenderTarget, &data.rcDamage, contextSave);
}


// ���Ʊ�pChildObj zOrder��С�Ķ���
void  Object::TestZorderOverlapLess(Object* pChildObj, IRenderTarget* pRenderTarget, RECT* prcClip, RenderContext context)
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
            RenderContext c = context;

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
void  Object::TestZorderOverlapGreat(Object* pChildObj, IRenderTarget* pRenderTarget, RECT* prcClip, RenderContext context)
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
            RenderContext c = context;

            if (!pTest->IsNcObject())
                context.DrawClient(m_pIObject);
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
	if (!this->IsMySelfVisible() || !this->CanRedraw())
        return false;

    if (m_pRenderLayer)
    {
        IRenderTarget*  pRenderTargetLayer = m_pRenderLayer->GetRenderTarget();
        if (m_pRenderLayer->IsDirty())
        {
            m_pRenderLayer->UpdateLayer(NULL);

            #ifdef TRACE_DRAW_PROCESS
                UI_LOG_DEBUG(_T("UpdateLayer: %s"), m_strId.c_str());
            #endif
        }

        m_pRenderLayer->Commit(pRenderTarget);  // pRenderTarget�п���ΪNULL
    }
    else
    {
        RealDrawObject(pRenderTarget, context);
    }
    return true;
}
// �ܶ�������治��Ҫ�ж��Լ��Ŀɼ��ԣ���Щ�����ƴ����DrawObject�����
void  Object::RealDrawObject(IRenderTarget* pRenderTarget, RenderContext context)
{
#ifdef TRACE_DRAW_PROCESS
    UI_LOG_DEBUG(_T("DrawObject: %s"), m_strId.c_str());
#endif
    ::UISendMessage(this, WM_ERASEBKGND, (WPARAM)pRenderTarget, (LPARAM)&context); 
	this->virtualOnPostDrawObjectErasebkgnd();

    this->_drawNcChildObject(pRenderTarget, context);

    context.DrawClient(m_pIObject);
    context.Update(pRenderTarget);

    ::UISendMessage(this, WM_PAINT, (WPARAM)pRenderTarget, (LPARAM)&context);    
    this->DrawChildObject(pRenderTarget, context);

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
		if (contextChild.DrawChild(pChild->GetIObject()))
		{
			contextChild.Update(pRenderTarget);
			pChild->DrawObject(pRenderTarget, contextChild);
		}
	}
}


void Object::_drawNcChildObject(IRenderTarget* pRenderTarget, RenderContext& content)
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
    HDC hDC = image.BeginDrawToMyself();
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
    image.EndDrawToMyself();
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
    HDC hDC = image.BeginDrawToMyself();
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
    image.EndDrawToMyself();

#ifdef _DEBUGx
    image.Save(L"C:\\aaa.png", Gdiplus::ImageFormatPNG);
#endif
    return image.Detach();
}

}