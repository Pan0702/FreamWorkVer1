#include "intersect.h"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <vector>
// ============================================================ //
// 最近接点ヘルパ。//
// ============================================================ //

Vec3 ClosestPointOnBox(const Vec3& p, const Box& box)
{
    // 各軸を AABB の範囲に丸めて、箱の表面または内部の最近接点を作る。//
    return Vec3(
        std::clamp(p.x, box.min.x, box.max.x),
        std::clamp(p.y, box.min.y, box.max.y),
        std::clamp(p.z, box.min.z, box.max.z));
}

Vec3 ClosestPointOnTriangle(const Vec3& p, const Vec3& a, const Vec3& b, const Vec3& c)
{
    // 頂点 A から伸びる 2 辺と、頂点 A から点 p への向きを作る。//
    Vec3 ab = b - a, ac = c - a, ap = p - a;
    float d1 = Dot(ab, ap), d2 = Dot(ac, ap);
    // 点 p が頂点 A の外側領域にある場合は、最近接点を A にする。//
    if (d1 <= 0 && d2 <= 0) return a;

    // 点 p が頂点 B の外側領域にあるか調べる。//
    Vec3 bp = p - b;
    float d3 = Dot(ab, bp), d4 = Dot(ac, bp);
    // 頂点 B 側に一番近い場合は B を返す。//
    if (d3 >= 0 && d4 <= d3) return b;

    // 点 p が頂点 C の外側領域にあるか調べる。//
    Vec3 cp = p - c;
    float d5 = Dot(ab, cp), d6 = Dot(ac, cp);
    // 頂点 C 側に一番近い場合は C を返す。//
    if (d6 >= 0 && d5 <= d6) return c;

    // 辺 AB の外側領域にあるか、重心座標の符号で調べる。//
    float vc = d1 * d4 - d3 * d2;
    if (vc <= 0 && d1 >= 0 && d3 <= 0)
    {
        float v = d1 / (d1 - d3);
        return a + ab * v;
    }

    // 辺 AC の外側領域にあるか、重心座標の符号で調べる。//
    float vb = d5 * d2 - d1 * d6;
    if (vb <= 0 && d2 >= 0 && d6 <= 0)
    {
        float w = d2 / (d2 - d6);
        return a + ac * w;
    }

    // 辺 BC の外側領域にあるか、重心座標の符号で調べる。//
    float va = d3 * d6 - d5 * d4;
    if (va <= 0 && (d4 - d3) >= 0 && (d5 - d6) >= 0)
    {
        float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        return b + (c - b) * w;
    }

    // どの頂点・辺領域にも入らない場合は、三角形面内へ射影する。//
    float denom = 1.0f / (va + vb + vc);
    float v = vb * denom, w = vc * denom;
    return a + ab * v + ac * w;
}


// ============================================================ //
// 接触判定（貫通法線・深さを出力）。//
// out.normal は第1引数を第2引数から押し出す単位ベクトル、out.depth は正の貫通量。//
// ============================================================ //

bool Contact(const Sphere& s1, const Sphere& s2, ContactInfo& out)
{
    Vec3 d = s1.center - s2.center;
    float r = s1.radius + s2.radius;
    float l = d.Length();
    // 中心間距離が半径の合計より大きい場合は接触していない。//
    if (l > r)
    {
        return false;
    }
    // 中心が重なって法線を作れない場合は、上方向を仮の押し出し方向にする。//
    out.normal = (l > kEpsilon) ? d / l : Vec3(0, 1, 0);
    out.depth = r - l;
    return true;
}

bool Contact(const Sphere& s, const Box& box, ContactInfo& out)
{
    Vec3 closest = ClosestPointOnBox(s.center, box);
    Vec3 d = s.center - closest;
    float dist2 = Dot(d, d);

    // 球中心から AABB 最近接点までが半径より遠い場合は接触していない。//
    if (dist2 > s.radius * s.radius)
    {
        return false;
    }

    // 球中心が箱の外側にある場合は、最近接点から中心への向きを法線にする。//
    if (dist2 > 1e-12f)
    {
        float dist = sqrtf(dist2);
        out.normal = d / dist;
        out.depth = s.radius - dist;
    }
    else
    {
        // 球中心が AABB 内部にある場合は、最も近い面から外へ押し出す。//
        float dx_min = s.center.x - box.min.x; // -x 面まで //
        float dx_max = box.max.x - s.center.x; // +x 面まで //
        float dy_min = s.center.y - box.min.y;
        float dy_max = box.max.y - s.center.y;
        float dz_min = s.center.z - box.min.z;
        float dz_max = box.max.z - s.center.z;

        // 6 面のうち一番近い面を探して、押し出し方向に使う。//
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
    // X 軸で重なっていない場合は AABB 同士が分離している。//
    if (ox < 0)
    {
        return false;
    }

    float oy = (std::min)(b1.max.y, b2.max.y) - (std::max)(b1.min.y, b2.min.y);
    // Y 軸で重なっていない場合は AABB 同士が分離している。//
    if (oy < 0)
    {
        return false;
    }

    float oz = (std::min)(b1.max.z, b2.max.z) - (std::max)(b1.min.z, b2.min.z);
    // Z 軸で重なっていない場合は AABB 同士が分離している。//
    if (oz < 0)
    {
        return false;
    }
    Vec3 b1_center = (b1.min + b1.max) * 0.5f;
    Vec3 b2_center = (b2.min + b2.max) * 0.5f;
    float best = ox;
    out.normal = (b1_center.x > b2_center.x) ? Vec3(1, 0, 0) : Vec3(-1, 0, 0);

    // Y 軸の重なりが一番浅い場合は、Y 方向を押し出し軸にする。//
    if (oy < best)
    {
        best = oy;
        out.normal = (b1_center.y > b2_center.y) ? Vec3(0, 1, 0) : Vec3(0, -1, 0);
    }
    // Z 軸の重なりが一番浅い場合は、Z 方向を押し出し軸にする。//
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
    // 三角形上の最近接点から球中心へ向かうベクトルを作る。//
    Vec3 d = s.center - q;

    float dist2 = Dot(d, d);

    // 球中心と三角形の最近接点が半径より遠い場合は接触していない。//
    if (dist2 > s.radius * s.radius)
    {
        return false;
    }
    float dist = sqrtf(dist2);
    // 最近接点と中心が離れている場合は、その方向を押し出し法線にする。//
    if (dist > kEpsilon)
    {
        out.normal = d / dist;
    }
    else
    {
        // 中心がちょうど三角形面上なら、三角形の面法線を代用する。//
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

    // AABB の各軸と三角形の各辺から分離軸候補を作る。//
    for (int i = 0; i < 3; i++)
    {
        axes[index++] = Cross(axes[i], e0);
        axes[index++] = Cross(axes[i], e1);
        axes[index++] = Cross(axes[i], e2);
    }

    float best = FLT_MAX;
    Vec3 best_normal;
    // すべての分離軸候補で射影範囲の重なりを調べる。//
    for (Vec3 axis : axes)
    {
        float len2 = axis.LengthSquared();
        // 長さがほぼ 0 の軸は、正規化できないので判定から外す。//
        if (len2 < 1e-8f)
        {
            continue;
        }
        // 射影計算で使えるように軸を正規化する。//
        axis = axis / sqrtf(len2);

        float t0 = Dot(axis, ta);
        float t1 = Dot(axis, tb);
        float t2 = Dot(axis, tc);
        float tri_max = (std::max)({t0, t1, t2});
        float tri_min = (std::min)({t0, t1, t2});

        float r = box_half_size.x * std::abs(axis.x)
            + box_half_size.y * std::abs(axis.y)
            + box_half_size.z * std::abs(axis.z);

        // box は [-r, r]、三角形は [tri_min, tri_max] の範囲へ射影される。//
        float overlap = (std::min)(tri_max, r) - (std::max)(tri_min, -r);
        // 射影範囲が重ならない軸があれば、AABB と三角形は分離している。//
        if (overlap <= 0.0f)
        {
            return false;
        }

        // 一番重なりが浅い軸を、最小移動量の押し出し軸として保持する。//
        if (overlap < best)
        {
            best = overlap;
            // box 中心はローカルで原点、三角形の射影中心は mid になる。//
            float mid = (tri_min + tri_max) * kHalfSize;
            // box を三角形の反対側へ押す向きにそろえる。//
            best_normal = (mid < 0.0f) ? axis : axis * -1.0f;
        }
    }

    out.depth = best;
    // 平行移動だけなので、ローカルで求めた法線をそのままワールド法線として使う。//
    out.normal = best_normal;
    return true;
}


bool Contact(const Ray& ray, const Vec3& a, const Vec3& b, const Vec3& c, ContactInfo& out)
{
    Vec3 e1 = b - a;
    Vec3 e2 = c - a;
    Vec3 h = Cross(ray.direction, e2);
    float det = Dot(e1, h);
    // 行列式が 0 に近い場合は、レイと三角形が平行なので当たらない。//
    if (det > -kEpsilon && det < kEpsilon)
    {
        return false;
    }
    float f = 1.0f / det;
    // 頂点 A からレイ始点へのベクトルを作り、重心座標 u を求める。//
    Vec3 s = ray.origin - a;
    float u = f * Dot(s, h);

    // u が範囲外なら、交点は三角形の外側にある。//
    if (u < 0.0f || u > 1.0f)
    {
        return false;
    }

    Vec3 q = Cross(s, e1);
    // 残りの重心座標 v を求める。//
    float v = f * Dot(ray.direction, q);

    // v または u + v が範囲外なら、交点は三角形の外側にある。//
    if (v < 0.0f || u + v > 1.0f)
    {
        return false;
    }

    // レイ始点から交点までの距離 t を求める。//
    float t = f * Dot(e2, q);

    // 交点がレイの始点より前ではなく、最大距離以内ならヒットとして扱う。//
    if (t > kEpsilon && t <= ray.distance)
    {
        out.depth = t;
        out.normal = Cross(e1, e2).Normalized();
        // 法線がレイの進行方向と同じ向きなら、表側へ向くよう反転する。//
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
    // カプセルの下端が三角形面へ届くか調べる。//
    if (PointToSurface(A, t, capsule.radius, q))
    {
        Vec3 d = A - q;
        float dist = d.Length();

        out.normal = (dist > kEpsilon) ? d / dist : Cross(b - a, c - a).Normalized();
        out.depth = capsule.radius - dist;
        return true;
    }
    // カプセルの上端が三角形面へ届くか調べる。//
    if (PointToSurface(B, t, capsule.radius, q))
    {
        Vec3 d = B - q;
        float dist = d.Length();
        out.normal = (dist > kEpsilon) ? d / dist : Cross(b - a, c - a).Normalized();
        out.depth = capsule.radius - dist;
        return true;
    }
    // 端点が面に届かない場合は、カプセル軸と三角形の辺の距離を調べる。//
    if (SegmentIntersect(A, B, t, capsule.radius, out))
    {
        return true;
    }

    // 高さがほぼ 0 の場合は、ここまでで球判定相当が終わっている。//
    if (capsule.height < kEpsilon)
    {
        return false;
    }
    Ray ray;
    ray.origin = A;
    ray.direction = (B - A).Normalized();
    ray.distance = capsule.height * 2.0f;
    // カプセル軸が三角形面を貫く場合は、面法線方向に押し戻す。//
    if (Contact(ray, a, b, c, out))
    {
        Vec3 n = Cross(b - a, c - a).Normalized();
        // 法線がカプセル中心と反対側を向いている場合は反転する。//
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
        // 現在の最短距離より近い辺なら、接触候補として保持する。//
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
    // どの辺も半径内に入らなければ、線分と三角形辺は接触していない。//
    if (!hit)
    {
        return false;
    }

    const Vec3 d = cap - tri;
    const float dist = std::sqrt(best_dist2);
    // 最近接点同士が離れている場合は、その向きを押し出し法線にする。//
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

    // 2 本の線分をパラメータ t, s で表し、クラメルの公式で最近点を求める。//
    const float a = Dot(cap_dir, cap_dir);
    const float b = Dot(cap_dir, tri_dir);
    const float c = Dot(tri_dir, tri_dir);
    const float e = Dot(diff, cap_dir);
    const float f = Dot(diff, tri_dir);

    const float denom = a * c - b * b;

    float t;
    float s;
    // 分母が小さい場合は線分同士がほぼ平行なので、片側の始点を使う。//
    if (denom < kEpsilon)
    {
        t = 0.0f;
    }
    else
    {
        t = (b * f - c * e) / denom;
        t = std::clamp(t, 0.0f, 1.0f);
    }

    // t を固定して、三角形辺側の最近点パラメータ s を求める。//
    if (c > kEpsilon)
    {
        s = (t * b + f) / c;
        s = std::clamp(s, 0.0f, 1.0f);
    }
    else
    {
        s = 0.0f;
    }

    // s を固定して、カプセル軸側の最近点パラメータ t を求め直す。//
    if (a > kEpsilon)
    {
        t = (s * b - e) / a;
        t = std::clamp(t, 0.0f, 1.0f);
    }
    else
    {
        t = 0.0f;
    }

    // それぞれの線分上の最近接点を作る。//
    closest1 = start + t * cap_dir;
    closest2 = tri_start + s * tri_dir;

    const Vec3 d = closest1 - closest2;
    return d.LengthSquared();
}

bool PointToSurface(const Vec3& end, const Triangle& t, float r, Vec3& q)
{
    // 三角形の面法線を作る。//
    const Vec3 n = Cross(t.b - t.a, t.c - t.a).Normalized();

    // 点を三角形平面へ射影する。//
    const float d = Dot(end - t.a, n);
    Vec3 p = end - d * n;

    // 射影点が三角形内で、点から面までの距離が半径内なら面に届いている。//
    if (IsInTriangle(p, t) && std::fabs(d) <= r)
    {
        q = p;
        return true;
    }
    return false;
}

bool IsInTriangle(const Vec3& p, const Triangle& t)
{
    // 射影した点を重心座標で判定する。//
    // クラメルの公式で u, v, w を求める。//
    Vec3 v0 = t.b - t.a;
    Vec3 v1 = t.c - t.a;
    Vec3 v2 = p - t.a;
    float d00 = Dot(v0, v0);
    float d01 = Dot(v0, v1);
    float d11 = Dot(v1, v1);
    float d20 = Dot(v2, v0);
    float d21 = Dot(v2, v1);
    
    float denom = d00 * d11 - d01 * d01;
    // 三角形がつぶれている場合は分母が 0 に近くなるので判定できない。//
    if (std::fabs(denom) < kEpsilon)
    {
        return false;
    }
    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    // すべての重心座標が 0 以上なら、点は三角形の内側または辺上にある。//
    return u >= -kEpsilon && v >= -kEpsilon && w >= -kEpsilon;
}