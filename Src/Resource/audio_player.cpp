#include "audio_player.h"
#include <cstring>
#include <fstream>
#include <vector>

namespace
{
    // ファイル入出力で使う ChunkHeader のバイナリ構造を表す。
    struct ChunkHeader
    {
        char id[4];
        uint32_t size;
    };
}

AudioPlayer::AudioPlayer() = default;
AudioPlayer::~AudioPlayer()
{
    DestroyVoice();
}

bool AudioPlayer::Initialize(IXAudio2* xaudio2)
{
    if (xaudio2 == nullptr)
    {
        return false;
    }

    xaudio2_ = xaudio2;
    return true;
}

bool AudioPlayer::LoadWaveFile(const char* filename, bool)
{
    DestroyVoice();
    wave_format_ = {};
    audio_data_.clear();

    std::ifstream file(filename, std::ios::binary);
    if (!file)
    {
        return false;
    }

    ChunkHeader riff{};
    file.read(reinterpret_cast<char*>(&riff), static_cast<std::streamsize>(sizeof(riff)));
    if (!file || std::strncmp(riff.id, "RIFF", 4) != 0)
    {
        return false;
    }

    char wave[4]{};
    file.read(wave, static_cast<std::streamsize>(sizeof(wave)));
    if (!file || std::strncmp(wave, "WAVE", 4) != 0)
    {
        return false;
    }

    bool has_format = false;
    bool has_data = false;

    while (file && (!has_format || !has_data))
    {
        ChunkHeader chunk{};
        file.read(reinterpret_cast<char*>(&chunk), static_cast<std::streamsize>(sizeof(chunk)));

        if (!file)
        {
            break;
        }

        if (std::strncmp(chunk.id, "fmt ", 4) == 0)
        {
            if (chunk.size < sizeof(PCMWAVEFORMAT))
            {
                return false;
            }

            std::vector<uint8_t> format_data(chunk.size);
            file.read(reinterpret_cast<char*>(format_data.data()), static_cast<std::streamsize>(chunk.size));
            if (!file)
            {
                return false;
            }

            const size_t copy_size = chunk.size < sizeof(WAVEFORMATEX) ? chunk.size : sizeof(WAVEFORMATEX);
            std::memcpy(&wave_format_, format_data.data(), copy_size);
            has_format = true;
        }
        else if (std::strncmp(chunk.id, "data", 4) == 0)
        {
            audio_data_.resize(chunk.size);
            file.read(reinterpret_cast<char*>(audio_data_.data()), static_cast<std::streamsize>(chunk.size));
            if (!file)
            {
                return false;
            }
            has_data = true;
        }
        else
        {
            file.seekg(static_cast<std::streamoff>(chunk.size), std::ios::cur);
        }
        if (chunk.size % 2 == 1)
        {
            file.seekg(static_cast<std::streamoff>(1), std::ios::cur);
        }
    }
    if (!has_format || !has_data)
    {
        return false;
    }
    if (wave_format_.wFormatTag != WAVE_FORMAT_PCM)
    {
        return false;
    }
    if (wave_format_.wBitsPerSample != 16)
    {
        return false;
    }
    return true;
}

bool AudioPlayer::Play(bool loop)
{
    if (xaudio2_ == nullptr)
    {
        return false;
    }
    if (audio_data_.empty())
    {
        return false;
    }
    if (source_voice_ != nullptr)
    {
        source_voice_->DestroyVoice();
        source_voice_ = nullptr;
    }
    HRESULT hr = xaudio2_->CreateSourceVoice(&source_voice_, &wave_format_);
    if (FAILED(hr))
    {
        return false;
    }
    
    XAUDIO2_BUFFER buffer{};
    buffer.AudioBytes = static_cast<UINT32>(audio_data_.size());
    buffer.pAudioData = audio_data_.data();
    buffer.Flags = XAUDIO2_END_OF_STREAM;
    buffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;
    hr = source_voice_->SubmitSourceBuffer(&buffer);
    if (FAILED(hr))
    {
        DestroyVoice();
        return false;
    }
    hr = source_voice_->Start();
    if (FAILED(hr))
    {
        DestroyVoice();
        return false;
    }
    return true;  
}

void AudioPlayer::Stop()
{
    if (source_voice_ != nullptr)
    {
        source_voice_->Stop();
        source_voice_->FlushSourceBuffers();
        source_voice_->DestroyVoice();
        source_voice_ = nullptr;
    }  
}

void AudioPlayer::SetVolume(float volume)
{
    if (source_voice_ != nullptr )
    {
        source_voice_->SetVolume(volume);
    }
}

void AudioPlayer::DestroyVoice()
{
    if (source_voice_ != nullptr)
    {
        source_voice_->DestroyVoice();
        source_voice_ = nullptr;
    }
}
