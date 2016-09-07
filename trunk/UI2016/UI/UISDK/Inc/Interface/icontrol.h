#ifndef _UI_ICONTROL_H_
#define _UI_ICONTROL_H_

#include "iobject.h"

namespace UI
{
	interface ITextRenderBase;

// control style
// #define CONTROL_STYLE_GROUP             0x00002000    // 是否是一个新组的开始
// #define CONTROL_STYLE_OWNERDRAW         0x00004000    // 是否自绘
typedef struct tagControlStyle
{
	bool  group : 1;
	bool  ownerdraw : 1;
}ControlStyle;

class Control;
interface __declspec(uuid("78EC44D9-137E-46A1-8ACA-0E12A209FF4F"))
UISDKAPI IControl : public IObject
{
   void  ModifyControlStyle(ControlStyle* add, ControlStyle* remove);
   bool  TestControlStyle(ControlStyle* test);
   bool  IsGroup();
   void  SetGroup(bool);

   ITextRenderBase*  CreateDefaultTextRender();
   ITextRenderBase*  GetTextRenderDefault();

   UI_DECLARE_INTERFACE(Control);
};

}

#endif  // _UI_ICONTROL_H_