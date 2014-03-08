#pragma once
#include "UISDK\Project\UI3D\inc\istage3d.h"
#include "UISDK\Project\UI3D\src\D3D\Wrap\camera\camera.h"
namespace UI
{
interface IStage3D;
class Element;

class Stage3D : public MessageProxy
{
public:
    Stage3D();
    ~Stage3D();

    UI_DECLARE_OBJECT3(Stage3D, OBJ_PANEL|CONTROL_GROUPBOX, _T("UICtrl/Container"));

	UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
        UIMSG_WM_SIZE(OnSize)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
		UIMSG_WM_OBJECTLOADED(OnObjectLoaded)

		{
			BOOL bHandled = FALSE;
			lResult = HandleMessage(uMsg, wParam, lParam, bHandled);
			if (bHandled)
				return TRUE;
		}

	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(Stage3D, IPanel)

    void  SetIStage3D(IStage3D* p) { m_pIStage3D = p; SetIMessageProxy(static_cast<IMessage*>(p)); }
	IStage3D*  GetIStage3D() { return m_pIStage3D; }

public:
	void  OnViewMatrixChanged();
	void  OnResetDevice();

    void  SetBkgndColor(DWORD dwColor);
    DWORD  GetBkgndColor();

	void  EnableCamera(bool b) { m_bEnableCamera = b; }
	bool  IsCameraEnable() { return m_bEnableCamera; }

	bool  AddElement(Element* pElem);
	bool  RemoveElement(Element* pElem);

	void  GetFullWorldCoord(float fzWorld, float*  pfWidth, float* pfHeight);
	void  Screen2World(int xScreen, int yScreen, float zWorld, float* pxWorld, float* pyWorld);

protected:
	void  OnObjectLoaded();
	LRESULT  HandleMessage(UINT message, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    void  OnPaint(IRenderTarget* pRenderTarget);
    void  OnSize(UINT nType, int cx, int cy);
    void  ResetAttribute();
    void  SetAttribute(IMapAttribute* pMapAttr, bool bReload);

    void  ClearElement();
    void  DrawBkgnd();

private:
    IStage3D*  m_pIStage3D;

    // Ë¢±³¾°
    float  m_fBkgndColor[4];  // red,green,blue,alpha, [0-1]

    SIZE  m_sizeStage;
    list<Element*>  m_listElement;
	
	DxCamera  m_camera;
	bool  m_bEnableCamera;
};

}