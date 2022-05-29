
#include "H_Constants.hlsli"

// Input struct
struct VSIn
{
    float3 position : POSITION;
    float4 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    float3 tangent : TANGENT0;
};



// Transform info
cbuffer CBuf : REGISTER_CBV_OBJECT_INFO
{
    matrix world;
    matrix projection;
};



float4 main(VSIn input) : SV_POSITION
{
    return mul(mul(float4(input.position, 1.0f), world), projection);
}
