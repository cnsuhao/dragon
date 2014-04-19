#ifndef IELEMENT_H_B6463E3E_A017_4e5d_8935_7C2B56A3AFC3
#define IELEMENT_H_B6463E3E_A017_4e5d_8935_7C2B56A3AFC3

namespace UI
{
class Element;
class MeshElement;
class ImageElement;
class ParticleElement;
class RectangleElement;
class ImageElement;

interface UI3D_API IElement
{
public:
	IElement(Element*);

	void  MoveWorld(float x, float y, float z);
    void  MoveWorldBy(float x, float y, float z);
	void  RotateWorld(float x, float y, float z);
    void  RotateWorldBy(float x, float y, float z);

    void  GetPos(float* px, float* py, float* pz);
    void  GetRotate(float* px, float* py, float* pz);
    void  GetScale(float* px, float* py, float* pz);

private:
	Element*  m_pElementImpl;
};

interface UI3D_API IRectangleElement : public IElement
{
public:
    IRectangleElement(RectangleElement* pImpl);
    void  SetRect(RECTF* prc, float z);

private:
    RectangleElement*  m_pRectangleElementImpl;
};

interface UI3D_API IImageElement : public IElement
{
public:
    IImageElement(ImageElement* pImpl);
//     bool  LoadFromFile(const TCHAR* szText);
//     void  SetDrawRect(float fLeft, float fTop, float fRight, float fBottom, float fZ);
//     void  SetDirectDrawRect(RECT* prc, float fz);
//     void  SetTransparency(float fAlpha);

private:
    ImageElement*  m_pImageElementImpl;
};


#if 0
interface UI3D_API IMeshElement : public IElement
{
public:
	IMeshElement(MeshElement* pImpl);
	bool  LoadFromFile(const TCHAR* szText);

private:
	MeshElement*  m_pMeshElementImpl;
};

interface UI3D_API IParticleElement : public IElement
{
public:
	IParticleElement(ParticleElement* pImpl);

	bool  SetTextureCount(unsigned int nTextureCount);
	bool  SetTexture(unsigned int nIndex, const TCHAR* szFile);
	void  InitParticles(unsigned int nCacheCount, unsigned int nAliveCount);

private:
	ParticleElement*  m_pParticleElementImpl;
};
#endif

}

#endif // IELEMENT_H_B6463E3E_A017_4e5d_8935_7C2B56A3AFC3