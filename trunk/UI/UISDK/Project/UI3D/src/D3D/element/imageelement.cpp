#include "stdafx.h"
#include "ImageElement.h"
#include "UISDK\Project\UI3D\inc\ielement.h"

namespace UI
{
    ID3D10EffectTechnique*  ImageElement::m_pEffectTechnique = NULL; // 不需要释放
    ID3D10InputLayout*      ImageElement::m_pInputLayout = NULL;
    D3D10_TECHNIQUE_DESC    ImageElement::m_techDesc;

    ImageElement::ImageElement()
    { 
        m_pIImageElement = NULL;
        m_pBuffer = NULL;

        if (!m_pEffectTechnique || !m_pInputLayout)
        {
            Init();
        }
    }
    ImageElement::~ImageElement()
    {
        SAFE_DELETE(m_pIImageElement);
        SAFE_RELEASE(m_pBuffer);
    }
    
    IImageElement*  ImageElement::GetIImageElement()
    {
        if (!m_pIImageElement)
        {
            m_pIImageElement = new IImageElement(this);
        }
    
        return m_pIImageElement;
    }

    bool  ImageElement::Init()
    {
        if (m_pEffectTechnique && m_pInputLayout)
            return true;

        HRESULT hr = S_OK;
        if (!m_pEffectTechnique)
        {
            m_pEffectTechnique = g_pD3DApp->m_pEffect->
                GetTechniqueByName("FillRectScreenRender");
            if (!m_pEffectTechnique)
                return false;
            
            m_pEffectTechnique->GetDesc( &m_techDesc );
        }
        

        // Define input layout
        D3D10_INPUT_ELEMENT_DESC  layout[] = 
        {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, 
                D3D10_INPUT_PER_VERTEX_DATA, 0
            },
        };

        if (!m_pInputLayout)
        {
            D3D10_PASS_DESC  passDesc = {0};
            hr = m_pEffectTechnique->GetPassByIndex(0)->GetDesc(&passDesc);
            GetDevice()->CreateInputLayout(layout, 1, passDesc.pIAInputSignature, 
                passDesc.IAInputSignatureSize, &m_pInputLayout);

            if (!m_pInputLayout)
                return false;
        }

        return true;
    }
    void  ImageElement::Release()
    {
        SAFE_RELEASE(m_pInputLayout);
        m_pEffectTechnique = NULL;
    }

    void  ImageElement::OnPaint()
    {
//         ID3D10Device*  pDevice = GetDevice();
//         UINT stride = sizeof( Vertex );
//         UINT offset = 0;
// 
//         pDevice->IASetVertexBuffers(0, 1, &m_pBuffer, &stride, &offset);
//         pDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
//         pDevice->IASetInputLayout(m_pInputLayout);
// 
//         for( UINT p = 0; p < m_techDesc.Passes; ++p )
//         {
//             m_pEffectTechnique->GetPassByIndex( p )->Apply( 0 );
//             pDevice->Draw(4, 0);
//         }
    }

}
    
