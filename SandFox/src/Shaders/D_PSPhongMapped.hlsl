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

PSOut main(PSIn input)
{
    PSOut o;

    o.position = float4(input.position.xyz, 1.0f);
    o.normal = float4(normalize(input.normal.xyz), 1.0f);
    
    float4 albedoSample = tAlbedo.Sample(samplerState, input.uv).xyzw;

    o.ambient = albedoSample * float4(materialAmbient, 1.0f);
    o.diffuse = albedoSample * float4(materialDiffuse, 1.0f);
    o.specular = albedoSample * float4(materialSpecular, 1.0f);
    o.exponent = tExponent.Sample(samplerState, input.uv).x * materialShininess;

    return o;
}