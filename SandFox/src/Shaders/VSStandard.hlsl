
#include "H_Constants.hlsli"

// Input struct
struct VSIn
{
    float3 position : POSITION;
    float4 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    float3 tangent : TANGENT0;
};

// Output struct
struct VSOut
{
    float4 clipPosition : SV_POSITION;
    float3 position : WORLD_POSITION;
    float4 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    float3 tangent : TANGENT0;
};



// Transform info
cbuffer ObjectInfo : REGISTER_CBV_OBJECT_INFO
{
    matrix world;
    matrix projection;
    
    float3 position;
    float distanceSquared;
};



VSOut main(VSIn input)
{
    VSOut o;

    float4 position = mul(float4(input.position, 1.0f), world);
    float4 normal = mul(float4(input.normal.xyz, 0.0f), world);

    o.clipPosition = mul(position, projection);
    o.position = position.xyz;
    o.normal = normal;
    //o.uv = float2(input.uv.x, 1 - input.uv.y);
    o.uv = input.uv;
    o.tangent = mul(float4(input.tangent, 0.0f), world).xyz;
    
    return o;
}