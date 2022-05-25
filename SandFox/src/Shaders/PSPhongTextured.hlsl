
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
Texture2D shaderTexture : REGISTER_SRV_TEX_COLOR;

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
    
    float2 uvScale;
}



// Main function
float4 main(PSIn input) : SV_TARGET
{
	float4 color = shaderTexture.Sample(samplerState, input.uv * uvScale);

    float3 position = input.position.xyz;
	float3 normal = normalize(input.normal.xyz);

    float3 light = ambient * materialAmbient;

    for (int i = 0; i < lightCount; i++)
    {
        light += phong(lights[i], viewerPosition, position, normal, materialDiffuse, materialSpecular, materialShininess);
    }

    return saturate(color * float4(light, 1.0f));
}