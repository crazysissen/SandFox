
#include "H_Constants.hlsli"

// Output struct
struct VSOut
{
    float4 clipPosition : SV_POSITION;
    float3 relativePosition : RELATIVE_POSITION;
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



VSOut main(float3 pos : POSITION)
{
    float4 wp = float4(pos + viewerPosition, 1.0f);
    
    VSOut o;
    o.clipPosition = mul(wp, projection);
    o.relativePosition = wp.xyz - viewerPosition;
    
    return o;
}