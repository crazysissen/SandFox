cbuffer CBuf
{
	matrix transform;
};

struct VSIn
{
	float3 pos : POSITION;
	float3 worldPosition : CHUNK_POSITION;
	float4 normal : NORMAL0;
	float4 texCoord : TEXCOORD0;
};

struct VSOut
{
	float4 position : SV_POSITION;
	float3 worldPosition : POSITION;
	float4 normal : NORMAL0;
	float2 texCoord : TEXCOORD0;
};

// BlockData uint is two 8 bit unsigned integers, first block id, then block side (0 = top, 1 = side, 2 = bottom)

VSOut main(VSIn input)
{
	VSOut o;

	o.position = mul(float4(input.pos, 1.0f), transform);
	o.worldPosition = input.pos + input.worldPosition;
	o.normal = input.normal;
	o.texCoord = input.texCoord;

	return o;
}