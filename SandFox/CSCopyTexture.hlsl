
RWTexture2D<float4> tTarget : register(u0);
Texture2D tSource : register(t0);

SamplerState samplerState : register(s8);
cbuffer ClientInfo : register(b11)
{
    uint2 screen;
    float2 screenInverse;
}

[numthreads(1, 1, 1)]
void main( uint3 id : SV_DispatchThreadID )
{
    float2 uv = float2(id.x * screenInverse.x, id.y * screenInverse.y);
    tTarget[id.xy] = float4(tSource.SampleLevel(samplerState, uv, 0));
}