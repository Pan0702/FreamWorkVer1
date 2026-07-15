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
     * @param sprite_renderer 処理対象の値。
     * @param ui_renderer 処理対象の値。
     * @param debug_line_renderer 処理対象の値。
     */
    void Initialize(SpriteRenderer* sprite_renderer, UIRenderer* ui_renderer
        ,DebugLineRenderer* debug_line_renderer);
    /**
     * @brief 保持しているリソースと登録状態を解放する。
     */
    void Shutdown();
    /**
     * @brief ログへ文字列を書き込む。
     * @param format 表示またはリソース作成に使うフォーマット。
     */
    void Log(const char* format, ...);
    /**
     * @brief デバッグ表示へ文字列を積む。
     * @param format 表示またはリソース作成に使うフォーマット。
     */
    void Print(const char* format, ...);
    /**
     * @brief 現在の状態をもとに描画コマンドを積む。
     * @param texture 使用するテクスチャ。
     * @param position 描画または配置に使う座標。
     * @param size 設定するサイズ。
     * @param color 設定する色。
     */
    void DrawSprite(Texture2D* texture, const Vec2& position, const Vec2& size, const Vec4& color) const;
  /**
     * @brief 現在の状態をもとに描画コマンドを積む。
     * @param position 描画または配置に使う座標。
     * @param size 設定するサイズ。
     * @param color 設定する色。
     * @param rotation 回転角度。
     * @param sort 描画順番。
     */
    /**
     * @brief 2D の塗りつぶし矩形を描画コマンドへ追加する。
     */
    void DrawBox(const Vec2& position, const Vec2& size, const Vec4& color, float rotation = 0.0f, uint64 sort = 100.0f) const;
  
    /**
     * @brief 現在の状態をもとに描画コマンドを積む。
     * @param start 線分または範囲の始点。
     * @param end 線分または範囲の終点。
     * @param color 設定する色。
     */
    void DrawLine(const Vec2& start, const Vec2& end, const Vec4& color) const;
    /**
     * @brief 現在の状態をもとに描画コマンドを積む。
     * @param center 形状の中心座標。
     * @param radius 形状の半径。
     * @param color 設定する色。
     */
    void DrawCircle(const Vec2& center, float radius, const Vec4& color) const;
    /**
     * @brief 現在の状態をもとに描画コマンドを積む。
     * @param start 線分または範囲の始点。
     * @param end 線分または範囲の終点。
     * @param color 設定する色。
     */
    void DrawLine3D(const Vec3& start, const Vec3& end, const Vec4& color) const;
    /**
     * @brief 現在の状態をもとに描画コマンドを積む。
     * @param center 形状の中心座標。
     * @param size 設定するサイズ。
     * @param color 設定する色。
     * @param rotation 回転角度。
     * @param fill 塗りつぶし描画を行うかどうか。
     */
    /**
     * @brief 3D のボックスをデバッグ描画コマンドへ追加する。
     */
    void DrawBox3D(const Vec3& center, const Vec3& size, const Vec4& color, const Vec3& rotation, bool fill = true) const;
    /**
     * @brief 現在の状態をもとに描画コマンドを積む。
     * @param center 形状の中心座標。
     * @param radius 形状の半径。
     * @param color 設定する色。
     */
    void DrawSphere3D(const Vec3& center, float radius, const Vec4& color) const;
    /**
     * @brief 現在の状態をもとに描画コマンドを積む。
     * @param texture 使用するテクスチャ。
     * @param position 描画または配置に使う座標。
     * @param rotation 回転角度。
     * @param size 設定するサイズ。
     * @param src_pos 切り出し元の左上座標。
     * @param src_size 切り出し元のサイズ。
     * @param alpha 描画時の透明度。
     */
    void DrawSprite3D(Texture2D* texture, const Vec3& position, const Vec3& rotation, 
        const Vec2& size, const Vec2& src_pos, const Vec2& src_size, float alpha);
    /**
     * @brief 現在の状態をもとに描画コマンドを積む。
     * @param texture 使用するテクスチャ。
     * @param mat 描画に使用するマテリアル。
     * @param size 設定するサイズ。
     * @param src_pos 切り出し元の左上座標。
     * @param src_size 切り出し元のサイズ。
     * @param alpha 描画時の透明度。
     */
    /**
     * @brief 3D 空間に配置するスプライトを描画コマンドへ追加する。
     */
    void DrawSprite3D(Texture2D* texture,const Mat& mat,const Vec2& size, const Vec2& src_pos, const Vec2& src_size, float alpha) const;
    /**
     * @brief 監視したい値をデバッグ表示へ登録する。
     * @param name 対象の名前。
     * @param value 表示または編集する値。
     */
    void Watch(const char* name, float value);
private:
    /**
     * @brief 値を初期化する。
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
