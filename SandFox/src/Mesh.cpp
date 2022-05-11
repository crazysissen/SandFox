#include "pch.h"
#include "Mesh.h"

#include <fstream>
#include <charconv>





// Loaders

inline int FastIndex(char* start, const char* end)
{
	uindex val = 0;
	while (start < end)
	{
		val = val * 10 + *start++ - '0';
	}
	return val;
}

int ReadFloats(char* start, int length, int maxNumbers, float* buffer) // <-- fast and branchless
{
	int breaks[] = { -1, length, length, length, length, -1};
	int breakIndex = 1;

	for (int i = 0; i < length; i++)
	{
		bool isDelimeter = (start[i] == ' ' || start[i] == '/');

		breaks[breakIndex] = i;
		breakIndex = breakIndex + isDelimeter;
	}

	int numbers = breakIndex ^ ((maxNumbers ^ breakIndex) & -(maxNumbers < breakIndex));
	breaks[numbers]++;

	for (int i = 0; i < numbers; i++)
	{
		std::from_chars(&(start[breaks[i] + 1]), &(start[breaks[i + 1]]), buffer[i]);
	}

	return numbers;
}

int ReadIndices(char* start, int length, int maxNumbers, uindex* buffer) // <-- fast and branchless
{
	int breaks[] = { -1, length, length, length, length, length, length, length, length, length, length, length, length, -1 };
	int breakIndex = 1;

	for (int i = 0; i < length && breakIndex <= maxNumbers; i++)
	{
		bool isDelimeter = (start[i] < '0');

		breaks[breakIndex] = i;
		breakIndex = breakIndex + isDelimeter;
	}

	int numbers = breakIndex ^ ((maxNumbers ^ breakIndex) & -(maxNumbers < breakIndex));
	breaks[numbers] += breakIndex <= maxNumbers;

	for (int i = 0; i < numbers; i++)
	{
		buffer[i] = FastIndex(&(start[breaks[i] + 1]), &(start[breaks[i + 1]])) - 1;
	}

	return numbers;
}


bool LoadOBJ(SandFox::Mesh& m, const wchar_t* path)
{
	FOX_TRACE_F("Loading Mesh object with OBJ-format at \"%ls\".", path);

	// File open

	std::ifstream fileObj(path);
	if (!fileObj.good())
	{
		FOX_ERROR("Could not open OBJ file.");
		return false;
	}



	// Structs

	struct Face
	{
		uindex indices[9]; // { vertex, texture, normal } * 3 consecutively
	};

	struct SubmeshPrimer
	{
		string name;
		string materialName;
		cs::List<Face> faces;
	};

	struct MaterialPrimer
	{
		string name;

		cs::Color ambient = cs::Color(0.0f, 0.0f, 0.0f);
		cs::Color diffuse = cs::Color(0.0f, 0.0f, 0.0f);
		cs::Color specular = cs::Color(0.0f, 0.0f, 0.0f);
		//cs::Color emissive = cs::Color(0.0f, 0.0f, 0.0f);
		float exponent = 1.0f;

		int albedoMap = -1;
		int exponentMap = -1;
	};



	// Initial loading of values

	cs::List<Vec3> vertices;
	cs::List<Vec3> normals;
	cs::List<Vec2> uvs;
	cs::List<string> mtlPaths;
	cs::List<SubmeshPrimer> submeshPrimers;
	cs::List<MaterialPrimer> materialPrimers;
	cs::List<string> textureStrings;

	cs::List<Face>* currentIndices = nullptr;

	string currentObject;
	int currentObjectIndex = 0;
	int currentTexture = 0;
	float maxDistanceSq = 0.0f;



	// Material/object file directory

	wstring dir = path;
	size_t begin = dir.find_last_of('/');
	if (begin == std::string::npos)
	{
		begin = dir.find_last_of('\\');
		if (begin == std::string::npos)
		{
			begin = 0;
		}
		else
		{
			begin++;
		}
	}
	else
	{
		begin++;
	}
	dir = dir.substr(0, begin);



	string line;
	float fBuffer[4] = { 0, 0, 0, 0 };
	uindex iBuffer[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };

	FOX_TRACE("Parsing OBJ file...");

#pragma region OBJ Reading
	while (std::getline(fileObj, line))
	{
		switch (line[0])
		{
			case 'v':
			{
				switch (line[1])
				{
					case ' ':
					{
						ReadFloats(&(line[2]), (int)line.length() - 2, 3, fBuffer);
						Vec3 vec(fBuffer[0], fBuffer[1], fBuffer[2]);
						vertices.Add(vec);

						maxDistanceSq = std::max(vec.LengthSq(), maxDistanceSq);

						// Following is for xyzw vectors

						/*fBuffer[3] = 1.0f;
						ReadFloats(&(line[2]), (int)line.length() - 2, 4, fBuffer);
						vertices.Add(Vec4(fBuffer[0], fBuffer[1], fBuffer[2], fBuffer[3]));*/
					}
					break;

					case 'n':
					{
						ReadFloats(&(line[3]), (int)line.length() - 3, 3, fBuffer);
						normals.Add(Vec3(fBuffer[0], fBuffer[1], fBuffer[2]));
					}
					break;

					case 't':
					{
						fBuffer[1] = 0.0f;
						ReadFloats(&(line[3]), (int)line.length() - 3, 2, fBuffer);
						uvs.Add(Vec2(fBuffer[0], fBuffer[1]));
					}
					break;
				}

			}
			break;

			case 'f':
			{
				int count = ReadIndices(&(line[2]), (int)line.length() - 2, 12, iBuffer);

				switch (count)
				{
					case 3:
						currentIndices->Add({ iBuffer[0], 0, 0, iBuffer[1], 0, 0, iBuffer[2], 0, 0 });
						break;

					case 4:
						currentIndices->Add({ iBuffer[0], 0, 0, iBuffer[1], 0, 0, iBuffer[2], 0, 0 });
						currentIndices->Add({ iBuffer[0], 0, 0, iBuffer[2], 0, 0, iBuffer[3], 0, 0 });
						break;

					case 6:
						currentIndices->Add({ iBuffer[0], iBuffer[1], 0, iBuffer[2], iBuffer[3], 0, iBuffer[4], iBuffer[5], 0 });
						break;

					case 8:
						currentIndices->Add({ iBuffer[0], iBuffer[1], 0, iBuffer[2], iBuffer[3], 0, iBuffer[4], iBuffer[5], 0 });
						currentIndices->Add({ iBuffer[0], iBuffer[1], 0, iBuffer[4], iBuffer[5], 0, iBuffer[6], iBuffer[7], 0 });
						break;

					case 9:
						currentIndices->Add({ iBuffer[0], iBuffer[1], iBuffer[2], iBuffer[3], iBuffer[4], iBuffer[5], iBuffer[6], iBuffer[7], iBuffer[8] });
						break;

					case 12:
						currentIndices->Add({ iBuffer[0], iBuffer[1], iBuffer[2], iBuffer[3], iBuffer[4], iBuffer[5], iBuffer[6], iBuffer[7], iBuffer[8] });
						currentIndices->Add({ iBuffer[0], iBuffer[1], iBuffer[2], iBuffer[6], iBuffer[7], iBuffer[8], iBuffer[9], iBuffer[10], iBuffer[11] });
						break;

					default:
						break;
				}
			}
			break;

			case 'm':
			{
				if (line.substr(0, 6) == "mtllib")
				{
					mtlPaths.Add(line.substr(7, line.length() - 7));
				}
			}
			break;

			case 'u':
			{
				if (line.substr(0, 6) == "usemtl")
				{
					submeshPrimers.Add({ currentObject + std::to_string(currentObjectIndex), line.substr(7, line.length() - 7), {} });
					currentIndices = &(submeshPrimers.Back().faces);
					currentObjectIndex++;
				}
			}
			break;

			case 'o':
			{
				currentObject = line.substr(2, line.length() - 2);
				currentObjectIndex = 0;
			}
			break;

			default:
				break;
		}
	}

	if (normals.Size() == 0)
	{
		normals.Add({ 0, 0, 0 });
	}

	if (uvs.Size() == 0)
	{
		uvs.Add({ 0, 0 });
	}
#pragma endregion

#pragma region MTL Reading

	FOX_TRACE("Parsing MTL file(s)...");

	for (int i = 0; i < mtlPaths.Size(); i++)
	{
		wstring mtlDir = dir + wstring(mtlPaths[i].begin(), mtlPaths[i].end());
		std::ifstream fileMtl(mtlDir);
		if (!fileMtl.good())
		{
			FOX_ERROR("Could not open MTL file.");
			continue;
		}

		while (std::getline(fileMtl, line))
		{
			switch (line[0])
			{
				case 'K': // Colors 
				{
					ReadFloats(&(line[3]), (int)line.length() - 3, 3, fBuffer);

					switch (line[1])
					{
						case 'a': // Ambient
							materialPrimers.Back().ambient = cs::Color(fBuffer[0], fBuffer[1], fBuffer[2]);
							break;

						case 'd': // Diffuse
							materialPrimers.Back().diffuse = cs::Color(fBuffer[0], fBuffer[1], fBuffer[2]);
							break;

						case 's': // Specular
							materialPrimers.Back().specular = cs::Color(fBuffer[0], fBuffer[1], fBuffer[2]);
							break;

						//case 'e': // Emissive
						//	materialPrimers.Back().emissive = cs::Color(fBuffer[0], fBuffer[1], fBuffer[2]);
						//	break;

						default:
							break;
					}
				}
				break;

				case 'N': 
				{
					if (line[1] == 's') // Specular exponent
					{
						std::from_chars(&(line[3]), &(line.back()), materialPrimers.Back().exponent);
					}
				}
				break;

				case 'm': // Color maps
				{
					string name = line.substr(7, line.length() - 7);

					switch (line[4])
					{
						case 'K':
						{
							switch (line[5])
							{
								//case 'a':
								//	materialPrimers.Back().ambientMap = currentTexture;
								//	break;

								case 'd':
									materialPrimers.Back().albedoMap = currentTexture;
									break;

								//case 's':
								//	materialPrimers.Back().specularMap = currentTexture;
								//	break;

								//case 'e':
								//	materialPrimers.Back().emissiveMap = currentTexture;
								//	break;

								default:
									break;
							}

							currentTexture++;
							textureStrings.Add(name);
						}
						break;

						case 'N':
						{
							if (line[5] == 's')
							{
								materialPrimers.Back().exponentMap = currentTexture;
							}

							currentTexture++;
							textureStrings.Add(name);
						}
						break;
					}
				}
				break;

				case 'd': // Translucency
					break;

				case 'n':
				{
					if (line.substr(0, 6) == "newmtl")
					{
						materialPrimers.Add(MaterialPrimer());
						materialPrimers.Back().name = line.substr(7, line.length() - 7);
					}
				}
				break;

				default:
					break;
			}
		}
	}


#pragma endregion

	int vertexCount = 0;
	int originalTextureCount = textureStrings.Size();

#pragma region Vertex Counting

	for (SubmeshPrimer& s : submeshPrimers)
	{
		vertexCount += s.faces.Size() * 3;
	}

#pragma endregion

#pragma region Texture Duplicates

	for (int i = 0; i < textureStrings.Size(); i++)
	{
		for (int j = i + 1; j < textureStrings.Size(); j++)
		{
			if (textureStrings[i] == textureStrings[j])
			{
				textureStrings.Remove(j);
				currentTexture--;
				j--;
			}
		}
	}

#pragma endregion



	// Final loading

	m.vertexCount = vertexCount;
	m.submeshCount = submeshPrimers.Size();
	m.materialCount = materialPrimers.Size();
	m.textureCount = materialPrimers.Size() * 5 - originalTextureCount + textureStrings.Size();

	m.vertexFurthest = sqrtf(maxDistanceSq);

	m.vertices = new SandFox::MeshVertex[m.vertexCount];
	m.submeshes = new SandFox::MeshSubmesh[m.submeshCount];
	m.materials = new SandFox::MeshMaterial[m.materialCount];
	m.textures = new SandFox::Texture[m.textureCount];



	// Textures

	FOX_TRACE("Loading image file(s)...");
	for (int i = 0; i < textureStrings.Size() && i < m.textureCount; i++)
	{
		m.textures[i].Load(dir + wstring(textureStrings[i].begin(), textureStrings[i].end()));
	}
	



	// Materials

	auto loadTextureColor = [&](int& target, int textureIndex, const cs::Color& color)
	{
		if (textureIndex == -1)
		{
			byte b[] = { (byte)(255 * color.r), (byte)(255 * color.g), (byte)(255 * color.b), 255};
			m.textures[currentTexture].Load(b, 1, 1, true, 4, DXGI_FORMAT_R8G8B8A8_UNORM);
			target = currentTexture++;
		}
		else
		{
			target = textureIndex;
		}
	};

	auto loadTextureLinear = [&](int& target, int textureIndex, float value)
	{
		if (textureIndex == -1)
		{
			byte b = (int)(value * 255);
			m.textures[currentTexture].Load(&b, 1, 1, true, 1, DXGI_FORMAT_R8_UNORM);
			target = currentTexture++;
		}
		else
		{
			target = textureIndex;
		}
	};

	FOX_TRACE("Loading material(s)...");
	for (int i = 0; i < m.materialCount; i++)
	{
		m.materials[i].name = materialPrimers[i].name;

		m.materials[i].ambient = materialPrimers[i].ambient;
		m.materials[i].diffuse = materialPrimers[i].diffuse;
		m.materials[i].specular = materialPrimers[i].specular;
		m.materials[i].exponent = materialPrimers[i].exponent;

		loadTextureColor(m.materials[i].albedoIndex, materialPrimers[i].albedoMap, cs::Color(1.0f, 1.0f, 1.0f));
		loadTextureLinear(m.materials[i].exponentIndex, materialPrimers[i].exponentMap, 1.0f);

		//if (materialPrimers[i].ambientMap == -1 && materialPrimers[i].ambient.r == -1.0f)
		//{
		//	if (materialPrimers[i].diffuseMap != -1)
		//	{
		//		materialPrimers[i].ambientMap = materialPrimers[i].diffuseMap;
		//	}
		//	else if (materialPrimers[i].diffuse.r != -1.0f)
		//	{
		//		materialPrimers[i].ambient = materialPrimers[i].diffuse;
		//	}
		//	else
		//	{
		//		materialPrimers[i].ambient = cs::Color(1.0f, 1.0f, 1.0f);
		//	}
		//}

		//if (materialPrimers[i].specularMap == -1 && materialPrimers[i].specular.r == -1.0f)
		//{
		//	if (materialPrimers[i].diffuseMap != -1)
		//	{
		//		materialPrimers[i].specularMap = materialPrimers[i].diffuseMap;
		//	}
		//	else if (materialPrimers[i].diffuse.r != -1.0f)
		//	{
		//		materialPrimers[i].specular = materialPrimers[i].diffuse;
		//	}
		//	else
		//	{
		//		materialPrimers[i].specular = cs::Color(1.0f, 1.0f, 1.0f);
		//	}
		//}

		//loadTextureColor(m.materials[i].ambientMapIndex, materialPrimers[i].ambientMap, materialPrimers[i].ambient);
		//loadTextureColor(m.materials[i].diffuseMapIndex, materialPrimers[i].diffuseMap, materialPrimers[i].diffuse);
		//loadTextureColor(m.materials[i].specularMapIndex, materialPrimers[i].specularMap, materialPrimers[i].specular);
		////loadTextureColor(m.materials[i].emissiveMapIndex, materialPrimers[i].emissiveMap, materialPrimers[i].emissive);
		//loadTextureLinear(m.materials[i].exponentMapIndex, materialPrimers[i].exponentMap, materialPrimers[i].exponent);
	}



	// Vertices and submeshes

	int currentVertex = 0;

	FOX_TRACE("Assembling submesh(es)...");
	for (int i = 0; i < m.submeshCount; i++)
	{
		SubmeshPrimer& s = submeshPrimers[i];

		m.submeshes[i].name = s.name;
		m.submeshes[i].indexCount = s.faces.Size() * 3;
		m.submeshes[i].indices = new uindex[m.submeshes[i].indexCount];

		int j;

		for (j = 0; j < materialPrimers.Size() && materialPrimers[j].name != s.materialName; j++);
		m.submeshes[i].materialIndex = j;

		for (j = 0; j < s.faces.Size(); j++)
		{
			m.vertices[currentVertex] = { vertices[s.faces[j].indices[0]], normals[s.faces[j].indices[2]], uvs[s.faces[j].indices[1]] };
			m.vertices[currentVertex + 1] = { vertices[s.faces[j].indices[3]], normals[s.faces[j].indices[5]], uvs[s.faces[j].indices[4]] };
			m.vertices[currentVertex + 2] = { vertices[s.faces[j].indices[6]], normals[s.faces[j].indices[8]], uvs[s.faces[j].indices[7]] };

			m.submeshes[i].indices[j * 3] = currentVertex;
			m.submeshes[i].indices[j * 3 + 1] = currentVertex + 1;
			m.submeshes[i].indices[j * 3 + 2] = currentVertex + 2;

			currentVertex += 3;
		}
	}

	FOX_TRACE("Done!");

	return true;
}





// Struct behavior

SandFox::Mesh::Mesh()
	:
	loaded(false),
	referenceCounter(nullptr),
	vertices(nullptr),
	submeshes(nullptr),
	materials(nullptr),
	textures(nullptr),
	vertexCount(0),
	submeshCount(0),
	materialCount(0),
	textureCount(0)
{
}

SandFox::Mesh::Mesh(const wchar_t* path, MeshLoader type)
	:
	Mesh()
{
	Load(path, type);
}

SandFox::Mesh::Mesh(const Mesh& rhs)
	:
	Mesh()
{
	*this = rhs;
}

SandFox::Mesh::~Mesh()
{
	Unload();
}

SandFox::Mesh& SandFox::Mesh::operator=(const Mesh& rhs)
{
	Unload();

	loaded = rhs.loaded;
	referenceCounter = rhs.referenceCounter;

	vertices = rhs.vertices;
	submeshes = rhs.submeshes;
	materials = rhs.materials;
	textures = rhs.textures;

	vertexCount = rhs.vertexCount;
	submeshCount = rhs.submeshCount;
	materialCount = rhs.materialCount;
	textureCount = rhs.textureCount;

	if (referenceCounter)
	{
		(*referenceCounter)++;
	}

	return *this;
}

bool SandFox::Mesh::Load(const wchar_t* path, MeshLoader type)
{
	Unload();

	bool result = false;

	switch (type)
	{
		case SandFox::MeshLoaderOBJ:
			result = LoadOBJ(*this, path);
			break;

		default:
			return false;
	}

	if (result)
	{
		loaded = true;
		referenceCounter = new int(1);
	}

	return result;
}

void SandFox::Mesh::Unload()
{
	if (!loaded)
	{
		return;
	}

	(*referenceCounter)--;

	if (*referenceCounter == 0)
	{
		for (int i = 0; i < submeshCount; i++)
		{
			delete[] submeshes[i].indices;
		}

		delete[] vertices;
		delete[] submeshes;
		delete[] materials;
		delete[] textures;
		delete referenceCounter;
	}

	loaded = false;

	referenceCounter = nullptr;
	submeshes = nullptr;
	materials = nullptr;
	textures = nullptr;

	vertexCount = 0;
	submeshCount = 0;
	materialCount = 0;
	textureCount = 0;
}
