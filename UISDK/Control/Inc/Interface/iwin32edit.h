#ifndef  IWIN32EDIT_H_LIBO_201408011401
#define  IWIN32EDIT_H_LIBO_201408011401
#include "UISDK\Kernel\Inc\Interface\icontrol.h"

namespace UI
{

struct StartEditParam
{
    bool  bTextLimit;
    bool  bByteLimit;
    long  lTextLimit;
};

#ifdef COMPILE_Win32Edit
// wparam: IWin32Edit*
#define UI_WEN_STARTEDIT  148042202
#define UI_WEN_ACCEPTEDIT 148042203
#define UI_WEN_CANCELEDIT 148042204

// ±à¼­¿ò±»Ïú»ÙÁË
// wParam: IWin32Edit*
#define UI_WEN_EDITDESTROY 148041315

class Win32Edit;
interface UICTRLAPI IWin32Edit : public IControl
{
    UI_DECLARE_Ixxx_INTERFACE(IWin32Edit, Win32Edit);
    void  SetText(LPCTSTR szText, bool bUpdateNow);

    bool  StartEdit(bool bSelAll);
    bool  IsEditing();
    void  DiscardEditNow();

    void  SetNotifyStartEdit(bool b);
    void  SetNotifyAcceptEdit(bool b);
    void  SetNotifyCancelEdit(bool b);
    void  SetNotifyDestroyEdit(bool b);
};
}

#endif
#endif // IWIN32EDIT_H_LIBO_201408011401