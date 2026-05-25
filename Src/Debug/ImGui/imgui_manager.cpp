#include "imgui_manager.h"

bool ImGuiManager::Initialize(HWND hwnd, ID3D12Device* device, int numFramesInFlight)
{
}

void ImGuiManager::BeginFrame()
{
}

void ImGuiManager::EndFrame(ID3D12GraphicsCommandList* command_list)
{
}

void ImGuiManager::Shutdown()
{
}
// ● OK。では Initialize → BeginFrame → EndFrame → Shutdown
//   の順に実装していきましょう。                     
//
//   Initialize の中でやること（順番通り）:
//   1. IMGUI_CHECKVERSION()
//   2. ImGui::CreateContext()
//   3. ImGui::StyleColorsDark()（好みで）
//   4. SRV DescriptorHeap 作成 — D3D12_DESCRIPTOR_HEAP_DESC で Type =
//   CBV_SRV_UAV, NumDescriptors = 1, Flags = SHADER_VISIBLE
//   5. ImGui_ImplWin32_Init(hwnd)
//   6. ImGui_ImplDX12_Init(device, numFramesInFlight,
//   DXGI_FORMAT_R8G8B8A8_UNORM, srvHeap_, CPUHandle, GPUHandle)
//
//   BeginFrame:
//   1. ImGui_ImplDX12_NewFrame()
//   2. ImGui_ImplWin32_NewFrame()
//   3. ImGui::NewFrame()
//
//   EndFrame(commandList):
//   1. ImGui::Render()
//   2. commandList->SetDescriptorHeaps(1, &srvHeap_) — 自前ヒープから ImGui
//   用に切り替え
//   3. ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList)
//
//   Shutdown:
//   1. ImGui_ImplDX12_Shutdown()
//   2. ImGui_ImplWin32_Shutdown()
//   3. ImGui::DestroyContext()
//
//   srvHeap_ は ComPtr<ID3D12DescriptorHeap> でメンバに持たせておけば
//   Shutdown 時に自動解放されます。
//
// //   詰まったら聞いてください。
// ● なりますね。ただし注意点が1つ — ダウンロードしたバージョンが 1.92以降
//   の新しい API を使っています。
//
//   前に私が説明した ImGui_ImplDX12_Init(device, numFrames, format, heap, 
//   cpuHandle, gpuHandle) は 旧API です。このサンプルでは
//   ImGui_ImplDX12_InitInfo 構造体を使う 新API になっています。
//
//   なので Initialize では旧式ではなくこっちに合わせてください:
//
//   ImGui_ImplDX12_InitInfo init_info = {};
// init_info.Device = device;
// init_info.CommandQueue = commandQueue;  // ← これも引数で受け取る必要あり
// init_info.NumFramesInFlight = numFramesInFlight;
// init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
// init_info.SrvDescriptorHeap = srvHeap_;
// init_info.SrvDescriptorAllocFn = ...;
// init_info.SrvDescriptorFreeFn = ...;
// ImGui_ImplDX12_Init(&init_info);
//
// つまり ImGuiManager::Initialize の引数 に ID3D12CommandQueue*
// も追加が必要です。
//
// また、SRV の Alloc/Free コールバックはサンプルの
// ExampleDescriptorHeapAllocator
// をそのまま参考にしていいです。フォント1個だけなら NumDescriptors=1
// でも動きますが、サンプルと同じく 64
// 程度確保しておくと将来テクスチャ表示等にも対応しやすいです。
//
// あとサンプルで見るべきポイント:
// - WndProc で ImGui_ImplWin32_WndProcHandler を先頭で呼ぶ部分（一番下）
// - EndFrame 相当 の箇所: SetDescriptorHeaps →
// ImGui_ImplDX12_RenderDrawData の流れ
//
// このサンプルをそのまま ImGuiManager に詰め替えていけば OK です。