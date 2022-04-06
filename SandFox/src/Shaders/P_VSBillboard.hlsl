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

cbuffer Matrices : register(b0)
{
    matrix clip;
};

cbuffer CameraInfo : register(b1)
{
    float3 cameraPosition;
}

VSOut main(VSIn input)
{
    VSOut o =
    {
        mul(float4(input.pos, 1.0f), clip),
        //distance(input.pos, cameraPosition),
        input.size
    };

    return o;
}