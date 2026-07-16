#pragma once
#include "../Core/common.h"


/**
 * @brief 静的メッシュ用の頂点1つを保持する。
 */
struct StaticVertex
{
    float position[3];
    float normal[3];
    float uv[2];
    float tangent[3];
    float bitangent[3];
};

static const D3D12_INPUT_ELEMENT_DESC kStaticVertexLayout[] =
{
    {
        "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
        offsetof(StaticVertex, position),
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
    },
    {
        "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
        offsetof(StaticVertex, normal),
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
    },
    {
        "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
        offsetof(StaticVertex, uv),
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
    },
    {
        "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
        offsetof(StaticVertex, tangent),
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
    },
    {
        "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
        offsetof(StaticVertex,
                 bitangent), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
    },
};

/**
 * @brief スプライト用の頂点1つを保持する。
 */
struct SpriteVertex
{
    float position[2];
    float uv[2];
};

static constexpr D3D12_INPUT_ELEMENT_DESC kSpriteVertexLayout[] = {
    {
        "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
        offsetof(SpriteVertex, position),
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
    },
    {
        "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
        offsetof(SpriteVertex, uv),
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
    },
};


/**
 * @brief 色付きデバッグライン用の頂点1つを保持する。
 */
struct DebugLineVertex
{
    float position[3];
    float color[4];
};

static constexpr D3D12_INPUT_ELEMENT_DESC kDebugLineLayout[] = {
    {
        "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
    },
    {
        "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
    },
};


/**
 * @brief スキンメッシュ用の頂点1つを保持する。
 */
struct SkinnedVertex
{
    float position[3];
    float normal[3];
    float uv[2];
    float tangent[3];
    float bitangent[3];
    uint32_t bone_ids[4];
    float bone_weights[4];
};

static constexpr D3D12_INPUT_ELEMENT_DESC kSkinnedVertexLayout[] = {
    {
        "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
        offsetof(SkinnedVertex, position),
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
    },
    {
        "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
        offsetof(SkinnedVertex, normal),
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
    },
    {
        "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
        offsetof(SkinnedVertex, uv),
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
    },
    {
        "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
        offsetof(SkinnedVertex, tangent),
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
    },
    {
        "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
        offsetof(SkinnedVertex, bitangent),
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
    },
    {
        "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
        offsetof(SkinnedVertex, bone_ids),
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA
    },
    {
        "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
        offsetof(SkinnedVertex, bone_weights),
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA
    }
};


static constexpr D3D12_INPUT_ELEMENT_DESC kSkyLayout[] = {
    {
        "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
    },
};

/**
 * @brief 空メッシュ用の位置情報のみを持つ頂点1つを保持する。
 */
struct SkyVertex
{
    float position[3];
};
