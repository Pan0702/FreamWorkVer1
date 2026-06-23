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
     * @brief インスタンスの初期状態を整える。
     */
    AnimatorManager();
    /**
     * @brief 保持している登録やリソースを解放する。
     */
    ~AnimatorManager();
    /**
     * @brief 保持しているリソースと登録状態を解放する。
     */
    void Shutdown();
    /**
     * @brief ファイルや外部データを読み込んで内部表現へ変換する。
     * @param path 読み書きするファイルパス。
     * @return 指定リソースの読み込みが完了した場合は true。
     */
    Animation* Load(const std::string& path);
    /**
     * @brief アニメーション を取得する。
     * @param name name に設定する値。
     * @return アニメーション。見つからない、または未作成の場合は nullptr。
     */
    Animation* GetAnimation(const std::string& name);
    /**
     * @brief AnimatorManager の共有インスタンスを取得する。
     * @return アニメーションキャッシュを管理する AnimatorManager への参照。
     */
    static AnimatorManager& Get();
private:
    std::unordered_map<std::string,std::unique_ptr<Animation>> cache_;
};
