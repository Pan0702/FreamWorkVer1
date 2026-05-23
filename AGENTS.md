# Repository Guidelines

## Scope

These guidelines apply to the whole repository.

## Project Direction

This project is a C++ DirectX 12 framework. Use `---.txt` as the primary design note for the intended layer structure:

- `Src/Core`
- `Src/Platform`
- `Src/Graphics`
- `Src/Resource`
- `Src/Renderer`
- `Src/Engine`
- `Src/Debug`
- `Src/Game`

Start with a minimal window and clear-screen loop before adding higher-level renderer features.

The repository owner wants to implement the code themselves. Do not add, modify, or generate implementation files unless the owner explicitly asks for code changes. Prefer explaining concepts, implementation order, API shape, pitfalls, and reviewing code the owner writes.

When the owner asks how to implement something, provide guidance and small illustrative snippets only when useful. Do not apply patches or update project files unless explicitly requested.

Recommended first milestone:

1. `Core/Common.h`, `Core/Types.h`, `Core/Logger`.
2. `Platform/Window`.
3. `Graphics/GraphicsDevice`.
4. `Graphics/CommandQueue`.
5. `Graphics/SwapChain`.
6. Frame command allocator/list management.
7. Back buffer transition, `ClearRenderTargetView`, `Present`.

## Naming Style

Follow Google C++ naming conventions.

- Types/classes/structs/enums: `UpperCamelCase`.
- Functions/methods: `UpperCamelCase`.
- Local variables and parameters: `lower_snake_case`.
- Private data members: `lower_snake_case_`.
- Constants: `kUpperCamelCase`.
- Enum values: `kUpperCamelCase`.
- Namespaces: `lower_snake_case`.
- Files: prefer `lower_snake_case.h` and `lower_snake_case.cpp`.

Keep Japanese comments acceptable when they explain intent, but prefer short comments and self-explanatory code.

## C++ / DirectX 12 Notes

- Use RAII for Win32 and D3D12 resources whenever possible.
- Use `Microsoft::WRL::ComPtr` for COM objects.
- Do not hide important D3D12 state transitions too early. This project is for learning DX12, so wrappers should stay thin and explicit.
- Check all `HRESULT` values.
- Keep initialization order explicit: window, DXGI factory/device, command queue, swap chain, descriptors, command lists.
- Wait for GPU completion before destroying or resizing swap-chain resources.
- Keep `Application` non-copyable because subsystem member order controls destruction order.

## Text Encoding

- Treat `std::string` and `std::string_view` as UTF-8 text by project convention unless explicitly noted otherwise.
- Prefer UTF-8 strings in non-Win32 layers such as `Core`, `Graphics`, `Resource`, `Renderer`, `Engine`, and `Game`.
- Keep `wchar_t`, `std::wstring`, `std::wstring_view`, and `const wchar_t*` usage near Win32 API boundaries, primarily inside `Platform`.
- Convert UTF-8 to wide strings only when calling Win32 `W` APIs such as `CreateWindowExW`, `RegisterClassExW`, or `SetWindowTextW`.

## Editing Expectations

- Keep changes small and aligned with `---.txt`.
- Do not introduce large third-party dependencies unless explicitly requested.
- Prefer x64 Debug as the first validation target.
- If project files are updated, keep Visual Studio filters in sync.
