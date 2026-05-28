#pragma once
#include "../Core/common.h"

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

struct SpriteVertex
{
    float position[2];
    float uv[2];
};

static const D3D12_INPUT_ELEMENT_DESC kSpriteVertexLayout[] = {
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
