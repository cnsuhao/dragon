#ifndef SKINMANAGER_H_FE1D24AF_CC02_4cd2_8500_5026F671D4CC
#define SKINMANAGER_H_FE1D24AF_CC02_4cd2_8500_5026F671D4CC

#include "UISDK\Kernel\Src\Resource\uiresource.h"

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
    void  SetSkinDirection(LPCTSTR  szDir);
    void  GetSkinDirection(TCHAR*  szOut);
    ISkinRes*  AddSkin(LPCTSTR  szName);

    SkinRes*  OnFindSkinInSkinDir(SKIN_PACKET_TYPE, LPCTSTR szName, LPCTSTR szPath);
	
	UINT  GetSkinCount();
    SkinRes*  GetActiveSkin();

	bool  Save(SkinRes* pSkinRes=NULL);
    ISkinManager*  GetISkinManager();
    IUIApplication*  GetUIApplication();

    HRESULT  ChangeSkin(ISkinRes*  pSkinRes, bool bSync=true); 
    HRESULT  ChangeSkinHLS( short h, short l, short s, int nFlag );
    HRESULT  SetActiveSkin(ISkinRes* pSkin);
    SkinRes*  GetSkinResByIndex(long lIndex);

	void  SetUIApplication(IUIApplication* pUIApp);
	bool  LoadSkin(LPCTSTR szName);
	SkinRes*  AddSkinRes(const String&  strSkinName, const String&  strSkinXmlFullPath);

private:
	int  GetSkinResIndex(SkinRes* pSkinRes);
    SkinRes*  GetSkinResByName(LPCTSTR szName);

private:
    ISkinManager*  m_pISkinManager;

	// 数据持久层
    String  m_strSkinDir;
	bool  m_bDirty;

	// 持久性属性对象
	SkinBuilderRes    m_SkinBuilderRes;

	// 运行时数据
	vector<SkinRes*>  m_vSkinRes;           // 所有的皮肤列表
	SkinRes*          m_pCurActiveSkinRes;  // 当前正在使用的皮肤

	IUIApplication*    m_pUIApplication;
};
}

#endif  // SKINMANAGER_H_FE1D24AF_CC02_4cd2_8500_5026F671D4CC