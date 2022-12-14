
#include "H_PhongAlg.hlsli"
#include "H_Constants.hlsli"



// Input struct
struct PSIn
{
    float4 viewPosition : SV_POSITION;
    float3 position     : WORLD_POSITION;
    float4 normal       : NORMAL;
    float2 uv           : TEXCOORD;
    float3 tangent      : TANGENT;
};



SamplerState samplerState   : REGISTER_SAMPLER_STANDARD;
Texture2D tAlbedo           : REGISTER_SRV_TEX_COLOR;
Texture2D tExponent         : REGISTER_SRV_TEX_SPECULARITY;
Texture2D tNormal           : REGISTER_SRV_TEX_NORMAL;

cbuffer CameraInfo          : REGISTER_CBV_CAMERA_INFO
{
    float3 viewerPosition;
};

cbuffer LightInfo           : REGISTER_CBV_LIGHT_INFO
{
    float3 ambient;

    int totalLightCount;
    int shadowCount;
    
    float3 nul;
    
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
    float3 position = input.position.xyz;

    float3 albedoSample = tAlbedo.Sample(samplerState, input.uv * uvScale).xyz;
    float exponentSample = tExponent.Sample(samplerState, input.uv * uvScale).x;
    float3 normalSample = tNormal.Sample(samplerState, input.uv * uvScale).xyz;
    
    // Calculate normal
    float3 normal = normalize(input.normal.xyz);
    float3 tangent = normalize(input.tangent - dot(input.tangent, input.normal.xyz) * input.normal.xyz);
    float3 bitangent = cross(tangent, normal);
    float3x3 texSpace = float3x3(tangent, bitangent, normal);
    float3 newNormal = normalize(mul(2.0f * normalSample - float3(1.0f, 1.0f, 1.0f), texSpace));

    float3 color = ambient * materialAmbient * albedoSample;

    //[unroll(FOX_C_MAX_SHADOWS)]
    for (int i = 0; i < totalLightCount; i++)
    {
        color += phongShadowed(
            i,
            lights[i],
            viewerPosition,
            position,
            newNormal,
            normal,
            albedoSample * materialDiffuse,
            albedoSample * materialSpecular,
            exponentSample * materialShininess
        );
    }
    
    //[unroll(FOX_C_MAX_LIGHTS)]
    //for (int j = shadowCount; j < totalLightCount - shadowCount; j++)
    //{
    //    color += phong(
    //        lights[j],
    //        viewerPosition,
    //        position,
    //        normal,
    //        albedoSample * materialDiffuse,
    //        albedoSample * materialSpecular,
    //        exponentSample * materialShininess
    //    );
    //}

    return saturate(float4(color, 1.0f));
}