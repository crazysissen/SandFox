SamplerState sampleType : register(s1);
Texture2D tex : register(t2);

float4 main(float2 texturePosition : TEXCOORD) : SV_TARGET
{
	int w = 1920, h = 1080;

	int x = w * texturePosition.x, y = h * texturePosition.y;

	//if (x % 50 < 10 != y % 50 < 10)
	//{
	//	return float4(1, 0, 0, 1);
	//}

	return tex.Sample(sampleType, texturePosition);
}