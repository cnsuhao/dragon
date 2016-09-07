#pragma once

namespace UI
{
namespace RichTextObjectModel
{

class Style
{
public:
    Style();
    ~Style();

public:
    bool  draw_link_underline;  // hover/press时自动绘制链接下划线
};

}
}