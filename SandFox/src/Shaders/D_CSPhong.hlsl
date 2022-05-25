
#include "H_PhongAlg.hlsli"
#include "H_Constants.hlsli"

RWTexture2D<float4> tTarget     : REGISTER_UAV_DEFAULT;

Texture2D tPosition             : REGISTER_SRV_DEF_POSITION;
Texture2D tNormal               : REGISTER_SRV_DEF_NORMAL;

Texture2D tAmbient              : REGISTER_SRV_DEF_AMBIENT;
Texture2D tDiffuse              : REGISTER_SRV_DEF_DIFFUSE;
Texture2D tSpecular             : REGISTER_SRV_DEF_SPECULAR;
Texture2D tExponent             : REGISTER_SRV_DEF_EXPONENT;

SamplerState samplerState : REGISTER_SAMPLER_STANDARD;



cbuffer CameraInfo              : REGISTER_CBV_CAMERA_INFO
{
    float3 viewerPosition;
};

cbuffer LightInfo               : REGISTER_CBV_LIGHT_INFO
{
    float3 ambient;
    int lightCount;
    
    
    Light lights[FOX_C_MAX_LIGHTS];
};

cbuffer ClientInfo              : REGISTER_CBV_CLIENT_INFO
{
    uint2 screen;
    float2 screenInverse;
    float exponent;
}





[numthreads(FOX_C_DEFERRED_PER_THREAD_X, FOX_C_DEFERRED_PER_THREAD_Y, 1)]
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

        for (int i = 0; i < FOX_C_MAX_LIGHTS; i++)
        {
            if (i < lightCount)
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
        }

        tTarget[id.xy] = saturate(float4(color, 1.0f));
    }
}