#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "../Core/common.h"
#include <xaudio2.h>

class AudioPlayer;

struct AudioData
{
    std::unique_ptr<AudioPlayer> player = nullptr;
    bool loop = false;
};

class AudioManager
{
public:
    AudioManager() = default;
    ~AudioManager();

    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    void LoadWaveFile(const char* filename, const char* name, bool loop);
    void Play(const char* name);
    void Stop(const char* name);
    void SetVolume(const char* name, float volume);
    static AudioManager& GetInstance();
private:
    bool Initialize();

    ComPtr<IXAudio2> xaudio2_;
    IXAudio2MasteringVoice* mastering_voice_ = nullptr;
    std::unordered_map<std::string, AudioData> audio_data_;
    
};
