
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
    float3 direction    : DIRECTION;
};

// Transform info
cbuffer CBuf : REGISTER_CBV_OBJECT_INFO
{
    matrix world;
    matrix projection;
};

cbuffer CameraInfo : REGISTER_CBV_CAMERA_INFO
{
    float3 viewerPosition;
};



VSOut main(VSIn input)
{
    float4 wp     = mul(float4(input.position, 1.0f), world);
    float4 normal = mul(float4(input.normal.xyz, 0.0f), world);
    
    float3 incident = normalize(wp.xyz - viewerPosition);
    float3 reflection = reflect(incident, normal.xyz);
    
    VSOut o;
    o.clipPosition = mul(wp, projection);
    o.direction = reflection * 1000.0f;
    
    return o;
}