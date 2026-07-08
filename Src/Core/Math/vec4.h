#pragma once
#include <cmath>
#include <DirectXMath.h>
#include <windows.h>
using namespace DirectX;

/**
 * @brief Vec4のデータと処理をまとめる型。
 */
struct Vec4 : public XMFLOAT4
{
    /**
     * @brief 値を初期化する。
     */
    Vec4() : XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)
    {
    }
    /**
     * @brief 値を初期化する。
     * @param x X 成分。
     * @param y Y 成分。
     * @param z Z 成分。
     * @param y Y 成分。
     * @param z Z 成分。
     * @param w W 成分。
     */
    Vec4(float x, float y, float z, float w) : XMFLOAT4(x, y, z, w)
    {
    }

    Vec4(const Vec3& v, float w = 0.0f)
    {
        this->x = v.x;
        this->y = v.y;
        this->z = v.z;
        this->w = w;
    }
    /**
     * @brief 値を初期化する。
     * @param v 演算に使うベクトル。
     */
    Vec4(const XMVECTOR& v)
    {
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(this), v);
    }

    /**
     * @brief 演算子 operator+ で値を扱う。
     * @return 演算結果として作成した新しい値。
     */
    Vec4 operator+(const Vec4& v) const { return Vec4(x + v.x, y + v.y, z + v.z, w + v.w); }
    /**
     * @brief 演算子 operator- で値を扱う。
     * @return 演算結果として作成した新しい値。
     */
    Vec4 operator-(const Vec4& v) const { return Vec4(x - v.x, y - v.y, z - v.z, w - v.w); }
    /**
     * @brief 演算子 operator* で値を扱う。
     * @return 演算結果として作成した新しい値。
     */
    Vec4 operator*(const Vec4& v) const { return Vec4(x * v.x, y * v.y, z * v.z, w * v.w); }
    /**
     * @brief 演算子 operator/ で値を扱う。
     * @return 演算結果として作成した新しい値。
     */
    Vec4 operator/(const Vec4& v) const { return Vec4(x / v.x, y / v.y, z / v.z, w / v.w); }
    /**
     * @brief 演算子 operator+ で値を扱う。
     * @param n 各成分に適用するスカラー値。
     * @param n 各成分に適用するスカラー値。
     * @param n 各成分に適用するスカラー値。
     * @param n 各成分に適用するスカラー値。
     * @return 演算結果として作成した新しい値。
     */
    Vec4 operator+(float n) const { return Vec4(x + n, y + n, z + n, w + n); }
    /**
     * @brief 演算子 operator- で値を扱う。
     * @param n 各成分に適用するスカラー値。
     * @param n 各成分に適用するスカラー値。
     * @param n 各成分に適用するスカラー値。
     * @param n 各成分に適用するスカラー値。
     * @return 演算結果として作成した新しい値。
     */
    Vec4 operator-(float n) const { return Vec4(x - n, y - n, z - n, w - n); }
    /**
     * @brief 演算子 operator* で値を扱う。
     * @param n 各成分に適用するスカラー値。
     * @param n 各成分に適用するスカラー値。
     * @param n 各成分に適用するスカラー値。
     * @param n 各成分に適用するスカラー値。
     * @return 演算結果として作成した新しい値。
     */
    Vec4 operator*(float n) const { return Vec4(x * n, y * n, z * n, w * n); }
    /**
     * @brief 演算子 operator/ で値を扱う。
     * @param n 各成分に適用するスカラー値。
     * @param n 各成分に適用するスカラー値。
     * @param n 各成分に適用するスカラー値。
     * @param n 各成分に適用するスカラー値。
     * @return 演算結果として作成した新しい値。
     */
    Vec4 operator/(float n) const { return Vec4(x / n, y / n, z / n, w / n); }

    /**
     * @brief 演算子 operator+= で値を扱う。
     * @param v 演算に使うベクトル。
     * @return 演算結果を反映した自分自身。
     */
    Vec4& operator+=(const Vec4& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
    /**
     * @brief 演算子 operator-= で値を扱う。
     * @param v 演算に使うベクトル。
     * @return 演算結果を反映した自分自身。
     */
    Vec4& operator-=(const Vec4& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
    /**
     * @brief 演算子 operator*= で値を扱う。
     * @param v 演算に使うベクトル。
     * @return 演算結果を反映した自分自身。
     */
    Vec4& operator*=(const Vec4& v) { x *= v.x; y *= v.y; z *= v.z; w *= v.w; return *this; }
    /**
     * @brief 演算子 operator/= で値を扱う。
     * @param v 演算に使うベクトル。
     * @return 演算結果を反映した自分自身。
     */
    Vec4& operator/=(const Vec4& v) { x /= v.x; y /= v.y; z /= v.z; w /= v.w; return *this; }
    /**
     * @brief 演算子 operator+= で値を扱う。
     * @param n 各成分に適用するスカラー値。
     * @return 演算結果を反映した自分自身。
     */
    Vec4& operator+=(float n) { x += n; y += n; z += n; w += n; return *this; }
    /**
     * @brief 演算子 operator-= で値を扱う。
     * @param n 各成分に適用するスカラー値。
     * @return 演算結果を反映した自分自身。
     */
    Vec4& operator-=(float n) { x -= n; y -= n; z -= n; w -= n; return *this; }
    /**
     * @brief 演算子 operator*= で値を扱う。
     * @param n 各成分に適用するスカラー値。
     * @return 演算結果を反映した自分自身。
     */
    Vec4& operator*=(float n) { x *= n; y *= n; z *= n; w *= n; return *this; }
    /**
     * @brief 演算子 operator/= で値を扱う。
     * @param n 各成分に適用するスカラー値。
     * @return 演算結果を反映した自分自身。
     */
    Vec4& operator/=(float n) { x /= n; y /= n; z /= n; w /= n; return *this; }

    bool operator==(const Vec4& v) const { return x == v.x && y == v.y && z == v.z && w == v.w; }
    /**
     * @brief 演算子 operator!= で値を扱う。
     * @return 左右の値が異なる場合は true。
     */
    bool operator!=(const Vec4& v) const { return !(*this == v); }

    /**
     * @brief 演算子 operator+ で値を扱う。
     * @param y Y 成分。
     * @param z Z 成分。
     * @param w W 成分。
     * @return 演算結果として作成した新しい値。
     */
    Vec4 operator+() const { return Vec4(x, y, z, w); }
    /**
     * @brief 演算子 operator- で値を扱う。
     * @return 演算結果として作成した新しい値。
     */
    Vec4 operator-() const { return Vec4(-x, -y, -z, -w); }

    /**
     * @brief 演算子 operator= で値を扱う。
     * @param v 演算に使うベクトル。
     * @return 演算結果を反映した自分自身。
     */
    Vec4& operator=(const Vec4& v) = default;

    /**
     * @brief 演算子 operator= で値を扱う。
     * @param v 演算に使うベクトル。
     * @return 演算結果を反映した自分自身。
     */
    Vec4& operator=(const XMVECTOR& v)
    {
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(this), v);
        return *this;
    }

    /**
     * @brief 値を初期化する。
     */
    operator XMVECTOR() const
    {
        return XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(this));
    }

    /**
     * @brief 値を初期化する。
     */
    operator XMFLOAT4() const
    {
        return {x, y, z, w};
    }

    /**
     * @brief ベクトルの長さの二乗を求める。
     * @return 平方根を取らない Vec4 の長さの二乗。
     */
    float LengthSquared() const { return x * x + y * y + z * z + w * w; }
    /**
     * @brief ベクトルの長さを求める。
     * @return Vec4 の長さ。
     */
    float Length() const { return std::sqrt(LengthSquared()); }

    /**
     * @brief ベクトルや角度の補助計算を行う。
     * @return 長さを 1 にそろえた Vec4。
     */
    Vec4 Normalized() const
    {
        float len = Length();
        return (len > 0) ? Vec4(x / len, y / len, z / len, w / len) : Vec4(0, 0, 0, 0);
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
inline Vec4 operator*(float n, const Vec4& v) { return v * n; }
/**
 * @brief ベクトルや角度の補助計算を行う。
 * @param a 1 つ目のベクトルまたは点。
 * @param b 2 つ目のベクトルまたは点。
 * @return 2 つのベクトルの内積。
 */
inline float Dot(const Vec4& a, const Vec4& b) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }
/**
 * @brief ベクトルや角度の補助計算を行う。
 * @param a 1 つ目のベクトルまたは点。
 * @return 保持している Vec4 値 への参照。
 */
inline float DistanceSquared(const Vec4& a, const Vec4& b) { return (a - b).LengthSquared(); }
/**
 * @brief ベクトルや角度の補助計算を行う。
 * @param a 1 つ目のベクトルまたは点。
 * @return ベクトルの長さ。
 */
inline float Distance(const Vec4& a, const Vec4& b) { return (a - b).Length(); }
/**
 * @brief ベクトルや角度の補助計算を行う。
 * @param a 1 つ目のベクトルまたは点。
 * @param b 2 つ目のベクトルまたは点。
 * @param a 1 つ目のベクトルまたは点。
 * @return a から b へ t だけ線形補間した値。
 */
inline Vec4 Lerp(const Vec4& a, const Vec4& b, float t) { return a + (b - a) * t; }
