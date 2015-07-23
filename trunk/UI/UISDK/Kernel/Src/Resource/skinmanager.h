#ifndef SKINMANAGER_H_FE1D24AF_CC02_4cd2_8500_5026F671D4CC
#define SKINMANAGER_H_FE1D24AF_CC02_4cd2_8500_5026F671D4CC
#include "UISDK\Kernel\Src\Resource\uiresource.h"

/*
 *  2015.7.19 
 *
 *  1. ����������TTPlayer��ʽ�Ķ�Ƥ�����ܣ�ͬ��ά�����е�Ƥ���ɱ�̫��
 *     ����������ĸ��£����õķ�ʽ��һ��Ƥ������������ļ���
 *
 *  2. ��չ֧�ֲ��ģʽ�Ķ�Ƥ������ÿ�����������Լ���Ƥ������������Ҳ
 *     �����м̳й�ϵ��
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
	
	SkinRes*  m_pCurActiveSkinRes;  // ��ǰ����ʹ�õ�Ƥ��
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

	// ���ݳ־ò�
//  String  m_strSkinDir;
	bool  m_bDirty;

	// �־������Զ���
	SkinBuilderRes    m_SkinBuilderRes;

	// ����ʱ����
	vector<SkinRes*>  m_vSkinRes;           // ���е�Ƥ���б�

	IUIApplication*    m_pUIApplication;
};
}

#endif  // SKINMANAGER_H_FE1D24AF_CC02_4cd2_8500_5026F671D4CC