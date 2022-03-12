
struct PSIn
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

// Textures and sample information
Texture2D shaderTexture : register(t4);
SamplerState samplerState : register(s5);

float4 main(PSIn input) : SV_TARGET
{
	return shaderTexture.Sample(samplerState, input.uv);
}
