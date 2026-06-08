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
     * @brief インスタンスを取得する関数。
     * @return 戻り値。
     */
    static Debug& Get();
    /**
     * @brief 初期化に必要な参照とリソースを設定する関数。
     * @param sprite_renderer 引数。
     * @param ui_renderer 引数。
     * @param debug_line_renderer 引数。
     */
    void Initialize(SpriteRenderer* sprite_renderer, UIRenderer* ui_renderer
        ,DebugLineRenderer* debug_line_renderer);
    /**
     * @brief 保持しているリソースを解放する関数。
     */
    void Shutdown();
    /**
     * @brief Logを行う関数。
     * @param format 出力する書式文字列。
     * @param value 設定する値。
     */
    void Log(const char* format, ...);
    /**
     * @brief Printを行う関数。
     * @param format 出力する書式文字列。
     * @param value 設定する値。
     */
    void Print(const char* format, ...);
    /**
     * @brief DrawSpriteを行う関数。
     * @param texture 描画に使うテクスチャ。
     * @param position 位置。
     * @param size サイズ情報。
     * @param color 色。
     */
    void DrawSprite(Texture2D* texture, const Vec2& position, const Vec2& size, const Vec4& color);
    /**
     * @brief DrawBoxを行う関数。
     * @param position 位置。
     * @param size サイズ情報。
     * @param color 色。
     * @param rotation 引数。
     */
    void DrawBox(const Vec2& position, const Vec2& size, const Vec4& color, float rotation = 0.0f);
    /**
     * @brief DrawLineを行う関数。
     * @param start 引数。
     * @param end 引数。
     * @param color 色。
     */
    void DrawLine(const Vec2& start, const Vec2& end, const Vec4& color);
    /**
     * @brief DrawCircleを行う関数。
     * @param center 引数。
     * @param radius 引数。
     * @param color 色。
     */
    void DrawCircle(const Vec2& center, float radius, const Vec4& color);
    /**
     * @brief DrawLine3Dを行う関数。
     * @param start 引数。
     * @param end 引数。
     * @param color 色。
     */
    void DrawLine3D(const Vec3& start, const Vec3& end, const Vec4& color);
    /**
     * @brief DrawBox3Dを行う関数。
     * @param center 引数。
     * @param size サイズ情報。
     * @param color 色。
     * @param rotation 引数。
     * @param fill 引数。
     */
    void DrawBox3D(const Vec3& center, const Vec3& size, const Vec4& color, const Vec3& rotation, bool fill = true);
    /**
     * @brief DrawSphere3Dを行う関数。
     * @param center 引数。
     * @param radius 引数。
     * @param color 色。
     */
    void DrawSphere3D(const Vec3& center, float radius, const Vec4& color);
    /**
     * @brief DrawSprite3Dを行う関数。
     * @param texture 描画に使うテクスチャ。
     * @param position 位置。
     * @param rotation 引数。
     * @param size サイズ情報。
     * @param src_pos 引数。
     * @param src_size 引数。
     * @param alpha 引数。
     */
    void DrawSprite3D(Texture2D* texture, const Vec3& position, const Vec3& rotation, 
        const Vec2& size, const Vec2& src_pos, const Vec2& src_size, float alpha);
    /**
     * @brief DrawSprite3Dを行う関数。
     * @param texture 描画に使うテクスチャ。
     * @param mat 引数。
     * @param size サイズ情報。
     * @param src_pos 引数。
     * @param src_size 引数。
     * @param alpha 引数。
     */
    void DrawSprite3D(Texture2D* texture,const Mat& mat,const Vec2& size, const Vec2& src_pos, const Vec2& src_size, float alpha);
    /**
     * @brief Watchを行う関数。
     * @param name 検索または識別に使う名前。
     * @param value 設定する値。
     */
    void Watch(const char* name, float value);
private:
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
