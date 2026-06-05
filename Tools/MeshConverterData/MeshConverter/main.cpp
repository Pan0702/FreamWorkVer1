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
#include "../../../Src/Resource/skmesh_file.h"
#include "../../../Src/Resource/vertex_types.h"

#include <algorithm>
#include <map>
#include <string>
#include <unordered_map>

namespace
{
    struct SubMeshData 
    {
        uint32_t index_start;
        uint32_t index_count;
        uint32_t material_slot;
    };
    
    struct MaterialData
    {
        Vec4 base_color;
        std::wstring diffuse_path;
    };

    bool WriteMesh(const char* out_path,
                   const std::vector<StaticVertex>& vertices,
                   const std::vector<std::uint32_t>& indices,
                   const std::vector<MaterialData>& materials,
                   const std::vector<SubMeshData>& sub_meshes);
    bool ConvertMesh(const char* in_path, const char* out_path, const std::string& tex_folder);

    bool CheckCountFitsUint32(std::size_t count, const char* label)
    {
        if (count > static_cast<std::size_t>((std::numeric_limits<std::uint32_t>::max)()))
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

    // assimp が返すパス（フォルダ込みの場合あり）からファイル名だけを取り出し、
    // Assets/Texture/<tex_folder>/<filename> という相対パスを組み立てる。
    std::wstring BuildTexturePath(const aiString& tex, const std::string& tex_folder)
    {
        std::filesystem::path src(tex.C_Str());
        const std::filesystem::path filename = src.filename();
        if (filename.empty())
        {
            return std::wstring();
        }

        std::filesystem::path full = "Assets/Texture";
        if (!tex_folder.empty())
        {
            full /= tex_folder;
        }
        full /= filename;
        return full.generic_wstring();
    }

    bool ConvertMesh(const char* in_path, const char* out_path, const std::string& tex_folder)
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
        std::vector<SubMeshData> sub_meshes;
        for (unsigned int m = 0; m < scene->mNumMeshes; ++m)
        {
            const aiMesh* mesh = scene->mMeshes[m];
            if (mesh == nullptr)
            {
                continue;
            }
            
            const uint32_t start = static_cast<uint32_t>(vertices.size());
            
            if (!AppendMesh(*mesh,vertices,indices)) return false;
            
            SubMeshData data = {};
            data.index_start = start;
            data.index_count = static_cast<uint32_t>(indices.size()) - start;
            data.material_slot = mesh->mMaterialIndex;
            sub_meshes.push_back(data);
        }
        
        std::vector<MaterialData> materials;
        
        for (unsigned int m = 0; m < scene->mNumMaterials; ++m)
        {
            const aiMaterial* material = scene->mMaterials[m];
            MaterialData data = {};
            data.base_color = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
            
            aiColor4D color;
            if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
            {
                data.base_color = Vec4(color.r, color.g, color.b, color.a);
            }
            aiString tex;
            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &tex) == AI_SUCCESS)
            {
                data.diffuse_path = BuildTexturePath(tex, tex_folder);
            }
            materials.push_back(data);
        }
        

        if (vertices.empty() || indices.empty())
        {
            std::printf("no geometry\n");
            return false;
        }

        return WriteMesh(out_path, vertices, indices, materials, sub_meshes);
    }

    bool WriteMesh(const char* out_path,
                   const std::vector<StaticVertex>& vertices,
                   const std::vector<std::uint32_t>& indices,
                   const std::vector<MaterialData>& materials,
                   const std::vector<SubMeshData>& sub_meshes)
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
        header.material_count = materials.size();
        header.submesh_count = sub_meshes.size();
        
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
        for (const auto& sub_mesh : sub_meshes)
        {
            SubMeshEntry e = { sub_mesh.index_start, sub_mesh.index_count,
                sub_mesh.material_slot };
            ofs.write(reinterpret_cast<const char*>(&e), sizeof(e));
        }
        for (const auto& mat : materials)
        {
            MaterialEntry e = {};
            e.base_color = mat.base_color;
            e.diffuse_texture_length =
        static_cast<std::uint32_t>(mat.diffuse_path.size());
            ofs.write(reinterpret_cast<const char*>(&e), sizeof(e));
        }
        for (const auto& mat : materials)
        {
            if (!mat.diffuse_path.empty())
            ofs.write(reinterpret_cast<const char*>(mat.diffuse_path.c_str()),
                      static_cast<std::streamsize>(mat.diffuse_path.size() * sizeof(std::wstring::value_type)));
            printf("write %ls\n", mat.diffuse_path.c_str());
        }
        
        if (!ofs)
        {
            std::printf("write failed: %s\n", out_path);
            return false;
        }

        return true;
    }

    // ===== ここからスケルタルメッシュ (.skmesh) =====

    // assimp の行列は row-major 格納だが column-vector 規約の値。
    // プロジェクトは row-vector 規約なので「転置」して取り込む。
    Mat ToRowVectorMat(const aiMatrix4x4& m)
    {
        // aiMatrix4x4 は m.a1..a4(1行目) ... という row-major。
        // 転置 = 行と列を入れ替えて格納する。
        return Mat(
            m.a1, m.b1, m.c1, m.d1,
            m.a2, m.b2, m.c2, m.d2,
            m.a3, m.b3, m.c3, m.d3,
            m.a4, m.b4, m.c4, m.d4);
    }

    struct SkinInfluence
    {
        uint32_t bone;
        float    weight;
    };

    // 1頂点に効くボーンを最大4本に絞り、合計1.0へ正規化して書き込む。
    void FinalizeSkinWeights(std::vector<SkinInfluence>& inf, SkinnedVertex& v)
    {
        // weight 降順で上位4本だけ残す
        std::sort(inf.begin(), inf.end(),
                  [](const SkinInfluence& a, const SkinInfluence& b)
                  { return a.weight > b.weight; });
        if (inf.size() > 4)
        {
            inf.resize(4);
        }

        float sum = 0.0f;
        for (const SkinInfluence& s : inf) sum += s.weight;

        for (int i = 0; i < 4; ++i)
        {
            if (i < static_cast<int>(inf.size()) && sum > 0.0f)
            {
                v.bone_ids[i]     = inf[i].bone;
                v.bone_weights[i] = inf[i].weight / sum;   // 正規化
            }
            else
            {
                v.bone_ids[i]     = 0;
                v.bone_weights[i] = 0.0f;
            }
        }
    }

    // aiNode ツリーを深さ優先で平坦化し、親が必ず子より前に来る topo 順に並べる。
    // name -> node_index マップも作る（後でボーン名・アニメ名の解決に使う）。
    struct FlatNode
    {
        std::string name;
        int32_t     parent_index;
        Mat         local_bind_transform;   // = aiNode::mTransformation（転置済み）
        int32_t     skin_index = -1;        // 後でボーンに対応するノードへ採番
        Mat         inverse_bind_pose;      // skin_index>=0 のみ有効
    };

    void FlattenNodes(const aiNode* node, int32_t parent_index,
                      std::vector<FlatNode>& out,
                      std::unordered_map<std::string, int32_t>& name_to_index)
    {
        FlatNode fn = {};
        fn.name = node->mName.C_Str();
        fn.parent_index = parent_index;
        fn.local_bind_transform = ToRowVectorMat(node->mTransformation);
        fn.inverse_bind_pose = Mat(); // 既定。ボーン採番時に上書き

        const int32_t my_index = static_cast<int32_t>(out.size());
        out.push_back(fn);
        name_to_index[out[my_index].name] = my_index;

        for (unsigned int i = 0; i < node->mNumChildren; ++i)
        {
            FlattenNodes(node->mChildren[i], my_index, out, name_to_index);
        }
    }

    // スキンド版の1メッシュ取り込み。頂点・インデックス・スキンウェイトを集約する。
    bool AppendSkinnedMesh(const aiMesh& mesh,
                           std::vector<SkinnedVertex>& vertices,
                           std::vector<std::uint32_t>& indices,
                           std::vector<std::vector<SkinInfluence>>& influences,
                           const std::unordered_map<std::string, int32_t>& name_to_index,
                           std::vector<FlatNode>& nodes,
                           uint32_t& skin_counter)
    {
        if (!CheckCountFitsUint32(vertices.size(), "base vertex"))
        {
            return false;
        }

        const std::uint32_t base = static_cast<std::uint32_t>(vertices.size());
        const bool has_uv = mesh.HasTextureCoords(0);
        const bool has_tangent = mesh.HasTangentsAndBitangents();

        vertices.reserve(vertices.size() + mesh.mNumVertices);
        influences.resize(vertices.size() + mesh.mNumVertices);

        for (unsigned int v = 0; v < mesh.mNumVertices; ++v)
        {
            SkinnedVertex out = {};
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
            // ウェイトはこの後 mBones から埋める
            vertices.push_back(out);
        }

        // インデックス（base オフセット込み）
        indices.reserve(indices.size() + static_cast<std::size_t>(mesh.mNumFaces) * 3);
        for (unsigned int f = 0; f < mesh.mNumFaces; ++f)
        {
            const aiFace& face = mesh.mFaces[f];
            if (face.mNumIndices != 3) continue;
            indices.push_back(base + face.mIndices[0]);
            indices.push_back(base + face.mIndices[1]);
            indices.push_back(base + face.mIndices[2]);
        }

        // スキンウェイト抽出: mBones[b] の各 mWeights を頂点側へ集約。
        for (unsigned int b = 0; b < mesh.mNumBones; ++b)
        {
            const aiBone* bone = mesh.mBones[b];
            if (bone == nullptr) continue;

            const std::string bone_name = bone->mName.C_Str();
            auto it = name_to_index.find(bone_name);
            if (it == name_to_index.end())
            {
                std::printf("warn: bone '%s' not found in node tree\n", bone_name.c_str());
                continue;
            }
            FlatNode& node = nodes[it->second];

            // 初めてボーンとして使われたノードに skin_index を採番し、
            // inverse_bind_pose（= mOffsetMatrix 転置）を設定。
            if (node.skin_index < 0)
            {
                node.skin_index = static_cast<int32_t>(skin_counter++);
                node.inverse_bind_pose = ToRowVectorMat(bone->mOffsetMatrix);
            }

            for (unsigned int w = 0; w < bone->mNumWeights; ++w)
            {
                const aiVertexWeight& vw = bone->mWeights[w];
                const std::size_t vi = static_cast<std::size_t>(base) + vw.mVertexId;
                influences[vi].push_back({ static_cast<uint32_t>(node.skin_index), vw.mWeight });
            }
        }

        return true;
    }

    bool WriteSkMesh(const char* out_path,
                     const std::vector<SkinnedVertex>& vertices,
                     const std::vector<std::uint32_t>& indices,
                     const std::vector<MaterialData>& materials,
                     const std::vector<SubMeshData>& sub_meshes,
                     const std::vector<FlatNode>& nodes,
                     uint32_t skin_count)
    {
        if (!CheckCountFitsUint32(vertices.size(), "vertex") ||
            !CheckCountFitsUint32(indices.size(), "index"))
        {
            return false;
        }

        SkeletonMeshFileHeader header = {};
        std::memcpy(header.magic, kSkMeshMagic, 4);
        header.version = kSkMeshVersion;
        header.vertex_count = static_cast<std::uint32_t>(vertices.size());
        header.index_count = static_cast<std::uint32_t>(indices.size());
        header.vertex_stride = sizeof(SkinnedVertex);
        header.index_stride = sizeof(std::uint32_t);
        header.submesh_count = static_cast<std::uint32_t>(sub_meshes.size());
        header.material_count = static_cast<std::uint32_t>(materials.size());
        header.node_count = static_cast<std::uint32_t>(nodes.size());
        header.skin_count = skin_count;

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
                  static_cast<std::streamsize>(vertices.size() * sizeof(SkinnedVertex)));
        ofs.write(reinterpret_cast<const char*>(indices.data()),
                  static_cast<std::streamsize>(indices.size() * sizeof(std::uint32_t)));

        for (const auto& sub_mesh : sub_meshes)
        {
            SubMeshEntry e = { sub_mesh.index_start, sub_mesh.index_count, sub_mesh.material_slot };
            ofs.write(reinterpret_cast<const char*>(&e), sizeof(e));
        }
        for (const auto& mat : materials)
        {
            MaterialEntry e = {};
            e.base_color = mat.base_color;
            e.diffuse_texture_length = static_cast<std::uint32_t>(mat.diffuse_path.size());
            ofs.write(reinterpret_cast<const char*>(&e), sizeof(e));
        }
        for (const auto& mat : materials)
        {
            if (!mat.diffuse_path.empty())
            {
                ofs.write(reinterpret_cast<const char*>(mat.diffuse_path.c_str()),
                          static_cast<std::streamsize>(mat.diffuse_path.size() * sizeof(std::wstring::value_type)));
            }
        }

        // ノード階層（topo 順）
        for (const FlatNode& node : nodes)
        {
            NodeEntry e = {};
            e.parent_index = node.parent_index;
            e.skin_index = node.skin_index;
            e.name_length = static_cast<std::uint32_t>(node.name.size());
            e.reserved = 0;
            e.local_bind_transform = node.local_bind_transform;
            e.inverse_bind_pose = node.inverse_bind_pose;
            ofs.write(reinterpret_cast<const char*>(&e), sizeof(e));
        }
        // ノード名文字列群（NodeEntry 順、char 列）
        for (const FlatNode& node : nodes)
        {
            if (!node.name.empty())
            {
                ofs.write(node.name.data(),
                          static_cast<std::streamsize>(node.name.size()));
            }
        }

        if (!ofs)
        {
            std::printf("write failed: %s\n", out_path);
            return false;
        }
        std::printf("nodes=%zu skin=%u verts=%zu indices=%zu\n",
                    nodes.size(), skin_count, vertices.size(), indices.size());
        return true;
    }

    bool ConvertSkMesh(const char* in_path, const char* out_path, const std::string& tex_folder)
    {
        Assimp::Importer importer;
        // スキンド用フラグ規約:
        //  - PreTransformVertices は使わない（階層を破壊する）
        //  - ConvertToLeftHanded は使わない（offset/transform/anim/頂点への一貫適用を保証できないうちは外す）
        const unsigned int flags =
            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_CalcTangentSpace |
            aiProcess_JoinIdenticalVertices |
            aiProcess_LimitBoneWeights |
            aiProcess_PopulateArmatureData;

        const aiScene* scene = importer.ReadFile(in_path, flags);
        if (scene == nullptr || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0 || scene->mRootNode == nullptr)
        {
            std::printf("assimp: %s\n", importer.GetErrorString());
            return false;
        }

        // 1. ノード階層を平坦化（topo 順 + name マップ）
        std::vector<FlatNode> nodes;
        std::unordered_map<std::string, int32_t> name_to_index;
        FlattenNodes(scene->mRootNode, -1, nodes, name_to_index);

        // 2. メッシュ取り込み（頂点・インデックス・スキンウェイト）
        std::vector<SkinnedVertex> vertices;
        std::vector<std::uint32_t> indices;
        std::vector<std::vector<SkinInfluence>> influences;
        std::vector<SubMeshData> sub_meshes;
        uint32_t skin_counter = 0;

        for (unsigned int m = 0; m < scene->mNumMeshes; ++m)
        {
            const aiMesh* mesh = scene->mMeshes[m];
            if (mesh == nullptr) continue;

            const uint32_t start = static_cast<uint32_t>(indices.size());
            if (!AppendSkinnedMesh(*mesh, vertices, indices, influences,
                                   name_to_index, nodes, skin_counter))
            {
                return false;
            }

            SubMeshData data = {};
            data.index_start = start;
            data.index_count = static_cast<uint32_t>(indices.size()) - start;
            data.material_slot = mesh->mMaterialIndex;
            sub_meshes.push_back(data);
        }

        // 3. スキンウェイトを頂点へ確定（上位4本・正規化）
        for (std::size_t i = 0; i < vertices.size(); ++i)
        {
            FinalizeSkinWeights(influences[i], vertices[i]);
        }

        // 4. マテリアル（静的版と同じ抽出）
        std::vector<MaterialData> materials;
        for (unsigned int m = 0; m < scene->mNumMaterials; ++m)
        {
            const aiMaterial* material = scene->mMaterials[m];
            MaterialData data = {};
            data.base_color = Vec4(1.0f, 1.0f, 1.0f, 1.0f);

            aiColor4D color;
            if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
            {
                data.base_color = Vec4(color.r, color.g, color.b, color.a);
            }
            aiString tex;
            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &tex) == AI_SUCCESS)
            {
                data.diffuse_path = BuildTexturePath(tex, tex_folder);
            }
            materials.push_back(data);
        }

        if (vertices.empty() || indices.empty())
        {
            std::printf("no geometry\n");
            return false;
        }

        return WriteSkMesh(out_path, vertices, indices, materials, sub_meshes, nodes, skin_counter);
    }

    // ===== ここまでスケルタルメッシュ =====

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
        in_path = argv[1];
        out_path = argv[2];
        // 任意: Assets/Texture/ の下のサブフォルダ
        const std::string tex_folder = (argc >= 4) ? argv[3] : "";

        // 出力拡張子が .skmesh ならスケルタル変換に分岐
        const bool skinned =
            std::filesystem::path(out_path).extension() == ".skmesh";
        const bool ok = skinned
            ? ConvertSkMesh(in_path.c_str(), out_path.c_str(), tex_folder)
            : ConvertMesh(in_path.c_str(), out_path.c_str(), tex_folder);
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

        std::printf("output (.mesh / .skmesh): ");
        std::getline(std::cin, out_path);
        out_path = StripQuotes(out_path);

        // Assets/Texture/ の下のサブフォルダ（空Enterでサブフォルダなし= Assets/Texture/直下）
        std::printf("texture folder (under Assets/Texture/, empty = none): ");
        std::string tex_folder;
        std::getline(std::cin, tex_folder);
        tex_folder = StripQuotes(tex_folder);

        if (in_path.empty() || out_path.empty())
        {
            std::printf("input/output is empty\n");
        }
        else
        {
            // 出力拡張子が .skmesh ならスケルタル変換に分岐
            const bool skinned =
                std::filesystem::path(out_path).extension() == ".skmesh";
            const bool ok = skinned
                ? ConvertSkMesh(in_path.c_str(), out_path.c_str(), tex_folder)
                : ConvertMesh(in_path.c_str(), out_path.c_str(), tex_folder);
            std::printf(ok ? "[OK] %s -> %s\n" : "[FAILED] %s\n",
                        in_path.c_str(), out_path.c_str());
        }

        std::printf("\n if continue convert anything key push / Esc key finish...\n");
        const int key = _getch();
        if (key == 27) // 27 = ESC
        {
            break;
        }
    }
    return 0;
}
