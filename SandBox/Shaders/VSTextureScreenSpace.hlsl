struct VSOut
{
	float2 texturePosition : TEXCOORD;
	float4 screenPosition : SV_POSITION;
};

VSOut main(float2 screenPosition : POSITION)
{
	VSOut o;
	o.texturePosition = float2(1 + screenPosition.x, 1 - screenPosition.y) * 0.5f;
	o.screenPosition = float4(screenPosition, 0, 1);
	
	return o;
}