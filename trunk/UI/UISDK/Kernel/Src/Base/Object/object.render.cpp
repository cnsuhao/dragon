#include "stdafx.h"
#include "object.h"
#include "UISDK\Kernel\Src\RenderLayer2\layer\renderlayer2.h"
#include "UISDK\Kernel\Src\RenderLayer2\layer\windowrender.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Src\RenderLayer2\soft3d\renderlayersoft3d.h"

namespace UI
{

//
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
//
void  Object::UpdateObjectEx(RECT* prcObjArray, int nCount, bool bUpdateNow)
{
    if (!prcObjArray || !nCount)
        return;

#ifdef TRACE_DRAW_PROCESS
    UI_LOG_DEBUG(_T("UpdateObjectEx id=%s -----------------------"), m_strId.c_str());
#endif

    //////////////////////////////////////////////////////////////////////////
    // 1. 状态判断
    //////////////////////////////////////////////////////////////////////////

    if (!IsVisible() || !CanRedraw())
        return;

    RealUpdateObjectEx(prcObjArray, nCount, bUpdateNow);
}

void  Object::RealUpdateObjectEx(RECT* prcObjArray, int nCount, bool bUpdateNow)
{
    //////////////////////////////////////////////////////////////////////////
    // 2. 向上遍历刷新
    //////////////////////////////////////////////////////////////////////////

    // 每个layer上被刷新的对象
    Object*  pObjDamage = this;   

    // 该对象的脏区域，正常nCount为1，在ListCtrl中可能为多个
    CRect*   prcObjDamage = new CRect[nCount];
    CRect*   prcDamageInLayer = new CRect[nCount];

    memcpy(prcObjDamage, prcObjArray, sizeof(RECT)*nCount);
    memset(prcDamageInLayer, 0, sizeof(CRect)*nCount);

    RenderLayer2*  pLayer = this->GetRenderLayer2();
    while (pLayer)
    {
        // 如果该对象的可见区域为空，则不需要再继续下去了
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

        // 上层layer
        RenderLayer2*  pParentLayer = pLayer->GetParentRenderLayer();
        if (!pParentLayer)
            break;

		CRect rcLayerDirty(0, 0, 0, 0);
        if (pLayer->IsTransformed())
        {
            // 由于层的旋转而产生的一些无效区域也添加进来
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
			// 返回的rcDamageInLayer参数即表示pObjDamage在pLayer中的脏区域
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
    // 3. 提交到窗口
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
void  Object::HandleDamageStackContext(Object* pObjDamage, IRenderTarget*  pRenderTarget, RenderContext context, stack<AncestorDamageInfo>& stackContext)
{
    //////////////////////////////////////////////////////////////////////////
    // 1. 取参数
    //////////////////////////////////////////////////////////////////////////

    AncestorDamageInfo  data = stackContext.top();
    stackContext.pop();

    Object*  pChildObj = NULL;
    if (stackContext.size())
        pChildObj = stackContext.top().pAncestor;
    else
        pChildObj = pObjDamage;
    
    RenderContext  contextSave = context;  // 专门用于z order处理

    //////////////////////////////////////////////////////////////////////////
    // 2. 画自己的背景 + less zorder
    //////////////////////////////////////////////////////////////////////////

    if (data.bDrawBkgnd)
    {
        context.Update(pRenderTarget);
        UISendMessage(m_pIObject, WM_ERASEBKGND, (WPARAM)pRenderTarget, (LPARAM)&context);
#ifdef TRACE_DRAW_PROCESS
        UI_LOG_DEBUG(_T("EraseBkgnd: %s"), m_strId.c_str());
#endif

        // 绘制比pChildObj zOrder小的对象
        TestZorderOverlapLess(pChildObj, pRenderTarget, &data.rcDamage, contextSave);
    }

    //////////////////////////////////////////////////////////////////////////
    // 3. 递归画子对象
    //////////////////////////////////////////////////////////////////////////

    if (!pChildObj->IsNcObject())
        context.DrawClient(m_pIObject);
    context.DrawChild(pChildObj->m_pIObject);

    Object*  pChild = NULL;
    if (stackContext.size())
    {
        pChildObj->HandleDamageStackContext(pObjDamage, pRenderTarget, context, stackContext);
    }
    else  // 遍历完成，轮到画自己了
    {
        context.m_bUpdateClip = true;
        context.Update(pRenderTarget);
        pChildObj->DrawObject(pRenderTarget, context); 

        // 注：DrawObject这里面在画子控件时，会导致剪裁区域变的更小
        // 因此返回之后，如果想再画great zorder，必然先还原剪裁区域。
    }

    //////////////////////////////////////////////////////////////////////////
    // 4. 判断great zorder
    //////////////////////////////////////////////////////////////////////////

    TestZorderOverlapGreat(pChildObj, pRenderTarget, &data.rcDamage, contextSave);
}


// 绘制比pChildObj zOrder更小的对象
void  Object::TestZorderOverlapLess(Object* pChildObj, IRenderTarget* pRenderTarget, RECT* prcClip, RenderContext context)
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

// 绘制比pChildObj zOrder更大的对象
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

        m_pRenderLayer->Commit(pRenderTarget);  // pRenderTarget有可能为NULL
    }
    else
    {
        RealDrawObject(pRenderTarget, context);
    }
    return true;
}
// 很多情况下面不需要判断自己的可见性，因些将绘制代码从DrawObject抽出来
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


// 不支持zorder重叠，直接取祖先的背景
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
    image.EndDrawToMyself();

#ifdef _DEBUGx
    image.Save(L"C:\\aaa.png", Gdiplus::ImageFormatPNG);
#endif
    return image.Detach();
}

}