#pragma once

#include "SandFoxCore.h"
#include "IndexBuffer.h"
#include "Texture.h"

namespace SandFox
{

	enum ModelLoader
	{
		ModelLoaderOBJ
	};

	struct ModelVertex
	{
		cs::Vec4 position;
		cs::Vec3 normal;
								PAD(1);
		cs::Vec2 uv;
	};

	struct ModelSubmesh
	{
		string name;

		uindex* indices;
		int indexCount;
		
		int materialIndex;
	};

	struct ModelMaterial
	{
		string name;

		int ambientMapIndex;
		int diffuseMapIndex;
		int specularMapIndex;
		int emissiveMapIndex;
		int exponentMapIndex;
	};

	struct FOX_API Model
	{
		// Functions

		Model();
		Model(const wchar_t* path, ModelLoader type = ModelLoaderOBJ);
		Model(const Model&);
		virtual ~Model();

		Model& operator=(const Model&);

		bool Load(const wchar_t* path, ModelLoader type = ModelLoaderOBJ);
		void Unload();



		// Data

		bool loaded;
		int* referenceCounter;

		ModelVertex* vertices;
		ModelSubmesh* submeshes;
		ModelMaterial* materials;
		Texture* textures;
		int vertexCount;
		int submeshCount;
		int materialCount;
		int textureCount;

	};

}
