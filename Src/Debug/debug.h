#pragma once
#include "../Core/common.h"
#include "../Core/Math/my_math.h"

class DebugLineRenderer;
class SpriteRenderer;
class Texture2D;
class UIRenderer;
/**
 * @brief Debugのデータと処理をまとめる型。
 */
class Debug
{
public:
    /**
     * @brief Debug の共有インスタンスを取得する。
     * @return アプリ全体で共有する Debug インスタンスへの参照。
     */
    static Debug& Get();
    /**
     * @brief 利用前に必要な参照とリソースを初期化する。
     * @param sprite_renderer sprite_renderer に設定する値。
     * @param ui_renderer ui_renderer に設定する値。
     * @param debug_line_renderer debug_line_renderer に設定する値。
     */
    void Initialize(SpriteRenderer* sprite_renderer, UIRenderer* ui_renderer
        ,DebugLineRenderer* debug_line_renderer);
    /**
     * @brief 保持しているリソースと登録状態を解放する。
     */
    void Shutdown();
    /**
     * @brief ログへ文字列を書き込む。
     * @param format format に設定する値。
     */
    void Log(const char* format, ...);
    /**
     * @brief デバッグ表示へ文字列を積む。
     * @param format format に設定する値。
     */
    void Print(const char* format, ...);
    /**
     * @brief 現在の状態をもとに描画コマンドを積む。
     * @param texture 使用するテクスチャ。
     * @param position position に設定する値。
     * @param size size に設定する値。
     * @param color 設定する色。
     */
    void DrawSprite(Texture2D* texture, const Vec2& position, const Vec2& size, const Vec4& color) const;
    /**
     * @brief 現在の状態をもとに描画コマンドを積む。
     * @param position position に設定する値。
     * @param size size に設定する値。
     * @param color 設定する色。
     * @param rotation rotation に設定する値。
     */
    void DrawBox(const Vec2& position, const Vec2& size, const Vec4& color, float rotation = 0.0f) const;
    /**
     * @brief 現在の状態をもとに描画コマンドを積む。
     * @param start start に設定する値。
     * @param end end に設定する値。
     * @param color 設定する色。
     */
    void DrawLine(const Vec2& start, const Vec2& end, const Vec4& color) const;
    /**
     * @brief 現在の状態をもとに描画コマンドを積む。
     * @param center center に設定する値。
     * @param radius radius に設定する値。
     * @param color 設定する色。
     */
    void DrawCircle(const Vec2& center, float radius, const Vec4& color) const;
    /**
     * @brief 現在の状態をもとに描画コマンドを積む。
     * @param start start に設定する値。
     * @param end end に設定する値。
     * @param color 設定する色。
     */
    void DrawLine3D(const Vec3& start, const Vec3& end, const Vec4& color) const;
    /**
     * @brief 現在の状態をもとに描画コマンドを積む。
     * @param center center に設定する値。
     * @param size size に設定する値。
     * @param color 設定する色。
     * @param rotation rotation に設定する値。
     * @param fill fill に設定する値。
     */
    void DrawBox3D(const Vec3& center, const Vec3& size, const Vec4& color, const Vec3& rotation, bool fill = true) const;
    /**
     * @brief 現在の状態をもとに描画コマンドを積む。
     * @param center center に設定する値。
     * @param radius radius に設定する値。
     * @param color 設定する色。
     */
    void DrawSphere3D(const Vec3& center, float radius, const Vec4& color) const;
    /**
     * @brief 現在の状態をもとに描画コマンドを積む。
     * @param texture 使用するテクスチャ。
     * @param position position に設定する値。
     * @param rotation rotation に設定する値。
     * @param size size に設定する値。
     * @param src_pos src_pos に設定する値。
     * @param src_size src_size に設定する値。
     * @param alpha alpha に設定する値。
     */
    void DrawSprite3D(Texture2D* texture, const Vec3& position, const Vec3& rotation, 
        const Vec2& size, const Vec2& src_pos, const Vec2& src_size, float alpha);
    /**
     * @brief 現在の状態をもとに描画コマンドを積む。
     * @param texture 使用するテクスチャ。
     * @param mat 描画に使用するマテリアル。
     * @param size size に設定する値。
     * @param src_pos src_pos に設定する値。
     * @param src_size src_size に設定する値。
     * @param alpha alpha に設定する値。
     */
    void DrawSprite3D(Texture2D* texture,const Mat& mat,const Vec2& size, const Vec2& src_pos, const Vec2& src_size, float alpha) const;
    /**
     * @brief 監視したい値をデバッグ表示へ登録する。
     * @param name name に設定する値。
     * @param value value に設定する値。
     */
    void Watch(const char* name, float value);
private:
    /**
     * @brief インスタンスの初期状態を整える。
     */
    Debug() = default;

    SpriteRenderer* sprite_renderer_ = nullptr;
    UIRenderer* ui_renderer_ = nullptr;
    DebugLineRenderer* line_renderer_ = nullptr;
};

#ifdef _DEBUG
#define DEBUG_LOG(...) Debug::Get().Log(__VA_ARGS__)
#else
#define DEBUG_LOG(...) ((void)0)
#endif
