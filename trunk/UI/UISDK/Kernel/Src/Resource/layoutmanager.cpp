#include "StdAfx.h"
#include "layoutmanager.h"

#include "UISDK\Kernel\Src\Resource\uiresource.h"
#include "UISDK\Kernel\Src\Resource\skinres.h"
#include "UISDK\Kernel\Inc\Interface\ixmlwrap.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"
#include "UISDK\Kernel\Src\Resource\stylemanager.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Inc\Interface\iuieditor.h"
#include "UISDK\Kernel\Inc\Interface\iuires.h"

LayoutManager::LayoutManager(SkinRes* p)
{
	m_pSkinRes = p;
    m_pILayoutManager = NULL;
}

LayoutManager::~LayoutManager(void)
{
	m_listUIElement.RELEASE_ALL();
    m_listLayoutConfig.DELETE_ALL();
    SAFE_DELETE(m_pILayoutManager);
}

ILayoutManager*  LayoutManager::GetILayoutManager()
{
    if (NULL == m_pILayoutManager)
        m_pILayoutManager = new ILayoutManager(this);

    return m_pILayoutManager;
}


// ��0��ʼ������һ��������
// ����szTagNameΪNULL����ʱֻƥ��szId
IObject*  LayoutManager::LoadLayout(LPCTSTR szWndName, LPCTSTR szWndId)
{
	IUIElement*  pObjUIElement = NULL;
	this->FindWindowElement(szWndName, szWndId, &pObjUIElement);

	if (!pObjUIElement)
	{
		UI_LOG_FATAL(_T("δ�ҵ�Ҫ���صĶ���name=%s, id=%s"), szWndName, szWndId);
		return NULL;
	}    
	IObject* pObject = this->ParseElement(pObjUIElement, NULL);
	SAFE_RELEASE(pObjUIElement);

	return pObject;
}

// �ⲿֻ֪��һ������id�����õ��ô��ڵ�tagname�õ���uiapplication->createinstancebyname
// bool  LayoutManager::GetWindowNameById(LPCTSTR szId, BSTR* bstrName)
// {
//     IUIElement* pElement = NULL;
//     if (!FindWindowElement(NULL, szId, &pElement))
//         return false;
// 
//     pElement->GetTagName(bstrName);
//     return true;
// }

// 1. �ӻ���m_listUIElement�в��ҽ�㣬ֱ��������skin.xml��
// 2. ��szId.xml��Ƥ����Ŀ¼�в��ң��������maindlg����ƥ��maindlg.xml
// 3. ��layoutconfig�е����ü��أ�����<layoutconfig> <item id="" path=""/> </layoutconfig>
bool  LayoutManager::FindWindowElement(LPCTSTR szTagName, LPCTSTR szId, IUIElement** ppElem)
{
	if (!m_pSkinRes || !szId)
		return false;

	// 1. �ӻ���m_listUIElement�в��ҽ��
	if (findWindowFromCache(szTagName, szId, ppElem))	
		return true;

    // UIEditor����֧��������ʽ������ȥ����Ŀ¼���Ȳ�֧����
#if 0
	// 2. ��szId.xml��Ƥ����Ŀ¼�в���
	if (loadWindowById(szTagName, szId, ppElem))
		return true;
#endif

	// 3. �������м���ָ������id
	if (loadWindowFromLayoutConfig(szTagName, szId, ppElem))
		return true;

	return false;
}

bool  LayoutManager::loadWindowById(LPCTSTR szTagName, LPCTSTR szId, IUIElement** ppElem)
{
	ISkinDataSource* pDataSource = m_pSkinRes->GetDataSource();
	if (!pDataSource)
		return false;

	String  strFile(szId);
	strFile.append(TEXT(".") XML_XML);

	SkinParseEngine engine(m_pSkinRes);
	if (engine.Parse(pDataSource, strFile.c_str()))
	{
		// TODO: parseֻ�ǽ��ļ���ӵ��б��У��������´��б����õ�xml���
		// �д��Ż�
		if (findWindowFromCache(szTagName, szId, ppElem))	
			return true;
	}

	return false;
}

// ��<layout>�������ָ����tag + id������IUIElement
bool  LayoutManager::findWindowFromCache(LPCTSTR szTagName, LPCTSTR szId, IUIElement** ppElem)
{
    // ����szTagNameΪ�գ�ֻƥ��szId
    if (!szId || !ppElem)
        return false;

    *ppElem = NULL;

    UIListItem<IUIElement*>* pUIElementItem = m_listUIElement.GetFirst();
    while (pUIElementItem)
    {
        IUIChildNodeEnum*  pEnum = (**pUIElementItem)->EnumChild();
        if (!pEnum)
            return false;

        for (IUIElement* pChildElem = NULL; pChildElem = pEnum->NextElement(); pChildElem->Release())
        {
			if (testUIElement(pChildElem, szTagName, szId))
			{
				*ppElem = pChildElem;
				SAFE_RELEASE(pEnum);
				return true;
			}
        }

        SAFE_RELEASE(pEnum);
        pUIElementItem = pUIElementItem->GetNext();
    }

    return false;
}

bool  LayoutManager::loadWindowFromLayoutConfig(LPCTSTR szTagName, LPCTSTR szId, IUIElement** ppElem)
{
	if (!szId)
		return false;

    ISkinDataSource* pDataSource = m_pSkinRes->GetDataSource();
    if (!pDataSource)
        return false;

	UIListItem<LayoutConfigItem*>* pListItem = m_listLayoutConfig.GetFirst();
	while (pListItem)
	{
        LayoutConfigItem* pConfigItem = **pListItem;
		if (pConfigItem->FindWindow(szId))
		{
			SkinParseEngine engine(m_pSkinRes);
			if (engine.Parse(pDataSource, pConfigItem->GetPath()))
			{
				// TODO: parseֻ�ǽ��ļ���ӵ��б��У��������´��б����õ�xml���
				// �д��Ż�
				if (findWindowFromCache(szTagName, szId, ppElem))
				{
					m_listLayoutConfig.Remove(pConfigItem);
                    SAFE_DELETE(pConfigItem);
					return true;
				}
			}
		}
		pListItem = pListItem->GetNext();
	}
	return false;
}

// ����һ������tag��id  
// ���Բ�ָ��szTagName
bool  LayoutManager::testUIElement(IUIElement* pElem, LPCTSTR szTagName, LPCTSTR szId)
{
	if (!pElem || !szId)
		return false;

	CComBSTR  bstrTagName;
	CComBSTR  bstrId;

	pElem->GetTagName(&bstrTagName);
	if (szTagName && bstrTagName != szTagName)
		return false;

	pElem->GetAttrib(XML_ID, &bstrId);
	if (bstrId != szId)
		return false;

	return true;
}


// �ݹ飬���������Ӷ����Ӷ��������
void  LayoutManager::ParseChildElement(IUIElement* pParentElement, IObject* pIObjParent)
{
	if (!pIObjParent || !pParentElement)
		return ;

	IUIElement*  pChildElem = NULL;
	IUIChildNodeEnum*  pEnum = pParentElement->EnumChild();

	while (pChildElem = pEnum->NextElement())
	{
		ParseElement(pChildElem, pIObjParent);        
		SAFE_RELEASE(pChildElem);
	}
	SAFE_RELEASE(pEnum);
}

bool IsTrue(LPCTSTR szValue)
{
	if (0 == _tcscmp(szValue, XML_BOOL_VALUE_TRUE)  || 
		0 == _tcscmp(szValue, XML_BOOL_VALUE_1)     ||
		0 == _tcscmp(szValue, XML_BOOL_VALUE_YES))
	{
		return true;
	}
	return false;
}

// �õ�һ��Ԫ��ָ�룬������Ԫ�ؼ����ӽ�㣬�����ض���ָ��
IObject*  LayoutManager::ParseElement(IUIElement* pUIElement, IObject* pParent)
{
    IUIApplication*  pUIApp = m_pSkinRes->GetUIApplication();

    Object*  pObj = NULL;
    CComBSTR  bstrTagName;
    pUIElement->GetTagName(&bstrTagName);

    // ����ǩ��ת��Ϊ��
	PARSE_CONTROL_RETURN eParseRet = ParseControl_CreateObject;

    IObject* pIObject = pUIApp->CreateInstanceByName(bstrTagName);
    if (NULL == pIObject)
    {
        // ����Ѱ�Ҹ�Tag�Ƿ�ע����
        pfnParseControlTag  func = NULL;
        pUIApp->GetControlTagParseFunc(bstrTagName, &func);
        if (func)
        {
            eParseRet = func(pUIElement, pUIApp, pParent, &pIObject);
			if (eParseRet == ParseControl_Failed)
            {
                UI_LOG_ERROR(_T("Parse Object:  %s Failed."), (BSTR)bstrTagName);    
				return NULL;
            }
        }
        else
        {
            UI_LOG_ERROR(_T("CreateObject Failed. name=%s"), (BSTR)bstrTagName);
			return NULL;
        }
    }

    if (NULL == pIObject)
        return NULL;

    pObj = pIObject->GetImpl();

	if (eParseRet < ParseControl_LoadObject)
	{
		// ��AddChild֮ǰ��ȡ��z order���Ա�����
// 		CComBSTR  bstrZ;
// 		if (pUIElement->GetAttrib(XML_ZORDER, &bstrZ))
// 			pObj->SetZorderDirect(_wtoi(bstrZ));
//
// 		if (pParent)
// 		{
// 			// ����nc child
// 			CComBSTR  bNcChild;
// 			pUIElement->GetAttrib(XML_ISNCCHILD, &bNcChild);
// 
// 			if (bNcChild && IsTrue(bNcChild))
// 				pParent->AddNcChild(pIObject);
// 			else
// 				pParent->AddChild(pIObject);
// 		}

		// �Լ�������
		pIObject->LoadAttributeFromXml(pUIElement, false);
		
		// �ȼ������ԣ���ȡ��zorder��ncchild����֮����addchild
		if (pParent)
		{
			ObjStyle s = {0};
			s.ncobject = 1;
			if (pObj->TestObjectStyle(s))
			{
				pParent->GetImpl()->AddNcChild(pObj);
			}
			else
			{
				pParent->GetImpl()->AddChild(pObj);
			}
		}
	}

	if (eParseRet < ParseControl_LoadDescendants)
	{
		// �����Ӷ���
		ParseChildElement(pUIElement, pIObject);
	}

    // Ĭ�Ͻ��ÿؼ������һ��notify objectΪ���ڶ��󣬲���msgmapid����Ϊ0
    WindowBase* pWindowBase = pObj->GetWindowObject();
    if (pObj != pWindowBase && pWindowBase)
        pObj->SetNotify(pWindowBase->GetIMessage(), 0);

    return pIObject;
}



//	����ָ���Ķ���Ĳ������ݣ���xml�н�����pRootObj��object name��id������ԭ�ж���������ԺͲ���
bool  LayoutManager::ReLoadLayout(Object* pRootObj, list<Object*>& listAllChild )
{
    IUIElement*  pObjUIElement = NULL;  // <<-- �ҵ��ö���󱣴��ڸñ�����

    LPCTSTR  szObjName = pRootObj->GetObjectName();
    this->FindWindowElement(pRootObj->GetObjectName(), pRootObj->GetId(), &pObjUIElement);

    if (NULL == pObjUIElement)
    {
        UI_LOG_FATAL(_T("δ�ҵ�Ҫ���صĶ���name=%s, id=%s"),  szObjName, pRootObj->GetId());
        return false;
    }    

	pRootObj->LoadAttributeFromXml(pObjUIElement, true);
    ReloadChildObjects(pRootObj, pObjUIElement, listAllChild);

    SAFE_RELEASE(pObjUIElement);
    return true;
}

void  LayoutManager::ReloadChildObjects(Object* pObjParent, IUIElement* pObjElement, list<Object*>& listAllChild)
{
    IUIApplication*  pUIApp = m_pSkinRes->GetUIApplication();
    IUIChildNodeEnum*  pEnum = pObjElement->EnumChild();
    IUIElement*  pChildElem = NULL;


    // ���������Ӷ���
    while (pChildElem = pEnum->NextElement())
    {
        CComBSTR  bstrChildElemTagName;
        CComBSTR  bstrId;

        Object*  pObj = NULL;
        pChildElem->GetTagName(&bstrChildElemTagName);
        if (0 == bstrChildElemTagName.Length())
        {
            UI_LOG_WARN( _T("xml invalid tag name."));
            SAFE_RELEASE(pChildElem);
            continue;
        }

        pChildElem->GetAttrib(XML_ID, &bstrId);
        if (0 != bstrId.Length())
        {
            // ���� tagName + id ��listAllChild�в��Ҹö���
            list<Object*>::iterator  iter = listAllChild.begin();
            list<Object*>::iterator  iterEnd = listAllChild.end();

            for (; iter != iterEnd; iter++)
            {
                Object* pTempObj = *iter;
                if (0 == _tcscmp((BSTR)bstrChildElemTagName, pTempObj->GetObjectName()) &&
                    0 == _tcscmp((BSTR)bstrId, pTempObj->GetId()))
                {
                    pObj = pTempObj;
                    break;
                }
            }
        }

        if (NULL == pObj)
        {
            // û���ҵ�
            // ֱ������������
            // ����ǩ��ת��Ϊ��

            IObject* pIObject = pUIApp->CreateInstanceByName(bstrChildElemTagName);
            if (NULL == pIObject)
            {
                UI_LOG_ERROR(_T("CreateObject Failed. name=%s"), (BSTR)bstrChildElemTagName);
                SAFE_RELEASE(pChildElem);
                continue;
            }
            pObj = pIObject->GetImpl();

            // ����
            pObjParent->AddChild(pObj);

            // ��������
			pObj->LoadAttributeFromXml(pChildElem, false);

            // Ĭ�Ͻ��ÿؼ������һ��notify objectΪ���ڶ��󣬲���msgmapid����Ϊ0
            WindowBase* pWindowBase = pObj->GetWindowObject();
            if (pWindowBase)
            {
                pObj->SetNotify(pWindowBase->GetIMessage(), 0);
            }

            // �ݹ飬��������Ӷ�����Ӷ���
            this->ReloadChildObjects(pObj, pChildElem, listAllChild);
        }
        else
        { 
            // ����
            pObjParent->AddChild(pObj);

            // ��������
			pObj->LoadAttributeFromXml(pChildElem, true);

            // �ݹ飬��������Ӷ�����Ӷ���
            this->ReloadChildObjects(pObj, pChildElem, listAllChild);

            // ���б����Ƴ�
            list<Object*>::iterator iter = listAllChild.begin();
            list<Object*>::iterator iterEnd = listAllChild.end();
            for (; iter != iterEnd; iter++)
            {
                if (pObj == (*iter))
                {
                    listAllChild.erase(iter);
                    break;
                }
            }
        }

        SAFE_RELEASE(pChildElem);
    }
    SAFE_RELEASE(pEnum);
}

LayoutRes&  LayoutManager::GetLayoutRes()
{
	return m_resLayout;
}

//////////////////////////////////////////////////////////////////////////

HRESULT  LayoutManager::UIParseLayoutTagCallback(IUIElement* pElem, ISkinRes* pSkinRes)
{
    ILayoutManager*  pLayoutMgr = pSkinRes->GetLayoutManager();
    if (NULL == pLayoutMgr)
        return E_FAIL;

    pLayoutMgr->GetImpl()->ParseNewElement(pElem);
	return true;
}
HRESULT  LayoutManager::UIParseLayoutConfigTagCallback(IUIElement* pElem, ISkinRes* pSkinRes)
{
	ILayoutManager*  pLayoutMgr = pSkinRes->GetLayoutManager();
	if (NULL == pLayoutMgr)
		return E_FAIL;

	pLayoutMgr->GetImpl()->ParseLayoutConfigTag(pElem);
	return S_OK;
}

void  LayoutManager::ParseNewElement(IUIElement* pElem)
{
    pElem->AddRef();
    m_listUIElement.Add(pElem);

    // �ɴ����Լ�����LoadLayout�ٽ���
}

//
// ����
//
//  <layoutconfig>
//      <file path="login_dlg.xml">
//        <window name="login.dlg"/>
//        <window name="login.userlist"/>
//      </file>
//  </layoutconfig>
//
void  LayoutManager::ParseLayoutConfigTag(IUIElement* pElem)
{
	if (!pElem)
		return;

	// ��������Ԫ��
	IUIChildNodeEnum* pEnum = pElem->EnumChild();
	if (!pEnum)
        return;

    CComBSTR bstrTagName;

    IUIElement* pChildElement = NULL;
    while (pChildElement = pEnum->NextElement())
    {
        pChildElement->GetTagName(&bstrTagName);
        if (bstrTagName != XML_FILE)
        {
            SAFE_RELEASE(pChildElement);
            continue;
        }
        
        ParseLayoutConfigFileTag(pChildElement);
        SAFE_RELEASE(pChildElement);
    }

    SAFE_RELEASE(pEnum);
}

bool  LayoutManager::ParseLayoutConfigFileTag(IUIElement* pElem)
{
    CComBSTR bstrPath;
    if (!pElem->GetAttrib(XML_PATH, &bstrPath))
        return false;

    IUIChildNodeEnum* pEnum = pElem->EnumChild();
    if (!pEnum)
        return false;

    LayoutConfigItem*  pFileitem = new LayoutConfigItem;
    pFileitem->SetPath(bstrPath);

    CComBSTR bstrTagName;
    CComBSTR bstrWindowName;

    IUIElement* pChildElement = NULL;
    while (pChildElement = pEnum->NextElement())
    {
        pChildElement->GetTagName(&bstrTagName);
        if (bstrTagName == XML_WINDOW)
        {
            if (pChildElement->GetAttrib(XML_NAME, &bstrWindowName))
            {
                pFileitem->AddWindow(bstrWindowName);
            }
        }

        SAFE_RELEASE(pChildElement);
    }

    SAFE_RELEASE(pEnum);

    m_listLayoutConfig.Add(pFileitem);
    return true;
}

//////////////////////////////////////////////////////////////////////////

void  LayoutConfigItem::AddWindow(LPCTSTR szName)
{
    if (szName)
    {
        m_vecWindow.push_back(String(szName));
    }
}
bool  LayoutConfigItem::FindWindow(LPCTSTR szName)
{
    if (!szName)
        return false;

    vector<String>::iterator iter = std::find(m_vecWindow.begin(), m_vecWindow.end(), String(szName));    
    if (iter == m_vecWindow.end())
        return false;

    return true;
}

void  LayoutConfigItem::SetPath(LPCTSTR szPath)
{
    SETSTRING(m_strPath, szPath);
}
LPCTSTR  LayoutConfigItem::GetPath()
{
    return m_strPath.c_str();
}

//////////////////////////////////////////////////////////////////////////


class LayoutWindowNodeList : public ILayoutWindowNodeList
{
public:
    static LayoutWindowNodeList* Create()
    {
        return new LayoutWindowNodeList;
    }
    virtual void  Release() override
    {
        delete this;
    }
    virtual UINT  GetCount() override
    {
        return (long)m_array.size();
    }
    virtual LPCTSTR  GetWindowName(UINT index)
    {
        if (index >= m_array.size())
            return NULL;

        return m_array[index].strName.c_str();
    }
    virtual LPCTSTR  GetWindowId(UINT index)
    {
        if (index >= m_array.size())
            return NULL;

        return m_array[index].strId.c_str();
    }
    virtual LPCTSTR  GetWindowPath(UINT index)
    {
        if (index >= m_array.size())
            return NULL;

        return m_array[index].strPath.c_str();
    }
public:
    void  AddWindow(LPCTSTR szPath, LPCTSTR szName, LPCTSTR szId)
    {
        if (szId && szPath)
        {
            WindowInfo info;
            SETSTRING(info.strName, szName);
            SETSTRING(info.strId, szId);
            SETSTRING(info.strPath, szPath);
            m_array.push_back(info);
        }
    }

private:
    struct  WindowInfo
    {
        String  strPath;
        String  strName;
        String  strId;
    };
    vector<WindowInfo> m_array;
};

bool  LayoutManager::LoadWindowNodeList(ILayoutWindowNodeList** ppInfo)
{
    if (!ppInfo)
        return false;

    LayoutWindowNodeList* pResult = LayoutWindowNodeList::Create();
    *ppInfo = pResult;

    // �ӻ����ж�ȡ
    UIListItem<IUIElement*>* pUIElementItem = m_listUIElement.GetFirst();
    while (pUIElementItem)
    {
        IUIElement* pLayoutElem = **pUIElementItem;

        // ��ȡ�ý�����ڵ�xml����
        LPCTSTR szDocName = NULL;
        IUIDocument* pDoc = NULL;
        pLayoutElem->GetDocument(&pDoc);
        if (pDoc)
        {
            szDocName = m_pSkinRes->GetXmlDocName(pDoc);
            pDoc->Release();
        }

        //  ������<layout>����Ĵ��ڽ��
        IUIChildNodeEnum*  pEnum = pLayoutElem->EnumChild();
        if (pEnum)
        {
            IUIElement*  pChildElem = NULL;
            CComBSTR  bstrTagName;
            CComBSTR  bstrId;

            while (pChildElem = pEnum->NextElement())
            {
                pChildElem->GetAttrib(XML_ID, &bstrId);
                pChildElem->GetTagName(&bstrTagName);
                pResult->AddWindow(szDocName, bstrTagName, bstrId);

                SAFE_RELEASE(pChildElem);
            }
            SAFE_RELEASE(pEnum);
        }
        pUIElementItem = pUIElementItem->GetNext();
    }

    // 2. ��layoutconfig�ж�ȡ
    UIListItem<LayoutConfigItem*>* pListItem = m_listLayoutConfig.GetFirst();
    while (pListItem)
    {
        LayoutConfigItem* pConfigItem = **pListItem;

        UINT nWindowCount = pConfigItem->GetCount();
        for (UINT i = 0; i < nWindowCount; i++)
        {
            pResult->AddWindow(pConfigItem->GetPath(), NULL, pConfigItem->GetWindowName(i));
        }
        pListItem = pListItem->GetNext();
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////