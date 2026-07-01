#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "../Core/common.h"
#include <xaudio2.h>

class AudioPlayer;


// AudioData に関係する状態と振る舞いをまとめる型。
struct AudioData
{
    std::unique_ptr<AudioPlayer> player = nullptr;
    bool loop = false;
};

// AudioManager が扱うリソースの生成と共有を管理する。
class AudioManager
{
public:
    /**
     * @brief 値を初期化する。
     */
    AudioManager() = default;
    /**
     * @brief 保持している登録やリソースを解放する。
     */
    ~AudioManager();

    /**
     * @brief 値を初期化する。
     */
    AudioManager(const AudioManager&) = delete;
    /**
     * @brief 演算子 operator= で値を扱う。
     * @return 演算結果を反映した自分自身。
     */
    AudioManager& operator=(const AudioManager&) = delete;

    /**
     * @brief ファイルや外部データを読み込んで内部表現へ変換する。
     * @param filename 読み書きするファイルパス。
     * @param name 対象の名前。
     * @param loop ループ再生するかどうか。
     */
    void LoadWaveFile(const char* filename, const char* name, bool loop);
    /**
     * @brief 指定したアニメーションの再生を開始する。
     * @param name 対象の名前。
     */
    void Play(const std::string& name);
    /**
     * @brief 再生中のアニメーションを停止する。
     * @param name 対象の名前。
     */
    void Stop(const char* name);
    /**
     * @brief Stop all registered audio.
     */
    void StopAll();
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param name 対象の名前。
     * @param volume 処理対象の値。
     */
    void SetVolume(const std::string& name, float volume);
    /**
     * @brief Win32 インスタンスハンドル を取得する。
     * @return シングルトンとして保持しているインスタンス。
     */
    static AudioManager& GetInstance();
private:
    /**
     * @brief 利用前に必要な参照とリソースを初期化する。
     * @return 必要なリソースを作成し、使用可能な状態にできた場合は true。
     */
    bool Initialize();

    ComPtr<IXAudio2> xaudio2_;
    IXAudio2MasteringVoice* mastering_voice_ = nullptr;
    std::unordered_map<std::string, AudioData> audio_data_;
    
};
