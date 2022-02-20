#pragma once

#ifdef FOX_COMPILE
#define FOX_API __declspec(dllexport)
#else
#define FOX_API __declspec(dllimport)
#endif

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