
#include "H_PhongAlg.hlsli"
#include "H_Constants.hlsli"

#define LIGHT_CAPACITY 16



// Input struct
struct PSIn
{
    float4 viewPosition : SV_POSITION;
    float3 position : WORLD_POSITION;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD;
};



SamplerState samplerState   : REGISTER_SAMPLER_STANDARD;
Texture2D tAlbedo           : REGISTER_SRV_TEX_COLOR;
Texture2D tExponent         : REGISTER_SRV_TEX_SPECULARITY;

cbuffer CameraInfo          : REGISTER_CBV_CAMERA_INFO
{
    float3 viewerPosition;
};

cbuffer LightInfo           : REGISTER_CBV_LIGHT_INFO
{
    float3 ambient;

    int lightCount;
    Light lights[FOX_C_MAX_LIGHTS];
};

cbuffer MaterialInfo        : REGISTER_CBV_MATERIAL_INFO
{
    float3 materialAmbient;
    float3 materialDiffuse;
    float3 materialSpecular;
    float materialShininess;
}



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