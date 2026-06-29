#pragma once
#include <memory>
#include <unordered_map>
#include <string>
enum class PlayerState : uint8_t;
class StateComponentBase;

// ComponentFactor に関係する状態と振る舞いをまとめる型。
class ComponentFactor
{
public:
    /**
     * @brief 値を初期化する。
     */
    ComponentFactor();
    using ComponentMap = std::unordered_map<PlayerState,StateComponentBase*>;
    /**
     * @brief Components を取得する。
     * @param names 作成する状態コンポーネントの一覧。
     * @return 現在保持している Components。
     */
    ComponentMap GetComponents(const std::vector<PlayerState>& names) const; 
private:
    std::unordered_map<PlayerState,std::unique_ptr<StateComponentBase>> components_;
};
