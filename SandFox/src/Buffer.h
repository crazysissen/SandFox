#pragma once

#include "SandFoxCore.h"
#include "IBindable.h"

namespace SandFox
{

	class FOX_API StructuredBuffer : public IBindable
	{
	public:
		StructuredBuffer();
		StructuredBuffer(void* data, int count, int structureSize, int bufferIndex, bool dynamic, D3D11_BIND_FLAG bindFlags);

		void LoadBuffer(void* data, int count, int structureSize, int bufferIndex, bool dynamic, D3D11_BIND_FLAG bindFlags);

		ComPtr<ID3D11Buffer> GetBuffer();

		virtual void Resize(int count);
		virtual void Write(void* data, int size) = 0;
		virtual void Bind() = 0;

	protected:
		int m_bufferIndex;

	private:
		ComPtr<ID3D11Buffer> m_buffer;
	};

	class FOX_API StructuredBufferUAV : public StructuredBuffer
	{
	public:
		StructuredBufferUAV();
		StructuredBufferUAV(void* data, int count, int structureSize, int bufferIndex);

		void Load(void* data, int count, int structureSize, int bufferIndex);
		void LoadUAV(int count, int structureSize);

		void Read(void* destination, int size);

		void Resize(int count) override;
		void Write(void* data, int size) override;
		void Bind() override;

	private:
		ComPtr<ID3D11Buffer> m_stageBuffer;
		ComPtr<ID3D11UnorderedAccessView> m_uav;
	};

	class FOX_API StructuredBufferSRV : public StructuredBuffer
	{
	public:
		StructuredBufferSRV();
		StructuredBufferSRV(void* data, int count, int structureSize, int bufferIndex);

		void Load(void* data, int count, int structureSize, int bufferIndex);
		void LoadSRV();

		void Resize(int count) override;
		void Write(void* data, int size) override;
		void Bind() override;

	private:
		ComPtr<ID3D11ShaderResourceView> m_srv;
	};

}