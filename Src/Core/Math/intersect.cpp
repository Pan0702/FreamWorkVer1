#include "intersect.h"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <vector>

namespace
{
    constexpr float kEpsilon = 1e-6f;
}

// ============================================================
// 最近接点ヘルパ
// ============================================================

Vec3 ClosestPointOnBox(const Vec3& p, const Box& box)
{
    return Vec3(
        std::clamp(p.x, box.min.x, box.max.x),
        std::clamp(p.y, box.min.y, box.max.y),
        std::clamp(p.z, box.min.z, box.max.z));
}

Vec3 ClosestPointOnTriangle(const Vec3& p, const Vec3& a, const Vec3& b, const Vec3& c)
{
    // 頂点a //
    Vec3 ab = b - a, ac = c - a, ap = p - a;
    float d1 = Dot(ab, ap), d2 = Dot(ac, ap);
    if (d1 <= 0 && d2 <= 0) return a;

    // 頂点b //
    Vec3 bp = p - b;
    float d3 = Dot(ab, bp), d4 = Dot(ac, bp);
    if (d3 >= 0 && d4 <= d3) return b;

    // 頂点c //
    Vec3 cp = p - c;
    float d5 = Dot(ab, cp), d6 = Dot(ac, cp);
    if (d6 >= 0 && d5 <= d6) return c;

    // 辺ab // 
    float vc = d1 * d4 - d3 * d2;
    if (vc <= 0 && d1 >= 0 && d3 <= 0)
    {
        float v = d1 / (d1 - d3);
        return a + ab * v;
    }

    // 辺ac //
    float vb = d5 * d2 - d1 * d6;
    if (vb <= 0 && d2 >= 0 && d6 <= 0)
    {
        float w = d2 / (d2 - d6);
        return a + ac * w;
    }

    // 辺bc 上 //
    float va = d3 * d6 - d5 * d4;
    if (va <= 0 && (d4 - d3) >= 0 && (d5 - d6) >= 0)
    {
        float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        return b + (c - b) * w;
    }

    // 面の内部（重心座標で分解）//
    float denom = 1.0f / (va + vb + vc);
    float v = vb * denom, w = vc * denom;
    return a + ab * v + ac * w;
}


// ============================================================
// 接触判定（貫通法線・深さを出力）
// out.normal は第1引数を第2引数から押し出す単位ベクトル、out.depth は正の貫通量
// ============================================================

bool Contact(const Sphere& s1, const Sphere& s2, ContactInfo& out)
{
    Vec3 d = s1.center - s2.center;
    float r = s1.radius + s2.radius;
    float l = d.Length();
    if (l > r)
    {
        return false;
    }
    out.normal = (l > kEpsilon) ? d / l : Vec3(0, 1, 0);
    out.depth = r - l;
    return true;
}

bool Contact(const Sphere& s, const Box& box, ContactInfo& out)
{
    Vec3 closest = ClosestPointOnBox(s.center, box);
    Vec3 d = s.center - closest;
    float dist2 = Dot(d, d);

    if (dist2 > s.radius * s.radius)
    {
        return false;
    }

    if (dist2 > 1e-12f)
    {
        float dist = sqrtf(dist2);
        out.normal = d / dist;
        out.depth = s.radius - dist;
    }
    else
    {
        float dx_min = s.center.x - box.min.x; // -x 面まで //
        float dx_max = box.max.x - s.center.x; // +x 面まで //
        float dy_min = s.center.y - box.min.y;
        float dy_max = box.max.y - s.center.y;
        float dz_min = s.center.z - box.min.z;
        float dz_max = box.max.z - s.center.z;

        // 6面のうち一番近い面を探す //
        float best = dx_min;
        out.normal = Vec3(-1, 0, 0);
        if (dx_max < best)
        {
            best = dx_max;
            out.normal = Vec3(1, 0, 0);
        }
        if (dy_min < best)
        {
            best = dy_min;
            out.normal = Vec3(0, -1, 0);
        }
        if (dy_max < best)
        {
            best = dy_max;
            out.normal = Vec3(0, 1, 0);
        }
        if (dz_min < best)
        {
            best = dz_min;
            out.normal = Vec3(0, 0, -1);
        }
        if (dz_max < best)
        {
            best = dz_max;
            out.normal = Vec3(0, 0, 1);
        }

        out.depth = s.radius + best;
    }
    return true;
}

bool Contact(const Box& b1, const Box& b2, ContactInfo& out)
{
    float ox = (std::min)(b1.max.x, b2.max.x) - (std::max)(b1.min.x, b2.min.x);
    if (ox < 0)
    {
        return false;
    }

    float oy = (std::min)(b1.max.y, b2.max.y) - (std::max)(b1.min.y, b2.min.y);
    if (oy < 0)
    {
        return false;
    }

    float oz = (std::min)(b1.max.z, b2.max.z) - (std::max)(b1.min.z, b2.min.z);
    if (oz < 0)
    {
        return false;
    }
    Vec3 b1_center = (b1.min + b1.max) * 0.5f;
    Vec3 b2_center = (b2.min + b2.max) * 0.5f;
    float best = ox;
    out.normal = (b1_center.x > b2_center.x) ? Vec3(1, 0, 0) : Vec3(-1, 0, 0);

    if (oy < best)
    {
        best = oy;
        out.normal = (b1_center.y > b2_center.y) ? Vec3(0, 1, 0) : Vec3(0, -1, 0);
    }
    if (oz < best)
    {
        best = oz;
        out.normal = (b1_center.z > b2_center.z) ? Vec3(0, 0, 1) : Vec3(0, 0, -1);
    }

    out.depth = best;
    return true;
}

bool Contact(const Sphere& s, const Vec3& a, const Vec3& b, const Vec3& c, ContactInfo& out)
{
    Vec3 q = ClosestPointOnTriangle(s.center, a, b, c);
    Vec3 d = s.center - q; // 三角形上の最近接点から球中心へ //

    float dist2 = Dot(d, d);

    if (dist2 > s.radius * s.radius)
    {
        return false;
    }
    float dist = sqrtf(dist2);
    if (dist > kEpsilon)
    {
        out.normal = d / dist;
    }
    else
    {
        // 中心がちょうど三角形面上 → 面の法線を代用 //
        out.normal = Cross(b - a, c - a).Normalized();
    }
    out.depth = s.radius - dist;
    return true;
}

bool Contact(const Box& box, const Vec3& a, const Vec3& b, const Vec3& c, ContactInfo& out)
{
    constexpr float kHalfSize = 0.5f;
    Vec3 box_center = (box.min + box.max) * kHalfSize;
    Vec3 box_half_size = (box.max - box.min) * kHalfSize;

    Vec3 axes[13];

    Vec3 ta = a - box_center;
    Vec3 tb = b - box_center;
    Vec3 tc = c - box_center;

    axes[0] = Vec3(1, 0, 0);
    axes[1] = Vec3(0, 1, 0);
    axes[2] = Vec3(0, 0, 1);

    Vec3 e0 = tb - ta;
    Vec3 e1 = tc - tb;
    Vec3 e2 = ta - tc;
    axes[3] = Cross(e1, e0);

    int index = 4;

    for (int i = 0; i < 3; i++)
    {
        axes[index++] = Cross(axes[i], e0);
        axes[index++] = Cross(axes[i], e1);
        axes[index++] = Cross(axes[i], e2);
    }

    float best = FLT_MAX;
    Vec3 best_normal;
    for (Vec3 axis : axes)
    {
        float len2 = axis.LengthSquared();
        if (len2 < 1e-8f)
        {
            continue;
        }
        axis = axis / sqrtf(len2); // 軸を正規化 //

        float t0 = Dot(axis, ta);
        float t1 = Dot(axis, tb);
        float t2 = Dot(axis, tc);
        float tri_max = (std::max)({t0, t1, t2});
        float tri_min = (std::min)({t0, t1, t2});

        float r = box_half_size.x * std::abs(axis.x)
            + box_half_size.y * std::abs(axis.y)
            + box_half_size.z * std::abs(axis.z);

        // box は [-r, r]、三角形は [tri_min, tri_max] //
        float overlap = (std::min)(tri_max, r) - (std::max)(tri_min, -r);
        if (overlap <= 0.0f)
        {
            return false;
        }

        if (overlap < best)
        {
            best = overlap;
            // box中心はローカルで原点(=0)、三角形の中心は mid //
            float mid = (tri_min + tri_max) * kHalfSize;
            // box を三角形の反対側へ押す向き //
            best_normal = (mid < 0.0f) ? axis : axis * -1.0f;
        }
    }

    out.depth = best;
    out.normal = best_normal; // 平行移動だけなのでローカル=ワールド //
    return true;
}


bool Contact(const Ray& ray, const Vec3& a, const Vec3& b, const Vec3& c, ContactInfo& out)
{
    Vec3 e1 = b - a;
    Vec3 e2 = c - a;
    Vec3 h = Cross(ray.direction, e2);
    float det = Dot(e1, h);
    // 0チェック //
    if (det > -kEpsilon && det < kEpsilon)
    {
        return false;
    }
    float f = 1.0f / det;
    Vec3 s = ray.origin - a; // 頂点aからレイの始点へのベクトル//
    float u = f * Dot(s, h); // uを計算//

    // 三角形の外側なら終了//
    if (u < 0.0f || u > 1.0f)
    {
        return false;
    }

    Vec3 q = Cross(s, e1);
    float v = f * Dot(ray.direction, q); // vを計算//

    // 三角形の外側なら終了//
    if (v < 0.0f || u + v > 1.0f)
    {
        return false;
    }

    float t = f * Dot(e2, q); // レイの距離tを計算//

    if (t > kEpsilon && t <= ray.distance)
    {
        out.depth = t;
        out.normal = Cross(e1, e2).Normalized();
        if (Dot(out.normal, ray.direction) > 0.0f)
        {
            out.normal = -out.normal;
        }
        return true;
    }
    return false;
}

bool Contact(const Capsule& capsule, const Vec3& a, const Vec3& b, const Vec3& c, ContactInfo& out)
{
    const Vec3 A = capsule.center - capsule.dir * capsule.height;
    const Vec3 B = capsule.center + capsule.dir * capsule.height;

    Triangle t;
    t.a = a;
    t.b = b;
    t.c = c;
    Vec3 q;
    if (PointToSurface(A, t, capsule.radius, q))
    {
        Vec3 d = A - q;
        float dist = d.Length();

        out.normal = (dist > kEpsilon) ? d / dist : Cross(b - a, c - a).Normalized();
        out.depth = capsule.radius - dist;
        return true;
    }
    if (PointToSurface(B, t, capsule.radius, q))
    {
        Vec3 d = B - q;
        float dist = d.Length();
        out.normal = (dist > kEpsilon) ? d / dist : Cross(b - a, c - a).Normalized();
        out.depth = capsule.radius - dist;
        return true;
    }
    if (SegmentIntersect(A, B, t, capsule.radius, out))
    {
        return true;
    }

    if (capsule.height < kEpsilon)
    {
        return false;
    }
    Ray ray;
    ray.origin = A;
    ray.direction = (B - A).Normalized();
    ray.distance = capsule.height * 2.0f;
    if (Contact(ray, a, b, c, out))
    {
        Vec3 n = Cross(b - a, c - a).Normalized();
        if (Dot(n, capsule.center - a) < 0.0f)
        {
            n = -n;
        }
        out.normal = n;
        out.depth = capsule.radius;
        return true;
    }
    return false;
}

bool SegmentIntersect(const Vec3& start, const Vec3& end, const Triangle& t, float r, ContactInfo& out)
{
    bool hit = false;
    float best_dist2 = r * r;
    Vec3 cap;
    Vec3 tri;

    auto TestEdge = [&](const Vec3& edge, const Vec3& tri_edge)
    {
        Vec3 cap_pos;
        Vec3 tri_pos;
        const float dist_pow2 = SegmentToSegment(start, end, edge, tri_edge, cap_pos, tri_pos);
        if (dist_pow2 <= best_dist2)
        {
            best_dist2 = dist_pow2;
            cap = cap_pos;
            tri = tri_pos;
            hit = true;
        }
    };
    TestEdge(t.a, t.b);
    TestEdge(t.b, t.c);
    TestEdge(t.c, t.a);
    if (!hit)
    {
        return false;
    }

    const Vec3 d = cap - tri;
    const float dist = std::sqrt(best_dist2);
    if (dist > kEpsilon)
    {
        out.normal = d / dist;
    }
    else
    {
        out.normal = Cross(t.b - t.a, t.c - t.a).Normalized();
    }
    out.depth = r - dist;
    return true;
}


float SegmentToSegment(const Vec3& start, const Vec3& end, const Vec3& tri_start, const Vec3& tri_end,
                       Vec3& closest1, Vec3& closest2)
{
    const Vec3 cap_dir = (end - start);
    const Vec3 tri_dir = (tri_end - tri_start);
    const Vec3 diff = start - tri_start;

    // クラメルの公式で解く //
    const float a = Dot(cap_dir, cap_dir);
    const float b = Dot(cap_dir, tri_dir);
    const float c = Dot(tri_dir, tri_dir);
    const float e = Dot(diff, cap_dir);
    const float f = Dot(diff, tri_dir);

    const float denom = a * c - b * b;

    float t;
    float s;
    if (denom < kEpsilon)
    {
        t = 0.0f;
    }
    else
    {
        t = (b * f - c * e) / denom;
        t = std::clamp(t, 0.0f, 1.0f);
    }

    // ｔを固定にしてｓを求める
    if (c > kEpsilon)
    {
        s = (t * b + f) / c;
        s = std::clamp(s, 0.0f, 1.0f);
    }
    else
    {
        s = 0.0f;
    }

    // ｓを固定にしてtを求める
    if (a > kEpsilon)
    {
        t = (s * b - e) / a;
        t = std::clamp(t, 0.0f, 1.0f);
    }
    else
    {
        t = 0.0f;
    }

    // 線分上の最近点 //
    closest1 = start + t * cap_dir;
    closest2 = tri_start + s * tri_dir;

    const Vec3 d = closest1 - closest2;
    return d.LengthSquared();
}

bool PointToSurface(const Vec3& end, const Triangle& t, float r, Vec3& q)
{
    // 法線の向き //
    const Vec3 n = Cross(t.b - t.a, t.c - t.a).Normalized();

    // 点を三角形に射影 //
    const float d = Dot(end - t.a, n);
    Vec3 p = end - d * n;

    if (IsInTriangle(p, t) && std::fabs(d) <= r)
    {
        q = p;
        return true;
    }
    return false;
}

bool IsInTriangle(const Vec3& p, const Triangle& t)
{
    // 射影した点の判定 
    // クラメルの公式で解く //
    Vec3 v0 = t.b - t.a;
    Vec3 v1 = t.c - t.a;
    Vec3 v2 = p - t.a;
    float d00 = Dot(v0, v0);
    float d01 = Dot(v0, v1);
    float d11 = Dot(v1, v1);
    float d20 = Dot(v2, v0);
    float d21 = Dot(v2, v1);

    // 各座標(u,v,w)を求める//
    float denom = d00 * d11 - d01 * d01;
    // ゼロ割り算回避 //
    if (std::fabs(denom) < kEpsilon)
    {
        return false;
    }
    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    // 0以下だったら三角形の外側 //
    return u >= -kEpsilon && v >= -kEpsilon && w >= -kEpsilon;
}
