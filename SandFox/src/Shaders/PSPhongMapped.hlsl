
#include "H_PhongAlg.hlsli"

#define LIGHT_CAPACITY 16



// Input struct
struct PSIn
{
    float4 viewPosition : SV_POSITION;
    float3 position : WORLD_POSITION;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD;
};

cbuffer SceneInfo : register(b10)
{
    float3 viewerPosition;
    float3 ambient;

    int lightCount;
    Light lights[LIGHT_CAPACITY];
};

cbuffer MaterialInfo : register(b2)
{
    float3 materialAmbient;
    float3 materialDiffuse;
    float3 materialSpecular;
    float materialShininess;
}

// Textures and sample information
SamplerState samplerState : register(s4);
Texture2D tAlbedo : register(t5);
Texture2D tExponent : register(t6);



// Main function
float4 main(PSIn input) : SV_TARGET
{
    float3 position = input.position.xyz;
    float3 normal = normalize(input.normal.xyz);

    float3 albedoSample = tAlbedo.Sample(samplerState, input.uv).xyz;
    float exponentSample = tExponent.Sample(samplerState, input.uv).x;

    float3 color = ambient * materialAmbient * albedoSample;

    for (int i = 0; i < lightCount; i++)
    {
        color += phong(
            lights[i], 
            viewerPosition,
            position, 
            normal, 
            albedoSample * materialDiffuse,
            albedoSample * materialSpecular, 
            exponentSample * materialShininess
        );
    }

    return saturate(float4(color, 1.0f));
}