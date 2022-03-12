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

[maxvertexcount(6)]
void main(
	point GSIn input[1],
	inout TriangleStream<GSOut> output
)
{
    float fromCenter = 0.5f / input[0].distance * input[0].size;

    GSOut tl = { float4(input[0].pos.x - fromCenter, input[0].pos.y + fromCenter, input[0].pos.z, input[0].pos.w), float2(0, 0) };
    GSOut tr = { float4(input[0].pos.x + fromCenter, input[0].pos.y + fromCenter, input[0].pos.z, input[0].pos.w), float2(1, 0) };
    GSOut br = { float4(input[0].pos.x + fromCenter, input[0].pos.y - fromCenter, input[0].pos.z, input[0].pos.w), float2(1, 1) };
    GSOut bl = { float4(input[0].pos.x - fromCenter, input[0].pos.y - fromCenter, input[0].pos.z, input[0].pos.w), float2(0, 1) };

    output.Append(tl);
    output.Append(tr);
    output.Append(br);
    output.Append(tl);
    output.Append(br);
    output.Append(bl);
}