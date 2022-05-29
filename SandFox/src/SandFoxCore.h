#pragma once

#ifdef FOX_COMPILE
#define FOX_API __declspec(dllexport)
#define IMGUI_API __declspec(dllexport)
#else
#define FOX_API __declspec(dllimport)
#define IMGUI_API __declspec(dllimport)
#endif

#pragma warning( push )
#pragma warning( disable : 4251 )
#pragma warning( disable : 4275 )
#pragma warning( disable : 6385 )
#pragma warning( disable : 6386 )

#include <CHSL\WindowsExt.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <d3d11.h>
namespace dx = DirectX;
namespace wrl = Microsoft::WRL;
using Microsoft::WRL::ComPtr;

#define CHSL_EXCEPT
#define CHSL_LINEAR
#define CHSL_DX
#include <CHSL.h>

#include "Constants.h"