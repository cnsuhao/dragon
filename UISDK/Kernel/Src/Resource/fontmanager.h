#pragma once
#include "UISDK\Kernel\Src\SkinParse\skinparseengine.h"
#include "UISDK\Kernel\Src\Util\DataStruct\list.h"
#include "fontres.h"

namespace UI
{

class  FontTagElementInfo
{
public:
	String  strId;  // ģ����
	IUIElement*  pXmlElement;  // xml���
};

class FontManager
{
public:
	FontManager(SkinRes* p);
	~FontManager(void);
   
    IFontManager*  GetIFontManager();
	FontRes&  GetFontRes();

public:
	void       Clear();
	int        GetFontCount();
	IFontResItem*  GetFontItemInfo(int nIndex);
	
    static HRESULT  UIParseFontTagCallback(IUIElement*, ISkinRes* pSkinRes);
private:
    HRESULT  ParseNewElement(IUIElement* pElem);
    void  OnNewChild(IUIElement* pElem);

private:
	// ���ݳ־ò�
    IFontManager*  m_pIFontManager;
   
	typedef  UIList<FontTagElementInfo>  _MyList;
	typedef  UIListItem<FontTagElementInfo>  _MyListItem;
	_MyList  m_listUIElement;

	// ��������
	FontRes   m_resFont;
	SkinRes*  m_pSkinRes;
};

}