#pragma once
#include "UISDK\Kernel\Src\Renderlibrary\renderresourceimpl.h"

namespace UI
{

class Direct2DRenderFont : public IRenderResourceImpl<IRenderFont>
{
public:
	Direct2DRenderFont();
	~Direct2DRenderFont();
	static  void  CreateInstance(IRenderFont** ppOutRef);
	virtual GRAPHICS_RENDER_LIBRARY_TYPE GetGraphicsRenderLibraryType() { return GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D; }

public:
	virtual bool  Load( LOGFONT* plogfont );
	virtual bool  ModifyFont(LOGFONT* plogfont);

	virtual void  Attach(HFONT hFont);
	virtual HFONT Detach();

	virtual SIZE  MeasureString( const TCHAR* szText, int nLimitWidth = -1 );
	virtual UINT  GetTextMetricsHeight( );

	virtual HFONT GetHFONT();
	virtual bool  GetLogFont(LOGFONT* plf);

public:
	IDWriteTextFormat*  GetFont() { return m_pTextFormat; }
protected:
	void    DestroyFont();

protected:
	IDWriteTextFormat*  m_pTextFormat;
	LOGFONT*            m_pLogFont;   // 记录传递进行的LOGFONT结构体，用于GetLogFont

	HFONT   m_hFont;   // For Attach
	bool    m_bCreateOrAttach;
};

}