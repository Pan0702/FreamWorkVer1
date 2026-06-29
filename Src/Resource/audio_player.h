#pragma once
#include "../Core/common.h"
#include <xaudio2.h>
#include <vector>

// AudioPlayer に関係する状態と振る舞いをまとめる型。
class AudioPlayer
{
public:
    /**
     * @brief 値を初期化する。
     */
    AudioPlayer();
    /**
     * @brief 保持している登録やリソースを解放する。
     */
    ~AudioPlayer();

    /**
     * @brief 値を初期化する。
     */
    AudioPlayer(const AudioPlayer&) = delete;
    /**
     * @brief 演算子 operator= で値を扱う。
     * @return 演算結果を反映した自分自身。
     */
    AudioPlayer& operator=(const AudioPlayer&) = delete;
    
    /**
     * @brief 利用前に必要な参照とリソースを初期化する。
     * @param xaudio2 ボイス作成に使う XAudio2 インスタンス。
     * @return 必要なリソースを作成し、使用可能な状態にできた場合は true。
     */
    bool Initialize(IXAudio2* xaudio2);
    /**
     * @brief ファイルや外部データを読み込んで内部表現へ変換する。
     * @param filename 読み書きするファイルパス。
     * @param loop ループ再生するかどうか。
     * @return 指定データの読み込みが完了した場合は true。
     */
    bool LoadWaveFile(const char* filename, bool loop);
    /**
     * @brief 指定したアニメーションの再生を開始する。
     * @param loop ループ再生するかどうか。
     * @return 指定したアニメーションの再生を開始できた 場合は true。
     */
    bool Play(bool loop);
    /**
     * @brief 再生中のアニメーションを停止する。
     */
    void Stop();
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param volume 処理対象の値。
     */
    void SetVolume(float volume);
    /**
     * @brief XAudio2 の再生ボイスを破棄する。
     */
    void DestroyVoice();

private:
    IXAudio2* xaudio2_ = nullptr;
    IXAudio2SourceVoice* source_voice_ = nullptr;
    
    WAVEFORMATEX wave_format_ = {};
    std::vector<uint8_t> audio_data_;
};
