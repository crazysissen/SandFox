#include "pch.h"
#include "ParticleStream.h"

#include "Window.h"
#include "BindHandler.h"

#include <algorithm>

SandFox::ParticleStream::ParticleStream(
	const SandFox::Transform& t, 
	const std::wstring& particleTexture, 
	const std::wstring& computeShader, 
	int particleDataSize, 
	int startCapacity, 
	float lifetime,
	float nearClip,
	float nearClipFeather)
	:
	transform(t),

	m_shader(Shader::Get(ShaderTypeParticleBasic)),
	m_texture(new Texture(particleTexture), RegSRVTexColor, true),
	m_tConstBuffer(transform),
	m_vertexBuffer(),
	m_scaleInfo(),
	m_indexBuffer(),

	m_particles(nullptr),
	m_particleData(nullptr),
	m_capacity(startCapacity + c_capacityBuffer),
	m_count(0),
	m_start(0),
	m_pDataSize(particleDataSize),
	m_pDataStructureSize(particleDataSize + sizeof(float)),
	m_lifetime(lifetime),
	m_nearClip(nearClip),
	m_nearClipFeather(nearClipFeather),

	m_computeShader(),
	m_particleBuffer(),
	m_particleDataBuffer(),
	m_computeInfo()

{
	BindHandler::ApplyPresetSampler(RegSamplerStandard, BindStagePS);



	ComPtr<ID3DBlob> blob;

	D3D11_INPUT_ELEMENT_DESC inputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SIZE", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};



	// Arrays
	m_particles = new Particle[m_capacity];
	m_particleData = new byte[m_capacity * m_pDataStructureSize];
	m_indices = new uindex[m_capacity];
	for (int i = 0; i < m_capacity; i++)
	{
		m_indices[i] = i;
	}

	// Index buffer
	m_indexBuffer.Load(m_indices, m_capacity, false);

	// Pixel buffer resources
	m_scaleInfo.Load(RegCBVSystem0, {}, sizeof(ScaleInfo), false);
	
	// Vertex buffer resources
	m_vertexBuffer.Load(m_particles, m_capacity, sizeof(Particle), true);

	// Compute buffer resources
	m_computeShader.Load(Graphics::Get().ShaderPath(computeShader));
	m_particleBuffer.Load(RegUAVDefault, nullptr, m_capacity, sizeof(Particle));
	m_particleDataBuffer.Load(RegSRVParticleData, nullptr, m_capacity, m_pDataStructureSize);
	m_computeInfo.Load(RegCBVSystem1, nullptr, sizeof(ComputeInfo), false);
}

SandFox::ParticleStream::~ParticleStream()
{
	delete[] m_particles;
	delete[] m_particleData;
	delete[] m_indices;
}

void SandFox::ParticleStream::Update(float dTime)
{
	int start = m_start;
	int count = m_count;

	// Update lifetimes
	for (int i = 0; i < count; i++)
	{
		float& lifetime = (float&)(m_particleData[(i + start) * m_pDataStructureSize + m_pDataSize]);

		lifetime += dTime;

		if (lifetime > m_lifetime)
		{
			m_start++;
			m_count--;

			//m_start += 1 * (lifetime > m_lifetime);
			//m_count -= 1 * (lifetime > m_lifetime);
		}
	}

	ComputeInfo computeInfo =
	{
		dTime,
		m_lifetime
	};

	// Bind compute info constant buffer
	m_computeInfo.Write(&computeInfo);
	m_computeInfo.Bind(BindStageCS);

	// Bind structured buffers
	m_particleBuffer.Write(m_particles + m_start, m_count * sizeof(Particle));
	m_particleBuffer.Bind(BindStageCS);

	m_particleDataBuffer.Write(m_particleData + m_start * m_pDataStructureSize, m_count * m_pDataStructureSize);
	m_particleDataBuffer.Bind(BindStageCS);

	// Run the compute shader
	m_computeShader.Dispatch(m_count / c_particlesPerThread, 1, 1);

	// Retrieve the particle data
	m_particleBuffer.Read(m_particles + m_start, m_count * sizeof(Particle));

	// Unbind the potentially large buffers from the GPU
	m_particleBuffer.Unbind(BindStageCS);
	m_particleDataBuffer.Unbind(BindStageCS);
}

void SandFox::ParticleStream::CreateParticle(const cs::Vec3& position, float size, const void* data)
{
	if (m_start + m_count >= m_capacity)
	{
		// Increase size
		if (m_start < c_capacityBuffer)
		{
			Particle* oldParticles = m_particles;
			byte* oldParticleData = m_particleData;
			uindex* oldIndices = m_indices;

			int newCapacity = (m_capacity - c_capacityBuffer) * 2 + c_capacityBuffer;

			m_particles = new Particle[newCapacity];
			m_particleData = new byte[newCapacity * m_pDataStructureSize];
			m_indices = new uindex[newCapacity];

			std::memcpy(m_particles, oldParticles + m_start, m_count * sizeof(Particle));
			std::memcpy(m_particleData, oldParticleData + m_start * m_pDataStructureSize, m_count * m_pDataStructureSize);
			std::memcpy(m_indices, oldIndices + m_start, m_count * sizeof(uindex));

			delete[] oldParticles;
			delete[] oldParticleData;
			delete[] oldIndices;

			m_vertexBuffer.Resize(newCapacity);
			m_particleBuffer.Resize(newCapacity);
			m_particleDataBuffer.Resize(newCapacity);
			m_indexBuffer.Resize(newCapacity);

			m_capacity = newCapacity;
			m_start = 0;

			for (int i = 0; i < m_capacity; i++)
			{
				m_indices[i] = i;
			}


			m_indexBuffer.Update(m_indices, m_capacity);
		}

		// Move buffer left 
		else
		{
			std::memmove(m_particles, m_particles + m_start, m_count * sizeof(Particle));
			std::memmove(m_particleData, m_particleData + m_start * m_pDataStructureSize, m_count * m_pDataStructureSize);

			m_start = 0;
		}
	}

	m_particles[m_start + m_count] = Particle
	{
		position,
		size
	};

	byte* dataTarget = &m_particleData[(m_start + m_count) * m_pDataStructureSize];

	memcpy(dataTarget, data, m_pDataSize);

	float& lifetime = (float&)*(dataTarget + m_pDataSize);

	lifetime = 0.0f;

	m_count++;
}

void SandFox::ParticleStream::SetLifetime(float lifetime)
{
	m_lifetime = lifetime;
}

void SandFox::ParticleStream::SetNearClip(float nearClip, float nearClipFeather)
{
	m_nearClip = nearClip;
	m_nearClipFeather = nearClipFeather;
}

void SandFox::ParticleStream::Draw()
{
	if (m_count == 0)
	{
		return;
	}

	DirectX::XMMATRIX transformationMatrix = transform.GetMatrix();
	DirectX::XMMATRIX projectionMatrix = Graphics::GetProjection();
	DirectX::XMMATRIX matrix = dx::XMMatrixTranspose // <- flips matrix cpu-side to make gpu calculations more efficient
	(
		transformationMatrix *
		projectionMatrix
	);

	float averageScreen = (Graphics::Get().GetWindow()->GetH() + Graphics::Get().GetWindow()->GetW()) * 0.5f;

	ScaleInfo scaleInfo =
	{
		{ averageScreen / Graphics::Get().GetWindow()->GetW(), averageScreen / Graphics::Get().GetWindow()->GetH() },
		m_nearClip,
		m_nearClipFeather
	};
	m_vertexBuffer.Update((void*)(m_particles + m_start), m_count);
	m_scaleInfo.Write(&scaleInfo);

	m_shader->Bind();
	m_vertexBuffer.Bind();
	m_indexBuffer.Bind();

	m_texture.Bind(BindStagePS);
	m_tConstBuffer.Bind(BindStageVS);
	m_scaleInfo.Bind(BindStageGS);

	Graphics::Get().SetDepthStencil(true, false);
	Graphics::Get().GetContext()->DrawIndexed(m_count, 0u, 0u);
	Graphics::Get().SetDepthStencil(true, true);
}