#pragma once
#include <memory>

#include "../../Engine/actor.h"

class MaterialSlot;

// Cube に関係する状態と振る舞いをまとめる型。
/**
 * @brief ステージ内に配置する静的なキューブを表す。
 */
class Cube : public Actor
{
public:
    /**
     * @brief 値を初期化する。
     */
    Cube();
    
    /**
     * @brief 指定した位置と大きさでキューブを生成する。
     * @param pos キューブのワールド座標。`r`n     * @param scale キューブの拡大縮小率。
     */
    Cube(const Vec3& pos, const Vec3& scale);
    /**
     * @brief 名前を指定してキューブを生成する。
     * @param name 生成するキューブの名前。
     */
    explicit Cube(const std::string& name);
private:
    std::unique_ptr<MaterialSlot> materials_;
};
