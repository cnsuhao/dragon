#pragma once
#include "layer.h"

namespace UI
{

class SoftwareLayer : public Layer
{
public:
    SoftwareLayer();
    ~SoftwareLayer();

    virtual LayerType  GetType() override { return Layer_Software; }
	virtual void  virtualOnSize(uint nWidth, uint nHeight) override;
	virtual void  UpdateDirty() override;

    IRenderTarget*  GetRenderTarget();

private:
    IRenderTarget*  m_pRenderTarget;
};

}