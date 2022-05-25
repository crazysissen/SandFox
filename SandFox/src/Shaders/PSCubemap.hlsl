
#include "H_Constants.hlsli"

// Input struct
struct PSIn
{
    float4 clipPosition : SV_POSITION;
    float3 relativePosition : RELATIVE_POSITION;
};

// Resources
SamplerState samplerState	: REGISTER_SAMPLER_STANDARD;
TextureCube cube			: REGISTER_SRV_CUBE_MAP;



float4 main(PSIn input) : SV_TARGET
{
    return cube.Sample(samplerState, input.relativePosition);
}