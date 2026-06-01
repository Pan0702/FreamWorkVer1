#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "../Core/common.h"
#include "texture2D.h"

class CommandList;
class CommandQueue;
class DescriptorHeap;

class TextureManager
{
public:
    ~TextureManager();

    void Initialize(ID3D12Device* device, DescriptorHeap* srv_heap,
                    CommandQueue* queue, CommandList* cmd);
    void Shutdown();

    Texture2D* Load(const wchar_t* path);

    static TextureManager& Get()
    {
        static TextureManager instance;
        return instance;
    }

private:
    ID3D12Device* device_ = nullptr;
    DescriptorHeap* srv_heap_ = nullptr;
    CommandQueue* queue_ = nullptr;
    CommandList* cmd_ = nullptr;
    std::unordered_map<std::wstring, std::unique_ptr<Texture2D>> cache_;
};
