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
}

void TextureManager::Shutdown()
{
    cache_.clear();
    device_ = nullptr;
    srv_heap_ = nullptr;
    queue_ = nullptr;
    cmd_ = nullptr;
}

Texture2D* TextureManager::Load(const wchar_t* path)
{
    if (path == nullptr || device_ == nullptr || srv_heap_ == nullptr || queue_ == nullptr || cmd_ == nullptr)
    {
        return nullptr;
    }

    // 同じパスは読み込み済みのものを使い回す
    if (const auto it = cache_.find(path); it != cache_.end())
    {
        return it->second.get();
    }

    // ? ファイル → CPU上の RGBA8 ピクセル
    LoadedImage image;
    if (!TextureLoader::LoadFromFile(path, image))
    {
        return nullptr;
    }

    // ? アップロードコマンドを記録（Texture2D は記録のみ）→ 実行 → GPU完了待ちはここで行う
    if (!cmd_->Reset())
    {
        return nullptr;
    }
    auto texture = std::make_unique<Texture2D>();
    if (!texture->Initialize(device_, cmd_->GetCommandList(), image))
    {
        return nullptr;
    }
    cmd_->Close();
    ID3D12CommandList* command_lists[] = {cmd_->GetCommandList()};
    queue_->GetCommandQueue()->ExecuteCommandLists(1, command_lists);
    queue_->WaitIdle(); // アップロード完了まで待つ（upload_buffer はこの間生存）

    // ? SRV ヒープに枠を確保
    UINT srv_index = 0;
    if (!srv_heap_->Allocate(srv_index))
    {
        return nullptr;
    }

    // ? SRV 作成
    texture->SetSrvIndex(srv_index);
    texture->CreateSrv(device_, srv_heap_->GetCpuHandle(srv_index));

    Texture2D* result = texture.get();
    cache_.emplace(path, std::move(texture));
    return result;
}