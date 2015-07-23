#pragma once
#include "UISDK\Kernel\Src\Resource\uiresource.h"
#include "UISDK\Kernel\Inc\Interface\iskindatasource.h"
#include "UISDK\Kernel\Src\Resource\imagemanager.h"
#include "UISDK\Kernel\Src\Resource\colormanager.h"
#include "UISDK\Kernel\Src\Resource\stylemanager.h"
#include "UISDK\Kernel\Src\Resource\layoutmanager.h"
#include "UISDK\Kernel\Src\Resource\fontmanager.h"

namespace UI
{
class UIApplication;
class Object;
class SkinManager;
interface  IUIDocument;
interface  ISkinDataSource;

enum SKIN_RES_LOAD_STATE
{
    SKIN_RES_LOAD_STATE_NONE,     // 仅new SkinRes
    SKIN_RES_LOAD_STATE_LOADED,   // 已加载
    
};

// 皮肤配置文件信息
struct  UISkinResDocInfo
{
    IUIDocument*  m_pDoc;   // doc指针
    String  m_strDocPath;   // doc路径
};

//
//	一款皮肤资源管理
//	
class SkinRes
{
public:
    SkinRes(SkinManager& o);
	~SkinRes();

	bool  Load();
	bool  Unload();
	bool  Save();
	LPCTSTR  GetName();
	LPCTSTR  GetPath();
	void  SetParam(SKIN_PACKET_TYPE eType, LPCTSTR szName, LPCTSTR szPath);
	bool  ReLoadLayout(Object* pRootObj, list<Object*>& listAllChild);
	bool  ChangeSkinHLS(short h, short l, short s, int nFlag);

	ISkinRes*        GetISkinRes();
    IUIApplication*  GetUIApplication();
    ISkinManager*    GetISkinManager();

    IImageManager*   GetIImageManager()   { return m_mgrImage.GetIImageManager(); }
    IColorManager*   GetIColorManager()   { return m_mgrColor.GetIColorManager(); }
    IFontManager*    GetIFontManager()    { return m_mgrFont.GetIFontManager(); }
    IStyleManager*   GetIStyleManager()   { return m_mgrStyle.GetIStyleManager(); }
    ILayoutManager*  GetILayoutManager()  { return m_mgrLayout.GetILayoutManager(); }
    IImageRes*       GetIImageRes()       { return m_mgrImage.GetImageRes().GetIImageRes(); }
    IFontRes*        GetIFontRes()        { return m_mgrFont.GetFontRes().GetIFontRes(); }
    IColorRes*       GetIColorRes()       { return m_mgrColor.GetColorRes().GetIColorRes(); }
    IStyleRes*       GetIStyleRes()       { return m_mgrStyle.GetStyleRes().GetIStyleRes(); }

	SkinManager&     GetSkinMgr()         { return m_mgrSkinRef; } //  内部调用
	ImageManager&    GetImageManager()    { return m_mgrImage;  }
	ColorManager&    GetColorManager()    { return m_mgrColor;  }
	FontManager&     GetFontManager()     { return m_mgrFont;   }
	StyleManager&    GetStyleManager()    { return m_mgrStyle;  }
	LayoutManager&   GetLayoutManager()   { return m_mgrLayout; }
	SKIN_HLS_INFO*   GetHLSInfo()         { return m_pHLSInfo; }
	ISkinDataSource*  GetDataSource()     { return m_pDataSource; }

    UINT  GetImageCount()    { return m_mgrImage.GetImageCount(); }
    UINT  GetColorCount()    { return m_mgrColor.GetColorCount(); }
    UINT  GetFontCount()     { return m_mgrFont.GetFontCount(); }
    UINT  GetStyleCount()    { return m_mgrStyle.GetStyleCount(); }
    UINT  GetXmlDocCount()   { return m_listDoc.size(); }

	IImageResItem*  GetImageItemInfo(UINT nIndex);
	IColorResItem*  GetColorItemInfo(UINT nIndex);
	IFontResItem*  GetFontItemInfo(UINT nIndex);
	LPCTSTR  GetXmlDoc(UINT nIndex, IUIDocument** pp);
	bool  GetXmlDocByName(LPCTSTR szName, IUIDocument** pp);
    LPCTSTR  GetXmlDocName(IUIDocument* p);

	void  OnNewUIDocument(IUIDocument* pDoc, LPCTSTR szPath);    

private:
    ISkinRes*  m_pISkinRes;
	SkinManager&  m_mgrSkinRef;

	String  m_strSkinResName;
    ISkinDataSource*  m_pDataSource;
	ImageManager   m_mgrImage;
	ColorManager   m_mgrColor;
	FontManager    m_mgrFont;
	StyleManager   m_mgrStyle;
	LayoutManager  m_mgrLayout;

	SKIN_RES_LOAD_STATE  m_eLoadState;
    SKIN_HLS_INFO*  m_pHLSInfo;

    typedef vector<UISkinResDocInfo> _DocList;
    _DocList  m_listDoc;   // 各个xml文档指针
};

}