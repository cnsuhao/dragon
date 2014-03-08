// d3d10 shader file


///////////////////////////////////////////////////////////
//
//   FillRect
//
//////////////////////////////////////////////////////////
float4  g_FillRectColor = float4(1.0f, 0.0f, 0.0f, 0.0f);

float4  FillRectScreenVS (float4 Pos : POSITION) : SV_POSITION
{
    return Pos;
}
// float4  FillRectWorldVS (float4 Pos : POSITION) : SV_POSITION
// {
//     return Pos;
// }

float4  FillRectPS (float4 Pos : SV_POSITION) : SV_Target
{
    //return g_FillRectColor;
    return float4( 1.0f, 1.0f, 0.0f, 1.0f );    // Yellow, with Alpha = 1
}    

technique10  FillRectScreenRender
{
    pass P0
    {   
        SetVertexShader( CompileShader ( vs_4_0, FillRectScreenVS() ) );
        SetGeometryShader ( NULL );
        SetPixelShader( CompileShader ( ps_4_0, FillRectPS() ) );
    }
}
// technique10  FillRectWorldRender
// {
//     pass P0
//     {   
//         SetVertexShader( CompileShader ( vs_4_0, FillRectWorldVS() ) );
//         SetGeometryShader ( NULL );
//         SetPixelShader( CompileShader ( ps_4_0, FillRectPS() ) );
//     }
// }

///////////////////////////////////////////////////////////
//
//   
//
//////////////////////////////////////////////////////////