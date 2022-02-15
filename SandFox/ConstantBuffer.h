#pragma once

#include "IBindable.h"

namespace ibind
{
	template<typename T>
	class ConstBuffer : public IBindable
	{
	public:
		ConstBuffer(const T& constants, uint bufferIndex = 0, bool immutable = false);
		virtual ~ConstBuffer();

		void Update(const T& constants);

	protected:
		ComPtr<ID3D11Buffer> m_constantBuffer;
		uint m_index;
	};

	template<typename T>
	class ConstBufferV : public ConstBuffer<T>
	{
	public:
		ConstBufferV(const T& constants, uint bufferIndex = 0, bool immutable = false);

		void Bind() override;

	protected:
		using ConstBuffer<T>::m_constantBuffer;
		using ConstBuffer<T>::m_index;
	};

	template<typename T>
	class ConstBufferP : public ConstBuffer<T>
	{
	public:
		ConstBufferP(const T& constants, uint bufferIndex = 0, bool immutable = false);

		void Bind() override;

	protected:
		using ConstBuffer<T>::m_constantBuffer;
		using ConstBuffer<T>::m_index;
	};



	// ---------------------- Definitions

	template<typename T>
	inline ConstBuffer<T>::ConstBuffer(const T& constants, uint bufferIndex, bool immutable)
		:
		m_index(bufferIndex)
	{
		D3D11_BUFFER_DESC bDesc = {};
		bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bDesc.Usage = immutable ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DYNAMIC;
		bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bDesc.MiscFlags = 0u;
		bDesc.ByteWidth = sizeof(constants);
		bDesc.StructureByteStride = 0u;

		D3D11_SUBRESOURCE_DATA srData = {};
		srData.pSysMem = &constants;

		EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateBuffer(&bDesc, &srData, &m_constantBuffer));
	}

	template<typename T>
	inline ConstBuffer<T>::~ConstBuffer()
	{
	}

	template<typename T>
	inline void ConstBuffer<T>::Update(const T& constants)
	{
		D3D11_MAPPED_SUBRESOURCE msr = {};

		EXC_COMCHECKINFO(Graphics::Get().GetContext()->Map(m_constantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));

		memcpy(msr.pData, &constants, sizeof(constants));

		EXC_COMINFO(Graphics::Get().GetContext()->Unmap(m_constantBuffer.Get(), 0u));
	}

	template<typename T>
	inline ConstBufferV<T>::ConstBufferV(const T& constants, uint bufferIndex, bool immutable)
		:
		ConstBuffer<T>(constants, bufferIndex, immutable)
	{
	}

	template<typename T>
	inline void ConstBufferV<T>::Bind()
	{
		EXC_COMINFO(Graphics::Get().GetContext()->VSSetConstantBuffers(m_index, 1u, m_constantBuffer.GetAddressOf()));
	}

	template<typename T>
	inline ConstBufferP<T>::ConstBufferP(const T& constants, uint bufferIndex, bool immutable)
		:
		ConstBuffer<T>(constants, bufferIndex, immutable)
	{
	}

	template<typename T>
	inline void ConstBufferP<T>::Bind()
	{
		EXC_COMINFO(Graphics::Get().GetContext()->PSSetConstantBuffers(m_index, 1u, m_constantBuffer.GetAddressOf()));
	}

}