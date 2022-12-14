#pragma once

#include "SandFoxCore.h"
#include "Drawable.h"
#include "Transform.h"
#include "Texture.h"

#include <string>

namespace SandFox
{ 
	namespace Prim
	{

		class FOX_API TexturePlane : public Drawable<TexturePlane>, public IDrawable
		{
		public:
			TexturePlane(Transform t, std::wstring textureName, cs::Point tiling = { 1, 1 });

			void Draw() override;

			void SetUVScale(Vec2 scale);

		private:
			struct MaterialInfo
			{
				cs::Color ambient;		PAD(4, 0);
				cs::Color diffuse;		PAD(4, 1);
				cs::Color specular;
				float shininess;

				Vec2 uvScale;			PAD(8, 2);
			};

			MaterialInfo m_materialInfoData;
			Texture m_texture;
			Bind::ConstBuffer* m_materialInfo;
		};

	}
}
