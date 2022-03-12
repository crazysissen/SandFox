#pragma once

#include "SandFoxCore.h"
#include "Drawable.h"
#include "Mesh.h"

namespace SandFox
{

	namespace Prim
	{

		class FOX_API MeshDrawable
		{
		public:
			MeshDrawable(Transform transform = Transform());
			MeshDrawable(Transform transform, const Mesh& mesh);
			~MeshDrawable();

			void Load(const Mesh& mesh);

			Transform GetTransform();
			void SetTransform(Transform t);

			void Draw();

		private:
			class SubmeshDrawable : public Drawable<SubmeshDrawable>
			{
			public:
				SubmeshDrawable(Transform t, Mesh* m, int index);

			private:
				Mesh* m_mesh;
				int m_index;
			};

		private:
			Mesh m_mesh;
			Transform m_transform;
			cs::List<SubmeshDrawable*> m_submeshes;
		};

	}

}
