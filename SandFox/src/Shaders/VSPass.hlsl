
struct Vertex
{
    float3 position : POSITION;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
};

Vertex main(Vertex v)
{
	return v;
}