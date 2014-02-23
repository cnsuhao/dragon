#pragma once
#include "UISDK\Project\UI3D\src\Element\element.h"
#include "UISDK\Project\UI3D\src\D3D\Wrap\mesh.h"

namespace UI
{
interface IMeshElement;

class MeshElement : public Element
{
public:
    MeshElement();
    ~MeshElement();

    virtual void OnPaint();
        
public:
    DxMesh*  GetMesh();
	IMeshElement*  GetIMeshElement();

private:
    DxMesh   m_mesh;
	IMeshElement*  m_pIMeshElement;
};
}