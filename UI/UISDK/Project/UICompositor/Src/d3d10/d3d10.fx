// d3d10 shader file

Texture2D  g_Texture;

// 3D变换
matrix    g_Matrix;       // 变换矩阵
matrix    g_orthMatrix;   // 正交投影矩阵（这里不采用透视投影矩阵，因为透视投影的对象大小随着z而改变。而我们这里是要求对象大小不变，只是旋转而矣）

SamplerState Sampler
{    
	Filter = MIN_MAG_MIP_LINEAR;    
	AddressU = Wrap;    
	AddressV = Wrap;
};

BlendState UIBlend
{    
	AlphaToCoverageEnable = FALSE;    
	BlendEnable[0] = TRUE;    
	SrcBlend = SRC_ALPHA;    
	DestBlend = INV_SRC_ALPHA;    
	BlendOp = ADD;    
	SrcBlendAlpha = ONE;    
	DestBlendAlpha = INV_SRC_ALPHA;    
	BlendOpAlpha = ADD;
	RenderTargetWriteMask[0] = 0x0F;
};

BlendState NoBlending
{    
	BlendEnable[0] = FALSE;    
	RenderTargetWriteMask[0] = 0x0F;
};

DepthStencilState DisableDepth
{    
	DepthEnable = false;
};

DepthStencilState EnableDepth
{    
	DepthEnable = true;
};

RasterizerState Rasterizer
{
    CullMode = NONE;
	MultisampleEnable = TRUE;
	AntialiasedLineEnable = FALSE;
};

struct VS_OUTPUT
{    
	float4 Pos : SV_POSITION;    
	float4 Dif : COLOR;    
	float2 Tex : TEXCOORD;
};

// 顶点着色器不执行透视除法，它只完成投影矩阵部分。
// 透视除法会随后由硬件完成
VS_OUTPUT VS( 
		float3 vPos : POSITION,              
		float4 Dif : COLOR,              
		float2 vTexCoord0 : TEXCOORD 
		)
{    
	VS_OUTPUT Output;   

    // 变换
	Output.Pos = mul(float4(vPos, 1.0f), g_orthMatrix);
	
	Output.Dif = Dif;    
	Output.Tex = vTexCoord0;    
	return Output;
}

// 带矩阵的映射
VS_OUTPUT VSMatrix( 
		float3 vPos : POSITION,              
		float4 Dif : COLOR,              
		float2 vTexCoord0 : TEXCOORD 
		)
{    
	VS_OUTPUT Output;    
	
	// 变换
	float4 fTrans = mul(float4(vPos, 1.0f), g_Matrix);    
	Output.Pos = mul(fTrans, g_orthMatrix);
	Output.Dif = Dif;    
	Output.Tex = vTexCoord0;    
	return Output;
}

// TODO: 考虑下使用clip能否使用剪裁功能
float4 PS( VS_OUTPUT In ) : SV_Target
{    
	return g_Texture.Sample( Sampler, In.Tex ) * In.Dif;
}

float4 PSUntex( VS_OUTPUT In ) : SV_Target
{    
	return In.Dif;
}

technique10 DrawTexture
{    
	pass P0    
	{        
		SetVertexShader( CompileShader( vs_4_0, VS() ) );        
		SetGeometryShader( NULL );        
		SetPixelShader( CompileShader( ps_4_0, PS() ) );        
		SetDepthStencilState( EnableDepth, 0 );        
		SetBlendState( UIBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );    
	}
}

// 矩阵绘制
technique10 DrawTextureMatrix
{    
	pass P0    
	{        
		SetVertexShader( CompileShader( vs_4_0, VSMatrix() ) );        
		SetGeometryShader( NULL );        
		SetPixelShader( CompileShader( ps_4_0, PS() ) );        
		SetDepthStencilState( DisableDepth, 0 );        
		SetBlendState( UIBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );    
		SetRasterizerState( Rasterizer );
	}
}

technique10 DrawRect
{    
	pass P0    
	{        
		SetVertexShader( CompileShader( vs_4_0, VS() ) );        
		SetGeometryShader( NULL );        
		SetPixelShader( CompileShader( ps_4_0, PSUntex() ) );        
		SetDepthStencilState( DisableDepth, 0 );        
		SetBlendState( UIBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );    
	}
}

// 类似于m_pDevice->ClearRenderTargetView功能，直接覆盖目标像素
technique10 FillRect
{    
	pass P0    
	{        
		SetVertexShader( CompileShader( vs_4_0, VS() ) );        
		SetGeometryShader( NULL );        
		SetPixelShader( CompileShader( ps_4_0, PSUntex() ) );        
		SetDepthStencilState( DisableDepth, 0 );        
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );    
	}
}


technique10 RestoreState
{    
	pass P0    
	{        
		SetDepthStencilState( EnableDepth, 0 );        
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );   
	}
}