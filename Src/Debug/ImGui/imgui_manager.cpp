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
//   詰まったら聞いてください。