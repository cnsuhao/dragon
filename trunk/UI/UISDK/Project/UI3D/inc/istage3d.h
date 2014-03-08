#ifndef ISTAGE3D_H_92AF8400_8E71_4f89_BF5E_8C2DD1B4A549
#define ISTAGE3D_H_92AF8400_8E71_4f89_BF5E_8C2DD1B4A549
#include "UISDK\Kernel\Inc\Interface\ipanel.h"

namespace UI
{
interface  IMeshElement;
interface  IParticleElement;
interface  IImageElement;
interface  IRectangleElement;

class Stage3D;
interface UI3D_API IStage3D : public IPanel
{
    UI_DECLARE_Ixxx_INTERFACE(IStage3D, Stage3D)

    void  SetClearColor(DWORD dwColor);
    DWORD  GetClearColor();

#if 0
	IMeshElement*  AddMeshElement();
	IParticleElement*  AddParticleElement();
    IImageElement*  AddImageElement();
#endif
    IRectangleElement*  AddRectangleElement();

	void  GetFullWorldCoord(float fzWorld, float*  pfWidth, float* pfHeight);
	void  EnableCamera(bool b);
	bool  IsCameraEnable();
};

}

#endif // ISTAGE3D_H_92AF8400_8E71_4f89_BF5E_8C2DD1B4A549