
#include "H_Constants.hlsli"

struct VSOut
{
	float3 color : COLOR;
	float4 pos : SV_POSITION;
	//nointerpolation float3 normal : NORMAL;
};



cbuffer CBuf : REGISTER_CBV_OBJECT_INFO
{
	matrix transform;
};



VSOut main(float3 pos : POSITION, float3 color : COLOR)
{
	VSOut o;
	o.pos = mul(float4(pos, 1.0f), transform);
	o.color = color;
	return o;
}