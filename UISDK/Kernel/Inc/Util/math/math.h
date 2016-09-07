#ifndef UI_MATH_H_LIBO_201412011725
#define UI_MATH_H_LIBO_201412011725

namespace UI
{

// webkit  Source\WebCore\platform\graphics\color.cpp
inline uint16_t fastDivideBy255(uint16_t value)
{
    // This is an approximate algorithm for division by 255, but it gives accurate results for 16bit values.
    uint16_t approximation = value >> 8;
    uint16_t remainder = value - (approximation * 255) + 1;
    return approximation + (remainder >> 8);
}
}

#endif // UI_MATH_H_LIBO_201412011725