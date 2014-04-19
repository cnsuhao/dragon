#include "stdafx.h"
#include "d2drenderfont.h"


namespace UI
{

Direct2DRenderFont::Direct2DRenderFont()
{
    m_pTextFormat = NULL;
    m_hFont = NULL;
    m_bCreateOrAttach = true;
    m_pLogFont = NULL;
}
Direct2DRenderFont::~Direct2DRenderFont()
{
    UI_LOG_DEBUG(_T("Direct2DRenderFont Delete. ptr=0x%08X"), this);
    this->DestroyFont();
}

void  Direct2DRenderFont::DestroyFont()
{
    SAFE_RELEASE(m_pTextFormat);
    m_hFont = NULL;
    m_bCreateOrAttach = true;
    m_pLogFont = NULL; 
}

void  Direct2DRenderFont::CreateInstance(IRenderFont** ppOutRef)
{
    UIASSERT(NULL != ppOutRef);
    if (NULL == ppOutRef)
        return;

    Direct2DRenderFont* p = new Direct2DRenderFont();
    p->AddRef();
    *ppOutRef = p;
}


bool Direct2DRenderFont::Load( LOGFONT* plogfont )
{
    if( NULL == plogfont )
    {
        UI_LOG_WARN(_T("%s plogfont == NULL"), FUNC_NAME);
        return false;
    }
    this->DestroyFont();

    m_pLogFont = plogfont;  // 该plogfont应该是pojo_fontitem中的成员变量

    HRESULT hr = E_FAIL;

    // TODO: 下划线要怎么搞
    UIASSERT(0 == plogfont->lfUnderline);

    // Create a DirectWrite text format object.
    hr = g_pD2DApp->m_pDWriteFactory->CreateTextFormat(
        plogfont->lfFaceName,
        NULL,
        plogfont->lfWeight>= FW_BOLD?DWRITE_FONT_WEIGHT_BOLD:DWRITE_FONT_WEIGHT_NORMAL,
        plogfont->lfItalic?DWRITE_FONT_STYLE_ITALIC:DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        (FLOAT)abs(plogfont->lfHeight),
        L"", //locale
        &m_pTextFormat
        );
	
    if (FAILED(hr))
    {
        UI_LOG_WARN(_T("%s CreateTextFormat failed. hr=0x%08x"), FUNC_NAME, hr);
    }


    m_bCreateOrAttach = true;

    return true;
}
bool Direct2DRenderFont::ModifyFont(LOGFONT* plogfont)
{
    bool bRet = this->Load(plogfont);
    if (false == bRet)
    {
        UI_LOG_WARN(_T("%s Load failed."), FUNC_NAME);
    }
    else
    {
    } 
    return bRet;
}


void  Direct2DRenderFont::Attach(HFONT hFont)
{
    this->DestroyFont();
    m_hFont = hFont;
    m_bCreateOrAttach = false;
}
HFONT Direct2DRenderFont::Detach()
{
    HFONT hSave = m_hFont;
    m_hFont = NULL;
    m_bCreateOrAttach = true;

    return hSave;
}

SIZE Direct2DRenderFont::MeasureString( const TCHAR* szText, int nLimitWidth)
{
    CSize sizeText(0, 0);
    if (m_pTextFormat)
    {
        const FLOAT MAX_LAYOUT_SIZE = 65535;

        if (-1 == nLimitWidth)
            nLimitWidth = (int)MAX_LAYOUT_SIZE;

        IDWriteTextLayout* pLayout = NULL;
        HRESULT hr = g_pD2DApp->m_pDWriteFactory->CreateTextLayout(
            szText, _tcslen(szText), m_pTextFormat, (FLOAT)nLimitWidth, MAX_LAYOUT_SIZE, &pLayout);

        if (SUCCEEDED(hr) && NULL != pLayout)
        {
            DWRITE_TEXT_METRICS m;
            hr = pLayout->GetMetrics(&m);
            if (FAILED(hr))
            {
                UI_LOG_WARN(_T("%s GetMetrics Failed. hr=0x%08X"), FUNC_NAME, hr);
            }
            else
            {
                sizeText.cx = (LONG) m.width+1;
                sizeText.cy = (LONG) m.height+1;
            }

        }
        else
        {
            UI_LOG_WARN(_T("%s CreateTextLayout Failed. hr=0x%08X"), FUNC_NAME, hr);
        }
        SAFE_RELEASE(pLayout);
    }

    return sizeText;
}

UINT Direct2DRenderFont::GetTextMetricsHeight()
{
    return round(m_pTextFormat->GetFontSize());
    return 0;
}

HFONT Direct2DRenderFont::GetHFONT()
{
    UIASSERT(0);
    if (m_bCreateOrAttach)
    {
        UIASSERT(0);
    }
    else
    {
        return m_hFont;
    }
    return NULL;
}


bool Direct2DRenderFont::GetLogFont(LOGFONT* plf)
{
    if (NULL == plf)
        return false;

    if (m_bCreateOrAttach)
    {
        memcpy(plf, m_pLogFont, sizeof(LOGFONT));
    }
    else
    {
        if (NULL == m_hFont)
            return false;

        GetObject(m_hFont, sizeof(LOGFONT), plf);
    }
    return true;
}


}