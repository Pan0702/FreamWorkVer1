#include "audio_manager.h"
#include "audio_player.h"

#pragma comment(lib, "xaudio2.lib")

namespace
{
    AudioManager instance;
}

AudioManager::~AudioManager()
{
    audio_data_.clear();
    if (mastering_voice_ != nullptr)
    {
        mastering_voice_->DestroyVoice();
        mastering_voice_ = nullptr;
    }
}

void AudioManager::LoadWaveFile(const char* filename, const char* name, bool loop)
{
    if (!Initialize())
    {
        return;
    }

    AudioData data{};
    data.player = std::make_unique<AudioPlayer>();
    if (!data.player->Initialize(xaudio2_.Get()))
    {
        return;
    }
    if (!data.player->LoadWaveFile(filename, loop))
    {
        return;
    }
    data.loop = loop;
    audio_data_[std::string(name)] = std::move(data);
}
bool AudioManager::Initialize()
{
    if (xaudio2_ != nullptr && mastering_voice_ != nullptr)
    {
        return true;
    }

    HRESULT hr = XAudio2Create(&xaudio2_);
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to create XAudio2", L"Error", MB_OK);
        return false;
    }

    hr = xaudio2_->CreateMasteringVoice(&mastering_voice_);
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to create mastering voice", L"Error", MB_OK);
        xaudio2_.Reset();
        return false;
    }

    return true;
}

void AudioManager::Play(const char* name)
{
    auto it = audio_data_.find(name);
    if (it == audio_data_.end())
    {
        return;
    }
    it->second.player->Play(it->second.loop);
}

void AudioManager::Stop(const char* name)
{
    auto it = audio_data_.find(name);
    if (it == audio_data_.end())
    {
        return;
    }
    it->second.player->Stop();
}

void AudioManager::SetVolume(const char* name, float volume)
{
    auto it = audio_data_.find(name);
    if (it == audio_data_.end())
    {
        return;
    }
    it->second.player->SetVolume(volume);
}

AudioManager& AudioManager::GetInstance()
{
    return instance;
}
