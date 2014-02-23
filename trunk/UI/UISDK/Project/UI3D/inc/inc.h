#ifndef UI3D_INC_H_6AD5E10F_2E5E_4c69_AD27_BD031F60C492
#define UI3D_INC_H_6AD5E10F_2E5E_4c69_AD27_BD031F60C492

#ifdef UI3D_EXPORTS
#define UI3D_API __declspec(dllexport)
#else
#define UI3D_API __declspec(dllimport)
#endif


namespace UI
{
	interface IUIApplication;

    extern "C"
    UI3D_API  bool  UI3D_RegisterUIObject(IUIApplication* p);

	extern "C"
	UI3D_API bool  UI3D_Init(IUIApplication* p);

	extern "C"
	UI3D_API bool  UI3D_Release();


    // {F7315FC9-D720-4410-ABFF-BA6BF7CA08BD}
    static const GUID IID_UI_IStage3D = 
    { 0xf7315fc9, 0xd720, 0x4410, { 0xab, 0xff, 0xba, 0x6b, 0xf7, 0xca, 0x8, 0xbd } };
    // {0CC1B36E-373C-4721-9DC6-89A381331EF3}
    static const GUID CLSID_UI_Stage3D = 
    { 0xcc1b36e, 0x373c, 0x4721, { 0x9d, 0xc6, 0x89, 0xa3, 0x81, 0x33, 0x1e, 0xf3 } };

}
#endif