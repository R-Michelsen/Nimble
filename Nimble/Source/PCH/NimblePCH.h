#pragma once

#define _HAS_EXCEPTIONS 0

#include <cstdio>

#include <Windows.h>
#include <Windowsx.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#include <d3d11_3.h>
#include <dxgidebug.h>
#include <d3dcompiler.h>
#include <DirectXColors.h>
#include <DirectXMath.h>

#include <EASTL/deque.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/algorithm.h>
#include <EASTL/vector.h>
#include <EASTL/fixed_vector.h>
#include <EASTL/array.h>
#include <EASTL/type_traits.h>
#include <EASTL/numeric_limits.h>
#include <EASTL/tuple.h>
#include <EASTL/functional.h>
#include <EASTL/string.h>

#include "../Application/Logging.h"