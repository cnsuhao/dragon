#ifndef _UI_ILABEL_H_
#define _UI_ILABEL_H_

#include "..\..\..\UISDK\Inc\Interface\icontrol.h"
namespace UI
{
class Label;
interface __declspec(uuid("D847937B-F1CF-4625-977A-21CB2E94CAA3"))
UICTRLAPI ILabel : public IControl
{
    LPCTSTR  GetText();
    void  SetText(LPCTSTR pszText, bool bRedraw=true);
	IBindSourceChangedListener*  GetListener();

	enum 
	{
		PROPERTY_TEXT = 168131737
	};
	UI_DECLARE_INTERFACE_ACROSSMODULE(Label);
};

class PictureCtrl;
interface __declspec(uuid("314EB8AB-E90C-488E-9FB4-016021BF7DF2"))
UICTRLAPI IPictureCtrl : public IControl
{
    UI_DECLARE_INTERFACE_ACROSSMODULE(PictureCtrl);
};

class GifCtrl;
interface __declspec(uuid("69C6BC37-4F49-429A-B8A8-D472D068B456"))
UICTRLAPI IGifCtrl : public IControl
{
    bool  Start();
    bool  Pause();
    bool  Stop();

	UI_DECLARE_INTERFACE_ACROSSMODULE(GifCtrl);
};

}

#endif // _UI_ILABEL_H_