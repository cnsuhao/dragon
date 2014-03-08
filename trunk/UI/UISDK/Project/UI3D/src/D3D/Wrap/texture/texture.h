#pragma once

namespace UI
{

class Texture
{
public:
	Texture();
	~Texture();


public: 
	bool  LoadFromFile(const TCHAR*  szPath);
	void  Destroy();

    bool  DrawInWorld(RECTF rcWorld);
    bool  DrawInScreen(RECT rcScreen);


private:
	D3DX10_IMAGE_INFO  m_SrcInfo;
	ID3D10ShaderResourceView*  m_pResourceView;

    RECTF  m_rect;
};

}