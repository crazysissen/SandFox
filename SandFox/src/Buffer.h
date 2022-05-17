#pragma once

#include "SandFoxCore.h"
#include "IBindable.h"

namespace SandFox
{

	class FOX_API StructuredBuffer : public BindableResource
	{
	public:
		StructuredBuffer();
		StructuredBuffer(char reg, void* data, int count, int structureSize, bool dynamic, D3D11_BIND_FLAG bindFlags);

		void LoadBuffer(char reg, void* data, int count, int structureSize, bool dynamic, D3D11_BIND_FLAG bindFlags);

		ComPtr<ID3D11Buffer> GetBuffer();

		virtual void Resize(int count);
		virtual void Write(void* data, int size) = 0;
		virtual void Unbind(BindStage stage) = 0;

	protected:
		int m_stride;

	private:
		ComPtr<ID3D11Buffer> m_buffer;
	};

	class FOX_API StructuredBufferUAV : public StructuredBuffer
	{
	public:
		StructuredBufferUAV();
		StructuredBufferUAV(RegUAV reg, void* data, int count, int structureSize);

		void Load(RegUAV reg, void* data, int count, int structureSize);
		void LoadUAV(int count, int structureSize);

		void Read(void* destination, int size);

		void Resize(int count) override;
		void Write(void* data, int size) override;
		void Unbind(BindStage stage) override;

		void Bind(BindStage stage) override;
		BindType Type() override;

	private:
		ComPtr<ID3D11Buffer> m_stageBuffer;
		ComPtr<ID3D11UnorderedAccessView> m_uav;
	};

	class FOX_API StructuredBufferSRV : public StructuredBuffer
	{
	public:
		StructuredBufferSRV();
		StructuredBufferSRV(RegSRV reg, void* data, int count, int structureSize);

		void Load(RegSRV reg, void* data, int count, int structureSize);
		void LoadSRV(int count, int structureSize);

		void Resize(int count) override;
		void Write(void* data, int size) override;
		void Unbind(BindStage stage) override;

		void Bind(BindStage stage) override;
		BindType Type() override;

	private:
		ComPtr<ID3D11ShaderResourceView> m_srv;
	};

}