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
     * @brief インスタンスの初期状態を整える。
     */
    ComponentFactor();
    using ComponentMap = std::unordered_map<PlayerState,StateComponentBase*>;
    /**
     * @brief Components を取得する。
     * @param names names に設定する値。
     * @return 現在保持している Components。
     */
    ComponentMap GetComponents(const std::vector<PlayerState>& names) const; 
private:
    std::unordered_map<PlayerState,std::unique_ptr<StateComponentBase>> components_;
};
