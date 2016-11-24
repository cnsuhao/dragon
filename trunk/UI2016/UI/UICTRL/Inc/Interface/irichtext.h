#ifndef _UI_IRICHTEXT_H_
#define _UI_IRICHTEXT_H_
#include "..\..\..\UISDK\Inc\Interface\icontrol.h"

namespace UI
{

struct CharFormat
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
    unsigned int  nFontHeight;
    unsigned int  nFontDescent;  // 用于下划线的绘制坐标计算
};



enum class PageMode
{
    singleline,
    multiline,
    multipage,
    error,
};

namespace RT
{
// 目前只实现了单行模式
enum class VALIGN : int
{
    top,
    bottom,
    center,
};

enum class HALIGN : int
{
    left,
    right,         // 每一行内容都右对齐
    center,        // 每一行内容都居中对齐
    paraRight,     // 整个段落右对齐，但段中的行还是左对齐
    paraCenter,    // 整个段落居中对齐，但段中的行还是左对齐
};

interface IParagraph
{
    virtual void  SetAlign(RT::HALIGN align) = 0;
    virtual void  SetLeftIndent(int indent) = 0;
    virtual void  SetRightIndent(int indent) = 0;
    virtual void  SetTopSpace(int space) = 0;
    virtual void  SetBottomSpace(int space) = 0;
    virtual void  SetCharformat(CharFormat*) = 0;
    virtual void  ShowBubbleBackgournd(bool b) = 0;
};

interface IChatMessage
{
    virtual void  SetAlign(RT::HALIGN align) = 0;
    virtual HANDLE  EnterBubble() = 0;
    virtual void  LeaveBubble(HANDLE) = 0;
};

// interface INode
// {
//     virtual IParagraph*  GetParagraphNode() = 0;
// };

// interface ISelection
// {
//     virtual INode*  GetAnchorNode() = 0;
//     virtual INode*  GetFocusNode() = 0;
// };


interface ILink
{
    virtual LPCTSTR  GetId() = 0;
    virtual void  SetId(LPCTSTR) = 0;

    virtual LPCTSTR  GetToolTip() = 0;
    virtual void  SetToolTip(LPCTSTR) = 0;

    virtual LPCTSTR  GetReference() = 0;
    virtual void  SetReference(LPCTSTR) = 0;
};
}


#ifdef ReplaceText
#undef ReplaceText
#endif

class RichText;
interface UICTRL_API_UUID(F1B67364-BA72-44CD-BA63-53DA6B23CB4A) IRichText : public IControl
{
    void  Clear();
    void  SetText(LPCTSTR szText);
    void  GetDefaultCharFormat(CharFormat* pcf);
	void  GetDefaultLinkCharFormat(CharFormat* pcf);
    
	void  LoadHtml(LPCTSTR szHtml);

    void  ReplaceText(LPCTSTR szText);
    void  ReplaceTextEx(LPCTSTR szText, CharFormat* pcf);
    void  AppendImage(LPCTSTR szPath);
    RT::ILink*  ReplaceLink(LPCTSTR);
    RT::ILink*  ReplaceLinkEx(LPCTSTR, CharFormat* pcf);
    RT::IChatMessage*  BeginInsertMessage();
    void  EndInsertMessage(RT::IChatMessage*);

    void  SetPageMode(PageMode e);
    PageMode  GetPageMode();

    //RT::ISelection*  GetSelection();
    RT::IParagraph*  GetSelectionAnchorParagraph();

    void  SetStyle_PageContentVAlign(RT::VALIGN);
    void  SetStyle_PageContentHAlign(RT::HALIGN);
    void  SetStyle_DrawLinkUnderline(bool b);

    signal<IRichText*, RT::ILink*>&  LickClickedEvent();
	UI_DECLARE_INTERFACE_ACROSSMODULE(RichText);
};


namespace RT
{
	class LinkUnit;

    // 编辑模式
    enum class EDITABLE
    {
        Disable,   // 不支持编辑，不显示光标，不支持复制
        Readonly,  // 显示光标，支持复制（未实现）
        Enable,    // 支持编辑（未实现）
    };
}

}

#endif // _UI_IRICHTEXT_H_