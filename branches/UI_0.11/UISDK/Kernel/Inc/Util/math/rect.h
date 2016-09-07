#ifndef UI_MATH_RECT_H_LIBO_201412011313
#define UI_MATH_RECT_H_LIBO_201412011313
#include "..\..\base\uidefine.h"  // UISDKAPI

namespace UI
{

typedef struct tagRECTF
{
	float  left;
	float  top;
	float  right;
	float  bottom;
}
RECTF, *LPRECTF;

class UISDKAPI RectF : public RECTF
{
public:
    RectF();
	RectF(LPCRECT prc);

    void  CopyFrom(LPRECTF pScr);
	void  Set(LPCRECT prc);
	void  Set(float _fLeft, float _fTop, float _fRight, float _fBottom);
	void  Set(int _Left, int _Top, int _Right, int _Bottom);
};

}

#endif // UI_MATH_RECT_H_LIBO_201412011313