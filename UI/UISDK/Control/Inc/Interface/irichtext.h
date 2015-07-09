#ifndef UI_IRICHTEXT_H_LEEIHCY_201412201820
#define UI_IRICHTEXT_H_LEEIHCY_201412201820

#include "UISDK\Kernel\Inc\Interface\icontrol.h"

namespace UI
{

struct RichTextCharFormat
{
    // HFONT����
    TCHAR  szFontName[LF_FACESIZE];
    long  lFontSize;
    bool  bBold;
    bool  bItalic;
    bool  bUnderline;
	bool  bDeleteline;

    // ��������
    long  textColor;
    long  backColor;
    bool  bTextColor;  // �Ƿ���������ɫtextColor
    bool  bBackColor;  // �Ƿ����ñ���ɫbackColor

    // �ڲ�ʹ��
    HFONT  hFont;
    uint  nFontHeight;
    uint  nFontDescent;  // �����»��ߵĻ����������
};

// һ��docֻ��һ��RichTextParaFormat
struct RichTextParaFormat
{
    REGION4  rPagePadding;  // ҳ�����ܿհ�
	long  lLineSpace;  // �м��
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

// ���richtext�е�����
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