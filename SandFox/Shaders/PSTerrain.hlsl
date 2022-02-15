struct PSIn
{
	float4 position : SV_POSITION;
	float3 worldPosition : POSITION;
	float4 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};

struct Light
{
	float3 position, color;
	float intensity;
};

struct EdgeDistance 
{ 
	float dist; 
	int side;
};

cbuffer EdgeBuffer : register(b1)
{
	int3 edges;
	float dist;
};

cbuffer LightBuffer : register(b2)
{
	struct
	{
		float4 color;
		float3 position;
		float intensity;
	} lights[49];

	uint lightCount;
	float normalStrength, ambient, saturation;
}


EdgeDistance closestEdgeDistance(float3 position)
{
	float currentClosest = 100000.0f;

	float dists[6] =
	{
		position.x, position.y, position.z,
		edges.x - position.x, edges.y - position.y, edges.z - position.z
	};

	int closestSide = 0; 

	for (int i = 0; i < 6; ++i)
	{
		if (dists[i] < currentClosest)
		{
			currentClosest = dists[i];
			closestSide = i;
		}
	}

	EdgeDistance ed;
	ed.dist = currentClosest;
	ed.side = closestSide;

	return ed;
}

Texture2D shaderTexture : register(t4);
Texture2D normalTexture : register(t6);
SamplerState sampleType : register(s5); 

float4 main(PSIn psIn) : SV_TARGET
{
	float4 color = sqrt(shaderTexture.Sample(sampleType, psIn.texCoord));
	float4 normalColor = sqrt(normalTexture.Sample(sampleType, psIn.texCoord)) * 2 - float4(1, 1, 1, 1);



	float3 normal = psIn.normal;
	float3 sampleNormal = normalize(float3(

		normalColor.z * normal.x + 
		normalColor.x * (normal.z - abs(normal.y)),

		normalColor.z * normal.y + 
		normalColor.y * (abs(normal.x) + abs(normal.z)), 

		normalColor.z * normal.z - 
		normalColor.x * normal.x + 
		normalColor.y * normal.y));

	sampleNormal = lerp(normal, sampleNormal, normalStrength);

	float4 ambientColor = color * float4(ambient, ambient, ambient, 1.0f),
		exclMultiplier = color - ambientColor;



	color = ambientColor;
	float4 additionalColor = 0;

	for (int i = 0; i < lightCount; ++i)
	{
		float3 posDiff = lights[i].position - psIn.worldPosition;
		float3 direction = normalize(posDiff);
		float normalDist = distance(direction, sampleNormal);

		if (normalDist > 1.41421f/* || distance(direction, normal) > 1.41421f*/)
		{
			continue;
		}

		float leng = length(posDiff);
		float lerp = cos(normalDist * 1.11072f);

		float3 tempColor = lights[i].color * (lights[i].intensity / /*(leng * leng)*/pow(leng, 2.f));
		float tempSat = tempColor.x + tempColor.y + tempColor.z;
		additionalColor += float4(lerp * (tempSat > saturation ? tempColor * saturation / tempSat : tempColor), 0.0f)/*clamp( , 0, saturation)*/;
	}

	color += exclMultiplier * additionalColor;



	////float edgeDistance = edges.y - psIn.worldPosition.y;
	////if (edgeDistance < dist) // Near the edge
	////{
	////	float distNormalized = sin(edgeDistance / dist * 0.5f * 3.14159f);

	////	color = float4((float3)(color * distNormalized) + float3(1.0f, 1.0f, 0.9f) * (1 - distNormalized), 1.0f);
	////}



	return color;
}

