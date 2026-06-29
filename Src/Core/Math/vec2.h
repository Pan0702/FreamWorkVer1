#pragma once
#include <DirectXMath.h>
#include <cmath>
using namespace DirectX;

/**
 * @brief Vec2のデータと処理をまとめる型。
 */
struct Vec2 : public XMFLOAT2
{
    /**
     * @brief 値を初期化する。
     */
    Vec2() : XMFLOAT2(0.0f, 0.0f)
    {
    }
    /**
     * @brief 値を初期化する。
     * @param x X 成分。
     * @param y Y 成分。
     */
    Vec2(float x, float y) : XMFLOAT2(x, y)
    {
    }

    /**
     * @brief 値を初期化する。
     * @param v 演算に使うベクトル。
     */
    Vec2(const XMVECTOR& v)
    {
        XMStoreFloat2((this), v);
    }

    /**
     * @brief 演算子 operator+ で値を扱う。
     * @return 演算結果として作成した新しい値。
     */
    Vec2 operator+(const Vec2& v) const { return Vec2(x + v.x, y + v.y); }
    /**
     * @brief 演算子 operator- で値を扱う。
     * @return 演算結果として作成した新しい値。
     */
    Vec2 operator-(const Vec2& v) const { return Vec2(x - v.x, y - v.y); }
    /**
     * @brief 演算子 operator* で値を扱う。
     * @return 演算結果として作成した新しい値。
     */
    Vec2 operator*(const Vec2& v) const { return Vec2(x * v.x, y * v.y); }
    /**
     * @brief 演算子 operator/ で値を扱う。
     * @return 演算結果として作成した新しい値。
     */
    Vec2 operator/(const Vec2& v) const { return Vec2(x / v.x, y / v.y); }
    /**
     * @brief 演算子 operator+ で値を扱う。
     * @param n 各成分に適用するスカラー値。
     * @param n 各成分に適用するスカラー値。
     * @return 演算結果として作成した新しい値。
     */
    Vec2 operator+(float n) const { return Vec2(x + n, y + n); }
    /**
     * @brief 演算子 operator- で値を扱う。
     * @param n 各成分に適用するスカラー値。
     * @param n 各成分に適用するスカラー値。
     * @return 演算結果として作成した新しい値。
     */
    Vec2 operator-(float n) const { return Vec2(x - n, y - n); }
    /**
     * @brief 演算子 operator* で値を扱う。
     * @param n 各成分に適用するスカラー値。
     * @param n 各成分に適用するスカラー値。
     * @return 演算結果として作成した新しい値。
     */
    Vec2 operator*(float n) const { return Vec2(x * n, y * n); }
    /**
     * @brief 演算子 operator/ で値を扱う。
     * @param n 各成分に適用するスカラー値。
     * @param n 各成分に適用するスカラー値。
     * @return 演算結果として作成した新しい値。
     */
    Vec2 operator/(float n) const { return Vec2(x / n, y / n); }

    /**
     * @brief 演算子 operator+= で値を扱う。
     * @param v 演算に使うベクトル。
     * @return 演算結果を反映した自分自身。
     */
    Vec2& operator+=(const Vec2& v) { x += v.x; y += v.y; return *this; }
    /**
     * @brief 演算子 operator-= で値を扱う。
     * @param v 演算に使うベクトル。
     * @return 演算結果を反映した自分自身。
     */
    Vec2& operator-=(const Vec2& v) { x -= v.x; y -= v.y; return *this; }
    /**
     * @brief 演算子 operator*= で値を扱う。
     * @param v 演算に使うベクトル。
     * @return 演算結果を反映した自分自身。
     */
    Vec2& operator*=(const Vec2& v) { x *= v.x; y *= v.y; return *this; }
    /**
     * @brief 演算子 operator/= で値を扱う。
     * @param v 演算に使うベクトル。
     * @return 演算結果を反映した自分自身。
     */
    Vec2& operator/=(const Vec2& v) { x /= v.x; y /= v.y; return *this; }
    /**
     * @brief 演算子 operator+= で値を扱う。
     * @param n 各成分に適用するスカラー値。
     * @return 演算結果を反映した自分自身。
     */
    Vec2& operator+=(float n) { x += n; y += n; return *this; }
    /**
     * @brief 演算子 operator-= で値を扱う。
     * @param n 各成分に適用するスカラー値。
     * @return 演算結果を反映した自分自身。
     */
    Vec2& operator-=(float n) { x -= n; y -= n; return *this; }
    /**
     * @brief 演算子 operator*= で値を扱う。
     * @param n 各成分に適用するスカラー値。
     * @return 演算結果を反映した自分自身。
     */
    Vec2& operator*=(float n) { x *= n; y *= n; return *this; }
    /**
     * @brief 演算子 operator/= で値を扱う。
     * @param n 各成分に適用するスカラー値。
     * @return 演算結果を反映した自分自身。
     */
    Vec2& operator/=(float n) { x /= n; y /= n; return *this; }

    bool operator==(const Vec2& v) const { return x == v.x && y == v.y; }
    /**
     * @brief 演算子 operator!= で値を扱う。
     * @return 左右の値が異なる場合は true。
     */
    bool operator!=(const Vec2& v) const { return !(*this == v); }

    /**
     * @brief 演算子 operator+ で値を扱う。
     * @param y Y 成分。
     * @return 演算結果として作成した新しい値。
     */
    Vec2 operator+() const { return Vec2(x, y); }
    /**
     * @brief 演算子 operator- で値を扱う。
     * @return 演算結果として作成した新しい値。
     */
    Vec2 operator-() const { return Vec2(-x, -y); }

    /**
     * @brief 演算子 operator= で値を扱う。
     * @param v 演算に使うベクトル。
     * @return 演算結果を反映した自分自身。
     */
    Vec2& operator=(const Vec2& v) = default;

    /**
     * @brief 演算子 operator= で値を扱う。
     * @param v 演算に使うベクトル。
     * @return 演算結果を反映した自分自身。
     */
    Vec2& operator=(const XMVECTOR& v)
    {
        XMStoreFloat2((this), v);
        return *this;
    }

    /**
     * @brief 値を初期化する。
     */
    explicit operator XMVECTOR() const
    {
        return XMLoadFloat2((this));
    }

    /**
     * @brief 値を初期化する。
     */
    operator XMFLOAT2() const
    {
        return {x, y};
    }

    /**
     * @brief ベクトルの長さの二乗を求める。
     * @return 平方根を取らない Vec2 の長さの二乗。
     */
    float LengthSquared() const { return x * x + y * y; }
    /**
     * @brief ベクトルの長さを求める。
     * @return Vec2 の長さ。
     */
    float Length() const { return std::sqrt(LengthSquared()); }

    /**
     * @brief ベクトルや角度の補助計算を行う。
     * @return 長さを 1 にそろえた Vec2。
     */
    Vec2 Normalized() const
    {
        float len = Length();
        return (len > 0) ? Vec2(x / len, y / len) : Vec2(0, 0);
    }
    /**
     * @brief ベクトルや角度の補助計算を行う。
     */
    void Normalize() { *this = Normalized(); }
};
/**
 * @brief 演算子 operator* で値を扱う。
 * @param n 各成分に適用するスカラー値。
 * @param v 演算に使うベクトル。
 * @return 演算結果として作成した新しい値。
 */
inline Vec2 operator*(float n, const Vec2& v) { return v * n; }
/**
 * @brief ベクトルや角度の補助計算を行う。
 * @param a 1 つ目のベクトルまたは点。
 * @param b 2 つ目のベクトルまたは点。
 * @return 2 つのベクトルの内積。
 */
inline float Dot(const Vec2& a, const Vec2& b) { return a.x * b.x + a.y * b.y; }
/**
 * @brief ベクトルや角度の補助計算を行う。
 * @param a 1 つ目のベクトルまたは点。
 * @param b 2 つ目のベクトルまたは点。
 * @return 2 つのベクトルの外積。
 */
inline float Cross(const Vec2& a, const Vec2& b) { return a.x * b.y - a.y * b.x; }
/**
 * @brief ベクトルや角度の補助計算を行う。
 * @param a 1 つ目のベクトルまたは点。
 * @return 2 点間の距離の二乗。
 */
inline float DistanceSquared(const Vec2& a, const Vec2& b) { return (a - b).LengthSquared(); }
/**
 * @brief ベクトルや角度の補助計算を行う。
 * @param a 1 つ目のベクトルまたは点。
 * @return ベクトルの長さ。
 */
inline float Distance(const Vec2& a, const Vec2& b) { return (a - b).Length(); }
/**
 * @brief ベクトルや角度の補助計算を行う。
 * @param a 1 つ目のベクトルまたは点。
 * @param b 2 つ目のベクトルまたは点。
 * @param a 1 つ目のベクトルまたは点。
 * @return a から b へ t だけ線形補間した値。
 */
inline Vec2 Lerp(const Vec2& a, const Vec2& b, float t) { return a + (b - a) * t; }
