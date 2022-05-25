
#include "H_Constants.hlsli"

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

    float4 exponent : SV_Target5;
};



SamplerState samplerState   : REGISTER_SAMPLER_STANDARD;
Texture2D tAlbedo           : REGISTER_SRV_TEX_COLOR;
Texture2D tExponent         : REGISTER_SRV_TEX_SPECULARITY;

cbuffer MaterialInfo        : REGISTER_CBV_MATERIAL_INFO
{
    float3 materialAmbient;
    float3 materialDiffuse;
    float3 materialSpecular;
    float materialShininess;
    
    float2 uvScale;
}



PSOut main(PSIn input)
{
    PSOut o;

    o.position = float4(input.position.xyz, 1.0f);
    o.normal = float4(normalize(input.normal.xyz), 1.0f);
    
    float4 albedoSample = tAlbedo.Sample(samplerState, input.uv * uvScale).xyzw;
    float exponentSample = materialShininess * tExponent.Sample(samplerState, input.uv * uvScale).x;
    
    o.ambient = albedoSample * float4(materialAmbient, 1.0f);
    o.diffuse = albedoSample * float4(materialDiffuse, 1.0f);
    o.specular = albedoSample * float4(materialSpecular, 1.0f);
    o.exponent = float4(exponentSample, exponentSample, exponentSample, 1.0f);

    return o;
}