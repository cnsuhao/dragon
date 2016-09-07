#include "stdafx.h"
#include "object.h"
#include "UISDK\Kernel\Src\RenderLayer2\layer\renderlayer.h"
#include "UISDK\Kernel\Src\RenderLayer2\layer\windowrender.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Src\RenderLayer2\soft3d\renderlayersoft3d.h"

using namespace UI;

// 重绘这个对象(一般是鼠标移上移下，状态变化等时候的重绘)
// 以layer为单位，由下往上进行更新
//
void  Object::UpdateObject(bool bUpdateNow)
{
    CRect  rcObjDamage;
    ::SetRect(&rcObjDamage, 0, 0, GetWidth(), GetHeight());
    UpdateObjectEx(&rcObjDamage, 1, bUpdateNow);
}

// 当对象隐藏/移动的时候，刷新背景
void Object::UpdateObjectBkgnd(bool bUpdateNow)
{
    CRect  rcObjDamage(0, 0, GetWidth(), GetHeight());

    bool bOldTrans = IsTransparent();
    SetTransparent(true);  // 强制画父对象背景
    SetCanRedraw(false);   // 强制不绘制自己
    {
        RealUpdateObjectEx(&rcObjDamage, 1, bUpdateNow);
    }
    SetCanRedraw(true);
    SetTransparent(bOldTrans);

}

//
// 1. 支持更新一个对象的多个局部区域，例如ListCtrl的刷新.
// 2. 为了实现某些场景不判断可见与可画，将一部分代码挪到
//    RealUpdateObjectEx中，例如UpdateObjectBkgnd
// 3. 注：这里没有做 prcObjArray 与 obj.rect 的交集处理。如果prcObjArray超出控件范围，将绘制出界。
//    由外面保证该区域正确性
//
void  Object::UpdateObjectEx(RECT* prcObjArray, int nCount, bool bUpdateNow)
{
    if (!prcObjArray || !nCount)
        return;

#ifdef TRACE_DRAW_PROCESS
    UI_LOG_DEBUG(_T("UpdateObjectEx: %s -----------------------"), m_strId.c_str());
#endif

    //////////////////////////////////////////////////////////////////////////
    // 1. 状态判断
    //////////////////////////////////////////////////////////////////////////

    if (!IsVisible() || !CanRedraw())
        return;

#if 1 // 从下到上刷新方式(更高效)

    RealUpdateObjectEx(prcObjArray, nCount, bUpdateNow);

#else // 从上到下刷新方式(更简单),但对分层窗口无效。分层窗口BeginPaint之后，拿到的ps.rcPaint为空

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
    // 2. 向上遍历刷新
    //////////////////////////////////////////////////////////////////////////

    // 每个layer上被刷新的对象
    Object*  pObjDamage = this;   

    // 该对象的脏区域，正常nCount为1，在ListCtrl中可能为多个
    RectArray   prcObjDamage;     // 对象本身的脏区域
    RectArray   prcDamageInLayer; // 对象在该层上的脏区域

	prcObjDamage.CopyFromArray(prcObjArray, nCount);
	prcDamageInLayer.SetSize(nCount);

    RenderLayer*  pLayer = this->GetRenderLayer2();
    while (pLayer)
    {
        // 如果该对象的可见区域为空，则不需要再继续下去了
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

        // 上层layer
        RenderLayer*  pParentLayer = pLayer->GetParentRenderLayer();
        if (!pParentLayer)
            break;

// 		CRect rcLayerDirty(0, 0, 0, 0);
//         if (pLayer->IsTransformed() && pLayer->m_pTransform)
//         {
//             // 由于层的旋转而产生的一些无效区域也添加进来
//             pLayer->m_pTransform->GetInvalidRect(&rcLayerDirty);
//         }
// 
// 		if (rcLayerDirty.IsRectEmpty())
		{
			// 返回的rcDamageInLayer参数即表示pObjDamage在pLayer中的脏区域
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
    // 3. 提交到窗口
    //////////////////////////////////////////////////////////////////////////

    if (bUpdateNow)
    {
        WindowRender* pWndRender = GetWindowRender();
		if (pWndRender)
	        pWndRender->Commit(NULL, prcDamageInLayer.m_prcArray, nCount);
    }

    return;
}

//  完成脏对象的绘制（处理z order覆盖问题），顺序：
//  1. 父控件背景
//  2. 比自己zorder小的sibling对象
//  3. 自己以及自己的子控件
//  4. 比自己zorder大的sibling对象
//
//  pObjDamage
//    [in] 真正需要被刷新的obj，也就是调用UpdateObject的对象
// 
//  pRenderTarget
//    [in] 绘制目标
//
//  context
//    [in] pRenderTarget的绘制上下文，指的是this的上下文
//
//  stackContext
//    [in] pObjDamage的祖先列表及其绘制参数
//
void  Object::HandleDamageStackContext(Object* pObjDamage, 
                                       IRenderTarget*  pRenderTarget,
                                       RenderContext context,
                                       deque<AncestorDamageInfo>& stackContext)
{
    //////////////////////////////////////////////////////////////////////////
    // 1. 取参数
    //////////////////////////////////////////////////////////////////////////

    AncestorDamageInfo  data = stackContext.back();
    stackContext.pop_back();

    Object*  pChildObj = NULL;
    if (stackContext.size())
        pChildObj = stackContext.back().pAncestor;
    else
        pChildObj = pObjDamage;
    
    RenderContext  contextSave = context;  // 专门用于z order处理

    //////////////////////////////////////////////////////////////////////////
    // 2. 画自己的背景 + less zorder
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

        // 悬浮对象的背景，还需要父对象的WM_PAINT内容
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

        // 绘制比pChildObj zOrder小的对象
        TestZorderOverlapLess(pChildObj, pRenderTarget, &data.rcDamage, &contextSave);
    }

    //////////////////////////////////////////////////////////////////////////
    // 3. 递归画子对象
    //////////////////////////////////////////////////////////////////////////

    if (!pChildObj->IsNcObject())
        context.DrawClient(m_pIObject);
    context.DrawChild(pChildObj->m_pIObject);

    if (stackContext.size())
    {
        pChildObj->HandleDamageStackContext(pObjDamage, pRenderTarget, context, stackContext);
    }
    else  // 遍历完成，轮到画自己了
    {
		context.m_bUpdateClip = pChildObj->NeedClip();
		context.Update(pRenderTarget);
		context.m_bUpdateClip = true;

		pChildObj->DrawObject(pRenderTarget, context); 

        // 注：DrawObject这里面在画子控件时，会导致剪裁区域变的更小
        // 因此返回之后，如果想再画great zorder，必然先还原剪裁区域。
    }

    //////////////////////////////////////////////////////////////////////////
    // 4. 判断great zorder
    //////////////////////////////////////////////////////////////////////////

    TestZorderOverlapGreat(pChildObj, pRenderTarget, &data.rcDamage, &contextSave);

    if (m_objStyle.post_paint)
    {
        DoPostPaint(pRenderTarget, contextSave);
    }
}


// 绘制比pChildObj zOrder更小的对象
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

    while (pTest)   // child是排好序的
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

// 绘制比pChildObj zOrder更大的对象
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
            c.m_bUpdateClip = true;  // 必须要这句。因为当前的剪裁区域不一定是init clip region
            c.Update(pRenderTarget);

            pTest->DrawObject(pRenderTarget, c);
        }

        pTest = pTest->m_pNext;
    }
}




//
// 递归. 将该对象及其子对象绘制到pRenderTarget上面
// 因为子控件的范围可能要比parent还大，因此在这里必须取子和父的交集clip rgn进行绘制  
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

        m_pRenderLayer->Commit(pRenderTarget);  // pRenderTarget有可能为NULL
    }
    else
    {
        RealDrawObject(pRenderTarget, context);
    }
    return true;
}
// 很多情况下面不需要判断自己的可见性，因此将绘制代码从DrawObject抽出来
// 2014.8.26 update. 非客户区控件也必须在WM_PAINT之后再绘制。例如飘浮子控件
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

        // 如果父对象不需要剪裁，那么自己也不剪裁
		bool bChildNeedClip = pChild->NeedClip();
		contextChild.m_bUpdateClip = bChildNeedClip;

		bool bNeedDrawChild = contextChild.DrawChild(pChild->GetIObject());
		if (bNeedDrawChild)
		{
			// 如果子对象是一个层，并且有变换，则不更新剪裁区域，而是切换到父对象的剪裁区域
			// (注：前一个邻居对象将pRenderTarget的剪裁区域修改了，因此要手动调用一下UpdateClipRgn)
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


#if 0 //过期
// 如果是透明背景的话，需要获取父窗口中的背景
// 但即使是不透明的object，也需要一层层遍历下来获取它的剪裁区域
//
//   为了适应于对象隐藏的时候能够刷新自己的父对象背景，需要增加一个场景来忽略自己的透明属性
//   因此增加bSelfTransparent，当普通刷新的时候传递this->IsTransparen()，其它时候直接传递false

void Object::DrawBkgndInLayer(IRenderTarget* pRTLayer, RenderContext& roc, bool bSelfTransparent)
{
	if (m_pRenderLayer)  // 自己就是layer的第一个对象，不需要画
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

		// 从pObjFirstParentToDrawBk开始绘制
		bool  bEraseBkgnd = true;
		if (pObjFirstParentToDrawBk)  
		{
			if (pObjParent == pObjFirstParentToDrawBk)  
			{
				// 清除标记
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


// 不支持zorder重叠，直接取祖先的背景
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
        // 检查祖父对象中有没有不透明的，从该父对象画起
        Object*  pObjAncestorFirst2DrawBkgnd = NULL;
        Object*  pChild = this;
        CRect    rcInAncestor(0, 0, GetWidth(), GetHeight());
        while (pObjAncestorFirst2DrawBkgnd = this->EnumParentObject(pObjAncestorFirst2DrawBkgnd))
        {
            ChildRect2ParentRect(pChild, &rcInAncestor, &rcInAncestor);

            // 找到不透明对象或者层对象
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
        // 子控件可能设置了剪裁区域。这里得先恢复再画postpaint
        context.m_bUpdateClip = true;
        context.Update(pRenderTarget);
        UI::UISendMessage(m_pIObject, UI_WM_POSTPAINT, (WPARAM)pRenderTarget);
    }
}
