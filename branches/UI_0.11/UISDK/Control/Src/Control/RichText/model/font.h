#pragma once
#include "UISDK\Control\Inc\Interface\irichtext.h"

namespace UI
{
namespace RichTextObjectModel
{


struct FontInfo
{
    HFONT  hFont;
    uint  nFontHeight;
    uint  nFontDescent;
};

class FontCache
{
public:
    FontCache();
    static FontCache&  Instance();

    void  AddRef();
    void  Release();

    bool  GetFont(RichTextCharFormat* pcf);

private:
    HFONT  CreateFont(RichTextCharFormat& cf);
    uint  GetFontHeight(HFONT hFont, uint* pnDescent);

private:
    long  m_lRef;
    vector<FontInfo>  m_fontArray;  // 该文本中使用的所有字体都集中存放，进行共享。0号字体为默认字体
};

}
}