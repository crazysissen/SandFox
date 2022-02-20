
#define LIGHT_CAPACITY 6



// Input struct
struct PSIn
{
	float4 viewPosition : SV_POSITION;
	float3 position : WORLD_POSITION;
	float4 normal : NORMAL;
	float2 uv : TEXCOORD;
};

// Light struct
struct Light
{
    float3 position;
    float intensity;

    float3 diffuse;
    float3 specular;
};

cbuffer SceneInfo : register(b1)
{
	float3 viewerPosition;
	float3 ambient;

    int lightCount;
    Light lights[LIGHT_CAPACITY];
};

cbuffer MaterialInfo : register(b2)
{
	float3 materialAmbient;
	float3 materialDiffuse;
	float3 materialSpecular;
    float materialShininess;
}

// Textures and sample information
Texture2D shaderTexture : register(t4);
SamplerState samplerState : register(s5);



float3 phong(Light l, float3 position, float3 N)
{
    float3 L = l.position - position; // Direction towards light
    float distInv = 1.0f / length(L);
    L *= distInv;

    float nDotL = dot(N, L);

	if (nDotL < 0.0f)
    {
        return float3(0.0f, 0.0f, 0.0f);
    }

    float3 R = 2 * N * nDotL - L; // Direction of reflected light
    float3 V = normalize(viewerPosition - position); // Direction towards viewer
    float rDotV = dot(R, V);

    float3 diffuse = materialDiffuse * l.diffuse * nDotL;
    float3 specular = max(materialSpecular * l.specular * pow(rDotV, materialShininess), float3(0, 0, 0));

    return l.intensity * (diffuse + specular) * distInv * distInv;
}



// Main function
float4 main(PSIn input) : SV_TARGET
{
	float4 color = shaderTexture.Sample(samplerState, input.uv);

    float3 position = input.position.xyz;
	float3 normal = normalize(input.normal.xyz);

    float3 light = ambient * materialAmbient;

    for (int i = 0; i < lightCount; i++)
    {
        light += phong(lights[i], position, normal);
    }

    return saturate(color * float4(light, 1.0f));
}