#ifndef _UI_IOBJECT_H_
#define _UI_IOBJECT_H_

namespace UI
{
	interface IWindowBase;
	interface ISkinRes;
	interface IObjectDescription;
	interface ITextRenderBase;
	interface IRenderFont;
	interface IRenderBase;
	interface ILayer;
	interface IMapAttribute;

	typedef struct tagObjStyle
	{
		bool  transparent : 1;
		bool  float_on_parent_content : 1;   // 在刷新该控件时，不仅仅需要重绘父对象背景(wm_erasebkgbkgnd)，还要重绘父对象内容(wm_paint)
		bool  post_paint : 1;
		bool  hscroll : 1;
		bool  vscroll : 1;
		bool  ncobject : 1;
		bool  reject_all_mouse_msg : 1;
		bool  reject_self_mouse_msg : 1;
		bool  receive_dragdrop_event : 1;
		bool  enable_ime : 1;
		bool  zindex_overlap : 1;
		bool  noclip : 1;
		bool  clip_client : 1;   // 绘制完对象的背景后，再绘制对象的client区域时，是否需要对client区域作剪裁。（一般的对象不做，但对于会滚动的listctrl则需要剪裁，否则滚动的内容会超出来覆盖背景
		bool  tabstop : 1;
		bool  layer;  // 开启分层

		// 默认值字段。用于派生类覆盖父类的默认行为
		// 可以考虑专门为default也构造一个同样的tagObjStyle，用于对应每一个样式的默认值
		bool  default_ncobject : 1;  // scrollbar默认为ncobj
		bool  default_reject_all_mouse_msg : 1; // 默认该控件就不接收鼠标消息。如panel/label
		bool  default_reject_self_mouse_msg : 1;
		bool  default_transparent : 1;  // 该控件默认是透明的。默认所有的控件不透明，用于提升绘制效率
		bool  default_tabstop : 1;  // 默认该控件能否tabstop

	}OBJSTYLE;
	typedef struct tagObjState
	{
		char  visibility_ : 2;  // 0:collapsed 1:visible 2:hidden 
		bool  disable : 1;
		bool  press : 1;
		bool  hover : 1;
		bool  force_press : 1;
		bool  force_hover : 1;
		bool  focus : 1;
		bool  default_ : 1;
		bool  selected : 1;
		bool  checked : 1;
		bool  radio_checked : 1; // ??
		bool  readonly : 1;
		bool  activate : 1;   // 给窗口使用
	}OBJSTATE;

	class Object;
	interface __declspec(uuid("B2E563E6-46E7-45F1-B977-5884F217ACD2"))
	UISDKAPI IObject : public IMessage
	{
		IUIApplication* GetUIApplication();
		ISkinRes*  GetSkinRes();
		IWindowBase*  GetWindowObject();
		void  SetDescription(IObjectDescription* p);
		IObjectDescription*  GetDescription();
		LPCTSTR  GetId();
		HWND  GetHWND();
		void  GetMapAttribute(IMapAttribute** ppMapAttribute);
		
		IObject*  GetParentObject();
		IObject*  FindChildObject(LPCTSTR szObjId);
		IObject*  FindNcChildObject(LPCTSTR szObjId);
		IObject*  GetChildObject();
		IObject*  GetNcChildObject();
		IObject*  GetNextObject();
		IObject*  GetPrevObject();

		void  AddChild(IObject* p);
		void  AddNcChild(IObject*p);

		void  Invalidate();
		void  Invalidate(RECT* prcObj);
		void  Invalidate(RECT* prcObj, int nCount);
		void  SetObjectPos(int x, int y, int cx, int cy, int nFlag);
		void  SetObjectPos(RECT* prc, int nFlag);

		bool  IsVisible();
		bool  IsCollapsed();
		bool  IsSelfVisible();
		void  SetVisible(bool b);
		bool  IsHover();
		bool  IsPress();
		bool  IsForcePress();
		bool  IsForceHover();
		bool  IsDefault();
		bool  IsEnable();
		bool  IsFocus();
		bool  CanTabstop();
		void  SetEnable(bool b);
		void  SetForceHover(bool b, bool bNotify);
		void  SetForcePress(bool b, bool bNotify);
		bool  SetMouseCapture(int nNotifyMsgId);
		bool  ReleaseMouseCapture();
		bool  SetFocusInWindow();

		void  ModifyObjectStyle(OBJSTYLE* add, OBJSTYLE* remove);
		bool  TestObjectStyle(const OBJSTYLE& test);
		void  RemoveMeInTheTree();


		void  GetParentRect(RECT* prc);
		void  GetWindowRect(RECT* prc);
		SIZE  GetDesiredSize();
		int   GetWidth();
		int   GetHeight();
		void  SetPaddingRegion(REGION4* prc);
		void  GetPaddingRegion(REGION4* prc);
		void  SetMarginRegion(REGION4* prc);
		void  GetMarginRegion(REGION4* prc);
		void  GetBorderRegion(REGION4* prc);
		void  GetNonClientRegion(REGION4* prc);
		void  SetExtNonClientRegion(REGION4* prc);
		void  GetExtNonClientRegion(REGION4* prc);
		void  GetClientRectInObject(RECT* prc);
		void  GetObjectClientRect(RECT* prc);
		void  ClientRect2ObjectRect(const RECT* rcClient, RECT* rcObj);
		void  WindowRect2ObjectRect(const RECT* rcWindow, RECT* rcObj);
		void  WindowPoint2ObjectPoint(const POINT* ptWindow, POINT* ptObj, bool bCalcTransform);
		void  WindowPoint2ObjectClientPoint(const POINT* ptWindow, POINT* ptObj, bool bCalcTransform);

		ITextRenderBase*  GetTextRender();
		IRenderFont*  GetRenderFont();
		IRenderBase*  GetBkRender();
		IRenderBase*  GetForeRender();
		ILayer*  GetLayer();

		UI_DECLARE_INTERFACE(Object);
	};
}

#endif // _UI_IOBJECT_H_