
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
// |    uint    4B      SandShed major version
// |    uint    4B      SandShed minor version
// |    uint    8B      Content hash
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
// |    uint    4B      Material count
// |    uint    8B      Material listings start
// |    uint    8B      Material data start
// :
// 
// Listings
// |
// |    Tex[]   32B[]   Texture listings
// |
// |        Tex
// |        | uint    8B      Texture size
// |        | uint    8B      Texture start
// |        | uint    8B      File hash
// |        | uint    8B      Name hash
// |
// |    Mesh[]  
// |
// |
// |
// :

constexpr int           MAJOR_VERSION   = 0;   // Marks major changes in the program. Defined as NOT being compatible or interlegible
constexpr int           MINOR_VERSION   = 1;   // Minor changes, most often intercompatible;

constexpr const char*   FILE_EXTENSION  = ".sfpkg";



// Loads D3D11 Library for the whole project
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

#include "ConsoleTools.h"

#include <filesystem>
#include <unordered_map>
#include <functional>

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include <filesystem>
#include <unordered_map>
#include <string>

using std::string;
using std::filesystem::path;





void processLibrary(path inDir, path outFile)
{
    CWrite("Processing library: ");
    CSetColor(ColorWhite)
}





int program()
{

    // Greet

    CSetColor(ColorWhite);
    CWriteLine("SandShed package manager\n");
    CSetColor(ColorLightGrey);

    



    // Make device

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
    CWriteInt(packages.size());
    CWriteLine();

    if (packages.size() == 0)
    {
        throw "Exiting because no input packages were found.";
    }





    // Scan package directory

    std::vector<path> modelPaths;
    std::vector<path> texturePaths;

    for (const auto& item : std::filesystem::directory_iterator(inDir))
    {
        if (item.path().extension() == ".obj")
        {
            modelPaths.push_back(item.path());
            continue;
        }

        if (item.path().extension() == ".png" || item.path().extension() == ".jpg" || item.path().extension() == ".bmp")
        {
            texturePaths.push_back(item.path());
            continue;
        }
    }

    CWriteLine("Done");

    CWrite("Models found: ");
    CWriteInt(modelPaths.size());

    CWrite("\nImages found: ");
    CWriteInt(texturePaths.size());
    CWriteLine();


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
