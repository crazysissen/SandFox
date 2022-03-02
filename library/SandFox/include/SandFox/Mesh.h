#pragma once

#include "SandFoxCore.h"
#include "IndexBuffer.h"
#include "Texture.h"

namespace SandFox
{

	enum MeshLoader
	{
		MeshLoaderOBJ
	};

	struct MeshVertex
	{
		cs::Vec3 position;
		cs::Vec3 normal;
		cs::Vec2 uv;
	};

	struct MeshSubmesh
	{
		string name;

		uindex* indices;
		int indexCount;
		
		int materialIndex;
	};

	struct MeshMaterial
	{
		string name;

		int ambientMapIndex;
		int diffuseMapIndex;
		int specularMapIndex;
		int emissiveMapIndex;
		int exponentMapIndex;
	};

	struct FOX_API Mesh
	{
		// Functions

		Mesh();
		Mesh(const wchar_t* path, MeshLoader type = MeshLoaderOBJ);
		Mesh(const Mesh&);
		virtual ~Mesh();

		Mesh& operator=(const Mesh&);

		bool Load(const wchar_t* path, MeshLoader type = MeshLoaderOBJ);
		void Unload();



		// Data

		bool loaded;
		int* referenceCounter;

		MeshVertex* vertices;
		MeshSubmesh* submeshes;
		MeshMaterial* materials;
		Texture* textures;
		int vertexCount;
		int submeshCount;
		int materialCount;
		int textureCount;

	};

}
