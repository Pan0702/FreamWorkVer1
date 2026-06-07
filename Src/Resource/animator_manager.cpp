#include "animator_manager.h"
#include <fstream>
#include "skmesh_file.h"
#include "animation.h"

namespace 
{
    AnimatorManager instance;
}
AnimatorManager::AnimatorManager() = default;

AnimatorManager::~AnimatorManager() = default;

void AnimatorManager::Shutdown()
{
    cache_.clear();
}

Animation* AnimatorManager::Load(const std::string& path)
{
    if (auto it = cache_.find(path); it != cache_.end())
    {
        return it->second.get();
    }
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs)
    {
        return nullptr;
    }
    SkAnimFileHeader header = {};
    ifs.read(reinterpret_cast<char*>(&header), sizeof(SkAnimFileHeader));
    if (!ifs)
    {
        MessageBox(nullptr, L"Failed to read SkAnim file", L"Error", MB_OK);
        return nullptr;
    }
    if (std::memcmp(header.magic, kSkAnimMagic, 4) != 0)
    {
        MessageBoxA(nullptr, "Invalid magic", "Error", MB_OK);
        return nullptr;
    }
    if (header.version != kSkAnimVersion)
    {
        MessageBoxA(nullptr, "Invalid version", "Error", MB_OK);
        return nullptr;
    }

    std::vector<NodeAnimation> channels(header.channel_count);
    for (uint32_t c = 0; c < header.channel_count; ++c)
    {
        SkAnimChannel ch;
        ifs.read(reinterpret_cast<char*>(&ch), sizeof(ch));

        // ノード名（node_name_length バイト）
        std::string name(ch.node_name_length, '\0');
        if (ch.node_name_length > 0)
        {
            ifs.read(name.data(), ch.node_name_length);
        }

        // 位置キー（ディスク PositionKey[] を一括読み）
        std::vector<PositionKey> pos(ch.pos_key_count);
        if (ch.pos_key_count > 0)
        {
            ifs.read(reinterpret_cast<char*>(pos.data()),
                      static_cast<std::streamsize>(ch.pos_key_count * sizeof(PositionKey)));
        }

        // 回転キー（RotationKey[]）
        std::vector<RotationKey> rot(ch.rot_key_count);
        if (ch.rot_key_count > 0)
        {
            ifs.read(reinterpret_cast<char*>(rot.data()),
                      static_cast<std::streamsize>(ch.rot_key_count * sizeof(RotationKey)));
        }

        // スケールキー（ScaleKey[]）
        std::vector<ScaleKey> scl(ch.scale_key_count);
        if (ch.scale_key_count > 0)
        {
            ifs.read(reinterpret_cast<char*>(scl.data()),
                 static_cast<std::streamsize>(ch.scale_key_count * sizeof(ScaleKey)));
        }

        // ディスク型 → メモリ型(Key<T>)に詰め替え
        channels[c].name = name;
        channels[c].position_keys.resize(ch.pos_key_count);
        for (uint32_t k = 0; k < ch.pos_key_count; ++k)
        {
            channels[c].position_keys[k].time = pos[k].time;
            channels[c].position_keys[k].value = pos[k].value;
        }
        channels[c].rotation_keys.resize(ch.rot_key_count);
        for (uint32_t k = 0; k < ch.rot_key_count; ++k)
        {
            channels[c].rotation_keys[k].time = rot[k].time;
            channels[c].rotation_keys[k].value = rot[k].value;
        }
        channels[c].scale_keys.resize(ch.scale_key_count);
        for (uint32_t k = 0; k < ch.scale_key_count; ++k)
        {
            channels[c].scale_keys[k].time = scl[k].time;
            channels[c].scale_keys[k].value = scl[k].value;
        }
    }
    if (!ifs) return nullptr;

    auto clip = std::make_unique<Animation>();
    clip->SetData(header.duration, header.ticks_per_second,
                  std::move(channels));
    Animation* result = clip.get();
    cache_[path] = std::move(clip);
    return result;
}

Animation* AnimatorManager::GetAnimation(const std::string& name)
{
    auto it = cache_.find(name);
    if (it != cache_.end())
    {
        return it->second.get();
    }
    return nullptr;
}

AnimatorManager& AnimatorManager::Get()
{
    return instance;
}
