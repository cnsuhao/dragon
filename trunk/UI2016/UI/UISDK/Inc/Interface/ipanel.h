#ifndef _UI_IPANEL_H_
#define _UI_IPANEL_H_

#include "iobject.h"

namespace UI
{
interface ILayout;
interface IObject;
interface IRenderBase;
class Panel;

interface __declspec(uuid("A591D454-65ED-40C3-8D84-612ABA3C8167"))
UISDKAPI IPanel : public IObject
{
    ILayout*   GetLayout();
    void  SetLayoutType(LAYOUTTYPE eLayoutType);

    void  SetTextureRender(IRenderBase* p);
    IRenderBase*  GetTextureRender();

    UI_DECLARE_INTERFACE(Panel);
};

}

#endif  