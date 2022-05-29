
#include "H_Constants.hlsli"

struct VSIn
{
    float3 pos : POSITION;
    float size : SIZE;
};

struct VSOut
{
    float4 clipPosition : SV_POSITION;
    //float distance : DISTANCE;
    float size : SIZE;
};



cbuffer ObjectInfo  : REGISTER_CBV_OBJECT_INFO
{
    matrix worldMatrix;
    matrix clipMatrix;
};

cbuffer CameraInfo  : REGISTER_CBV_CAMERA_INFO
{
    float3 cameraPosition;
}



VSOut main(VSIn input)
{
    VSOut o =
    {
        mul(float4(input.pos, 1.0f), clipMatrix),
        input.size
    };

    return o;
}