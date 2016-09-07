#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\iricheditbubble.h"
#include "rebubble.h"
using namespace UI;

IREBubble::IREBubble(REBubble* p)
{
	m_pBubbleImpl = p;
}

IREPortraitBubble::IREPortraitBubble(REPortraitBubble* p):IREBubble(p)
{
	m_pImpl = p;
}

IREInfoBubble::IREInfoBubble(REInfoBubble* p):IREBubble(p)
{
	m_pImpl = p;
}