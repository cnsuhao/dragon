#ifndef UI_IRICHTEXT_H_LEEIHCY_201412201820
#define UI_IRICHTEXT_H_LEEIHCY_201412201820

#include "UISDK\Kernel\Inc\Interface\icontrol.h"

namespace UI
{

struct RichTextCharFormat
{
    // HFONT参数
    TCHAR  szFontName[LF_FACESIZE];
    long  lFontSize;
    bool  bBold;
    bool  bItalic;
    bool  bUnderline;
	bool  bDeleteline;

    // 其它参数
    long  textColor;
    long  backColor;
    bool  bTextColor;  // 是否启用文字色textColor
    bool  bBackColor;  // 是否启用背景色backColor

    // 内部使用
    HFONT  hFont;
    uint  nFontHeight;
    uint  nFontDescent;  // 用于下划线的绘制坐标计算
};

// 一个doc只有一个RichTextParaFormat
struct RichTextParaFormat
{
    REGION4  rPagePadding;  // 页面四周空白
	long  lLineSpace;  // 行间距
};

struct RichTextLinkInfo
{
	LPCTSTR  szText;
	LPCTSTR  szTooltip;
	long  lId;
	WPARAM  wParam;
	LPARAM  lParam;
};

class RichText;
interface UICTRLAPI IRichText : public IControl
{
    UI_DECLARE_Ixxx_INTERFACE(IRichText, RichText);

	void  AppendText(LPCTSTR szText);
    void  AppendText2(LPCTSTR szText, RichTextCharFormat* pcf);
	void  AppendImage(LPCTSTR szPath);
	void  AppendLink(RichTextLinkInfo* pInfo);
	void  AppendLink2(RichTextLinkInfo* pInfo, RichTextCharFormat* pcf);
    void  GetDefaultCharFormat(RichTextCharFormat* pcf);
	void  GetDefaultLinkCharFormat(RichTextCharFormat* pcf);

	void  LoadHtml(LPCTSTR szHtml);
};

// 点击richtext中的链接
// wParam: IRichTextLinkUnit*
#define UI_RICHTEXT_NM_LICK_CLICK  151021724

namespace RichTextObjectModel
{
	class LinkUnit;
}
interface UICTRLAPI IRichTextLinkUnit
{
public:
	IRichTextLinkUnit(RichTextObjectModel::LinkUnit* p);

	long  GetId();

private:
	RichTextObjectModel::LinkUnit*  m_pImpl;
};

}

#endif // UI_IRICHTEXT_H_LEEIHCY_201412201820