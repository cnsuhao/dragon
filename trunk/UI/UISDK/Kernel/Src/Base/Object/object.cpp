#include "stdafx.h"
#include "object.h"

#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Kernel\Inc\Interface\ilayout.h"
#include "UISDK\Kernel\Inc\Interface\ipanel.h"
#include "UISDK\Kernel\Src\Resource\uiresource.h"
#include "UISDK\Kernel\Src\Renderbase\renderbase\renderbase.h"
#include "UISDK\Kernel\Src\Renderbase\textrenderbase\textrender.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Src\UIObject\HwndHost\HwndHost.h"
#include "UISDK\Kernel\Src\Renderlibrary\uicursor.h"
#include "UISDK\Kernel\Src\RenderLayer2\layer\windowrender.h"
#include "UISDK\Kernel\Src\Helper\layout\layout.h"
#include "UISDK\Kernel\Src\RenderLayer2\layer\renderlayer2.h"
#include "UISDK\Kernel\Src\RenderLayer2\soft3d\renderlayersoft3d.h"
#include "UISDK\Kernel\Src\Base\Applicatoin\uiapplication.h"

namespace UI
{

Object::Object()
{
    m_pIObject = NULL;
	::SetRectEmpty(&m_rcParent);
	::SetRectEmpty(&m_rcNonClient);
	::SetRectEmpty(&m_rcMargin);
	::SetRectEmpty(&m_rcPadding);
	::SetRectEmpty(&m_rcBorder);

	this->m_nCanRedrawRef = 0;
	
	this->m_hRgn = NULL;
	this->m_nStateBit = 0;
	this->m_nStyle2 = 0;
	this->m_nStyle  = 0;
	this->m_pUserData = NULL;
	this->m_pUIApplication = NULL;
	this->m_ppOutRef = NULL;
    this->m_nzOrder = 0;

	m_pBkgndRender = NULL;
	m_pForegndRender = NULL;
	m_pTextRender = NULL;
	m_pCursor = NULL;
    m_pIMapAttributeRemain = NULL;
    m_pLayoutParam = NULL;
    m_pRenderLayer = NULL;
}

 // ע�⣺��Ҫ�ڹ���������������е����麯��

Object::~Object(void)
{
	if (m_ppOutRef)
		*m_ppOutRef = NULL;

    SAFE_RELEASE(m_pIMapAttributeRemain);
    SAFE_RELEASE(m_pLayoutParam);
}

HRESULT Object::FinalConstruct(IUIApplication* p)
{
    m_pUIApplication = p;
    return S_OK;
}

void Object::FinalRelease()
{
	// ��ֹ������;���߻��������У�һЩ����ͻȻ�����٣����µ�Ұָ�����
	WindowBase* pWindow = GetWindowObject();
	if (pWindow)  // ������DestroyChildObjectǰ���ã�����parent��ϵ�����
	{
		pWindow->OnObjectDeleteInd(this);
	}

	this->DestroyChildObject();  // ��ɾ���Ӷ���

	IUIEditor* pEditor = m_pUIApplication->GetUIEditorPtr();
	if (pEditor)
	{
		// ������������֪ͨ�����ڴ�skinbuilder��tree��һ����ɾ��
		pEditor->OnObjectDeleteInd(m_pIObject);
	}

	//	�����Լ����ھӹ�ϵ
	RemoveMeInTheTree();

	m_rcParent.left = m_rcParent.right = m_rcParent.top = m_rcParent.bottom = 0;
	SAFE_RELEASE(m_pBkgndRender);
	SAFE_RELEASE(m_pForegndRender);
	SAFE_RELEASE(m_pTextRender);
	SAFE_RELEASE(m_pCursor);
	SAFE_DELETE_GDIOBJECT(m_hRgn);
    SAFE_DELETE(m_pRenderLayer);
}

// ע�����������ģ��ֱ�ӵ��� pCtrl->m_strID=L"..."�Ļ����ڶ����ͷ�ʱ�������
void Object::SetId(const TCHAR* szText)
{
	if (NULL == szText)
	{
		m_strId.clear();
		return;
	}

	m_strId = szText;
}
const  TCHAR*  Object::GetId() 
{
    return m_strId.c_str(); 
}

TCHAR* Object::GetObjectName()
{
    TCHAR* szName = NULL;
    int nType = 0;
    UISendMessage(this, UI_WM_GETOBJECTINFO, (WPARAM)&szName, (LPARAM)&nType);
    return szName;
}
OBJ_TYPE Object::GetObjectType()
{
    TCHAR* szName = NULL;
    int nType = 0;
    UISendMessage(this, UI_WM_GETOBJECTINFO, (WPARAM)&szName, (LPARAM)&nType);
    return GetObjectMajorType(nType);
}
CONTROL_TYPE Object::GetObjectExtentType()
{
    TCHAR* szName = NULL;
    int nType = 0;
    UISendMessage(this, UI_WM_GETOBJECTINFO, (WPARAM)&szName, (LPARAM)&nType);
    return ::GetObjectExtentType(nType);
}

void Object::SetCanRedraw( bool bReDraw )
{
	if (bReDraw)
		m_nCanRedrawRef --;
	else
		m_nCanRedrawRef ++;
}

// TODO: ��Ҫ���ϵݹ�����ж�
bool Object::CanRedraw()
{
	return 0==m_nCanRedrawRef;
}

bool Object::SetCursor(const TCHAR* szCursorId)
{
	SAFE_RELEASE(m_pCursor);

	ICursorRes* pCursorRes = m_pUIApplication->GetActiveSkinCursorRes();
	if (NULL == pCursorRes)
		return false;

    pCursorRes->GetCursor(szCursorId, &m_pCursor); 
	if (NULL == m_pCursor)
	{
		UI_LOG_WARN(_T("%s get cursor failed. Object id=%s, cursor id=%s"), FUNC_NAME, m_strId.c_str(), szCursorId);	
		return false;
	}

	return true;
}

// ��ȡ�Լ������layerָ��
RenderLayer2*  Object::GetSelfRenderLayer2()
{
    return m_pRenderLayer;
}
// ��ȡ�Լ����ڵ�layerָ��
RenderLayer2*  Object::GetRenderLayer2()
{
    RenderLayer2*  pRenderLayer = NULL;

    Object* pObj = this;
    while (pObj)
    {
        pRenderLayer = pObj->GetSelfRenderLayer2();
        if (pRenderLayer)
            break;

        pObj = pObj->m_pParent;
    }

    return pRenderLayer;
}

// ��ȡ�Լ����ڲ�Ĳ����
Object*  Object::GetRenderLayerCreateObject()
{
    RenderLayer2*  p = GetRenderLayer2();
    if (p)
        return p->GetCreateObject();

    return NULL;
}

WindowRender*  Object::GetWindowRender()
{
    WindowBase* pWindow = GetWindowObject();
    if (!pWindow)
        return NULL;

    return pWindow->GetWindowRender();
}

// ��ȡһ���ؼ����ڴ��ھ��
WindowBase* Object::GetWindowObject()
{
    Object*  pParent = this;
    do 
    {
        if (!pParent->m_pParent)
            break;
        pParent = pParent->m_pParent;
    } 
    while (1);

    //return static_cast<WindowBase*>(pParent);
    IWindowBase*  pWindowBase = (IWindowBase*)pParent->GetIObject()->QueryInterface(uiiidof(IWindowBase));
    if (pWindowBase)
        return pWindowBase->GetImpl();
    return NULL;
}

HWND Object::GetHWND()
{
	WindowBase* pWindow = this->GetWindowObject();
	if (NULL == pWindow)
		return NULL;

	return pWindow->GetHWND();
}
/*
**	[public] ��ȡĳһ����·���µ��Ӷ��󣬵�������Ӷ���Ҳ�����Ӷ�����Ӷ���
**
**	parameter
**		objIdPath
**			[in]	Ҫ��ȡ�Ķ����·�����������Լ�����·����/�ָ�
**	
**	return
**		�ɹ����ظö���ʧ�ܷ���NULL��ʧ�ܿɲ鿴��־���
*/
// Object*  Object::GetChildObject( const String& objIdPath )
// {
// 	Object*   pObjRet  = this;
// 
// 	vector<String>  vPath;
// 	UI_Split( objIdPath, XML_PATH_SEPERATOR, vPath );
// 
// 	if (vPath.empty() )
// 	{
// 		UI_LOG_ERROR( _T("Object::GetObjectItem����Ч�Ĳ���:%s"), objIdPath.c_str() );
// 		return NULL;
// 	}
// 
// 	size_t  s = vPath.size();
// 	for( size_t i = 0; i < s; i++ )
// 	{
// 		Object* pTempObj = NULL;
// 		while( pTempObj = pObjRet->EnumChildObject( pTempObj ) )
// 		{
// 			if (vPath[i] == pTempObj->m_strId )
// 			{
// 				pObjRet = pTempObj;
// 				break;  // jump out of while
// 			}
// 		}
// 
// 		// û���ҵ�ָ���Ķ���
// 		if (NULL == pTempObj )
// 		{
// 			UI_LOG_ERROR( _T("Object::GetObjectItem��δ�ҵ�ָ���Ķ�����%s�£�"), this->m_strID.c_str(), objIdPath.c_str() );
// 			return NULL;
// 		}
// 	}
// 
// 	return pObjRet;
// }

//
//	�����е����������в���idΪobjId�Ķ���ֻ���ص�һ���ҵ��Ķ���
//
//	Parameter
//		objId
//			[in]	Ҫ���ҵĶ���id
//
//	Return
//		�ɹ����ض����ַ��ʧ�ܷ���NULL
//

Object*  Object::FindChildObject(const TCHAR* szObjId)
{
    if (NULL == szObjId)
        return NULL;

	Object* pRet = this->_findChildObjectItem(szObjId);
	if (NULL == pRet)
	{
		UI_LOG_WARN(_T("%s Find \"%s\" from \"%s\" failed."), FUNC_NAME, szObjId, this->m_strId.c_str() );
	}
	return pRet;
}


Object* Object::_findChildObjectItem(const TCHAR* szobjId)
{
    if (NULL == szobjId)
        return NULL;

	Object* pObjChild = NULL;
	while (pObjChild = this->EnumChildObject(pObjChild))
	{
		if (0 == _tcscmp(pObjChild->GetId(), szobjId))
        {
			return pObjChild;
        }
		else
		{
			Object* pRet = ((Object*)pObjChild)->_findChildObjectItem(szobjId);
			if (pRet )
				return pRet;
		}
	}	
	return NULL;
}


void Object::ResetAttribute()
{
	::SetRectEmpty(&m_rcMargin);
	::SetRectEmpty(&m_rcPadding);
	::SetRectEmpty(&m_rcNonClient);
	::SetRectEmpty(&m_rcBorder);

    m_nzOrder = 0;

    SAFE_RELEASE(m_pLayoutParam);
	SAFE_RELEASE(m_pBkgndRender);
	SAFE_RELEASE(m_pForegndRender);
	SAFE_RELEASE(m_pTextRender);
	SAFE_RELEASE(m_pCursor);
}

void  Object::OnSerialize(SERIALIZEDATA* pData)
{
	if (pData->nFlag & SERIALIZEFLAG_SAVE)
	{

	}
	else if (pData->nFlag & SERIALIZEFLAG_LOAD)
	{
		SetAttribute(pData->pMapAttrib, pData->nFlag&SERIALIZEFLAG_RELOAD?true:false);
	}

    // ��������
    if (m_pLayoutParam)
    {
        m_pLayoutParam->Serialize(pData);
    }
}

bool Object::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
    // �������ԣ�������չ��
    SAFE_RELEASE(m_pIMapAttributeRemain);
    m_pIMapAttributeRemain = pMapAttrib;
    if (m_pIMapAttributeRemain)
        m_pIMapAttributeRemain->AddRef();

	if (NULL == m_pUIApplication || NULL == pMapAttrib)
	{
		UI_LOG_ERROR(_T("%s Invalid Argument."), FUNC_NAME);
		return false;
	}

	//	����id����
    const TCHAR* szText = NULL;
    szText = pMapAttrib->GetAttr(XML_ID, true);
    if (szText)
        m_strId = szText;
	
    // �ɼ���
    if (!bReload)
    {
	    bool bVisible = true;
        pMapAttrib->GetAttr_bool(XML_VISIBLE, true, &bVisible);
        if (false == bVisible)
            m_nStateBit |= OSB_UNVISIBLE;
        else
            m_nStateBit &= ~OSB_UNVISIBLE;
    }

    if (!bReload)
    {
        bool bDisable = false;
        pMapAttrib->GetAttr_bool(XML_DISABLE, true, &bDisable);
        if (bDisable)
            m_nStateBit |= OSB_DISABLE;
        else
            m_nStateBit &= ~OSB_DISABLE;
    }

    pMapAttrib->GetAttr_CRegion4(XML_MARGIN, true, &m_rcMargin);

    CRegion4 region4;
    if (MAPATTR_RET_NOT_EXIST != pMapAttrib->GetAttr_CRegion4(XML_PADDING, true, &region4))
        this->SetPaddingRegion(&region4);

    if (MAPATTR_RET_NOT_EXIST != pMapAttrib->GetAttr_CRegion4(XML_BORDER, true, &region4))
        this->SetBorderRegion(&region4);

	// ���ñ�����Ⱦ��
    pMapAttrib->GetAttr_RenderBase(XML_BACKGND_RENDER_PREFIX, XML_RENDER_TYPE, true, m_pUIApplication, this->GetIObject(), &m_pBkgndRender);

	// ����ǰ������
    pMapAttrib->GetAttr_RenderBase(XML_FOREGND_RENDER_PREFIX, XML_RENDER_TYPE, true, m_pUIApplication, this->GetIObject(), &m_pForegndRender);

// 	// ����  -- �����������Ƴ���˭��Ҫ��������˭�Լ�����������������еĶ�����Ҫ����
	//          ��window��font������windowbase::setattribute�н�����"font"���Եõ�
// 	iter = m_mapAttribute.find(XML_TEXTRENDER_TYPE);
// 	if (m_mapAttribute.end() != iter)
// 	{
// 		SAFE_RELEASE(m_pTextRender);
// 		const String& strTextRenderType = iter->second;
// 		m_pTextRender = TextRenderFactory::GetTextRender(strTextRenderType, this);
// 		m_pTextRender->SetAttribute(_T(""),m_mapAttribute);
// 
// 		this->m_mapAttribute.erase(iter);
// 	}
// 	else if (NULL == m_pTextRender )
// 	{
// 		m_pTextRender = TextRenderFactory::GetTextRender(TEXTRENDER_TYPE_NORMAL, this);
// 		m_pTextRender->SetAttribute(_T(""),m_mapAttribute);
// 	}
	
	// ˢ������
	bool bTransparent = false;
    if (MAPATTR_RET_NOT_EXIST != pMapAttrib->GetAttr_bool(XML_BACKGND_IS_TRANSPARENT, true, &bTransparent))
	{
		if (bTransparent)
			m_nStyle |= OBJECT_STYLE_TRANSPARENT;
		else
			 m_nStyle &= ~OBJECT_STYLE_TRANSPARENT;
	}

    bool bRejectMouseMsg = false;
    if (MAPATTR_RET_NOT_EXIST != pMapAttrib->GetAttr_bool(XML_REJEST_MOUSE_MSG, true, &bRejectMouseMsg))
    {
        if (bRejectMouseMsg)
            m_nStyle |= OBJECT_STYLE_REJECT_MOUSE_MSG_ALL;
        else
            m_nStyle &= ~OBJECT_STYLE_REJECT_MOUSE_MSG_ALL;
    }

	// �����ʽ
	szText = pMapAttrib->GetAttr(XML_CURSOR, true);
	if (szText)
		this->SetCursor(szText);

    // z�� (ע��Ĭ���ڴ�xml����֮��AddChild֮ǰ���Ƚ���һ��)
    pMapAttrib->GetAttr_int(XML_ZORDER, true, (int*)&m_nzOrder);
//     bool bZindexOverlap = false;
//     if (MAPATTR_RET_NOT_EXIST != pMapAttrib->GetAttr_bool(XML_ZINDEX_OVERLAP, true, &bZindexOverlap))
//     {
//         if (bZindexOverlap)
//             m_nStyle |= OBJECT_STYLE_ZINDEX_OVERLAP;
//         else
//             m_nStyle &= ~OBJECT_STYLE_ZINDEX_OVERLAP;
//     }

	bool  bCreateLayer = false;
    pMapAttrib->GetAttr_bool(XML_LAYER, true, &bCreateLayer);
    if (bCreateLayer)
    {
		CreateRenderLayer();
    }

	return true;
}


void  Object::OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA*  pData)
{
    IUIEditor* pEditor = pData->pEditor;
    const TCHAR* szPrefix = pData->szPrefix;

	IUIEditorGroupAttribute*  pObjGroup = pEditor->CreateGroupAttribute(pData->pGroupAttr, Object::GetXmlName(), NULL);
 	pEditor->CreateTextAttribute(pObjGroup, XML_ID, szPrefix, NULL, L"�ؼ�ID");
    pEditor->CreateBoolAttribute(pObjGroup, XML_VISIBLE, true, szPrefix, NULL, L"�Ƿ�ɼ���Ĭ�Ͽɼ�");
	pEditor->CreateBoolAttribute(pObjGroup, XML_DISABLE, false, szPrefix, NULL, L"�Ƿ񱻽��á�Ĭ�Ͽ���");
    pEditor->CreateBoolAttribute(pObjGroup, XML_BACKGND_IS_TRANSPARENT, false, szPrefix, NULL, L"�ؼ������Ƿ�͸��������ؼ�����͸�����ڻ���ʱ����Ҫ�ٻ��Ƹ��ؼ�������Ĭ�ϲ�͸��");
    pEditor->CreateBoolAttribute(pObjGroup, XML_REJEST_MOUSE_MSG, false, szPrefix, NULL, L"�Ƿ����������Ϣ��Ĭ�Ͻ���");
//  pEditor->CreateBoolAttribute(pObjGroup, XML_ZINDEX_OVERLAP, false, szPrefix, NULL, L"�Ƿ�������ؼ����ص�����Ĭ��û��");
    pEditor->CreateTextAttribute(pObjGroup, XML_ZORDER, szPrefix, NULL, L"�ؼ�z��");
    pEditor->CreateBoolAttribute(pObjGroup, XML_LAYER, false, szPrefix, NULL, L"�Ƿ�Ϊ�ÿؼ�����һ���㣨����һ�����棩����������֧����Ч");
	pEditor->CreateTextAttribute(pObjGroup, XML_MARGIN, szPrefix, NULL, L"����");
	pEditor->CreateTextAttribute(pObjGroup, XML_PADDING, szPrefix, NULL, L"�ڼ��");
	pEditor->CreateTextAttribute(pObjGroup, XML_BORDER, szPrefix, NULL, L"�߿�");
    pEditor->CreateTextAttribute(pObjGroup, XML_CURSOR, szPrefix, NULL, L"�����ʽ");

    IUIEditorGroupAttribute* pLayoutGroup = pEditor->CreateGroupAttribute(pObjGroup, XML_LAYOUT, NULL);
    if (m_pLayoutParam)
    {
        EDITORGETOBJECTATTRLISTDATA data = {0};
        data.pEditor = pEditor;
        data.pGroupAttr = pLayoutGroup;
        m_pLayoutParam->OnEditorGetAttrList(&data);
    }

    // bkgnd.render.xxxx
    IUIEditorGroupAttribute* pBkRenderGroup = pEditor->CreateGroupAttribute(pObjGroup, XML_BACKGND_RENDER_PREFIX, NULL);
	IUIEditorComboBoxAttribute*  pBkgndRenderType = pEditor->CreateComboBoxAttribute(pBkRenderGroup, XML_RENDER_TYPE, szPrefix, XML_BACKGND_RENDER_PREFIX);
    pBkgndRenderType->SetReloadOnChanged(true);  
    m_pUIApplication->GetImpl()->UIEditor_FillRenderBaseType2Combobox(pBkgndRenderType);
    if (m_pBkgndRender)
    {
        String strPrefix;
        if (pData->szPrefix)
            strPrefix.append(pData->szPrefix);
        strPrefix.append(XML_BACKGND_RENDER_PREFIX);

        EDITORGETOBJECTATTRLISTDATA data;
        data.pEditor = pData->pEditor;
        data.pGroupAttr = pBkRenderGroup;
        data.szPrefix = strPrefix.c_str();
        UISendMessage(m_pBkgndRender, UI_EDITOR_GETOBJECTATTRLIST, (WPARAM)&data);
    }

    // foregnd.render.xxxx
    IUIEditorGroupAttribute* pForeRenderGroup = pEditor->CreateGroupAttribute(pObjGroup, XML_FOREGND_RENDER_PREFIX, NULL);
	IUIEditorComboBoxAttribute*  pForegndRenderType = pEditor->CreateComboBoxAttribute(pForeRenderGroup, XML_RENDER_TYPE, szPrefix, XML_FOREGND_RENDER_PREFIX);
    pForegndRenderType->SetReloadOnChanged(true);  
    m_pUIApplication->GetImpl()->UIEditor_FillRenderBaseType2Combobox(pForegndRenderType);
    if (m_pForegndRender)
    {
        String strPrefix;
        if (pData->szPrefix)
            strPrefix.append(pData->szPrefix);
        strPrefix.append(XML_FOREGND_RENDER_PREFIX);

        EDITORGETOBJECTATTRLISTDATA data;
        data.pEditor = pData->pEditor;
        data.pGroupAttr = pForeRenderGroup;
        data.szPrefix = strPrefix.c_str();
        UISendMessage(m_pForegndRender, UI_EDITOR_GETOBJECTATTRLIST, (WPARAM)&data);
    }
}

void  Object::ParseStyleAndSetAttribute(IMapAttribute* pMatAttrib, bool bReload)
{
    // ������ʽ
    const TCHAR* szStyleClass = pMatAttrib->GetAttr(XML_STYLECLASS, false);

    if (m_pUIApplication)
    {
        ISkinRes* pSkinRes = m_pUIApplication->GetActiveSkinRes();
        if (pSkinRes)
        {
            IStyleManager* pStyleMgr = pSkinRes->GetStyleManager();
            if (pStyleMgr)
            {
                pStyleMgr->LoadStyle(GetObjectName(), szStyleClass, m_strId.c_str(), pMatAttrib);
            }
        }
    }

	SERIALIZEDATA data = {0};
	data.pMapAttrib = pMatAttrib;
	data.nFlag = SERIALIZEFLAG_LOAD;
	if (bReload)
		data.nFlag |= SERIALIZEFLAG_RELOAD;
	UISendMessage(m_pIObject, UI_WM_SERIALIZE, (WPARAM)&data);
//    UISendMessage(m_pIObject, UI_WM_SETATTRIBUTE, (WPARAM)pMatAttrib, (LPARAM)bReload); // ��ʹΪ�գ�ҲҪ֪ͨ��ȥ
}
void Object::SetAttributeByPrefix(const TCHAR* szPrefix, IMapAttribute* pMapAttrib, bool bReload, bool bErase)
{
    IMapAttribute* pMapChildObjAttrib = NULL;

    if (szPrefix)
    {
        if (false == pMapAttrib->ExtractMapAttrByPrefix(szPrefix, bErase, &pMapChildObjAttrib))
        {
            SAFE_RELEASE(pMapChildObjAttrib);
            return;
        }

        if (NULL == pMapChildObjAttrib)
            return;
    }
    else
    {
        pMapChildObjAttrib = pMapAttrib;
        pMapChildObjAttrib->AddRef();
    }

    ParseStyleAndSetAttribute(pMapChildObjAttrib, bReload);
    SAFE_RELEASE(pMapChildObjAttrib);
}
// bool Object::SetChildObjectAttribute(Object* pChildObj, const String& strPrifix, IMapAttribute* pMapAttrib, bool bReload)
// {
// 	if (NULL == pChildObj)
// 		return false;
// 
// 	IMapAttribute* pMapChildObjAttrib = NULL;
//     if (FALSE == pMapAttrib->ExtractMapAttrByPrefix(strPrifix.c_str(), true, &pMapChildObjAttrib))
//         return false;
//     if (NULL == pMapChildObjAttrib)
//         return false;
// 
//     // ������ʽ
// 	const TCHAR* szStyleClass = pMapChildObjAttrib->GetAttr(XML_STYLECLASS, false);
// 
// 	if (m_pUIApplication)
// 	{
// 		ISkinRes* pSkinRes = NULL;
// 		m_pUIApplication->GetActiveSkinRes(&pSkinRes);
// 		if (pSkinRes)
// 		{
// 			IStyleManager* pStyleMgr = NULL;
// 			pSkinRes->GetStyleManager(&pStyleMgr);
// 			if (pStyleMgr)
// 			{
// 				pStyleMgr->LoadStyle(pChildObj->GetObjectName(), szStyleClass, pChildObj->m_strId.c_str(), pMapAttrib);
// 			}
// 		}
// 	}
// 
// 	bool bRet = pChildObj->SetAttribute(pMapChildObjAttrib, bReload);  // ��ʹΪ�գ�ҲҪ֪ͨ��ȥ
//     SAFE_RELEASE(pMapChildObjAttrib);
//     return bRet;
// }


// ��ȡһ��δ���������ԡ����bErase==true���򽫷���һ����ʱ���ַ���ָ�룬������Ӧ�þ��챣����߽���ʱʹ��
const TCHAR*  Object::GetAttribute(const TCHAR* szKey, bool bErase)
{
    if (NULL == szKey || NULL == m_pIMapAttributeRemain)
        return NULL;

    return m_pIMapAttributeRemain->GetAttr(szKey, bErase);
}
void  Object::AddAttribute(const TCHAR* szKey, const TCHAR*  szValue)
{
    if (NULL == m_pIMapAttributeRemain) 
    {
        UICreateIMapAttribute(&m_pIMapAttributeRemain);
    }
    m_pIMapAttributeRemain->AddAttr(szKey, szValue);

}
void  Object::GetMapAttribute(IMapAttribute** ppMapAttribute)
{
    if (ppMapAttribute && m_pIMapAttributeRemain)
    {
        *ppMapAttribute = m_pIMapAttributeRemain;
        m_pIMapAttributeRemain->AddRef();
    }
}


// ����padding��ֵ��ͬʱ���·ǿͻ����Ĵ�С
void Object::SetPaddingRegion(CRegion4* prc)
{
	if (NULL == prc)
		return;

	REGION4 rcOther = { 
		m_rcNonClient.left   - m_rcPadding.left,
		m_rcNonClient.top    - m_rcPadding.top,
		m_rcNonClient.right  - m_rcPadding.right,
		m_rcNonClient.bottom - m_rcPadding.bottom };

	m_rcPadding.CopyRect(prc);
	m_rcNonClient.SetRect(
		m_rcPadding.left   + rcOther.left,
		m_rcPadding.top    + rcOther.top,
		m_rcPadding.right  + rcOther.right,
		m_rcPadding.bottom + rcOther.bottom );
}


void Object::SetBorderRegion( CRegion4* prc )
{
	if (NULL == prc)
		return;

	REGION4 rcOther = { 
		m_rcNonClient.left   - m_rcBorder.left,
		m_rcNonClient.top    - m_rcBorder.top,
		m_rcNonClient.right  - m_rcBorder.right,
		m_rcNonClient.bottom - m_rcBorder.bottom };

	m_rcBorder.CopyRect(prc);
	m_rcNonClient.SetRect(
		m_rcBorder.left   + rcOther.left,
		m_rcBorder.top    + rcOther.top,
		m_rcBorder.right  + rcOther.right,
		m_rcBorder.bottom + rcOther.bottom );
}


void*  Object::QueryInterface(const IID* pIID)
{
    if (IsEqualIID(*pIID, uiiidof(IObject)) ||
        IsEqualIID(*pIID, uiiidof(IMessage)) )
    {
        return m_pIObject;
    }

    return NULL;
}

void Object::OnEraseBkgnd(IRenderTarget* pRenderTarget)
{
	if (m_pBkgndRender)
	{
		CRect rc(0,0, this->GetWidth(), this->GetHeight());
		m_pBkgndRender->DrawState(pRenderTarget, &rc, 0);
	}
}

//
//	����Object��Ĭ�������ʽ
//
//	Remark:
//		�����ʵ�ָ���Ϣ�����ܵ�������ƶ���һ��Object����ʱ����Ȼ��ʾ����Window��OnSetCursor
//

BOOL Object::OnSetCursor(HWND hWnd, UINT nHitTest, UINT message)
{
	if (NULL == m_pCursor || NULL == m_pCursor->GetCursor())
	{
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));
 		return TRUE;
	}

	::SetCursor(m_pCursor->GetCursor());
	return TRUE;
}

//	��ȡ�Լ�������,���ﷵ�صĶ���ֻ��һ����ʱ����
//	�����Ҫ����ʹ�ã�����Ҫ����AddRef
IRenderFont* Object::GetRenderFont()
{
    IRenderFont* pRenderFont = NULL;

    // ���Լ���textrender��ȡ
    if (m_pTextRender)
        pRenderFont = m_pTextRender->GetRenderFont();

    if (pRenderFont)
        return pRenderFont;

    // �򴰿ڻ�ȡĬ��
    WindowBase* pWindow = GetWindowObject();
    if (pWindow)
        return pWindow->GetWindowDefaultRenderFont();

    return NULL;
}

//
//	[public] [virtual]  UINT HitTest( POINT* ptInParent )
//
//	Parameters
//		ptInParent
//			[in]  Ҫ��̽��λ�ã�ͨ��Ϊ��굱ǰλ��
//
//	Return
//		��겻�ڶ������о�Χ�ڷ���HTNOWHERE�����򷵻�HTCLIENT�����������������չ��
//
//	Remark
//		����������������һ�����⣬��ΪObject�䱸һ��RECT�����ʾ��Χ�����Ǹ�һ����ǿ���RGN��ʾ��Χ?
//		����RGN���������ڴ棬����ʱ��͵�ʹ��RGN��ʵ��ĳЩ���ܣ������������������������ߵ����ߣ�
//		�����λ�ô�����ʲô��λ��ͬʱ��Object�䱸һ��RECT������ʾÿһ������ķ�Χ����Ϊ�󲿷�ʱ��ʹ�õĶ���RECT����
//

UINT Object::OnHitTest(POINT* ptInParent, __out POINT* ptInChild)
{
	BOOL  bIn = FALSE;

	if (this->m_hRgn != NULL)
	{
        UIASSERT(0);  // δʵ��
// 		bIn = ::PtInRegion(m_hRgn, ptInParent->x, ptInParent->y);
//      if (bIn)
// 		    return HTCLIENT;
	}
    else if (m_pRenderLayer && m_pRenderLayer->m_pTransform)
    {
        bIn =  m_pRenderLayer->m_pTransform->HitTest(ptInParent, ptInChild) ?  TRUE:FALSE;
        if (bIn)
            return HTCLIENT;
    }
	else
	{
		bIn = ::PtInRect(&m_rcParent, *ptInParent);

		if (bIn)
		{
			CRect rcClient;
			this->GetClientRect(&rcClient);

            if (ptInChild)
            {
                ptInChild->x = ptInParent->x - m_rcParent.left;
                ptInChild->y = ptInParent->y - m_rcParent.top;
            }

			if (::PtInRect(&rcClient, *ptInParent))
				return HTCLIENT;
			else
				return HTBORDER;
		}
	}

	return HTNOWHERE;
}

void Object::OnThemeChanged()
{
	SetMsgHandled(FALSE);
    
    if (m_pBkgndRender)
        m_pBkgndRender->CheckThemeChanged();
    if (m_pForegndRender)
        m_pForegndRender->CheckThemeChanged();
}

LRESULT  Object::OnSkinTextureChanged(UINT, WPARAM, LPARAM)
{
    if (m_pTextRender)
    {
        m_pTextRender->CheckSkinTextureChanged();
    }
    return 0;
}

void  Object::OnVisibleChanged(BOOL bVisible, IObject* pObjChanged)
{
//     if (!bVisible)
//     {
//         // ��ֹ������;���߻��������У�һЩ����ͻȻ�����٣����µ�Ұָ�����
//         WindowBase* pWindow = GetWindowObject();
//         if (pWindow)  // ������DestroyChildObjectǰ���ã�����parent��ϵ�����
//         {
//             pWindow->OnObjectHideInd(this);
//         }
//     }

    // ������صĶ�����һ����������򽫽��������лص���һ������
    // ע�������objectҲ������λ��һ��listctrl�У������Ӧ��mouse/keyboard
    //     manager�����Ǵ��ڵģ�����listctrl�Լ���
    if (false == bVisible)
    {
        Object*  pParent = m_pParent;
        IMKMgr*  pMKMgr = NULL;
        while (pParent)
        {
            pMKMgr = (IMKMgr*)UISendMessage(pParent->GetIMessage(), UI_WM_GET_MOUSEKEYBOARD_MANAGER);
            if (pMKMgr)
            {
                pMKMgr->OnObjectHideInd(this);
                break;
            }

            pParent = pParent->GetParentObject();
        }
    }
}

// Ĭ��ʵ��Ϊ����ˢ�¡�
// ����Ϣ������ͨ��PostMessage�����ӳ�ˢ�£�����ֻ���õ�һ��IMessage����ʱ����
void  Object::OnRedrawObject()
{
    this->UpdateObject();
}

/*
**	[private] bool    testStateBit( BYTE nbit );
**	[private] bool    setStateBit( BYTE nbit );
**
**	parameters
**		nbit
**			[in] Ҫ���Եı���λ����OSB_VISIBLE��OSB_DISABLE��CSB_TABABLE��CSB_DEFAULT
**				 CSB_FOCUS��CSB_READONLY��CSB_HOVER��CSB_PRESS
**	
**	return
**		��λΪ1������true����λΪ0������false
**
**	remark
**		Ϊ�˽�ʡ�ڴ�ռ�ã���һ���ؼ��Ķ���״̬����Ϊbitλ���д���
*/

bool Object::testStateBit( UINT bit )
{
	if (m_nStateBit & bit )
		return true;
	else 
		return false;
}

void Object::setStateBit( UINT bit )
{
	m_nStateBit |= bit;
}

void Object::clearStateBit( UINT nbit )
{
	m_nStateBit &= ~nbit;
}


void Object::ModifyStyle( UINT nStyleAdd, UINT nStyleRemove, bool bNotify )
{
	STYLESTRUCT s;
	s.styleOld = m_nStyle;
	s.styleNew = m_nStyle;

	UINT nOldStyle = m_nStyle;
	if (nStyleAdd != 0)
	{
		s.styleNew |= nStyleAdd;
	}
	if (nStyleRemove != 0)
	{
		s.styleNew &= ~nStyleRemove;
	}

	if (s.styleNew == s.styleOld)
		return;

	if (bNotify)
	{
		::UISendMessage(this, WM_STYLECHANGING, GWL_STYLE, (LPARAM)&s);
	}

	m_nStyle = s.styleNew;

	if (bNotify)
	{
		s.styleOld = s.styleOld;
		::UISendMessage(this, WM_STYLECHANGED, GWL_STYLE, (LPARAM)&s);
	}
}

void Object::ModifyStyleEx(UINT nStyleAdd, UINT nStyleRemove, bool bNotify)
{
    STYLESTRUCT s;
    s.styleOld = m_nStyle2;
    s.styleNew = m_nStyle2;

    UINT nOldStyle = m_nStyle2;
    if (nStyleAdd != 0)
    {
        s.styleNew |= nStyleAdd;
    }
    if (nStyleRemove != 0)
    {
        s.styleNew &= ~nStyleRemove;
    }

    if (s.styleNew == s.styleOld)
        return;

    if (bNotify)
    {
        ::UISendMessage(this, WM_STYLECHANGING, GWL_EXSTYLE, (LPARAM)&s);
    }

    m_nStyle2 = s.styleNew;

    if (bNotify)
    {
        s.styleOld = s.styleOld;
        ::UISendMessage(this, WM_STYLECHANGED, GWL_EXSTYLE, (LPARAM)&s);
    }
}

bool Object::IsTransparent()
{
	if (m_nStyle & OBJECT_STYLE_TRANSPARENT)
		return true;

	return false;
}
void  Object::SetTransparent(bool b)
{
    if (b)
        m_nStyle |= OBJECT_STYLE_TRANSPARENT;
    else
        m_nStyle &= ~OBJECT_STYLE_TRANSPARENT;
}


bool Object::IsFocus()
{
	return this->testStateBit(OSB_FOCUS);
}


void Object::SetFocus(bool b, bool bNoitfy)
{
    UINT nOld = m_nStateBit;
	if (b)
		this->setStateBit(OSB_FOCUS);
	else
		this->clearStateBit(OSB_FOCUS);
	
    if (bNoitfy && nOld != m_nStateBit)
    {
        UIMSG  msg;
        msg.message = UI_WM_STATECHANGED2;
        msg.wParam = OSB_FOCUS;
        msg.pMsgTo = GetIMessage();
        UISendMessage(&msg);
    }

}


bool Object::IsTabstop()
{
	return (m_nStyle & CONTROL_STYLE_TABSTOP)?true:false;
}


bool Object::CanTabstop()
{
	return this->IsTabstop() && this->IsEnable() && this->IsVisible();
}

bool Object::IsGroup()
{
	return  (m_nStyle&CONTROL_STYLE_GROUP)?true:false;
}


void Object::SetTabstop(bool b)
{
	if (b)
		this->ModifyStyle(CONTROL_STYLE_TABSTOP, 0, false);
	else
		this->ModifyStyle(0, CONTROL_STYLE_TABSTOP, false);
}

void Object::SetGroup(bool b)
{
	if (b )
		this->ModifyStyle(CONTROL_STYLE_GROUP, 0, false);
	else
		this->ModifyStyle(0, CONTROL_STYLE_GROUP, false);
}


bool Object::IsMySelfVisible()
{
	bool bVisible = !this->testStateBit( OSB_UNVISIBLE );
	return bVisible;
}

bool Object::IsVisible()
{
    int nType = this->GetObjectType();
	if (nType == OBJ_WINDOW)
	{
		return ::IsWindowVisible(((WindowBase*)this)->m_hWnd)?true:false;
	}
	if (nType == OBJ_HWNDHOST)
	{
		return ::IsWindowVisible(((HwndHost*)this)->m_hWnd)?true:false;
	}

	bool bVisible = !this->testStateBit(OSB_UNVISIBLE);
	if (false == bVisible || NULL == m_pParent)
	{
		return bVisible;
	}
	// �жϸ����󲻿ɼ������ 
	else
	{
		if (OBJ_WINDOW == m_pParent->GetObjectType())
		{ 
			return true;           // ע�������ﲻ�������Ĵ��ڽ����жϵ�ԭ���ǣ��������ڴ��ڳ�ʼ���ĺ������棬��Ȼ������ʱ�ǲ��ɼ��ģ�������Ķ����IsVisibleӦ���Ƿ���true�źô���
		}
		else
		{
			return m_pParent->IsVisible();
		}
	}
    return true;
}


bool Object::IsCollapsed()
{
	// TODO:  
	UIASSERT(0);
	return false;
}


bool Object::IsEnable()
{
    int nObjType = this->GetObjectType();
	if (nObjType == OBJ_WINDOW)
	{
		return ::IsWindowEnabled(((WindowBase*)this)->m_hWnd)?true:false;
	}
	if (nObjType == OBJ_HWNDHOST)
	{
		return ::IsWindowEnabled(((HwndHost*)this)->m_hWnd)?true:false;
	}
	return !this->testStateBit(OSB_DISABLE);
}

//
//	��ע���ֲ㴰���У�����һ������ʱ������ֱ�ӵ���::InvalidateRect(&rc..)
//        ������� GetWindowObject(); pWindow->InvalidateObject(GetWindowObject(),...);
//

void Object::SetVisible(bool b, bool bRedraw, bool bUpdateLayout)
{
	bool bOld = IsVisible();

    int nObjType = this->GetObjectType();
	if (nObjType == OBJ_WINDOW)
	{
		::ShowWindow(((WindowBase*)this)->m_hWnd, b?SW_SHOW:SW_HIDE);
		return;
	}
	else if (nObjType == OBJ_HWNDHOST)
	{
		::ShowWindow(((HwndHost*)this)->m_hWnd, b?SW_SHOW:SW_HIDE);
		return;
	}

	if (b)
		this->clearStateBit(OSB_UNVISIBLE);
	else
		this->setStateBit(OSB_UNVISIBLE);

	// ֪ͨ�Ӷ���
	UISendMessage(this, UI_WM_VISIBLE_CHANGED, b?TRUE:FALSE, (LPARAM)this);

    UIMSG msg;
    msg.message = UI_WM_VISIBLE_CHANGED;
    msg.wParam = b?TRUE:FALSE;
    msg.lParam = (LPARAM)this;
	Message::ForwardMessageToChildObject(this, &msg);

	if (b != bOld)
	{
        // ȷʵ�����ڳ�ʼ���е���setvisible������ʱ��û�� m_pParent ���������SystemScrollBar
        RenderLayer2*  p = GetRenderLayer2();

		if (bUpdateLayout)
		{
			this->UpdateLayout(bRedraw);
            if (!bRedraw && p)
                p->SetDirty(true);
        }
        else if(bRedraw)
        {
            if (b)
                this->UpdateObject(); 
            else
                this->UpdateObjectBkgnd(true); 
        }
        else
        {
            if (p)
                p->SetDirty(true);
        }
	}
}

void Object::SetEnable(bool b, bool bUpdateNow, bool bNoitfy)
{
    int nObjType = this->GetObjectType();
	if (nObjType == OBJ_WINDOW )
	{
		::EnableWindow(((WindowBase*)this)->m_hWnd, b?TRUE:FALSE);
		return;
	}
	else if (nObjType == OBJ_HWNDHOST )
	{
		::EnableWindow(((HwndHost*)this)->m_hWnd, b?TRUE:FALSE);
		return;
	}

	bool bOld = IsEnable();
	int  nOldBits = m_nStateBit;
	if (b)
		this->clearStateBit(OSB_DISABLE);
	else
		this->setStateBit(OSB_DISABLE);
	
    if (bNoitfy && b != bOld)
	    UISendMessage(GetIMessage(), UI_WM_STATECHANGED2, (WPARAM)OSB_DISABLE);

	if (b != bOld)
    {
        if (bUpdateNow)
		    this->UpdateObject();
        else
            GetRenderLayer2()->SetDirty(true);

        // ���·��������Ϣ�������������λ�ڸÿؼ����棬����Ҫ���ÿؼ�����Ϊhover����������Ч
        POINT pt = {0, 0};
        ::GetCursorPos(&pt);
        HWND hWnd = GetHWND();
        ::MapWindowPoints(NULL, hWnd, &pt, 1);
        ::SendMessage(hWnd, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));

    }
}


bool Object::IsDefault()
{
	return this->testStateBit( OSB_DEFAULT );
}

// bool Object::IsReadonly()
// {
// 	return this->testStateBit(OSB_READONLY);
// }

bool Object::IsHover()
{
	return this->testStateBit(OSB_FORCEHOVER) || this->testStateBit(OSB_HOVER);
}

bool Object::IsPress()
{
	return this->testStateBit(OSB_FORCEPRESS) || this->testStateBit(OSB_PRESS);
}

bool Object::IsForceHover()
{
	return this->testStateBit(OSB_FORCEHOVER);
}

bool Object::IsForcePress()
{
	return this->testStateBit(OSB_FORCEPRESS);
}
bool  Object::IsSelected()
{
    return this->testStateBit(OSB_SELECTED);
}

void  Object::SetDefault(bool b, bool bNotify)
{
    UINT nOld = m_nStateBit;
	if (b)
		this->setStateBit(OSB_DEFAULT);
	else
        this->clearStateBit(OSB_DEFAULT);

    if (bNotify && m_nStateBit != nOld)
    {
        UIMSG  msg;
        msg.message = UI_WM_STATECHANGED2;
        msg.wParam = OSB_DEFAULT;
        msg.pMsgTo = GetIMessage();
        UISendMessage(&msg);
    }
}

void  Object::SetSelected(bool b, bool bNotify)
{
    UINT nOld = m_nStateBit;
    if (b)
        this->setStateBit(OSB_SELECTED);
    else
        this->clearStateBit(OSB_SELECTED);

    if (bNotify && m_nStateBit != nOld)
    {
        UIMSG  msg;
        msg.message = UI_WM_STATECHANGED2;
        msg.wParam = OSB_SELECTED;
        msg.pMsgTo = GetIMessage();
        UISendMessage(&msg);
    }
}

// void Object::SetReadonly(bool b)
// {
// 	if (b)
// 		this->setStateBit(OSB_READONLY);
// 	else
// 		this->clearStateBit(OSB_READONLY);
// }


void Object::SetForceHover(bool b, bool bNotify)
{
    UINT nOld = m_nStateBit;

	if (b)
		this->setStateBit(OSB_FORCEHOVER);
	else
		this->clearStateBit(OSB_FORCEHOVER);

    if (bNotify && m_nStateBit != nOld)
    {
        UIMSG  msg;
        msg.message = UI_WM_STATECHANGED2;
        msg.wParam = OSB_FORCEHOVER;
        msg.pMsgTo = GetIMessage();
        UISendMessage(&msg);
    }
}


void Object::SetForcePress(bool b, bool bNotify)
{
    UINT nOld = m_nStateBit;
	if (b)
		this->setStateBit(OSB_FORCEPRESS);
	else
		this->clearStateBit(OSB_FORCEPRESS);

    if (bNotify && m_nStateBit != nOld)
    {
        UIMSG  msg;
        msg.message = UI_WM_STATECHANGED2;
        msg.wParam = OSB_FORCEPRESS;
        msg.pMsgTo = GetIMessage();
        UISendMessage(&msg);
    }
}


void Object::SetHover(bool b, bool bNotify)
{
    UINT nOld = m_nStateBit;
	if (b)
		this->setStateBit(OSB_HOVER);
	else
		this->clearStateBit(OSB_HOVER);

    if (bNotify && m_nStateBit != nOld)
    {
        UIMSG  msg;
        msg.message = UI_WM_STATECHANGED2;
        msg.wParam = OSB_HOVER;
        msg.pMsgTo = GetIMessage();
        UISendMessage(&msg);
    }
}


void Object::SetPress(bool b, bool bNotify)
{
    UINT nOld = m_nStateBit;
	if (b)
		this->setStateBit(OSB_PRESS);
	else
		this->clearStateBit(OSB_PRESS);

    if (bNotify && m_nStateBit != nOld)
    {
        UIMSG  msg;
        msg.message = UI_WM_STATECHANGED2;
        msg.wParam = OSB_HOVER;
        msg.pMsgTo = GetIMessage();
        UISendMessage(&msg);
    }
}

void  Object::SetAsNcObject(bool b) 
{
    if (b)
        this->ModifyStyle(OBJECT_STYLE_NCOBJ, 0, 0); 
    else
        this->ModifyStyle(0, OBJECT_STYLE_NCOBJ, 0); 
}
bool  Object::IsNcObject()   
{
    return TestStyle(OBJECT_STYLE_NCOBJ);
}
bool  Object::IsRejectMouseMsgAll()  
{
    return TestStyle(OBJECT_STYLE_REJECT_MOUSE_MSG_ALL); 
}
bool  Object::IsRejectMouseMsgSelf() 
{
    return TestStyle(OBJECT_STYLE_REJECT_MOUSE_MSG_SELF); 
}


// ���ֶ�����һ�����󣨷Ǵ�xml�м���ʱ�����Ե��øú�����ȫĬ�����Եļ���)
void  Object::InitDefaultAttrib()
{
    IMapAttribute* pMapAttrib = NULL;
    UICreateIMapAttribute(&pMapAttrib);

    // ������ʽ
    if (m_pUIApplication)
    {
        ISkinRes* pSkinRes = m_pUIApplication->GetActiveSkinRes();
        if (pSkinRes)
        {
            IStyleManager* pStyleMgr = pSkinRes->GetStyleManager();
            if (pStyleMgr)
            {
                pStyleMgr->LoadStyle(GetObjectName(), NULL, m_strId.c_str(), pMapAttrib);
            }
        }
    }

	SERIALIZEDATA data = {0};
	data.pMapAttrib = pMapAttrib;
	data.nFlag = SERIALIZEFLAG_LOAD;
	UISendMessage(m_pIObject, UI_WM_SERIALIZE, (WPARAM)&data);
//    UISendMessage(m_pIObject, UI_WM_SETATTRIBUTE, (WPARAM)pMapAttrib, 0);

    SAFE_RELEASE(pMapAttrib);
}
void Object::SetUserData(LPVOID p)
{
	m_pUserData = p;
}


LPVOID Object::GetUserData()
{
	return m_pUserData;
}


void  Object::SetOutRef(void** ppOutRef)
{
	m_ppOutRef = ppOutRef;
}


void  Object::SetUIApplication(IUIApplication* p)
{ 
	m_pUIApplication = p;
}
IUIApplication*  Object::GetUIApplication() 
{ 
    return m_pUIApplication; 
}

HRGN  Object::GetRgn()
{
    return m_hRgn; 
}

int  Object::GetZOrder() 
{
    return m_nzOrder; 
}

void  Object::SetZorderDirect(int z) 
{
    m_nzOrder = z;
}

void Object::SetBkgndRender(IRenderBase* p)
{
	SAFE_RELEASE(m_pBkgndRender);
	m_pBkgndRender = p;

	if (m_pBkgndRender)
		m_pBkgndRender->AddRef();
}


void Object::SetForegndRender(IRenderBase* p)
{
	SAFE_RELEASE(m_pForegndRender);
	
	m_pForegndRender = p;
	if (m_pForegndRender)
		m_pForegndRender->AddRef();
}


void Object::SetTextRender(ITextRenderBase* p)
{
	SAFE_RELEASE(m_pTextRender);
	
	m_pTextRender = p;
	if (m_pTextRender)
		m_pTextRender->AddRef();
}

ITextRenderBase*  Object::GetTextRender()
{
    return m_pTextRender; 
}
IRenderBase*  Object::GetBkRender() 
{ 
    return m_pBkgndRender; 
}
IRenderBase*  Object::GetForeRender()
{
    return m_pForegndRender; 
}
//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                               ��Ϣӳ��                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


//
//   �����յ�WM_DESTROY��Ϣ֮�󣬴��ݸ����е��Ӷ���
//
//void Object::OnDestroy()
//{
//	Object*  pObj = NULL;
//	while( pObj = EnumChildObject(pObj) )
//	{
//		// ���Ӷ�����WM_DESTROY��Ϣ
//		UIMsg  cloneMsg = *(this->m_pCurMsg);
//
//		cloneMsg.pObjMsgTo = pObj;
//		::UISendMessage( &cloneMsg );
//	}
//}


//void Object::OnPaint( HDC hDC )


// �ڽ��յ�OnSize��Ϣ��ʱ����Ҫ�ı��Լ���pos,rectP,rectW��Ա��
// ��֪ͨ�Լ����Ӷ���ȥ����
//void Object::OnSize( UINT nType, int cx, int cy )
//{
//	this->UpdateRect();
//
//	Object*  pObj = NULL;
//	while( pObj = EnumChildObject(pObj) )
//	{
//		// ���Ӷ�����WM_ERASEBKGND��Ϣ
//		UIMsg  cloneMsg = *(this->m_pCurMsg);
//		cloneMsg.pObjMsgTo = pObj;
//		::UISendMessage( &cloneMsg );
//	}
//}

// ע���ú��������𽫹����Ķ������Ӧָ���ÿգ����粻�Ὣparent�е�child�е��Լ����
//     Ŀǰ�����ڻ���ʱ������

void Object::ClearMyTreeRelationOnly()
{
	this->m_pParent = NULL;
	this->m_pNext = NULL;
	this->m_pPrev = NULL;

	if (OBJ_CONTROL != this->GetObjectType()) // �ؼ�������xml��ֻռһ����㣬������� 
	{
		this->m_pChild = NULL;
		this->m_pNcChild = NULL;
	}

	// ��Ҫ���hook/notify����Ϊ����ʱ��Ҫ������Щ��Ϣ
	// 			pThis->ClearHook();
	// 			pThis->ClearNotify();
}


//
//  ���ݶ������ڵı��������������ı���ɫ������ͼƬ����
//
// Two colors provide good color visibility if the brightness difference and the color difference between the two colors are greater than a set range.
// Color brightness is determined by the following formula:
// ((Red value X 299) + (Green value X 587) + (Blue value X 114)) / 1000
// Color difference is determined by the following formula:
// (max (Red 1, Red 2) - min (Red 1, Red 2)) + (max (Green 1, Green 2) - min (Green 1, Green 2)) + (max (Blue 1, Blue 2) - min (Blue 1, Blue 2))
//  The rage for color brightness difference is 125. The range for color difference is 500.
// 
//  google text background color black white
//  http://juicystudio.com/services/aertcolourcontrast.php
//  http://www.colourlovers.com/web/blog/2010/03/11/calculating-color-contrast-for-legible-text
//
DWORD  Object::CalcContrastTextColor()
{
    WindowBase*  pWindowBase = GetWindowObject();
    if (NULL == pWindowBase)
        return 0;

    HDC hDC = pWindowBase->GetRenderLayer2()->GetRenderTarget()->GetHDC();
    if (NULL == hDC)
        return 0;

    HBITMAP  hBitmap = (HBITMAP)GetCurrentObject(hDC, OBJ_BITMAP);
    if (NULL == hBitmap)
        return 0;

    CRect rcLayer;
    GetRectInWindow(&rcLayer, true);

    Image  image;
    image.Attach(hBitmap, Image::DIBOR_TOPDOWN);
// #ifdef _DEBUG
//     image.Save(L"C:\\aa.png", Gdiplus::ImageFormatPNG);
// #endif
    DWORD  dwAverageColor = image.GetAverageColor(&rcLayer);
    image.Detach();

    // ��alphaֵӦ����
    int a = dwAverageColor >> 24;
    int r = GetRValue(dwAverageColor);
    int g = GetGValue(dwAverageColor);
    int b = GetBValue(dwAverageColor);

//     r = r * a >> 8;
//     g = g * a >> 8;
//     b = b * a >> 8;

    if (a)
    {
        r = (r<<8)/a;
        g = (g<<8)/a;
        b = (b<<8)/a;

        r = min(255, r);
        g = min(255, g);
        b = min(255, b);
    }

    int v = ((r * 299) + (g * 587) + (b * 114)) >> 10;
    if (v < 125)
        return 0xFFFFFFFF;
    else
        return 0xFF000000;
}


Object*  Object::GetObjectByPos(POINT* pt)
{
    WindowBase* pWindow = GetWindowObject();
    if (NULL == pWindow)
        return NULL;

    return pWindow->GetObjectByPos(this, pt, false);
}

bool  Object::SetMouseCapture(int nNotifyMsgId)
{
    WindowBase* pWindow = GetWindowObject();
    if (!pWindow)
        return false;

    pWindow->GetMouseMgr()->SetMouseCapture(m_pIObject, nNotifyMsgId);
    return true;
}
bool  Object::ReleaseMouseCapture()
{
    WindowBase* pWindow = GetWindowObject();
    if (!pWindow)
        return false;

    pWindow->GetMouseMgr()->ReleaseMouseCapture(m_pIObject);
    return true;
}
bool  Object::SetKeyboardCapture(int nNotifyMsgId)
{
    WindowBase* pWindow = GetWindowObject();
    if (!pWindow)
        return false;

    pWindow->GetMouseMgr()->SetKeyboardCapture(m_pIObject, nNotifyMsgId);
    return true;
}
bool  Object::ReleaseKeyboardCapture()
{
    WindowBase* pWindow = GetWindowObject();
    if (!pWindow)
        return false;

    pWindow->GetMouseMgr()->ReleaseKeyboardCapture(m_pIObject);
    return true;
}


bool  Object::CreateRenderLayer()
{
    if (m_pRenderLayer)
        return true;
	
	m_pRenderLayer = new RenderLayer2(this);
    return true;   
}

GRAPHICS_RENDER_LIBRARY_TYPE  Object::GetGraphicsRenderLibraryType()
{
    WindowRender*  pWindowRender = GetWindowRender();
    if (!pWindowRender)
    {
		return GRAPHICS_RENDER_LIBRARY_TYPE_GDI;
		//return (GRAPHICS_RENDER_LIBRARY_TYPE)UISendMessage(m_pIObject, UI_WM_GET_GRAPHICS_RENDER_LIBRARY_TYPE);
    }

    return pWindowRender->GetGraphicsRenderType();
}

}