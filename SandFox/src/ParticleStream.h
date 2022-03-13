#pragma once

#include "SandFoxCore.h"
#include "Graphics.h"

#include "Shader.h"
#include "Drawable.h"
#include "Transform.h"
#include "TransformConstantBuffer.h"
#include "ConstantBuffer.h"
#include "ComputeShader.h"
#include "Buffer.h"

#include <cstring>
#include <string>



//
// Compute shader layout: 
// | X bytes	- T_particleData data
// | 4 bytes	- float lifetime
//



namespace SandFox
{
	namespace Prim
	{

		template<typename T_particleData>
		class FOX_API ParticleStream : public Drawable<ParticleStream<typename T_particleData>>
		{
		public:
			struct Particle
			{
				cs::Vec3 position;
				float size;
			};

			struct ParticleData
			{
				T_particleData data;
				float lifetime;
			};
			

		public:
			ParticleStream(Transform t, const std::wstring& particleTexture, const std::wstring& computeShader, int startCapacity, float lifetime);

			void Write(float dTime);
			void CreateParticle(const cs::Vec3& position, float size, const T_particleData& data);
			void SetLifetime(float lifetime);

			void Draw() override;

		private:
			struct CameraInfo
			{
				cs::Vec3 position;
				PAD(4, 0);
			};

			struct ComputeInfo
			{
				float deltaTime;
				PAD(12, 0);
			};

			static constexpr int c_capacityBuffer = 16;

			ComputeShader m_computeShader;
			Bind::ConstBufferP<CameraInfo> m_cameraInfo;
			Bind::ConstBufferC<ComputeInfo> m_computeInfo;
			StructuredBufferUAV m_particleBuffer;
			StructuredBufferSRV m_particleDataBuffer;

			Particle* m_particles;
			ParticleData* m_particleData;
			int m_capacity;
			int m_count;
			int m_start;

			

			float m_lifetime;
		};





		// Implementation

		
		template<typename T_particleData>
		ParticleStream<T_particleData>::ParticleStream(SandFox::Transform t, const std::wstring& particleTexture, const std::wstring& computeShader, int startCapacity, float lifetime)
			:
		Drawable<ParticleStream<T_particleData>>(t),
		m_computeShader(computeShader),
		m_cameraInfo(),
		m_particles(nullptr),
		m_particleData(nullptr),
		m_capacity(startCapacity + c_capacityBuffer),
		m_count(0),
		m_lifetime(lifetime)
		{
			if (Drawable<ParticleStream<T_particleData>>::StaticInitialization())
			{
				ComPtr<ID3DBlob> blob;

				D3D11_INPUT_ELEMENT_DESC inputElements[] =
				{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "SIZE", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
				};

				Drawable<ParticleStream<T_particleData>>::AddStaticBind(Shader::Get(ShaderTypeParticleBasic));

				// Light data blablabla.

				Drawable<ParticleStream<T_particleData>>::AddStaticBind(new SandFox::Bind::TextureBindable(particleTexture, 4u));
				Drawable<ParticleStream<T_particleData>>::AddStaticBind(new SandFox::Bind::SamplerState(5u, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP));
			}

			m_particles = new Particle[m_capacity];
			m_particleData = new ParticleData[m_capacity];

			m_cameraInfo.Load({}, 1, false);
			m_computeInfo.Load({}, 1, false);

			Drawable<ParticleStream<T_particleData>>::AddBind(&m_cameraInfo);
			Drawable<ParticleStream<T_particleData>>::AddBind(new SandFox::Bind::VertexBuffer(m_particles, 0));
			Drawable<ParticleStream<T_particleData>>::AddBind(new SandFox::Bind::TConstBuffer(*this));
		}

		template<typename T_particleData>
		void ParticleStream<T_particleData>::Write(float dTime)
		{
			for (int i = 0; i < m_count; i++)
			{
				m_particleData[i + m_start].lifetime += dTime;
				m_start += 1 * (m_particleData[i + m_start].lifetime > m_lifetime);
			}

			m_computeShader.Dispatch(m_count, 1, 1);
		}

		template<typename T_particleData>
		void ParticleStream<T_particleData>::CreateParticle(const cs::Vec3& position, float size, const T_particleData& data)
		{
			if (m_start + m_count >= m_capacity)
			{
				// Increase size
				if (m_start < c_capacityBuffer)
				{
					Particle* oldParticles = m_particles;
					ParticleData* oldParticleData = m_particleData;

					int newCapacity = (m_capacity - c_capacityBuffer) * 2 + c_capacityBuffer;

					m_particles = new Particle[newCapacity];
					m_particleData = new ParticleData[newCapacity];

					std::memcpy(m_particles, oldParticles + m_start, m_count * sizeof(Particle));
					std::memcpy(m_particleData, oldParticleData + m_start, m_count * sizeof(ParticleData));

					delete[] oldParticles;
					delete[] oldParticleData;

					m_capacity = newCapacity;
					m_start = 0;
				}

				// Move buffer left 
				else
				{
					std::memmove(m_particles, m_particles + m_start, m_count * sizeof(Particle));
					std::memmove(m_particleData, m_particleData + m_start, m_count * sizeof(ParticleData));

					m_start = 0;
				}
			}

			m_particles[m_start + m_count] = Particle
			{
				position,
				size
			};

			m_particleData[m_start + m_count] = ParticleData
			{
				data,
				0.0f
			};

			m_count++;
		}

		template<typename T_particleData>
		inline void ParticleStream<T_particleData>::SetLifetime(float lifetime)
		{
			m_lifetime = lifetime;
		}

		template<typename T_particleData>
		void ParticleStream<T_particleData>::Draw()
		{
			int indexCount = Drawable<ParticleStream<T_particleData>>::BindStatic();

			CameraInfo c =
			{
				Graphics::Get().GetCamera()->position
			};

			m_cameraInfo.Write(c);

			for (IBindable* b : *Drawable<ParticleStream<T_particleData>>::m_bindables)
			{
				b->Bind();
			}

			EXC_COMINFO(Graphics::Get().GetContext()->Draw(m_count, m_start));
		}

	}
}

