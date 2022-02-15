#ifndef CORE_H
#define CORE_H

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif 

// windows
#include <CHSL\WindowsExt.h>

// libraries
#include <iostream>
#include <memory>
#include <thread>
#include <utility>

// DX
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl.h>
namespace dx = DirectX;
namespace wrl = Microsoft::WRL;
using Microsoft::WRL::ComPtr;

// data structures
#include <string>
#include <stack>
#include <queue>
#include <array>
#include <vector>
#include <bitset>
//#include <list>
#include <map>
#include <unordered_map>
//#include <functional>
//#include <sstream>
//#include <optional>

// own standards
#define CHSL_EXCEPT
#define CHSL_LINEAR
#define CHSL_DX
#include <CHSL.h>

#pragma warning( push )
#pragma warning( disable : 26451 )

#endif
