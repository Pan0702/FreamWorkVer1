#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMIMAX
#define NOMINMAX
#endif

#include <windows.h>

#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <d3dcompiler.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <wrl/client.h>

#include <array>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <string>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"d3dcompiler.lib")

// COM オブジェクトを自動解放するための共有別名。
template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

// アプリケーション起動時の標準ウィンドウサイズ。
inline constexpr uint32_t kWindowWidth = 1920;
inline constexpr uint32_t kWindwoHeight = 1080;
