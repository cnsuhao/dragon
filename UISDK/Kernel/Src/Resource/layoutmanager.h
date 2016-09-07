#pragma once
#include "UISDK\Kernel\Src\SkinParse\skinparseengine.h"
#include "UISDK\Kernel\Src\Resource\uiresource.h"
#include "UISDK\Kernel\Src\Util\DataStruct\list.h"

namespace UI
{
class Object;

//  延迟加载的布局配置
//  一个xml配置中，允许配置多个相关联的窗口
class LayoutConfigItem
{
public:
    void  SetPath(LPCTSTR szPath);
    LPCTSTR  GetPath();

    void  AddWindow(LPCTSTR szName);
    bool  FindWindow(LPCTSTR szName);

    UINT  GetCount() { return m_vecWindow.size(); }
    LPCTSTR  GetWindowName(UINT index)
    {
        if (index <= m_vecWindow.size())
            return m_vecWindow[index].c_str();
        return NULL;
    }
private:
	String  m_strPath;
    vector<String>  m_vecWindow;
};

class LayoutManager
{
public:
	LayoutManager(SkinRes*);
	~LayoutManager(void);
	
	LayoutRes&  GetLayoutRes();
    ILayoutManager*  GetILayoutManager();

	IObject*  LoadLayout(LPCTSTR szObjName, LPCTSTR szId);
	bool  FindWindowElement(LPCTSTR szTagName, LPCTSTR szId, IUIElement** ppElem);

	IObject*  ParseElement(IUIElement* pUIElement, IObject* pParent);
	void  ParseChildElement(IUIElement* pParentElement, IObject* pIObjParent);

	bool  ReLoadLayout(Object* pRootObj, list<Object*>& listAllChild );
	void  ReloadChildObjects(Object* pObjParent, IUIElement* pObjElement, list<Object*>& listAllChild);

    static HRESULT  UIParseLayoutTagCallback(IUIElement*, ISkinRes* pSkinRes);
	static HRESULT  UIParseLayoutConfigTagCallback(IUIElement*, ISkinRes* pSkinRes);
	
	// 编辑器专用函数
	bool  LoadWindowNodeList(ILayoutWindowNodeList** pp);

private:
    void  ParseNewElement(IUIElement* pElem);
	void  ParseLayoutConfigTag(IUIElement* pElem);
    bool  ParseLayoutConfigFileTag(IUIElement* pElem);

    bool  findWindowFromCache(LPCTSTR szTagName, LPCTSTR szId, IUIElement** ppElem);
	bool  loadWindowById(LPCTSTR szTagName, LPCTSTR szId, IUIElement** ppElem);
	bool  loadWindowFromLayoutConfig(LPCTSTR szTagName, LPCTSTR szId, IUIElement** ppElem);
	bool  testUIElement(IUIElement* pParentElem, LPCTSTR szTagName, LPCTSTR szId);

private:
    ILayoutManager*  m_pILayoutManager;
    UIList<IUIElement*>  m_listUIElement;  // 已加载的布局项
	UIList<LayoutConfigItem*>  m_listLayoutConfig;  // 还未加载的布局项

	// 对象属性
	LayoutRes        m_resLayout;
	SkinRes*         m_pSkinRes;
};

}