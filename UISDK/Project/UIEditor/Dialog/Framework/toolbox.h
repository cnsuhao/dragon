#pragma once
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Control\Inc\Interface\itreeview.h"

class CToolBox : public IWindow
{
public:
    CToolBox();
    ~CToolBox();

    UI_BEGIN_MSG_MAP_Ixxx(CToolBox)
        UIMSG_WM_INITIALIZE(OnInitialize)
        UIMSG_WM_PRECREATEWINDOW(PreCreateWindow)
    UI_END_MSG_MAP_CHAIN_PARENT(IWindow)

public:
    void  InsertObject(const TCHAR* szName, const TCHAR* szCategory, UINT nObjType);

protected:
    BOOL  PreCreateWindow(CREATESTRUCT* pcs);
    void  OnInitialize();

private:
    ITreeView*  m_pTreeView;
};
