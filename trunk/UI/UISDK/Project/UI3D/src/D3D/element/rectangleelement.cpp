#include "stdafx.h"
#include "RectangleElement.h"
#include "UISDK\Project\UI3D\inc\ielement.h"

//void  D3D10App::FillRectScreen(RECT* prc, float fz)
//{
    // 必须按照顺时针的顺序才能显示
    // 	Vertex vertices[] =
    // 	{
    // 		D3DXVECTOR3( (float)prc->left,  (float)prc->bottom, fz ),
    // 		D3DXVECTOR3( (float)prc->right,  (float)prc->bottom, fz ),
    // 		D3DXVECTOR3( (float)prc->left, (float)prc->top, fz ),
    // 		D3DXVECTOR3( (float)prc->right, (float)prc->top, fz ),
    // 		// 		D3DXVECTOR3( 0.0f, 1.f, 0.f ),
    // 		// 		D3DXVECTOR3( 1.f, -1.f, 0.f ),
    // 		// 		D3DXVECTOR3( -1.f, -1.f, 0.f ),
    // 	};
//}
namespace UI
{
    ID3D10EffectTechnique*  RectangleElement::m_pEffectTechnique = NULL; // 不需要释放
    ID3D10InputLayout*      RectangleElement::m_pInputLayout = NULL;
    D3D10_TECHNIQUE_DESC    RectangleElement::m_techDesc;

    RectangleElement::RectangleElement()
    { 
        m_pIRectangleElement = NULL;
        m_pBuffer = NULL;

        if (!m_pEffectTechnique || !m_pInputLayout)
        {
            Init();
        }
    }
    RectangleElement::~RectangleElement()
    {
        SAFE_DELETE(m_pIRectangleElement);
        SAFE_RELEASE(m_pBuffer);
    }
    
    IRectangleElement*  RectangleElement::GetIRectangleElement()
    {
        if (!m_pIRectangleElement)
        {
            m_pIRectangleElement = new IRectangleElement(this);
        }
    
        return m_pIRectangleElement;
    }

    bool  RectangleElement::Init()
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
    void  RectangleElement::Release()
    {
        SAFE_RELEASE(m_pInputLayout);
        m_pEffectTechnique = NULL;
    }

    bool  RectangleElement::CreateBuffer(RECTF* prc, float fz)
    {
        if (m_pBuffer)
            return false;

        // Create vertex buffer
        // the vertices in the vertex buffer will usually be in object space as well. 
        // This also means that the vertex shader receives input vertex data in object space.
        // 必须按照顺时针的顺序才能显示
        Vertex vertices[] =
        {
            D3DXVECTOR3( (float)prc->fLeft,   (float)prc->fTop, fz ),
            D3DXVECTOR3( (float)prc->fRight,  (float)prc->fTop, fz ),
            D3DXVECTOR3( (float)prc->fLeft,   (float)prc->fBottom, fz ),
            D3DXVECTOR3( (float)prc->fRight,  (float)prc->fBottom, fz ),
        };
        int nCount = sizeof(vertices)/sizeof(Vertex);

        D3D10_BUFFER_DESC bd;
        bd.Usage = D3D10_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(Vertex)*nCount;
        bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags = 0;

        D3D10_SUBRESOURCE_DATA InitData;
        InitData.pSysMem = vertices;

        HRESULT hr = GetDevice()->CreateBuffer( &bd, &InitData, &m_pBuffer);
        if (FAILED(hr))
            return false;

        return true;
    }

    void  RectangleElement::OnPaint()
    {
        ID3D10Device*  pDevice = GetDevice();
        UINT stride = sizeof( Vertex );
        UINT offset = 0;

        pDevice->IASetVertexBuffers(0, 1, &m_pBuffer, &stride, &offset);
        pDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
        pDevice->IASetInputLayout(m_pInputLayout);

        for( UINT p = 0; p < m_techDesc.Passes; ++p )
        {
            m_pEffectTechnique->GetPassByIndex( p )->Apply( 0 );
            pDevice->Draw(4, 0);
        }
    }

}
    
