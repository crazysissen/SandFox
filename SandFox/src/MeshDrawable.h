#pragma once

#include "SandFoxCore.h"
#include "ConstantBuffer.h"
#include "Drawable.h"
#include "Mesh.h"

namespace SandFox
{

	namespace Prim
	{

		class FOX_API MeshDrawable : public IDrawable
		{
		public:
			MeshDrawable(Transform transform = Transform(), bool tesselation = false);
			MeshDrawable(Transform transform, const Mesh& mesh, bool tesselation = false);
			virtual ~MeshDrawable();

			void Draw() override;

			void Load(const Mesh& mesh);

			Transform GetTransform();
			void SetTransform(Transform t);
			void SetTesselation(bool tesselation);
			void SetUVScaleAll(Vec2 scale);

			bool GetTesselation();

		private:
			struct MaterialInfo
			{
				cs::Color ambient;		PAD(4, 0);
				cs::Color diffuse;		PAD(4, 1);
				cs::Color specular;		
				float shininess;	

				Vec2 uvScale;			PAD(8, 2);
			};

			class FOX_API SubmeshDrawable : public Drawable<SubmeshDrawable>, public IDrawable
			{
			public:
				SubmeshDrawable(Transform t, Mesh* m, int index);

				void Draw() override;

				void SetUVScale(Vec2 scale);

			private:
				Mesh* m_mesh;
				int m_index;
				Vec2 m_uvScale;
				Bind::ConstBuffer* m_materialInfo;
			};

		private:
			Mesh m_mesh;
			Transform m_transform;
			cs::List<SubmeshDrawable*> m_submeshes;
			bool m_tesselation;
		};

	}

}
