
#include "H_Constants.hlsli"

struct DSOut
{
    float4 clipPosition : SV_POSITION;
    float3 position : WORLD_POSITION;
    float4 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    float3 tangent : TANGENT0;
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

    float3 phongPos =   domain.x * patch[0].position +      domain.y * patch[1].position +      domain.z * patch[2].position;
    float3 phongNorm =  domain.x * patch[0].normal.xyz +    domain.y * patch[1].normal.xyz +    domain.z * patch[2].normal.xyz;
    float2 phongUV =    domain.x * patch[0].uv +            domain.y * patch[1].uv +            domain.z * patch[2].uv;
    float3 phongTang =  domain.x * patch[0].tangent.xyz +   domain.y * patch[1].tangent.xyz +   domain.z * patch[2].tangent.xyz;
    
    float3 smoothPos =
        domain.x * project(phongPos, patch[0].position, normalize(patch[0].normal.xyz)) +
        domain.y * project(phongPos, patch[1].position, normalize(patch[1].normal.xyz)) +
        domain.z * project(phongPos, patch[2].position, normalize(patch[2].normal.xyz));
    
    o.position =        mul(float4(lerp(phongPos, smoothPos, interpolationFactor), 1.0f), world).xyz;
    o.clipPosition =    mul(float4(o.position, 1.0f), projection);
    o.normal =          mul(float4(phongNorm, 0.0f), world);
    o.uv =              /*float2(phongUV.x, 1 - phongUV.y);*/phongUV;
    o.tangent =         mul(float4(phongTang, 0.0f), world).xyz;

	return o;
}
