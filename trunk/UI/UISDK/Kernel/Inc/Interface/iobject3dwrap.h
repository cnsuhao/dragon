#ifndef IOBJECT3DWRAP_H_F124B85E_14D1_47a5_928E_36DB8D9EE782
#define IOBJECT3DWRAP_H_F124B85E_14D1_47a5_928E_36DB8D9EE782

#include "UISDK\Kernel\Inc\Interface\imessage.h"

namespace UI
{
class Object3DWrap;
interface UISDKAPI IObject3DWrap
{
    IObject3DWrap(Object3DWrap*);
    Object3DWrap*  GetImpl();

    void  Begin();
    void  End();

    void  SetOriginPoint(POINT pt);
    void  Rotate(int x, int y, int z);
    void  Move(int x, int y, int z);
private:
    Object3DWrap*  m_pImpl;
};

}

#endif  // IOBJECT3DWRAP_H_F124B85E_14D1_47a5_928E_36DB8D9EE782