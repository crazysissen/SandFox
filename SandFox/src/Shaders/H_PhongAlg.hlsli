
#include "H_Constants.hlsli"

// Light struct
struct Light
{
    float3 position;
    int type;
    float3 direction;
    float spreadDotLimit;
    float3 color;
    float intensity;
};



Texture2D tLightMaps[FOX_C_MAX_LIGHTS] : REGISTER_SRV_SHADOW_DEPTH;





float3 phong(Light l, float3 viewerPosition, float3 position, float3 normal, float3 mDiffuse, float3 mSpecular, float mExponent)
{
    // Direction towards light
    float3 towardsLight = 
        (l.type == 0) * -(l.direction) +
        (l.type != 0) * (l.position - position);

    // Normalize the direction and save the inverse
    float distInv = 1.0f / length(towardsLight);
    towardsLight *= distInv;

    float distScalar =
        (l.type == 0) * 1 +
        (l.type != 0) * distInv * distInv;

    // Scalar by angle of incidence, where 1.0f is straight-on, and <=0.0f is parallell or behind 
    float incidence = dot(normal, towardsLight);

    float3 phongValue = float3(0, 0, 0);

    // Return no light for backsides of objects, or parts outside the spread of a spotlight
    if (!(incidence < 0.0f || (l.type == 2 && dot(-l.direction, towardsLight) < l.spreadDotLimit)))
    {
        // Direction of reflected light
        float3 reflection = 2 * normal * incidence - towardsLight;

        // Direction towards viewer
        float3 towardsViewer = normalize(viewerPosition - position);

        // Scalar indicating how close the reflected light is to hitting the camera
        float specularBase = max(0.0f, dot(reflection, towardsViewer));

        float3 diffuse = mDiffuse * l.color * incidence;
        float3 specular = max(mSpecular * l.color * pow(specularBase, mExponent), float3(0, 0, 0));

        phongValue =  l.intensity * (diffuse + specular) * distScalar;
    }

    return phongValue;
}