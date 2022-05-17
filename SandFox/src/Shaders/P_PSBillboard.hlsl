
#include "H_Constants.hlsli"

struct PSIn
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};



// Textures and sample information
Texture2D shaderTexture     : REGISTER_SRV_TEX_COLOR;
SamplerState samplerState   : REGISTER_SAMPLER_STANDARD;



float4 main(PSIn input) : SV_TARGET
{
	return shaderTexture.Sample(samplerState, input.uv);
}
