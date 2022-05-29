
#include "H_Constants.hlsli"

struct DSOut
{
    float4 clipPosition : SV_POSITION;
    float3 direction    : DIRECTION;
};

struct DSIn
{
    float Edges[3] : SV_TessFactor;
    float Inside : SV_InsideTessFactor;
};

struct Vertex
{
    float3 position : POSITION;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
};



// Tesselation info
cbuffer TesselationInfo : REGISTER_CBV_TESSELATION_INFO
{
    float nearTesselation; // Effectively the maximum amount of tesselation
    float interpolationFactor;
    float invNearSquared; // How close the object must be for maximum tesselation, squared inverse
    
    float overrideTesselation;
}

// Transform info
cbuffer ObjectInfo : REGISTER_CBV_OBJECT_INFO
{
    matrix world;
    matrix projection;
    
    float3 position;
    float distanceSquared;
};

cbuffer CameraInfo : REGISTER_CBV_CAMERA_INFO
{
    float3 viewerPosition;
};



float3 project(float3 position, float3 cornerPosition, float3 cornerNormal)
{
    return position - dot(position - cornerPosition, cornerNormal) * cornerNormal;
}



[domain("tri")]
DSOut main(
	DSIn input,
	float3 domain : SV_DomainLocation,
	const OutputPatch<Vertex, 3> patch)
{
    DSOut o;

    float3 phongPos = domain.x * patch[0].position + domain.y * patch[1].position + domain.z * patch[2].position;
    float3 phongNorm = domain.x * patch[0].normal.xyz + domain.y * patch[1].normal.xyz + domain.z * patch[2].normal.xyz;
    float2 phongUV = domain.x * patch[0].uv + domain.y * patch[1].uv + domain.z * patch[2].uv;
    
    float3 smoothPos =
        domain.x * project(phongPos, patch[0].position, normalize(patch[0].normal.xyz)) +
        domain.y * project(phongPos, patch[1].position, normalize(patch[1].normal.xyz)) +
        domain.z * project(phongPos, patch[2].position, normalize(patch[2].normal.xyz));
    
    float3 newPosition = mul(float4(lerp(phongPos, smoothPos, interpolationFactor), 1.0f), world).xyz;
    float3 newNormal = mul(float4(phongNorm, 0.0f), world).xyz;
    
    o.clipPosition = mul(float4(newPosition, 1.0f), projection);
    o.direction = reflect(normalize(newPosition - viewerPosition), newNormal.xyz) * 1000.0f;

    return o;
}
