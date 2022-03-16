
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

struct PSOut
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;

    float4 ambient : SV_Target2;
    float4 diffuse : SV_Target3;
    float4 specular : SV_Target4;

    float exponent : SV_Target5;
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


PSOut main(PSIn input)
{
    float3 color = shaderTexture.Sample(samplerState, input.uv).xyz;

    PSOut o;

    o.position = float4(input.position.xyz, 1.0f);
    o.normal = float4(normalize(input.normal.xyz), 1.0f);

    o.ambient = float4(color * materialAmbient, 1.0f);
    o.diffuse = float4(color * materialDiffuse, 1.0f);
    o.specular = float4(color * materialSpecular, 1.0f);
    o.exponent = materialShininess;

    return o;
}