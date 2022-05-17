#pragma once

#include "SandFoxCore.h"
#include "Texture.h"

namespace SandFox
{

	class FOX_API TexSRVArray : public BindableResource
	{
	public:
		TexSRVArray();
		TexSRVArray(RegSRV reg, Texture* textures, int count, bool ownTextures, int formatOverride = -1);
		~TexSRVArray();

		void Load(RegSRV reg, Texture* textures, int count, bool ownTextures, int formatOverride = -1);

		void Bind(BindStage stage = BindStageNone) override;
		BindType Type() override;

	private:
		int m_count;
		bool m_owner;
		Texture* m_textures;
		ID3D11ShaderResourceView** m_srvs;
	};

}

