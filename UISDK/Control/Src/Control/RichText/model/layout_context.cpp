#include "stdafx.h"
#include "layout_context.h"
#include "doc.h"
#include "page.h"
#include "element.h"
#include "line.h"
#include "run.h"

using namespace UI;
using namespace UI::RichTextObjectModel;;

void  MultiLineLayoutContext::SetElementLayoutFinish()
{
    pElement = NULL;
    nElementFrom = 0;
}

bool  MultiLineLayoutContext::IsElementLayoutFinish()
{
    return !pElement;
}

void  MultiLineLayoutContext::AppendNewLine()
{
	if (pLine)
	{
		SetLineFinish();
	}
    pLine = pPage->AppendLine();
    xLineOffset = 0;

    pLine->SetPosition(xLineOffset, yLine);
}

void  MultiLineLayoutContext::SetLineFinish()
{
    if (pLine)
    {
        yLine += pLine->GetHeight();
        pLine = NULL;
    }

    yLine += pDoc->GetDefaultParaFormat()->lLineSpace;
    xLineOffset = 0;
}