#ifndef UI3DDEFINE_H_AF407F2D_7AD7_4315_8D43_863EC829881C
#define UI3DDEFINE_H_AF407F2D_7AD7_4315_8D43_863EC829881C

namespace UI
{

class  RECTF
{
public:
    RECTF()
    {
        fLeft = fTop = fRight = fBottom = 0;
    }

    void CopyFromm(RECTF* pScr)
    {
        fLeft = pScr->fLeft;
        fTop = pScr->fTop;
        fRight = pScr->fRight;
        fBottom = pScr->fBottom;
    }
	void  Set(float _fLeft, float _fTop, float _fRight, float _fBottom)
	{
		fLeft = _fLeft;
		fTop = _fTop;
		fRight = _fRight;
		fBottom = _fBottom;
	}
public:
    float  fLeft;
    float  fTop;
    float  fRight;
    float  fBottom;
};


}

#endif // UI3DDEFINE_H_AF407F2D_7AD7_4315_8D43_863EC829881C