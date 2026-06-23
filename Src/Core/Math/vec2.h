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
     * @brief インスタンスの初期状態を整える。
     */
    Vec2() : XMFLOAT2(0.0f, 0.0f)
    {
    }
    /**
     * @brief インスタンスの初期状態を整える。
     * @param x x に設定する値。
     * @param y y に設定する値。
     */
    Vec2(float x, float y) : XMFLOAT2(x, y)
    {
    }

    /**
     * @brief インスタンスの初期状態を整える。
     * @param v 計算に使用するベクトルまたは点。
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
     * @param n n に設定する値。
     * @param n n に設定する値。
     * @return 演算結果として作成した新しい値。
     */
    Vec2 operator+(float n) const { return Vec2(x + n, y + n); }
    /**
     * @brief 演算子 operator- で値を扱う。
     * @param n n に設定する値。
     * @param n n に設定する値。
     * @return 演算結果として作成した新しい値。
     */
    Vec2 operator-(float n) const { return Vec2(x - n, y - n); }
    /**
     * @brief 演算子 operator* で値を扱う。
     * @param n n に設定する値。
     * @param n n に設定する値。
     * @return 演算結果として作成した新しい値。
     */
    Vec2 operator*(float n) const { return Vec2(x * n, y * n); }
    /**
     * @brief 演算子 operator/ で値を扱う。
     * @param n n に設定する値。
     * @param n n に設定する値。
     * @return 演算結果として作成した新しい値。
     */
    Vec2 operator/(float n) const { return Vec2(x / n, y / n); }

    /**
     * @brief 演算子 operator+= で値を扱う。
     * @param v 計算に使用するベクトルまたは点。
     * @return 演算結果を反映した自分自身。
     */
    Vec2& operator+=(const Vec2& v) { x += v.x; y += v.y; return *this; }
    /**
     * @brief 演算子 operator-= で値を扱う。
     * @param v 計算に使用するベクトルまたは点。
     * @return 演算結果を反映した自分自身。
     */
    Vec2& operator-=(const Vec2& v) { x -= v.x; y -= v.y; return *this; }
    /**
     * @brief 演算子 operator*= で値を扱う。
     * @param v 計算に使用するベクトルまたは点。
     * @return 演算結果を反映した自分自身。
     */
    Vec2& operator*=(const Vec2& v) { x *= v.x; y *= v.y; return *this; }
    /**
     * @brief 演算子 operator/= で値を扱う。
     * @param v 計算に使用するベクトルまたは点。
     * @return 演算結果を反映した自分自身。
     */
    Vec2& operator/=(const Vec2& v) { x /= v.x; y /= v.y; return *this; }
    /**
     * @brief 演算子 operator+= で値を扱う。
     * @param n n に設定する値。
     * @return 演算結果を反映した自分自身。
     */
    Vec2& operator+=(float n) { x += n; y += n; return *this; }
    /**
     * @brief 演算子 operator-= で値を扱う。
     * @param n n に設定する値。
     * @return 演算結果を反映した自分自身。
     */
    Vec2& operator-=(float n) { x -= n; y -= n; return *this; }
    /**
     * @brief 演算子 operator*= で値を扱う。
     * @param n n に設定する値。
     * @return 演算結果を反映した自分自身。
     */
    Vec2& operator*=(float n) { x *= n; y *= n; return *this; }
    /**
     * @brief 演算子 operator/= で値を扱う。
     * @param n n に設定する値。
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
     * @param y y に設定する値。
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
     * @param v 計算に使用するベクトルまたは点。
     * @return 演算結果を反映した自分自身。
     */
    Vec2& operator=(const Vec2& v) = default;

    /**
     * @brief 演算子 operator= で値を扱う。
     * @param v 計算に使用するベクトルまたは点。
     * @return 演算結果を反映した自分自身。
     */
    Vec2& operator=(const XMVECTOR& v)
    {
        XMStoreFloat2((this), v);
        return *this;
    }

    /**
     * @brief インスタンスの初期状態を整える。
     */
    explicit operator XMVECTOR() const
    {
        return XMLoadFloat2((this));
    }

    /**
     * @brief インスタンスの初期状態を整える。
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
     * @brief 数学計算の結果を求める。
     * @return 長さを 1 にそろえた Vec2。
     */
    Vec2 Normalized() const
    {
        float len = Length();
        return (len > 0) ? Vec2(x / len, y / len) : Vec2(0, 0);
    }
    /**
     * @brief 数学計算の結果を求める。
     * @param this this に設定する値。
     */
    void Normalize() { *this = Normalized(); }
};
/**
 * @brief 演算子 operator* で値を扱う。
 * @param n n に設定する値。
 * @param v 計算に使用するベクトルまたは点。
 * @return 演算結果として作成した新しい値。
 */
inline Vec2 operator*(float n, const Vec2& v) { return v * n; }
/**
 * @brief 数学計算の結果を求める。
 * @param a 計算に使用するベクトルまたは点。
 * @param b 計算に使用するベクトルまたは点。
 * @return 2 つのベクトルの内積。
 */
inline float Dot(const Vec2& a, const Vec2& b) { return a.x * b.x + a.y * b.y; }
/**
 * @brief 数学計算の結果を求める。
 * @param a 計算に使用するベクトルまたは点。
 * @param b 計算に使用するベクトルまたは点。
 * @return 2 つのベクトルの外積。
 */
inline float Cross(const Vec2& a, const Vec2& b) { return a.x * b.y - a.y * b.x; }
/**
 * @brief 数学計算の結果を求める。
 * @param a 計算に使用するベクトルまたは点。
 * @return 保持している Vec2 値 への参照。
 */
inline float DistanceSquared(const Vec2& a, const Vec2& b) { return (a - b).LengthSquared(); }
/**
 * @brief 数学計算の結果を求める。
 * @param a 計算に使用するベクトルまたは点。
 * @return ベクトルの長さ。
 */
inline float Distance(const Vec2& a, const Vec2& b) { return (a - b).Length(); }
/**
 * @brief 数学計算の結果を求める。
 * @param a 計算に使用するベクトルまたは点。
 * @param b 計算に使用するベクトルまたは点。
 * @param a 計算に使用するベクトルまたは点。
 * @return a から b へ t だけ線形補間した値。
 */
inline Vec2 Lerp(const Vec2& a, const Vec2& b, float t) { return a + (b - a) * t; }
