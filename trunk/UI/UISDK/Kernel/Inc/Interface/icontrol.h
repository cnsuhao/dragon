#ifndef ICONTROL_H_12B21A73_15E1_4562_9D1D_B910CC43921D
#define ICONTROL_H_12B21A73_15E1_4562_9D1D_B910CC43921D

#include "UISDK\Kernel\Inc\Interface\iobject.h"

namespace UI
{

// control style
// #define CONTROL_STYLE_GROUP             0x00002000    // 是否是一个新组的开始
// #define CONTROL_STYLE_OWNERDRAW         0x00004000    // 是否自绘
typedef struct tagControlStyle
{
	bool  group : 1;
	bool  ownerdraw : 1;
}ControlStyle;

class Control;
interface UISDKAPI IControl : public IObject
{
   UI_DECLARE_Ixxx_INTERFACE(IControl, Control);

   void  ModifyControlStyle(ControlStyle* add, ControlStyle* remove);
   bool  TestControlStyle(ControlStyle* test);
   bool  IsGroup();
   void  SetGroup(bool);

   ITextRenderBase*  CreateDefaultTextRender();
};

}

#endif  // ICONTROL_H_12B21A73_15E1_4562_9D1D_B910CC43921D