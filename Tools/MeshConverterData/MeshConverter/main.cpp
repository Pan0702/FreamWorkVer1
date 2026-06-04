#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <system_error>
#include <vector>
#include <conio.h>

#include "../../../Src/Resource/mesh_file.h"
#include "../../../Src/Resource/vertex_types.h"

namespace
{
    bool WriteMesh(const char* out_path,
                   const std::vector<StaticVertex>& vertices,
                   const std::vector<std::uint32_t>& indices);
    bool ConvertMesh(const char* in_path, const char* out_path);

    bool CheckCountFitsUint32(std::size_t count, const char* label)
    {
        if (count > static_cast<std::size_t>(std::numeric_limits<std::uint32_t>::max()))
        {
            std::printf("%s count exceeds uint32_t range\n", label);
            return false;
        }

        return true;
    }

    bool AppendMesh(const aiMesh& mesh,
                    std::vector<StaticVertex>& vertices,
                    std::vector<std::uint32_t>& indices)
    {
        if (!CheckCountFitsUint32(vertices.size(), "base vertex"))
        {
            return false;
        }

        const std::uint32_t base = static_cast<std::uint32_t>(vertices.size());
        const bool has_uv = mesh.HasTextureCoords(0);
        const bool has_tangent = mesh.HasTangentsAndBitangents();

        vertices.reserve(vertices.size() + mesh.mNumVertices);

        for (unsigned int v = 0; v < mesh.mNumVertices; ++v)
        {
            StaticVertex out = {};

            out.position[0] = mesh.mVertices[v].x;
            out.position[1] = mesh.mVertices[v].y;
            out.position[2] = mesh.mVertices[v].z;

            if (mesh.HasNormals())
            {
                out.normal[0] = mesh.mNormals[v].x;
                out.normal[1] = mesh.mNormals[v].y;
                out.normal[2] = mesh.mNormals[v].z;
            }

            if (has_uv)
            {
                out.uv[0] = mesh.mTextureCoords[0][v].x;
                out.uv[1] = mesh.mTextureCoords[0][v].y;
            }

            if (has_tangent)
            {
                out.tangent[0] = mesh.mTangents[v].x;
                out.tangent[1] = mesh.mTangents[v].y;
                out.tangent[2] = mesh.mTangents[v].z;
                out.bitangent[0] = mesh.mBitangents[v].x;
                out.bitangent[1] = mesh.mBitangents[v].y;
                out.bitangent[2] = mesh.mBitangents[v].z;
            }

            vertices.push_back(out);
        }

        if (!CheckCountFitsUint32(vertices.size(), "vertex"))
        {
            return false;
        }

        indices.reserve(indices.size() + static_cast<std::size_t>(mesh.mNumFaces) * 3);

        for (unsigned int f = 0; f < mesh.mNumFaces; ++f)
        {
            const aiFace& face = mesh.mFaces[f];
            if (face.mNumIndices != 3)
            {
                continue;
            }

            indices.push_back(base + face.mIndices[0]);
            indices.push_back(base + face.mIndices[1]);
            indices.push_back(base + face.mIndices[2]);
        }

        return true;
    }

    bool ConvertMesh(const char* in_path, const char* out_path)
    {
        Assimp::Importer importer;
        const unsigned int flags =
            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_CalcTangentSpace |
            aiProcess_ConvertToLeftHanded |
            aiProcess_JoinIdenticalVertices |
            aiProcess_PreTransformVertices;


        const aiScene* scene = importer.ReadFile(in_path, flags);
        if (scene == nullptr || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0 || scene->mRootNode == nullptr)
        {
            std::printf("assimp: %s\n", importer.GetErrorString());
            return false;
        }

        std::vector<StaticVertex> vertices;
        std::vector<std::uint32_t> indices;

        for (unsigned int m = 0; m < scene->mNumMeshes; ++m)
        {
            const aiMesh* mesh = scene->mMeshes[m];
            if (mesh == nullptr)
            {
                continue;
            }

            if (!AppendMesh(*mesh, vertices, indices))
            {
                return false;
            }
        }

        if (vertices.empty() || indices.empty())
        {
            std::printf("no geometry\n");
            return false;
        }

        return WriteMesh(out_path, vertices, indices);
    }

    bool WriteMesh(const char* out_path,
                   const std::vector<StaticVertex>& vertices,
                   const std::vector<std::uint32_t>& indices)
    {
        if (!CheckCountFitsUint32(vertices.size(), "vertex") ||
            !CheckCountFitsUint32(indices.size(), "index"))
        {
            return false;
        }

        MeshFileHeader header = {};
        std::memcpy(header.magic, kMeshMagic, 4);
        header.version = kMeshVersion;
        header.vertex_count = static_cast<std::uint32_t>(vertices.size());
        header.index_count = static_cast<std::uint32_t>(indices.size());
        header.vertex_stride = sizeof(StaticVertex);
        header.index_stride = sizeof(std::uint32_t);

        const std::filesystem::path output_path(out_path);
        const std::filesystem::path parent = output_path.parent_path();
        if (!parent.empty())
        {
            std::error_code ec;
            std::filesystem::create_directories(parent, ec);
            if (ec)
            {
                std::printf("cannot create output directory: %s\n", parent.string().c_str());
                return false;
            }
        }

        std::ofstream ofs(out_path, std::ios::binary);
        if (!ofs)
        {
            std::printf("cannot open output: %s\n", out_path);
            return false;
        }

        ofs.write(reinterpret_cast<const char*>(&header), sizeof(header));
        ofs.write(reinterpret_cast<const char*>(vertices.data()),
                  static_cast<std::streamsize>(vertices.size() * sizeof(StaticVertex)));
        ofs.write(reinterpret_cast<const char*>(indices.data()),
                  static_cast<std::streamsize>(indices.size() * sizeof(std::uint32_t)));

        if (!ofs)
        {
            std::printf("write failed: %s\n", out_path);
            return false;
        }

        return true;
    }
    // 前後のダブルクオートを除去（パスをドラッグ＆ペーストすると " が付くことがある）
    std::string StripQuotes(std::string s)
    {
        if (s.size() >= 2 && s.front() == '"' && s.back() == '"')
        {
            s = s.substr(1, s.size() - 2);
        }
        return s;
    }

    void WaitEnter()
    {
        std::printf("\nPushEnter Finish...");
        std::string dummy;
        std::getline(std::cin, dummy);
    }
}

int main(int argc, char** argv)
{
    std::string in_path;
    std::string out_path;

    if (argc >= 3)
    {
        // --- CLIモード（引数あり）: バッチ実行向け。終了時に待たない ---
        in_path  = argv[1];
        out_path = argv[2];

        const bool ok = ConvertMesh(in_path.c_str(), out_path.c_str());
        std::printf(ok ? "[OK] %s -> %s\n" : "[FAILED] %s\n", in_path.c_str(), out_path.c_str());
        return ok ? 0 : 2;
    }

    std::printf("=== MeshConverter ===\n");

    while (true)
    {
        std::string in_path;
        std::string out_path;

        std::printf("\ninput  (.fbx/.gltf/.glb): ");
        std::getline(std::cin, in_path);
        in_path = StripQuotes(in_path);

        std::printf("output (.mesh): ");
        std::getline(std::cin, out_path);
        out_path = StripQuotes(out_path);

        if (in_path.empty() || out_path.empty())
        {
            std::printf("パスが空です\n");
        }
        else
        {
            const bool ok = ConvertMesh(in_path.c_str(), out_path.c_str());
            std::printf(ok ? "[OK] %s -> %s\n" : "[FAILED] %s\n",
                        in_path.c_str(), out_path.c_str());
        }

        std::printf("\n if continue convert anything key push / Esc key finish...\n");
        const int key = _getch();
        if (key == 27)   // 27 = ESC
        {
            break;
        }
    }
    return 0;
}
