// Input struct
struct VSIn
{
	float3 position : POSITION;
	float4 normal : NORMAL0;
	float2 uv : TEXCOORD0;
};

// Output struct
struct VSOut
{
	float4 clipPosition : SV_POSITION;
	float3 position : WORLD_POSITION;
	float4 normal : NORMAL0;
	float2 uv : TEXCOORD0;
};

// Transform info
cbuffer CBuf
{
	matrix world;
	matrix projection;
};

VSOut main(VSIn input)
{
	VSOut o;

	float4 position = mul(float4(input.position, 1.0f), world);
    float4 normal = mul(float4(input.normal.xyz, 0.0f), world);

	o.clipPosition = mul(position, projection);
	o.position = position.xyz;
	o.normal = normal;
	o.uv = input.uv;

	return o;
}