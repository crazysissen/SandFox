
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
    float lifetime;
};



RWStructuredBuffer<Particle> particles : register(u0);
StructuredBuffer<ParticleData> particleData : register(t1);

cbuffer ComputeInfo : register(b2)
{
    float deltaTime;
    float maxLifetime;
};

cbuffer NoiseInfo : register(b3)
{
    float4 noise[4];
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
    p.position += newVelocity * deltaTime;

    particles[id.x] = p;
}