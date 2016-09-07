#ifndef _UI_ICOLORRENDER_H_
#define _UI_ICOLORRENDER_H_
#include "Inc\Interface\irenderbase.h"

namespace UI
{

class ColorRender;
interface __declspec(uuid("A3AE0787-D67E-40A2-8AFE-2B6783A55DAF"))
UISDKAPI IColorRender : public IRenderBase
{
    void  SetBkColor(COLORREF col);
    void  SetBorderColor(COLORREF col);
    void  SetBorderRegion(const CRegion4* prc);

	UI_DECLARE_INTERFACE(ColorRender);
};

//////////////////////////////////////////////////////////////////////////

class SysColorRender;
interface __declspec(uuid("D9619AFA-291B-41CC-83FE-43CA5079A962"))
UISDKAPI ISysColorRender : public IRenderBase
{
    void  SetBkColor(int nColorIndex);
    void  SetBorderColor(int nColorIndex);

	UI_DECLARE_INTERFACE(SysColorRender);
};

//////////////////////////////////////////////////////////////////////////

class GradientRender;
interface __declspec(uuid("B7B00FDE-964F-4F67-B859-58EA62D8946E"))
UISDKAPI IGradientRender : public IRenderBase
{
    UI_DECLARE_INTERFACE(GradientRender);
};

//////////////////////////////////////////////////////////////////////////

class ColorListRender;
interface __declspec(uuid("BB63B44B-831C-4BE0-9053-6FE62C88A5FF"))
UISDKAPI IColorListRender : public IRenderBase
{
    void  SetStateColor(int nState, COLORREF colorBk, bool bSetBk, COLORREF colBorder, bool bSetBorder);
    void  SetCount(int n);

	UI_DECLARE_INTERFACE(ColorListRender);
};
}

#endif // _UI_ICOLORRENDER_H_