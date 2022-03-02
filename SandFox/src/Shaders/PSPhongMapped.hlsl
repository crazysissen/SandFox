
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

// Textures and sample information
SamplerState samplerState : register(s4);
Texture2D ambientTexture : register(t5);
Texture2D diffuseTexture : register(t6);
Texture2D specularTexture : register(t7);
Texture2D exponentTexture : register(t8);



float3 phong(Light l, float3 position, float3 N, float3 mDiffuse, float3 mSpecular, float mExponent)
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

    float3 diffuse = mDiffuse * l.diffuse * nDotL;
    float3 specular = max(mSpecular * l.specular * pow(rDotV, mExponent), float3(0, 0, 0));

    return l.intensity * (diffuse + specular) * distInv * distInv;
}



// Main function
float4 main(PSIn input) : SV_TARGET
{
    float3 position = input.position.xyz;
    float3 normal = normalize(input.normal.xyz);

    float3 ambientSample = ambientTexture.Sample(samplerState, input.uv).xyz;
    float3 color = ambient * ambientSample;

    for (int i = 0; i < lightCount; i++)
    {
        color += phong(
            lights[i], 
            position, 
            normal, 
            diffuseTexture.Sample(samplerState, input.uv).xyz, 
            specularTexture.Sample(samplerState, input.uv).xyz,
            exponentTexture.Sample(samplerState, input.uv).x
        );
    }

    return saturate(float4(color, 1.0f));
}