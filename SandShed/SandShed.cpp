
// /-------------------------\
// | Package manager program |
// \-------------------------/
// 
// Preloads models and textures into .sfpkg files.
// Based around a "fake" folder structure, where the hierarchy is baked into the name hashes.
// Name hash could be, as an example:   hash("Textures/Stone1/Color.png")
// File structured as follows:
// 
// |    TYPE    SIZE    DESCRIPTION
// 
// Header
// |
// |    uint    4B      SandShed major version
// |    uint    4B      SandShed minor version
// |    uint    8B      Content hash                - Currently not used
// |    uint    8B      Content size in bytes
// |
// |    uint    4B      Texture count
// |    uint    8B      Texture listings start
// |    uint    8B      Texture data start
// |
// |    uint    4B      Model count
// |    uint    8B      Model listings start
// |    uint    8B      Model data start
// |
// :
// 
// Listings
// |
// |    Tex[]   36B[]   Texture listings
// |
// |        Tex
// |        |   uint    8B      Texture size
// |        |   uint    8B      Texture start
// |        |   uint    8B      File hash           - Currently not used
// |        |   uint    8B      Name hash
// |        |   uint    4B      Texture type/flags
// |    
// |    Mesh[]  48B[]   Mesh listings
// |        
// |        Mesh        
// |        |
// |        |   uint    8B      Mesh size (total)
// |        |   uint    8B      Mesh start
// |        |   uint    4B      Vertex count
// |        |   uint    4B      Material count
// |        |   uint    4B      Texture count
// |        |   uint    4B      Submesh count
// |        |   uint    8B      File hash           - Currently not used
// |        |   uint    8B      Name hash
// |
// :
//
// Data
// |   
// |    Texture data
// |
// |    Mesh data
// |
// |        Vertex      44B
// |        |
// |        |   Vec3    12B     Position
// |        |   Vec3    12B     Normal
// |        |   Vec2    8B      UV/Texture coordinate
// |        |   Vec3    12B     Tangent
// |        
// |        Material    60B
// |        |
// |        |   Vec3    12B     Ambient color
// |        |   Vec3    12B     Diffuse color
// |        |   Vec3    12B     Specular color
// |        |   float   4B      Specular exponent
// |        |   uint    4B      Albedo texture index
// |        |   uint    4B      Ambient texture index
// |        |   uint    4B      Specular texture index
// |        |   uint    4B      Specular exponent texture index
// |        |   uint    4B      Normal map texture index
// |
// |        Texture     9B
// |        |
// |        |   byte    1B      Texture listing type        [ 0: Package texture index, 1: Texture name hash ]
// |        |   uint    8B      Texture information
// |
// |        Submesh
// |        |
// |        |   uint    4B      Index count
// |        |   uint    4B      Material index
// |        |   uint[]  4B*n    Indices
// |        :
// |
// :
//




constexpr unsigned int      START_HEADER = 0;
constexpr unsigned int      START_LISTINGS = START_HEADER + 64;
constexpr unsigned int      LISTING_TEXTURE_SIZE = 36;
constexpr unsigned int      LISTING_MESH_SIZE = 48;

constexpr unsigned int      MAJOR_VERSION   = 0;   // Marks major changes in the program. Defined as NOT being compatible or interlegible
constexpr unsigned int      MINOR_VERSION   = 1;   // Minor changes, most often intercompatible;

constexpr const char*       FILE_EXTENSION  = ".sfpkg";





// Loads D3D11 Library for the whole project
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

#include "ConsoleTools.h"

#include <SandFox/Mesh.h>

#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <functional>

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include <WICTextureLoader.h>
#include <filesystem>
#include <unordered_map>
#include <string>
#include <xmmintrin.h>

using std::string;
using std::filesystem::path;





struct SandShedHeader
{
    uint majorVersion;
    uint minorVersion;
    ullong contentHash;
    ullong contentSize;

    uint textureCount;
    ullong textureListStart;
    ullong textureDataStart;

    uint modelCount;
    ullong modelListStart;
    ullong modelDataStart;

    uint materialCount;
    ullong materialListStart;
    ullong materialDataStart;
};

enum TextureType : uint
{
    TextureTypeDDS,
    TextureTypeWICPNG,
    TextureTypeWICJPG
};

struct TextureListing
{
    ullong size;
    ullong start;
    ullong fileHash;
    ullong nameHash;
    TextureType type;
};

struct MeshListing
{
    ullong size;
    ullong start;
    uint vertexCount;
    uint materialCount;
    uint textureCount;
    uint submeshCount;
    ullong fileHash;
    ullong nameHash;
};

struct PathRaw
{
    string substring;
    path path;
};

struct SandShedRaw
{
    std::vector<PathRaw> texturePaths;
    std::vector<PathRaw> modelPaths;
};



ullong strHash(string str)
{
    static std::hash<string> hash;
    return hash(str);

    //if (str.size() <= 8)
    //{
    //    // Algorithm A

    //    char buffer[16] = { 0 };
    //    memcpy(buffer, str.data(), str.size());

    //    return *((ullong*)buffer);
    //}
    //else
    //{
    //    // Algorithm B

    //    __m128i result = {};

    //    int x16 = str.size() / 16;
    //    for (int i = 0; i < x16; i++)
    //    {
    //        ullong data[2] =
    //        {
    //            (*((ullong*)(str.data() + i * 32))) +
    //            (*((ullong*)(str.data() + i * 32 + 8)) << 1),

    //            (*((ullong*)(str.data() + i * 32 + 16))) +
    //            (*((ullong*)(str.data() + i * 32 + 24)) << 1)
    //        };

    //        __m128i digest = _mm_loadu_si128((__m128i*)(str.data() + i * 16));

    //    }
    //}
}



void showConsoleCursor(bool showFlag)
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO     cursorInfo;

    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag; // set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);
}



void scanDir(const path& current, const string& currentSub, SandShedRaw* raw)
{
    for (const auto& item : std::filesystem::directory_iterator(current))
    {
        if (item.is_directory())
        {
            scanDir(item.path(), item.path().filename().string() + "/", raw);
        }

        if (item.path().extension() == ".png" || item.path().extension() == ".jpg" || item.path().extension() == ".bmp")
        {
            raw->texturePaths.push_back(
                {
                    currentSub + item.path().stem().string(),
                    item.path()
                });
            continue;
        }

        if (item.path().extension() == ".obj")
        {
            raw->modelPaths.push_back(
                {
                    currentSub + item.path().stem().string(),
                    item.path()
                });
            continue;
        }
    }
}

void processPackage(path inDir, path outFile, ID3D11Device* device, ID3D11DeviceContext* context)
{
        // Start

    CSetColor(ColorLightGrey);
    CWrite("\nProcessing package: ");
    CSetColor(ColorGreen);
    CWriteLine(inDir.filename().string().c_str());
    CSetColor(ColorLightGrey);

    

        // Scan package directory

    SandShedRaw raw;

    CWrite("Scanning directory...");
    scanDir(inDir, "", & raw);
    CWriteLine("Done");

    CWrite("Images found:     ");
    CWriteInt((int)raw.texturePaths.size());

    CWrite("\nModels found:     ");
    CWriteInt((int)raw.modelPaths.size());
    CWriteLine();

    showConsoleCursor(false);



        // Open file and allocate memory

    ullong fileSize = 
        START_LISTINGS +
        LISTING_TEXTURE_SIZE * raw.texturePaths.size() +
        LISTING_MESH_SIZE * raw.modelPaths.size();
    char* fileMem = (char*)malloc(fileSize);

    ullong currentDataStart = fileSize;

    std::ofstream output;
    output.open(outFile.string(), std::ios::binary | std::ios::trunc);



        // Load images

    DirectX::Blob blob;
    ullong startTextureListings = START_LISTINGS;

    for (int i = 0; i < raw.texturePaths.size(); i++)
    {
        ullong currentListingStart = startTextureListings + i * LISTING_TEXTURE_SIZE;

        DirectX::ScratchImage image;

        DirectX::LoadFromWICFile(
            raw.texturePaths[i].path.c_str(),
            DirectX::WIC_FLAGS_IGNORE_SRGB | DirectX::WIC_FLAGS_FORCE_RGB,
            nullptr,
            image
        );

        DirectX::SaveToDDSMemory(
            *image.GetImage(0, 0, 0),
            DirectX::DDS_FLAGS_NONE,
            blob
        );

        fileSize += blob.GetBufferSize();

        void* temp = realloc(fileMem, fileSize);
        if (temp == nullptr)
        {
            throw "Failed, not enough memory.";
        }
        fileMem = (char*)temp;
        memcpy(fileMem + currentDataStart, blob.GetBufferPointer(), blob.GetBufferSize());

        TextureListing listing;
        listing.size = blob.GetBufferSize();
        listing.start = currentDataStart;
        listing.fileHash = 0;                   // TODO: Implement file hashing
        listing.nameHash = strHash(raw.texturePaths[i].substring);
        listing.type = TextureTypeDDS;

        currentDataStart += blob.GetBufferSize();

        CWrite("\rImages processed: ");
        CWriteInt(i + 1);
        CWrite(" of ");
        CWriteInt((int)raw.texturePaths.size());
    }

    CWriteLine();



        // Load models

    ullong startModelListings = START_LISTINGS + raw.texturePaths.size() * LISTING_TEXTURE_SIZE;

    for (int i = 0; i < raw.modelPaths.size(); i++)
    {
        ullong currentListingStart = startModelListings + i * LISTING_MESH_SIZE;

        SandFox::Mesh mesh;
        mesh.Load(raw.modelPaths[i].path.c_str(), SandFox::MeshLoaderOBJ);



        // TODO: Finish haha.



        CWrite("\rModels processed: ");
        CWriteInt(i + 1);
        CWrite(" of ");
        CWriteInt((int)raw.modelPaths.size());
    }

    CWriteLine();



        // Build header

    SandShedHeader header;
    header.majorVersion     = MAJOR_VERSION;
    header.minorVersion     = MINOR_VERSION;
    header.contentHash;
    header.contentSize;

    header.textureCount;
    header.textureListStart;
    header.textureDataStart;

    header.modelCount;
    header.modelListStart;
    header.modelDataStart;

    header.materialCount;
    header.materialListStart;
    header.materialDataStart;

}





void program()
{

    // Greet

    CSetColor(ColorWhite);
    CWriteLine("SandShed package manager\n");
    CSetColor(ColorLightGrey);

    



    // Init

    CWrite("Initializing...");
    CoInitialize(NULL);
    CWriteLine("Done");

    CWrite("Creating device...");

    ID3D11Device* device;
    ID3D11DeviceContext* context;

    int featureLevelCount = 4;
    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };

    HRESULT hr = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        featureLevels,
        featureLevelCount,
        D3D11_SDK_VERSION,
        &device,
        nullptr,
        &context
    );

    if (FAILED(hr))
    {
        throw "Failed";
    }

    CWriteLine("Done");





    // Find directories

    CWrite("Looking for adjacent input and output folders...");

    path inDir = "";
    path outDir = "";

    int stringCount = 4;
    string inputStrings[] = { "Input", "input", "In", "in" };
    string outputStrings[] = { "Output", "output", "Out", "out" };

    for (const auto& item : std::filesystem::directory_iterator(std::filesystem::current_path()))
    {
        if (item.is_directory())
        {
            string name = item.path().filename().string();
            
            for (int i = 0; i < stringCount; i++)
            {
                if (inputStrings[i] == name)
                {
                    inDir = item.path();
                    break;
                }

                if (outputStrings[i] == name)
                {
                    outDir = item.path();
                    break;
                }
            }
        }
    }

    CWriteLine("Done");

    CSetColor(ColorYellow);

    if (outDir == "")
    {
        std::filesystem::create_directory(std::filesystem::current_path().string() + "/Output");
        CWriteLine("Output directory created.");
    }

    if (inDir == "")
    {
        std::filesystem::create_directory(std::filesystem::current_path().string() + "/Input");
        CWriteLine("Input directory created.");

        throw "Exiting because no input was found.";
    }

    CSetColor(ColorLightGrey);





    // Scan for packages

    CWrite("Scanning input directory for package directories...");

    struct PackageDir
    {
        path inDirPath;
        path outFilePath;
    };

    std::vector<PackageDir> packages;

    for (const auto& item : std::filesystem::directory_iterator(inDir))
    {
        if (item.is_directory())
        {
            packages.push_back(
                {
                    item.path(),
                    outDir
                }
            );

            packages.back().outFilePath += (item.path().filename().string() + FILE_EXTENSION);
        }
    }

    CWriteLine("Done");
    CWrite("Packages found: ");
    CWriteInt((int)packages.size());
    CWriteLine();

    if (packages.size() == 0)
    {
        throw "Exiting because no input packages were found.";
    }





    // Import packages

    for (int i = 0; i < packages.size(); i++)
    {
        processPackage(packages[i].inDirPath, packages[i].outFilePath, device, context);
    }
}

int main()
{
#if _DEBUG

    program();
    CSetColor(ColorLightGrey);

#else

    try
    {
        program();
        CSetColor(ColorLightGrey);
    }
    catch (const char* s)
    {
        CSetColor(ColorRed);
        CWriteLine(s);
    }
    catch (...)
    {
        CSetColor(ColorRed);
        CWriteLine("Unexpected error.");
    }

#endif

    CWriteLine("\nExiting...");
    CSetColor(ColorLightGrey);
}
