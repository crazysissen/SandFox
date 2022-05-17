
#include "H_Constants.hlsli"

RWTexture2D<float4> tTarget : REGISTER_UAV_DEFAULT;
Texture2D tSource           : REGISTER_SRV_COPY_SOURCE;

SamplerState samplerState   : REGISTER_SAMPLER_STANDARD;

cbuffer ClientInfo          : REGISTER_CBV_CLIENT_INFO
{
    uint2 screen;
    float2 screenInverse;
    float exponent;
}

[numthreads(1, 1, 1)]
void main( uint3 id : SV_DispatchThreadID )
{
    float2 uv = float2(id.x * screenInverse.x, id.y * screenInverse.y);
    float4 smp = float4(tSource.SampleLevel(samplerState, uv, 0));

    if (exponent != 0.0f)
    {
        float expSmp = pow(smp.x, exponent);
        tTarget[id.xy] = float4(expSmp, expSmp, expSmp, 1.0f);
    }
    else
    {
        tTarget[id.xy] = smp;
    }
}