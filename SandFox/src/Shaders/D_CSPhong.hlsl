
#include "H_PhongAlg.hlsli"

RWTexture2D<float4> tTarget  : register(u0);

Texture2D tPosition     : register(t1);
Texture2D tNormal       : register(t2);

Texture2D tAmbient     : register(t3);
Texture2D tDiffuse     : register(t4);
Texture2D tSpecular    : register(t5);
Texture2D tExponent    : register(t6);

SamplerState samplerState   : register(s8);



#define LIGHT_CAPACITY 16

cbuffer SceneInfo : register(b10)
{
    float3 viewerPosition;
    float3 ambient;

    int lightCount;
    Light lights[LIGHT_CAPACITY];
};

cbuffer ClientInfo : register(b11)
{
    uint2 screen;
    float2 screenInverse;
    float exponent;
}





[numthreads(1, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    float2 uv = float2(id.x * screenInverse.x, id.y * screenInverse.y);

    float3 position = tPosition.SampleLevel(samplerState, uv, 0).xyz;
    float3 normal = tNormal.SampleLevel(samplerState, uv, 0).xyz;

    float4 diffuseSample = tDiffuse.SampleLevel(samplerState, uv, 0);
    
    if (diffuseSample.w != 0)
    {
        float3 ambientSample = tAmbient.SampleLevel(samplerState, uv, 0).xyz;
        float3 specularSample = tSpecular.SampleLevel(samplerState, uv, 0).xyz;
        float exponentSample = tExponent.SampleLevel(samplerState, uv, 0).x;

        float3 color = ambient * ambientSample;

        for (int i = 0; i < lightCount; i++)
        {
            color += phong(
                lights[i],
                viewerPosition,
                position,
                normal,
                diffuseSample.xyz,
                specularSample,
                exponentSample
            );
        }

        tTarget[id.xy] = saturate(float4(color, 1.0f));
    }
}