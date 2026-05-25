#pragma once
#include "../../Platform/Window.h"

class ImGuiManager
{
public:
    bool Initialize(HWND hwnd,ID3D12Device* device,int numFramesInFlight);
    void BeginFrame();
    void EndFrame(ID3D12GraphicsCommandList* command_list);
    void Shutdown();
    
    private:
    ComPtr<ID3D12DescriptorHeap> srv_heap_;
};
