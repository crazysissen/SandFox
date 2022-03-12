#pragma once

#include "SandFoxCore.h"
#include "IBindable.h"

namespace SandFox
{
	namespace Bind
	{

		class Buffer : public IBindable
		{
		public:
			Buffer();
			Buffer(void* data, int size, int bufferIndex, bool dynamic, DXGI_FORMAT format, D3D11_BIND_FLAG bindFlag);

			void Load(void* data, int size, int bufferIndex);

			void Resize(int size);
			void Update(void* data, int size);
			void Read(void* destination, int size);

			virtual void Bind() = 0;
			virtual void LoadView() = 0;

		protected:
			int m_bufferIndex;

		private:
			ComPtr<ID3D11Buffer> m_buffer;
		};

		class BufferUAV : public Buffer
		{
		public:
			BufferUAV();
			BufferUAV(void* data, int size, int bufferIndex, bool dynamic, DXGI_FORMAT format);
			
			void Bind() override;
			void LoadView() override;

		private:
			ComPtr<ID3D11UnorderedAccessView> m_uav;
		};

		class BufferSRV : public Buffer
		{
		public:
			BufferSRV();
			BufferSRV(void* data, int size, int bufferIndex, bool dynamic, DXGI_FORMAT format);

			void Bind() override;
			void LoadView() override;

		private:
			ComPtr<ID3D11ShaderResourceView> m_srv;
		};

	}
}