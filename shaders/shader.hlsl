//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-11-27 16:55:56
//

static const float3 POSITIONS[] = {
    float3(-0.5, -0.5, 0.0),
    float3( 0.5, -0.5, 0.0),
    float3( 0.0,  0.5, 0.0)
};

static const float3 COLORS[] = {
    float3( 1.0,  0.0, 0.0),
    float3( 0.0,  1.0, 0.0),
    float3( 0.0,  0.0, 1.0)
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float3 Color : COLOR;
};

VertexOutput VSMain(uint VertexID : SV_VertexID)
{
    VertexOutput Output = (VertexOutput)0;
    Output.Position = float4(POSITIONS[VertexID], 1.0);
    Output.Color = COLORS[VertexID];
    return Output;
}

float4 PSMain(VertexOutput Input) : SV_Target
{
    return float4(Input.Color, 1.0);
}
