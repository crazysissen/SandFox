#pragma once

#include "SandFoxCore.h"
#include "IBindable.h"

namespace SandFox
{
	namespace Bind
	{

		class StructuredBuffer : public IBindable
		{
		public:
			StructuredBuffer();
			StructuredBuffer(void* data, int count, int structureSize, int bufferIndex, bool dynamic, D3D11_BIND_FLAG bindFlags);

			void Load(void* data, int count, int structureSize, int bufferIndex, bool dynamic, D3D11_BIND_FLAG bindFlags);

			void Resize(int size);
			void Update(void* data, int size);
			void Read(void* destination, int size);

			ComPtr<ID3D11Buffer> GetBuffer();

			virtual void Bind() = 0;
			virtual void LoadView() = 0;

		protected:
			int m_bufferIndex;

		private:
			ComPtr<ID3D11Buffer> m_buffer;
		};

		class StructuredBufferUAV : public StructuredBuffer
		{
		public:
			StructuredBufferUAV();
			StructuredBufferUAV(void* data, int count, int structureSize, int bufferIndex, bool dynamic);
			
			void Bind() override;
			void LoadView() override;

		private:
			ComPtr<ID3D11UnorderedAccessView> m_uav;
		};

		class StructuredBufferSRV : public StructuredBuffer
		{
		public:
			StructuredBufferSRV();
			StructuredBufferSRV(void* data, int count, int structureSize, int bufferIndex, bool dynamic);

			void Bind() override;
			void LoadView() override;

		private:
			ComPtr<ID3D11ShaderResourceView> m_srv;
		};

	}
}