#pragma once

#include "SandFoxCore.h"
#include "Graphics.h"

#include "Shader.h"
#include "Drawable.h"
#include "Transform.h"
#include "TransformConstantBuffer.h"
#include "ConstantBuffer.h"
#include "ComputeShader.h"

#include <string>

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
			

		public:
			ParticleStream(Transform t, const std::wstring& particleTexture, const std::wstring& computeShader, int startCapacity);

			void CreateParticle(const cs::Vec3& position, float size, const T_particleData& data);

			void Draw() override;

		private:
			struct CameraInfo
			{
				cs::Vec3 position;
				PAD(4, 0);
			};

			ComputeShader m_computeShader;
			Bind::ConstBuffer<CameraInfo> m_cameraInfo;

			Particle* m_particles;
			T_particleData* m_particleData;
			int m_capacity;
			int m_count;
		};





		// Implementation

		
		template<typename T_particleData>
		ParticleStream<T_particleData>::ParticleStream(SandFox::Transform t, const std::wstring& particleTexture, const std::wstring& computeShader, int startCapacity)
			:
		Drawable(t),
		m_computeShader(computeShader),
		m_cameraInfo(),
		m_particles(nullptr),
		m_particleData(nullptr),
		m_capacity(startCapacity),
		m_count(0)
		{
			if (Drawable::StaticInitialization())
			{
				ComPtr<ID3DBlob> blob;

				D3D11_INPUT_ELEMENT_DESC inputElements[] =
				{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "SIZE", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
				};

				Drawable::AddStaticBind(Shader::Get(ShaderTypeParticleBasic));

				// Light data blablabla.

				Drawable::AddStaticBind(new SandFox::Bind::TextureBindable(particleTexture, 4u));
				Drawable::AddStaticBind(new SandFox::Bind::SamplerState(5u, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP));
			}

			m_particles = new Particle[startCapacity];
			m_particleData = new T_particleData[startCapacity];

			Drawable::AddBind(m_cameraInfo);
			Drawable::AddBind(new SandFox::Bind::VertexBuffer(m_particles, 0));
			Drawable::AddBind(new SandFox::Bind::TConstBuffer(*this));
		}

		template<typename T_particleData>
		void ParticleStream<T_particleData>::Draw()
		{
			int indexCount = Drawable::BindStatic();

			CameraInfo c =
			{
				Graphics::Get().GetCamera()->position
			};

			m_cameraInfo->Update(c);

			for (IBindable* b : *Drawable::m_bindables)
			{
				b->Bind();
			}

			EXC_COMINFO(Graphics::Get().GetContext()->Draw(m_count, 0u));
		}

	}
}

