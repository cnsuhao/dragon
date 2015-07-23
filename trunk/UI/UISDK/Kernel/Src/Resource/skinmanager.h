#ifndef SKINMANAGER_H_FE1D24AF_CC02_4cd2_8500_5026F671D4CC
#define SKINMANAGER_H_FE1D24AF_CC02_4cd2_8500_5026F671D4CC
#include "UISDK\Kernel\Src\Resource\uiresource.h"

/*
 *  2015.7.19 
 *
 *  1. 废弃了类似TTPlayer形式的多皮肤功能，同步维护所有的皮肤成本太大，
 *     不利于软件的更新，更好的方式是一套皮肤，多个配置文件。
 *
 *  2. 扩展支持插件模式的多皮肤包，每个插件允许的自己的皮肤包，包肤包也
 *     可以有继承关系。
 */
namespace UI
{
class UIApplication;
class IUISkinDescParse;
enum  SKIN_PACKET_TYPE;

class SkinManager
{
public:
	SkinManager(); 
	~SkinManager(); 
    void  Destroy();

public:
	/*
	void  SetSkinDirection(LPCTSTR  szDir);
	void  GetSkinDirection(TCHAR*  szOut);
	ISkinRes*  AddSkin(LPCTSTR  szPath);
	HRESULT  ChangeSkin(ISkinRes*  pSkinRes, bool bSync=true); 
	HRESULT  SetActiveSkin(ISkinRes* pSkin);
	SkinRes*  GetSkinResByIndex(long lIndex);
	int  GetSkinResIndex(SkinRes* pSkinRes);
	SkinRes*  OnFindSkinInSkinDir(SKIN_PACKET_TYPE, LPCTSTR szPath, LPCTSTR szPath);
	
	SkinRes*  m_pCurActiveSkinRes;  // 当前正在使用的皮肤
	*/
	
	ISkinManager*  GetISkinManager();
	IUIApplication*  GetUIApplication();
	void  SetUIApplication(IUIApplication* pUIApp);

	SkinRes*  LoadSkinRes(LPCTSTR szPath);
	SkinRes*  GetDefaultSkinRes();
	SkinRes*  GetSkinResByName(LPCTSTR szName);

	bool  Save(SkinRes* pSkinRes=NULL);
    void  ChangeSkinHLS( short h, short l, short s, int nFlag );

private:
    ISkinManager*  m_pISkinManager;

	// 数据持久层
//  String  m_strSkinDir;
	bool  m_bDirty;

	// 持久性属性对象
	SkinBuilderRes    m_SkinBuilderRes;

	// 运行时数据
	vector<SkinRes*>  m_vSkinRes;           // 所有的皮肤列表

	IUIApplication*    m_pUIApplication;
};
}

#endif  // SKINMANAGER_H_FE1D24AF_CC02_4cd2_8500_5026F671D4CC