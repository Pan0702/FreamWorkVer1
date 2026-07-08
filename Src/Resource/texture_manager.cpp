#include "texture_manager.h"

#include "texture2D.h"
#include "texture_loder.h"
#include "../Graphics/command_list.h"
#include "../Graphics/command_queue.h"
#include "../Graphics/descriptor_heap.h"

TextureManager::~TextureManager() = default;

void TextureManager::Initialize(ID3D12Device* device, DescriptorHeap* srv_heap,
                                CommandQueue* queue, CommandList* cmd)
{
    device_ = device;
    srv_heap_ = srv_heap;
    queue_ = queue;
    cmd_ = cmd;
    
    device_->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(&upload_allocator_));
}

void TextureManager::Shutdown()
{
    cache_.clear();
    device_ = nullptr;
    srv_heap_ = nullptr;
    queue_ = nullptr;
    cmd_ = nullptr;
}

Texture2D* TextureManager::Load(const wchar_t* path, bool is_srgb)
{
    if (path == nullptr || device_ == nullptr || srv_heap_ == nullptr || queue_ == nullptr || cmd_ == nullptr)
    {
        return nullptr;
    }

    std::wstring key = std::wstring(path) + (is_srgb ? L"#srgb" : L"#linear");
    
    // 同じパスは読み込み済みのものを使い回す
    if (const auto it = cache_.find(key); it != cache_.end())
    {
        printf("[TexCache HIT] %ls (srgb=%d)\n", path, is_srgb);  // ← 追加
        return it->second.get();
    }

    //  ファイル → CPU上の RGBA8 ピクセル
    LoadedImage image;
    if (!TextureLoader::LoadFromFile(path, image))
    {
        return nullptr;
    }
    return LoadTexture(key.c_str(), image, is_srgb);
}

Texture2D* TextureManager::CreateFromImage(const wchar_t* cache_key, const LoadedImage& image, bool is_srgb)
{
    if (cache_key == nullptr || device_ == nullptr || srv_heap_ == nullptr || queue_ == nullptr || cmd_ == nullptr)
    {
        return nullptr;
    }

    // 同じキーなら作成済みのものを使う
    if (const auto it = cache_.find(cache_key); it != cache_.end())
    {
        return it->second.get();
    }
    return LoadTexture(cache_key, image, is_srgb);
}

Texture2D* TextureManager::LoadTexture(const wchar_t* path, const LoadedImage& image, bool is_srgb)
{
    //  アップロードコマンドを記録（Texture2D は記録のみ）→ 実行 → GPU完了待ちはここで行う
    if (!cmd_->Reset(upload_allocator_.Get()))
    {
        return nullptr;
    }
    auto texture = std::make_unique<Texture2D>();
    texture->SetSRGB(is_srgb);
    
    if (!texture->Initialize(device_, cmd_->GetCommandList(), image))
    {
        return nullptr;
    }
    
    if (!cmd_->Close())
    {
        return nullptr;
    }
    ID3D12CommandList* command_lists[] = {cmd_->GetCommandList()};
    queue_->GetCommandQueue()->ExecuteCommandLists(1, command_lists);
    queue_->WaitIdle(); // アップロード完了まで待つ（upload_buffer はこの間生存）

    // ? SRV ヒープに枠を確保
    UINT srv_index = 0;
    if (!srv_heap_->Allocate(srv_index))
    {
        return nullptr;
    }

    //  SRV 作成
    texture->SetSrvIndex(srv_index);
    texture->CreateSrv(device_, srv_heap_->GetCpuHandle(srv_index));

    Texture2D* result = texture.get();
    cache_.emplace(path, std::move(texture));
    return result;
}
