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
    vector<FontInfo>  m_fontArray;  // ���ı���ʹ�õ��������嶼���д�ţ����й���0������ΪĬ������
};

}
}