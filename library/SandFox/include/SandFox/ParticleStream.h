#pragma once

#include "SandFoxCore.h"
#include "Graphics.h"

//#include "Drawable.h"
#include "Transform.h"
#include "ComputeShader.h"
#include "Buffer.h"
#include "Bindables.h"
#include "Shader.h"

#include <cstring>
#include <string>



//
// RW Structured buffer: Particle	[u0]
// 
// Structured buffer: ParticleData	[t1]
// 
// Constant buffer: Compute info	[b2]
// | int	- index offset
// | float	- delta time
// | float	- max lifetime
// 
// 
// 
// Particle:
// | 12 bytes	- float3 position
// | 4 bytes	- float size multiplier
// 
// ParticleData: 
// | X bytes	- particle data
// | 4 bytes	- float lifetime
//



namespace SandFox
{

	class FOX_API ParticleStream : public IDrawable
	{
	public:
		struct Particle
		{
			cs::Vec3 position;
			float size;
		};

	public:
		ParticleStream(const Transform& t, const std::wstring& particleTexture, const std::wstring& computeShader, int particleDataSize, int startCapacity, float lifetime, float nearClip = 0.0f, float nearClipFeather = 0.0f);
		~ParticleStream();

		void Update(float dTime);
		void CreateParticle(const cs::Vec3& position, float size, const void* data);
		void SetLifetime(float lifetime);
		void SetNearClip(float nearClip, float nearClipFeather);

		void Draw() override;

	public:
		Transform transform;

	private:
		struct ScaleInfo
		{
			cs::Vec2 scale;
			float nearClip;
			float nearClipFeather;
		};

		struct ComputeInfo
		{
			float deltaTime;
			float maxLifetime;
			PAD(8, 0);
		};

		static constexpr int c_capacityBuffer = 64;
		static constexpr int c_particlesPerThread = 32;

		Shader* m_shader;
		TextureSRV m_texture;
		Bind::VertexBuffer m_vertexBuffer;
		Bind::TConstBuffer m_tConstBuffer;
		Bind::ConstBuffer m_scaleInfo;
		Bind::IndexBuffer m_indexBuffer;

		Particle* m_particles;
		byte* m_particleData;
		uindex* m_indices;
		int m_capacity;
		int m_count;
		int m_start;
		int m_pDataSize;
		int m_pDataStructureSize;
		float m_lifetime;
		float m_nearClip;
		float m_nearClipFeather;

		ComputeShader m_computeShader;
		StructuredBufferUAV m_particleBuffer;
		StructuredBufferSRV m_particleDataBuffer;
		Bind::ConstBuffer m_computeInfo;

	};
	
}

