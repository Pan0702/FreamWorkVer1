#pragma once

/**
 * @brief レベル遷移の視覚効果を定義するインターフェース。
 */
class ITransitionEffect
{
public:
    /**
     * @brief インターフェース経由で遷移効果を破棄する。
     */
    virtual ~ITransitionEffect() = default;
    /**
     * @brief 指定した進行度で効果を描画する。
     * @param progress 0から1の範囲に正規化した遷移の進行度。
     */
    virtual void Draw(float progress) = 0;
};
