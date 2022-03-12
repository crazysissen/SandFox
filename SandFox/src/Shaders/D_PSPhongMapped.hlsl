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

// Textures and sample information
SamplerState samplerState : register(s4);
Texture2D tAmbient : register(t5);
Texture2D tDiffuse : register(t6);
Texture2D tSpecular : register(t7);
Texture2D tExponent : register(t8);

PSOut main(PSIn input)
{
    PSOut o;

    o.position = float4(input.position.xyz, 1.0f);
    o.normal = float4(normalize(input.normal.xyz), 1.0f);

    o.ambient = tAmbient.Sample(samplerState, input.uv).xyzw;
    o.diffuse = tDiffuse.Sample(samplerState, input.uv).xyzw;
    o.specular = tSpecular.Sample(samplerState, input.uv).xyzw;
    o.exponent = tExponent.Sample(samplerState, input.uv).x;

    return o;
}