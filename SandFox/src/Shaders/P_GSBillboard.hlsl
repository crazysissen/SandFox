struct GSIn
{
    float4 pos : SV_POSITION;
    float distance : DISTANCE;
    float size : SIZE;
};

struct GSOut
{
	float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

cbuffer ScaleInfo : register(b0)
{
    float2 scale;
};

[maxvertexcount(6)]
void main(
	point GSIn input[1],
	inout TriangleStream<GSOut> output
)
{
    float fromCenter = 0.5f / input[0].distance * input[0].size;

    GSOut tl = { float4(input[0].pos.x - fromCenter * scale.x, input[0].pos.y + fromCenter * scale.y, input[0].pos.z, input[0].pos.w), float2(0, 0) };
    GSOut tr = { float4(input[0].pos.x + fromCenter * scale.x, input[0].pos.y + fromCenter * scale.y, input[0].pos.z, input[0].pos.w), float2(1, 0) };
    GSOut br = { float4(input[0].pos.x + fromCenter * scale.x, input[0].pos.y - fromCenter * scale.y, input[0].pos.z, input[0].pos.w), float2(1, 1) };
    GSOut bl = { float4(input[0].pos.x - fromCenter * scale.x, input[0].pos.y - fromCenter * scale.y, input[0].pos.z, input[0].pos.w), float2(0, 1) };

    output.Append(tr);
    output.Append(br);
    output.Append(tl);
    output.Append(bl);
}