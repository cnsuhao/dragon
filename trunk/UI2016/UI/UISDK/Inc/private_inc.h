#pragma once

// 模块内部公共头文件

#include "Inc/Util/struct.h"
#include "Inc/Util/color.h"
#include "Inc/Util/util.h"
#include "Inc/Util/iimage.h"
#include "Inc/Interface/iuieditor.h"
#include "Inc/Interface/iobjectdescription.h"
#include "Src/Util/Log/log.h"

namespace UI
{
	
// 常用缩写定义
#define SAFE_DELETE(p) \
	if (p) \
{ \
	delete (p); \
	(p) = NULL; \
}

#define SAFE_ARRAY_DELETE(p) \
	if (p) \
{ \
	delete[] (p); \
	(p) = NULL; \
}

#define SAFE_RELEASE(p) \
	if (p) \
{ \
	(p)->Release(); \
	(p) = NULL; \
}

#define SAFE_RELEASE2(p) \
	if (p) \
{ \
	(p).Release(); \
	(p) = NULL; \
}

#define SAFE_DELETE_GDIOBJECT(h) \
{\
	if (h)\
	{\
		/*BOOL b = */::DeleteObject(h);\
		/*if (0 == b) UIASSERT(b);*/ \
		(h) = NULL;\
	}\
}

#ifdef _UNICODE
	typedef wstring String;
#else
	typedef string  String;
#endif

//typedef unsigned char byte;

#ifndef uint
typedef unsigned int uint;
#endif

typedef CRect CRegion4;

typedef map<String, String>  ATTRMAP;


// TODO: Delete Below!
//
//	声明一个String类型变量的get/set方法
//
//	参数:
//		xxx
//			如果成员名为 m_strName，则xxx取值为 Name
//
//	注意：
//		在其它dll/exe模块中不要调用Get方法,而应调用GetRef方法,否则析构时会造成heap错误
//
#define DECLARE_STRING_SETGET(xxx)    \
	const TCHAR*     Get##xxx()      { return m_str##xxx .c_str(); } \
	const String&    Get##xxx##Ref() { return m_str##xxx; } \
	void             Set##xxx( const String& str ) { m_str##xxx = str; };

#define DECLARE_bool_SETGET(xxx) \
	bool    Get##xxx() { return m_b##xxx; } \
	void    Set##xxx( bool b ) { m_b##xxx = b; }

#define DECLARE_byte_SETGET(xxx) \
	unsigned char  Get##xxx() { return m_byte##xxx; } \
	void    Set##xxx( byte b ) { m_byte##xxx = b; }

#define DECLARE_INT_SETGET(xxx) \
	int     Get##xxx() { return m_n##xxx; } \
	void    Set##xxx( int n ) { m_n##xxx = n; }

// String安全赋值
#define SETSTRING(str, sz) \
	if (sz){str = sz;} else {str.clear();}

// 内部api
void  UI_Split(const String& str, TCHAR szSep, vector<String>& vRet);
long  UISendMessage(Message* pMsgTo, UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
interface IRenderFont;
void  UI_AttachFont(IRenderFont** pOut, HFONT hFont, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType);
interface IRenderTarget;
interface IUIApplication;
IRenderTarget*  UICreateRenderTarget(IUIApplication* pUIApp, GRAPHICS_RENDER_LIBRARY_TYPE eType, bool bNeedAlphaChannel);
LPTSTR  GetTempBuffer(int nMaxSize = 255);
String&  GetTempBufferString();
void  DeflatRect(RECT* pfc, LPCRECT pDeflatRc);

class Object;
interface IMKMgr : public IRootInterface
{
	virtual void  OnObjectVisibleChangeInd(Object* pObj, bool bVisible) = 0;
	virtual void  OnObjectRemoveInd(Object* pObj) = 0;
	virtual void  SetFocusObject(Object* pObj) = 0;
};

inline int _round(const float& f)
{
	if (f < 0.0f)
		return int(f - 0.5f);
	else
		return int(f + 0.5f);
}
inline int _round(const double& f)
{
	if (f < 0.0f)
		return int(f - 0.5);
	else
		return int(f + 0.5);
}


}