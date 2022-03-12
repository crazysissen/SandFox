
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

cbuffer TextureInfo : register(b3)
{
    float2 uvScale;
}

// Textures and sample information
Texture2D shaderTexture : register(t4);
SamplerState samplerState : register(s5);





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