#pragma once

#include "UISDK\Kernel\Src\SkinParse\skinparseengine.h"
#include "UISDK\Kernel\Src\Util\DataStruct\list.h"
#include "colorres.h"

namespace UI
{

class  ColorTagElementInfo
{
public:
	String  strId;  // 模块名
	IUIElement*  pXmlElement;  // xml结点
};

class ColorManager
{
public:
    ColorManager(SkinRes* pSkinRes);
    ~ColorManager(void);

    ColorRes&  GetColorRes();
    HRESULT  ParseNewElement(IUIElement* pElem);
    IColorManager*  GetIColorManager();

public:
    void  Clear();
    int   GetColorCount();
    IColorResItem*  GetColorItemInfo(int nIndex);

    bool  ChangeSkinHLS(short h, short l, short s, int nFlag);

    static HRESULT  UIParseColorTagCallback(IUIElement*, ISkinRes* pSkinRes);

private:
    void  OnNewChild(IUIElement* pElem);

private:
    IColorManager*  m_pIColorManager;

	typedef UIList<ColorTagElementInfo> _MyList;
	typedef UIListItem<ColorTagElementInfo> _MyListItem;
	_MyList  m_listUIElement;

    ColorRes     m_resColor;              
    SkinRes*     m_pSkinRes;
};
}