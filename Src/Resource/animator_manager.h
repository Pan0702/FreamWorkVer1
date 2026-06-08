#pragma once
#include <memory>
#include <string>
#include <unordered_map>
class Animation;
/**
 * @brief AnimatorManagerのデータと処理をまとめる型。
 */
class AnimatorManager
{
public:
    /**
     * @brief AnimatorManagerを初期化するコンストラクタ。
     */
    AnimatorManager();
    /**
     * @brief AnimatorManagerの終了処理を行うデストラクタ。
     */
    ~AnimatorManager();
    /**
     * @brief 保持しているリソースを解放する関数。
     */
    void Shutdown();
    /**
     * @brief 指定されたリソースを読み込む関数。
     * @param path 読み込むファイルパスまたは検索キー。
     * @return 戻り値。
     */
    Animation* Load(const std::string& path);
    /**
     * @brief Animationを取得する関数。
     * @param name 検索または識別に使う名前。
     * @return 戻り値。
     */
    Animation* GetAnimation(const std::string& name);
    /**
     * @brief インスタンスを取得する関数。
     * @return 戻り値。
     */
    static AnimatorManager& Get();
private:
    std::unordered_map<std::string,std::unique_ptr<Animation>> cache_;
};
