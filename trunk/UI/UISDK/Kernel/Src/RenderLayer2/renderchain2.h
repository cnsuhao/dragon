#pragma once


namespace UI
{
	class RenderLayer2;
	class WindowDefaultLayer;
	class Object;

    class RenderChain2
    {
    public:
        RenderChain2();
        ~RenderChain2();

    public:
        RenderLayer2*  CreateRenderLayer(Object*  pObj, int zOrder);
        void  DestroyRenderLayer(RenderLayer2*  pLayer);

    private:
        WindowDefaultLayer*  m_pRootLayer;
    };
}