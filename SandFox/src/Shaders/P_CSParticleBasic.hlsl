
#define PARTICLES_PER_THREAD 1



struct Particle
{
    float3 position;
    float size;
};

struct ParticleData
{
    float4 noiseScalar;
    float3 velocity;
    float dampening;
    float3 acceleration;
    float size;
    float lifetime;
};



RWStructuredBuffer<Particle> particles : register(u0);
StructuredBuffer<ParticleData> particleData : register(t1);

cbuffer ComputeInfo : register(b2)
{
    float deltaTime;
    float maxLifetime;
};

cbuffer UpdateInfo : register(b3)
{
    float4 noise[4];
    float3 cameraPosition;
}



[numthreads(PARTICLES_PER_THREAD, 1, 1)]
void main( uint3 id : SV_DispatchThreadID )
{
    ParticleData d = particleData[id.x];

    float3 newVelocity =
        noise[0].xyz * d.noiseScalar.x +
        noise[1].xyz * d.noiseScalar.y +
        noise[2].xyz * d.noiseScalar.z +
        noise[3].xyz * d.noiseScalar.w +
        d.velocity +
        d.acceleration * d.lifetime;

    newVelocity *= pow(abs(1.0f - d.dampening), d.lifetime);
    
    Particle p = particles[id.x];
    
    //float3 posDiff = p.position - cameraPosition;
    //float lenSq = posDiff.x * posDiff.x + posDiff.y * posDiff.y + posDiff.z * posDiff.z - 4.0f;
    //float sizeMod = clamp(lenSq, 0.0f, 1.0f);
    
    p.size = d.size/* * sizeMod*/;
    p.position += newVelocity * deltaTime;

    particles[id.x] = p;
}