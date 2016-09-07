#ifndef IANIMATEDEF_H_D4A2D364_BE5D_4a61_985C_C09506B05ACB
#define IANIMATEDEF_H_D4A2D364_BE5D_4a61_985C_C09506B05ACB

namespace UI
{
    //////////////////////////////////////////////////////////////////////////
    //
    //     动画接口枚举定义
    //
    //////////////////////////////////////////////////////////////////////////

//     enum TIMELINE_VALUE_TYPE
//     {
//         TV_NONE,
//         TV_INT,
//         TV_FLOAT,
//         TV_RECT,  // not implement
//     };

    enum TIMELINE_TIME_TYPE
    {
        TT_BY_MS,
        TT_BY_SECOND,
        TT_BY_FRAME,
    };

//     enum TIMELINE_MOVE_ALGORITHM
//     {
//         TMA_Linear,      // 匀速
//         TMA_Accelerate,  // 匀加速
//         TMA_Easing,      // 缓动
//     };

    enum EaseType
    {
        // cubic
        ease = 0,     // default, 逐渐变慢
        linear,       // 匀速
        ease_in,      // 加速
        ease_out,     // 减速
        ease_in_out,  // 加速然后减速

        // sine
        ease_in_sine,
        ease_out_sine,
        ease_in_out_sine, 

        // quad
        ease_in_quad,
        ease_out_quad,
        ease_in_out_quad,

        // quart
        ease_in_quart,
        ease_out_quart,
        ease_in_out_quart,

        // quint
        ease_in_quint,
        ease_out_quint,
        ease_in_out_quint,

        // expo
        ease_in_expo,
        ease_out_expo,
        ease_in_out_expo,

        // circ
        ease_in_circ,
        ease_out_circ,
        ease_in_out_circ,

        // back
        ease_in_back,      // 先回退，再加速
        ease_out_back,     // 先加速超出，再回来
        ease_in_out_back,  // 下超出 + 上超出 

        ease_bezier_support = ease_in_out_back,   // 目前bezier只能支持到这里，后面的采用easing函数模拟

        // elastic
        ease_in_elastic,
        ease_out_elastic,
        ease_in_out_elastic,

        // bounce
        ease_in_bounce,
        ease_out_bounce,
        ease_in_out_bounce,

        // bezier_custom
        ease_bezier_custom,

        ease_end,
    };

    enum SLIDE_ANIMATE_FLAG
    {
        SLIDE_LEFT2RIGHT,
        SLIDE_TOP2BOTTOM,
        SLIDE_RIGHT2LEFT,
        SLIDE_BOTTOM2TOP,
    };
    enum E_WINDOW_ANIMATE_TYPE
    {
        E_WINDOW_ANIMATE_3D_ROTATE,
        E_WINDOW_ANIMATE_UPDOWNALPHASHOW,
        E_WINDOW_ANIMATE_ALPHASHOW,
    };
    enum E_CONTROL_ANIMATE_TYPE
    {
        E_CONTROL_ANIMATE_SLIDE,
    };
}

#endif // IANIMATEDEF_H_D4A2D364_BE5D_4a61_985C_C09506B05ACB