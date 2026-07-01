#pragma once
#include <functional>
#include <memory>
#include <string>
#include "level_base.h"

namespace LevelName
{
    inline constexpr std::string_view kStage1 = "Stage1";
    inline constexpr std::string_view kStage2 = "Stage2";
    inline constexpr std::string_view kTest = "Test";
    inline constexpr std::string_view kTitle = "Title";
    inline constexpr std::string_view kSelect = "Select";
}

/**
 * @brief LevelFactoryのデータと処理をまとめる型。
 */
class LevelFactory
{
public:
    /**
     * @brief 利用前に必要な参照とリソースを初期化する。
     */
    void Initialize();
    using CreateFunc = std::function<std::unique_ptr<LevelBase>()>;

    /**
     * @brief 対象を管理リストへ登録する。
     * @param name 対象の名前。
     */
    template<class T>
    void Register(const std::string& name)
    {
        static_assert(std::is_base_of_v<LevelBase, T>);
        factories_[name] = []{return std::make_unique<T>();};
    }
    /**
     * @brief 内部で使用するリソースを作成する。
     * @param name 対象の名前。
     * @return 対象リソースの作成が完了した場合は true。
     */
    std::unique_ptr<LevelBase> Create(const std::string& name);

private:
    std::unordered_map<std::string, CreateFunc> factories_;
};
