#pragma once
#include <functional>
#include <memory>
#include <string>
#include "level_base.h"

/**
 * @brief LevelFactoryのデータと処理をまとめる型。
 */
class LevelFactory
{
public:
    /**
     * @brief 初期化に必要な参照とリソースを設定する関数。
     */
    void Initialize();
    using CreateFunc = std::function<std::unique_ptr<LevelBase>()>;

    // T: 登録する LevelBase 派生型。name: 生成時に使う名前。
    template<class T>
    void Register(const std::string& name)
    {
        static_assert(std::is_base_of_v<LevelBase, T>);
        factories_[name] = []{return std::make_unique<T>();};
    }
    /**
     * @brief DirectX 12 リソースを作成する関数。
     * @param name 検索または識別に使う名前。
     * @return 戻り値。
     */
    std::unique_ptr<LevelBase> Create(const std::string& name);

private:
    std::unordered_map<std::string, CreateFunc> factories_;
};
