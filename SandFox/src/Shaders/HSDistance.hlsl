
#include "H_Constants.hlsli"

struct Vertex
{
    float3 position : POSITION;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
};

struct HSOutput
{
	float Edges[3]	: SV_TessFactor;
	float Inside	: SV_InsideTessFactor;
};



cbuffer TesselationInfo : REGISTER_CBV_TESSELATION_INFO
{
    float nearTesselation;  // Effectively the maximum amount of tesselation
    float interpolationFactor;     
    float invNearSquared;   // How close the object must be for maximum tesselation, squared inverse
    
    float overrideTesselation;
}

cbuffer ObjectInfo		: REGISTER_CBV_OBJECT_INFO
{
    matrix world;
    matrix projection;
    
    float3 position;
    float distanceSquared;
};



// Constant function per patch
HSOutput mainPatch(
	InputPatch<Vertex, 3> patch,
	uint id : SV_PrimitiveID)
{
    HSOutput o;
    
    float tessFactor = 1.0f;
    
    if (overrideTesselation > 0.0f)
    {
        tessFactor = overrideTesselation;
    }
    else
    {
        // Inverse square law clamped between 1 and the maximum
        // Logarithm /2 because the inner value is ^2, per standard logarithmic rule
        tessFactor = max(nearTesselation - (log2(max(distanceSquared * invNearSquared, 1.0f)) * 0.5f), 1.0f);
    }

    [unroll] for (int i = 0; i < 3; i++)
    {
        o.Edges[i] = tessFactor;
    }
    
    o.Inside = tessFactor;

    return o;
}



// Main function per point/vertex
[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("mainPatch")]
Vertex main(
	InputPatch<Vertex, 3> patch,
	uint index : SV_OutputControlPointID,
	uint id : SV_PrimitiveID )
{
	return patch[index];
}
