#pragma once
#include "../Core/common.h"
#include <xaudio2.h>
#include <vector>

class AudioPlayer
{
public:
    AudioPlayer();
    ~AudioPlayer();

    AudioPlayer(const AudioPlayer&) = delete;
    AudioPlayer& operator=(const AudioPlayer&) = delete;
    
    bool Initialize(IXAudio2* xaudio2);
    bool LoadWaveFile(const char* filename, bool loop);
    bool Play(bool loop);
    void Stop();
    void SetVolume(float volume);
    void DestroyVoice();

private:
    IXAudio2* xaudio2_ = nullptr;
    IXAudio2SourceVoice* source_voice_ = nullptr;
    
    WAVEFORMATEX wave_format_ = {};
    std::vector<uint8_t> audio_data_;
};
