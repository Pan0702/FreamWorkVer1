#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

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
    // 埋め込みテクスチャが書き出されるファイル名（例 "diffuse.png"）を返す。 //
    // 書き出し側・参照側の両方からこれを使い、名前を必ず一致させる。 //
    std::string EmbeddedTextureFileName(const aiTexture* tex, unsigned int index)
    {
        std::string stem = std::filesystem::path(tex->mFilename.C_Str()).stem().string();
        if (stem.empty())
        {
            stem = "tex" + std::to_string(index);
        }
        // achFormatHint は "png" / ".png" / "jpg" 等。先頭ドットを正規化する。 //
        std::string ext = (tex->achFormatHint[0] != '\0') ? tex->achFormatHint : "bin";
        if (!ext.empty() && ext.front() == '.')
        {
            ext.erase(0, 1);
        }
        return stem + "." + ext;
    }

    // 埋め込みテクスチャ(scene->mTextures)を out_dir に書き出す。 //
    // FBX(Mixamoなど)ではPNG/JPGが圧縮済みバイト列として埋め込まれる。 //
    // mHeight == 0 の場合は pcData をそのままファイルへ保存する。 //
    void ExtractEmbeddedTextures(const aiScene* scene, const std::string& out_dir)
    {
        if (scene == nullptr || !scene->HasTextures())
        {
            std::printf("no embedded textures\n");
            return;
        }

        std::error_code ec;
        std::filesystem::create_directories(out_dir, ec);

        for (unsigned int i = 0; i < scene->mNumTextures; ++i)
        {
            const aiTexture* tex = scene->mTextures[i];
            if (tex == nullptr)
            {
                continue;
            }

            if (tex->mHeight == 0)
            {
                // 圧縮済みデータ。achFormatHint には "png" / "jpg" などが入る。 //
                const std::filesystem::path out =
                    std::filesystem::path(out_dir) / EmbeddedTextureFileName(tex, i);

                std::ofstream ofs(out, std::ios::binary);
                if (ofs)
                {
                    ofs.write(reinterpret_cast<const char*>(tex->pcData),
                              static_cast<std::streamsize>(tex->mWidth));
                    std::printf("extracted: %s (%u bytes)\n", out.string().c_str(), tex->mWidth);
                }
                else
                {
                    std::printf("cannot write: %s\n", out.string().c_str());
                }
            }
            else
            {
                // 生のBGRAデータ。FBXではまれなので、ここでは再エンコードしない。 //
                std::printf("skip raw texture %u (%ux%u): re-encode not implemented\n",
                            i, tex->mWidth, tex->mHeight);
            }
        }
    }

    /**
     * @brief 1つのサブメッシュで使用するインデックス範囲とマテリアルを表す。
     */
    struct SubMeshData
    {
        uint32_t index_start; // サブメッシュの開始インデックス。
        uint32_t index_count; // サブメッシュで使うインデックス数。
        uint32_t material_slot; // 参照するマテリアル番号。
    };

    /**
     * @brief 書き出すマテリアルの色とディフューズテクスチャパスを表す。
     */
    struct MaterialData
    {
        Vec4 base_color; // マテリアルの基本色。
        std::wstring diffuse_path; // ディフューズテクスチャへのパス。
        std::wstring normal_path;
        std::wstring specular_path;  
        std::wstring height_path; 
        
    };

    /**
     * @brief 静的メッシュの頂点、インデックス、マテリアル、サブメッシュを .mesh に書き出す。
     * @param out_path 書き出し先 .mesh ファイルのパス。
     * @param vertices 書き出す静的頂点配列。
     * @param indices 書き出すインデックス配列。
     * @param materials 書き出すマテリアル配列。
     * @param sub_meshes 書き出すサブメッシュ配列。
     * @return 書き出しに成功したら true。
     */
    bool WriteMesh(const char* out_path,
                   const std::vector<StaticVertex>& vertices,
                   const std::vector<std::uint32_t>& indices,
                   const std::vector<MaterialData>& materials,
                   const std::vector<SubMeshData>& sub_meshes);
    bool ConvertMesh(const char* in_path, const char* out_path, const std::string& tex_folder);

    /**
     * @brief 要素数が uint32_t に収まるか確認する。
     * @param count 確認する要素数。
     * @param label エラー表示に使う名前。
     * @return uint32_t に収まるなら true。
     */
    bool CheckCountFitsUint32(std::size_t count, const char* label)
    {
        if (count > static_cast<std::size_t>((std::numeric_limits<std::uint32_t>::max)()))
        {
            std::printf("%s count exceeds uint32_t range\n", label);
            return false;
        }

        return true;
    }

    /**
     * @brief Assimp の静的メッシュから頂点とインデックスを取り出して配列へ追加する。
     * @param mesh 読み込む Assimp メッシュ。
     * @param vertices 追加先の静的頂点配列。
     * @param indices 追加先のインデックス配列。
     * @return 追加に成功したら true。
     */
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
                //反転させる。
                out.uv[1] = 1.0f - mesh.mTextureCoords[0][v].y;
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

    //　埋め込みテクスチャを実際に書き出す先。out_pathを基準にする。
    std::filesystem::path BuildTextureDir(const char* out_path, const std::string& tex_folder)
    {
        std::filesystem::path base = std::filesystem::path(out_path).parent_path().parent_path() / "Texture";
        if (!tex_folder.empty())
        {
            base /= tex_folder;
        }
        return base;
    }

    // .mesh内部に書くコムテクスチャパス。
    // Assets/Texture基準//
    std::filesystem::path BuildTextureRefDir(const std::string& tex_folder)
    {
        std::filesystem::path base = std::filesystem::path("Assets") / "Texture";
        if (!tex_folder.empty())
        {
            base /= tex_folder;
        }
        return base;
    }

    /**
     * @brief Assimp のテクスチャ参照から、出力で使うテクスチャパスを作る。
     * @param tex Assimp から取得したテクスチャパス。
     * @param tex_dir Assets/Texture からの相対サブフォルダー名。
     * @return Assets/Texture から始まるワイド文字列のパス。
     */
    std::wstring BuildTexturePath(const aiString& tex, const std::filesystem::path& tex_dir)
    {
        // aiString is UTF-8. On Windows, constructing a path from const char*
        // decodes it with the ANSI code page (CP932) and garbles non-ASCII
        // names. Build via char8_t so it is interpreted as UTF-8.
        std::filesystem::path src(reinterpret_cast<const char8_t*>(tex.C_Str()));
        const std::filesystem::path filename =
            src.filename();
        if (filename.empty())
        {
            return {};
        }
        return (tex_dir / filename).generic_wstring();
    }

    // マテリアルのテクスチャ参照を、実際に書き出すファイル名へ解決する。 //
    // *N 形式の埋め込み参照は ExtractEmbeddedTextures と同じ名前に合わせる。 //
    std::wstring BuildTexturePathResolved(const aiScene* scene, const aiString& tex,
                                          const std::filesystem::path& tex_dir)
    {
        if (const aiTexture* emb =
            scene->GetEmbeddedTexture(tex.C_Str()))
        {
            unsigned int idx = 0;
            for (; idx < scene->mNumTextures; ++idx)
            {
                if (scene->mTextures[idx] == emb)
                    break;
            }
            return (tex_dir / EmbeddedTextureFileName(emb, idx)).generic_wstring();
        }
        return BuildTexturePath(tex, tex_dir);
    }


    /**
     * @brief 入力モデルを静的メッシュとして .mesh ファイルへ変換する。
     * @param in_path 読み込むモデルファイルのパス。
     * @param out_path 書き出し先 .mesh ファイルのパス。
     * @param tex_folder テクスチャ用サブフォルダー名。
     * @return 変換と書き出しに成功したら true。
     */
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
        {
            //埋め込みテクスチャはout_path基準の絶対パスに書き出す
            // マテリアルパスが指す場所に合わせて埋め込みテクスチャを書き出す。 //
            const std::filesystem::path tex_dir = BuildTextureDir(out_path, tex_folder);
            ExtractEmbeddedTextures(scene, tex_dir.string());
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

            const uint32_t start = static_cast<uint32_t>(indices.size());

            if (!AppendMesh(*mesh, vertices, indices)) return false;

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
            aiString diffuse_tex;
            const std::filesystem::path tex_dir = BuildTextureRefDir( tex_folder);
            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &tex) == AI_SUCCESS)
            {
                data.diffuse_path = BuildTexturePathResolved(scene, tex, tex_dir);
            }
            if (material->GetTexture(aiTextureType_NORMALS, 0, &tex) == AI_SUCCESS)
            {
                data.normal_path = BuildTexturePathResolved(scene, tex, tex_dir);
            }
            if (material->GetTexture(aiTextureType_SPECULAR, 0, &tex) == AI_SUCCESS)
            {
                data.specular_path = BuildTexturePathResolved(scene, tex, tex_dir);
            }
            if (material->GetTexture(aiTextureType_HEIGHT, 0, &tex) == AI_SUCCESS)
            {
                data.height_path = BuildTexturePathResolved(scene, tex, tex_dir);
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

    /**
     * @brief 静的メッシュの頂点、インデックス、マテリアル、サブメッシュを .mesh に書き出す。
     * @param out_path 書き出し先 .mesh ファイルのパス。
     * @param vertices 書き出す静的頂点配列。
     * @param indices 書き出すインデックス配列。
     * @param materials 書き出すマテリアル配列。
     * @param sub_meshes 書き出すサブメッシュ配列。
     * @return 書き出しに成功したら true。
     */
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
        header.material_count = static_cast<std::uint32_t>(materials.size());
        header.submesh_count = static_cast<std::uint32_t>(sub_meshes.size());

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
            SubMeshEntry e = {
                sub_mesh.index_start, sub_mesh.index_count,
                sub_mesh.material_slot
            };
            ofs.write(reinterpret_cast<const char*>(&e), sizeof(e));
        }
        for (const auto& mat : materials)
        {
            MaterialEntry e = {};
            e.base_color = mat.base_color;
            e.diffuse_texture_length =
                static_cast<std::uint32_t>(mat.diffuse_path.size());
            e.normal_texture_length =
                static_cast<std::uint32_t>(mat.normal_path.size());
            e.specular_texture_length =
                static_cast<std::uint32_t>(mat.specular_path.size());
            e.height_texture_length =
                static_cast<std::uint32_t>(mat.height_path.size());
            ofs.write(reinterpret_cast<const char*>(&e), sizeof(e));
        }
        for (const auto& mat : materials)
        {
            if (!mat.diffuse_path.empty())
                ofs.write(reinterpret_cast<const char*>(mat.diffuse_path.c_str()),
                          static_cast<std::streamsize>(mat.diffuse_path.size() * sizeof(std::wstring::value_type)));
            if (!mat.normal_path.empty())  
                ofs.write(reinterpret_cast<const char*>(mat.normal_path.c_str()),
                    static_cast<std::streamsize>(mat.normal_path.size() * sizeof(std::wstring::value_type)));
            if (!mat.specular_path.empty())
                ofs.write(reinterpret_cast<const char*>(mat.specular_path.c_str()),
                    static_cast<std::streamsize>(mat.specular_path.size() * sizeof(std::wstring::value_type)));
            if (!mat.height_path.empty())
                ofs.write(reinterpret_cast<const char*>(mat.height_path.c_str()),
                    static_cast<std::streamsize>(mat.height_path.size() * sizeof(std::wstring::value_type)));
            printf("write %ls\n", mat.diffuse_path.c_str());
        }

        if (!ofs)
        {
            std::printf("write failed: %s\n", out_path);
            return false;
        }

        return true;
    }

    /**
     * @brief Assimp の行列をプロジェクトの row-vector 形式に合わせて転置する。
     * @param m 変換する Assimp 行列。
     * @return プロジェクトで使う Mat。
     */
    Mat ToRowVectorMat(const aiMatrix4x4& m)
    {
        // aiMatrix4x4 は m.a1..a4(1行目) ... の row-major。
        // 転置して、このプロジェクト側の行列表現に合わせる。
        return Mat(
            m.a1, m.b1, m.c1, m.d1,
            m.a2, m.b2, m.c2, m.d2,
            m.a3, m.b3, m.c3, m.d3,
            m.a4, m.b4, m.c4, m.d4);
    }

    /**
     * @brief 1頂点に対する1本分のボーン影響を表す。
     */
    struct SkinInfluence
    {
        uint32_t bone; // ボーンパレット内のボーン番号。
        float weight; // このボーンが頂点へ与える重み。
    };

    /**
     * @brief 1頂点のボーン影響を最大4本に絞り、重みを正規化して頂点へ書き込む。
     * @param inf 頂点に集めたボーン影響配列。
     * @param v 書き込み先のスキニング頂点。
     */
    void FinalizeSkinWeights(std::vector<SkinInfluence>& inf, SkinnedVertex& v)
    {
        // weight が大きい順に並べて上位4本だけ残す。
        std::sort(inf.begin(), inf.end(),
                  [](const SkinInfluence& a, const SkinInfluence& b)
                  {
                      return a.weight > b.weight;
                  });
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
                v.bone_ids[i] = inf[i].bone;
                v.bone_weights[i] = inf[i].weight / sum; // 正規化
            }
            else
            {
                v.bone_ids[i] = 0;
                v.bone_weights[i] = 0.0f;
            }
        }
    }

    /**
     * @brief Assimp のノード階層を平坦化した1ノード分の情報を表す。
     */
    struct FlatNode
    {
        std::string name; // ノード名。
        int32_t parent_index; // 親ノードのインデックス。ルートは -1。
        Mat local_bind_transform; // = aiNode::mTransformation(転置済み)
        int32_t skin_index = -1; // ボーンとして使うノードだけに設定する。
        Mat inverse_bind_pose; // skin_index >= 0 のとき有効。
    };

    /**
     * @brief Assimp のノードツリーを親から子の順で平坦な配列へ変換する。
     * @param node 変換する Assimp ノード。
     * @param parent_index 親ノードのインデックス。ルートは -1。
     * @param out 変換結果のノード配列。
     * @param name_to_index ノード名からインデックスを引くためのマップ。
     */
    void FlattenNodes(const aiNode* node, int32_t parent_index,
                      std::vector<FlatNode>& out,
                      std::unordered_map<std::string, int32_t>& name_to_index)
    {
        FlatNode fn = {};
        fn.name = node->mName.C_Str();
        fn.parent_index = parent_index;
        fn.local_bind_transform = ToRowVectorMat(node->mTransformation);
        fn.inverse_bind_pose = Mat(); // 初期値。ボーンでなければ使わない。

        const int32_t my_index = static_cast<int32_t>(out.size());
        out.push_back(fn);
        name_to_index[out[my_index].name] = my_index;

        for (unsigned int i = 0; i < node->mNumChildren; ++i)
        {
            FlattenNodes(node->mChildren[i], my_index, out, name_to_index);
        }
    }

    /**
     * @brief Assimp のスキニングメッシュから頂点、インデックス、ボーンウェイトを取り出す。
     * @param mesh 読み込む Assimp メッシュ。
     * @param vertices 追加先のスキニング頂点配列。
     * @param indices 追加先のインデックス配列。
     * @param influences 頂点ごとに集めるボーン影響。
     * @param name_to_index ノード名からノード番号を引くためのマップ。
     * @param nodes スケルトンのノード配列。
     * @param skin_counter 新しく割り当てるボーン番号。
     * @return 追加に成功したら true。
     */
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
                //反転させる。
                out.uv[1] = 1.0f - mesh.mTextureCoords[0][v].y;
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
            // ウェイトはあとで mBones から埋める。
            vertices.push_back(out);
        }

        // インデックスには base オフセットを足す。
        indices.reserve(indices.size() + static_cast<std::size_t>(mesh.mNumFaces) * 3);
        for (unsigned int f = 0; f < mesh.mNumFaces; ++f)
        {
            const aiFace& face = mesh.mFaces[f];
            if (face.mNumIndices != 3) continue;
            indices.push_back(base + face.mIndices[0]);
            indices.push_back(base + face.mIndices[1]);
            indices.push_back(base + face.mIndices[2]);
        }

        // スキンウェイトを抽出: mBones[b] の各 mWeights を頂点へ集める。
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

            // 初めてボーンとして使うノードに skin_index を割り当て、
            // inverse_bind_pose(= mOffsetMatrix 転置済み)を保存する。
            if (node.skin_index < 0)
            {
                node.skin_index = static_cast<int32_t>(skin_counter++);
                node.inverse_bind_pose = ToRowVectorMat(bone->mOffsetMatrix);
            }

            for (unsigned int w = 0; w < bone->mNumWeights; ++w)
            {
                const aiVertexWeight& vw = bone->mWeights[w];
                const std::size_t vi = static_cast<std::size_t>(base) + vw.mVertexId;
                influences[vi].push_back({static_cast<uint32_t>(node.skin_index), vw.mWeight});
            }
        }

        return true;
    }
    
    /**
     * @brief スキニングメッシュ、マテリアル、サブメッシュ、スケルトン階層を .skmesh に書き出す。
     * @param out_path 書き出し先 .skmesh ファイルのパス。
     * @param vertices 書き出すスキニング頂点配列。
     * @param indices 書き出すインデックス配列。
     * @param materials 書き出すマテリアル配列。
     * @param sub_meshes 書き出すサブメッシュ配列。
     * @param nodes 書き出すスケルトンノード配列。
     * @param skin_count ボーンパレット内のボーン数。
     * @return 書き出しに成功したら true。
     */
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

        SkMeshFileHeader header = {};
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
            SubMeshEntry e = {sub_mesh.index_start, sub_mesh.index_count, sub_mesh.material_slot};
            ofs.write(reinterpret_cast<const char*>(&e), sizeof(e));
        }
        for (const auto& mat : materials)
        {
            MaterialEntry e = {};
            e.base_color = mat.base_color;
            e.diffuse_texture_length =
                static_cast<std::uint32_t>(mat.diffuse_path.size());
            e.normal_texture_length =
                static_cast<std::uint32_t>(mat.normal_path.size());
            e.specular_texture_length =
                static_cast<std::uint32_t>(mat.specular_path.size());
            e.height_texture_length =
                static_cast<std::uint32_t>(mat.height_path.size());
            ofs.write(reinterpret_cast<const char*>(&e), sizeof(e));
        }
        for (const auto& mat : materials)
        {
            if (!mat.diffuse_path.empty())
                ofs.write(reinterpret_cast<const char*>(mat.diffuse_path.c_str()),
                          static_cast<std::streamsize>(mat.diffuse_path.size() * sizeof(std::wstring::value_type)));
            if (!mat.normal_path.empty())  
                ofs.write(reinterpret_cast<const char*>(mat.normal_path.c_str()),
                    static_cast<std::streamsize>(mat.normal_path.size() * sizeof(std::wstring::value_type)));
            if (!mat.specular_path.empty())
                ofs.write(reinterpret_cast<const char*>(mat.specular_path.c_str()),
                    static_cast<std::streamsize>(mat.specular_path.size() * sizeof(std::wstring::value_type)));
            if (!mat.height_path.empty())
                ofs.write(reinterpret_cast<const char*>(mat.height_path.c_str()),
                    static_cast<std::streamsize>(mat.height_path.size() * sizeof(std::wstring::value_type)));
            printf("write %ls\n", mat.diffuse_path.c_str());
        }

        // ノード階層(topo順)。
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
        // ノード名を連続して書く(NodeEntry のあとに char 列)。
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

    /**
     * @brief アニメーション名をファイル名として使える文字だけに置き換える。
     * @param s 変換するアニメーション名。
     * @return ファイル名として使える文字列。
     */
    std::string SanitizeName(const std::string& s)
    {
        std::string out;
        out.reserve(s.size());
        for (char c : s)
        {
            const bool ok = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9') || c == '-' || c == '_';
            out.push_back(ok ? c : '_');
        }
        return out;
    }

    int NodeDepth(const aiNode* node, const std::string& name, int depth)
    {
        if (name == node->mName.C_Str())
        {
            return depth;
        }
        for (unsigned int i = 0; i < node->mNumChildren; ++i)
        {
            const int d = NodeDepth(node->mChildren[i], name, depth + 1);
            if (d >= 0)
            {
                return d;
            }
        }
        return -1;
    }

    std::string FindRootMotionNode(const aiNode* root, const aiAnimation& anim)
    {
        std::string best;
        int best_depth = -1;
        for (unsigned int i = 0; i < anim.mNumChannels; ++i)
        {
            const std::string name = anim.mChannels[i]->mNodeName.C_Str();
            const int d = NodeDepth(root, name, 0);
            if (d >= 0 && (best_depth < 0 || d < best_depth))
            {
                best = name;
                best_depth = d;
            }
        }
        return best;
    }

    /**
     * @brief Assimp の1つのアニメーションクリップを .anim ファイルへ書き出す。
     * @param out_path 書き出し先 .anim ファイルのパス。
     * @param anim 書き出す Assimp アニメーション。
     * @return 書き出しに成功したら true。
     */
    bool WriteAnim(const std::string& out_path, const aiAnimation& anim, bool in_place, const std::string& root_node)
    {
        SkAnimFileHeader header = {};
        std::memcpy(header.magic, kSkAnimMagic, 4);
        header.version = kSkAnimVersion;
        // mTicksPerSecond が 0 の場合がある(FBXで値がないケース)。Assimp の既定値として 25 を使う。
        const double tps = (anim.mTicksPerSecond != 0.0) ? anim.mTicksPerSecond : 25.0;
        header.ticks_per_second = static_cast<float>(tps);
        header.duration = static_cast<float>(anim.mDuration); // ticks 単位
        header.channel_count = anim.mNumChannels;

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
            std::printf("cannot open output: %s\n", out_path.c_str());
            return false;
        }

        ofs.write(reinterpret_cast<const char*>(&header), sizeof(header));

        for (unsigned int c = 0; c < anim.mNumChannels; ++c)
        {
            const aiNodeAnim* ch = anim.mChannels[c];
            const std::string node_name = ch->mNodeName.C_Str();

            SkAnimChannel ce = {};
            ce.node_name_length = static_cast<uint32_t>(node_name.size());
            ce.pos_key_count = ch->mNumPositionKeys;
            ce.rot_key_count = ch->mNumRotationKeys;
            ce.scale_key_count = ch->mNumScalingKeys;
            ofs.write(reinterpret_cast<const char*>(&ce), sizeof(ce));

            // ノード名。読み込み側では Skeleton の name->index へ対応させる。
            if (!node_name.empty())
            {
                ofs.write(node_name.data(), static_cast<std::streamsize>(node_name.size()));
            }

            // 位置キー(ticks 時刻 + Vec3)。座標系は .skmesh と同じ前提。
            for (unsigned int k = 0; k < ch->mNumPositionKeys; ++k)
            {
                const aiVectorKey& src = ch->mPositionKeys[k];
                PositionKey key = {};
                key.time = static_cast<float>(src.mTime);
                key.value = Vec3(src.mValue.x, src.mValue.y, src.mValue.z);
                ofs.write(reinterpret_cast<const char*>(&key), sizeof(key));
            }
            // 回転キー(ticks 時刻 + Quat)。
            // 注意: Assimp の aiQuaternion は (w,x,y,z) だが、プロジェクトの Quat(XMFLOAT4) は (x,y,z,w)。
            for (unsigned int k = 0; k < ch->mNumRotationKeys; ++k)
            {
                const aiQuatKey& src = ch->mRotationKeys[k];
                RotationKey key = {};
                key.time = static_cast<float>(src.mTime);
                key.value = Quat(src.mValue.x, src.mValue.y, src.mValue.z, src.mValue.w);
                ofs.write(reinterpret_cast<const char*>(&key), sizeof(key));
            }
            // in_placeかつチャンネルノードがルートならx/zを初期フレーム値に固定する。
            const bool lock = in_place && !root_node.empty() && node_name == root_node && ch->mNumPositionKeys > 0;
            const float lock_x = lock ? ch->mPositionKeys[0].mValue.x : 0.0f;
            const float lock_z = lock ? ch->mPositionKeys[0].mValue.z : 0.0f;
            // スケールキー(ticks 時刻 + Vec3)。
            for (unsigned int k = 0; k < ch->mNumScalingKeys; ++k)
            {
                const aiVectorKey& src = ch->mScalingKeys[k];
                ScaleKey key = {};
                key.time = static_cast<float>(src.mTime);
                key.value = Vec3(src.mValue.x, src.mValue.y, src.mValue.z);
                ofs.write(reinterpret_cast<const char*>(&key), sizeof(key));
            }
        }

        if (!ofs)
        {
            std::printf("write failed: %s\n", out_path.c_str());
            return false;
        }
        std::printf("anim '%s': channels=%u duration=%.3f tps=%.1f -> %s\n",
                    anim.mName.C_Str(), anim.mNumChannels,
                    header.duration, header.ticks_per_second, out_path.c_str());
        return true;
    }

    /**
     * @brief シーン内の全アニメーションを .anim ファイルとして書き出す。
     * @param skmesh_out_path .skmesh の出力パス。アニメーション出力名の基準に使う。
     * @param scene アニメーションを含む Assimp シーン。
     * @return 全アニメーションを書き出せたら true。
     */
    bool WriteAnimations(const char* skmesh_out_path, const aiScene& scene, bool in_place)
    {
        if (scene.mNumAnimations == 0)
        {
            std::printf("no animation in scene\n");
            return true;
        }

        std::filesystem::path base(skmesh_out_path);
        base.replace_extension(); // ".skmesh" を外す(ディレクトリは残る)。
        const std::string stem = base.string();

        bool all_ok = true;
        for (unsigned int a = 0; a < scene.mNumAnimations; ++a)
        {
            const aiAnimation* anim = scene.mAnimations[a];
            if (anim == nullptr) continue;

            std::string out = stem;
            if (scene.mNumAnimations > 1)
            {
                std::string name = SanitizeName(anim->mName.C_Str());
                if (name.empty()) name = std::to_string(a);
                out += "_" + name;
            }
            out += ".anim";
            const std::string root_node = FindRootMotionNode(scene.mRootNode, *anim);
            if (!WriteAnim(out, *anim, in_place, root_node))
            {
                all_ok = false;
            }
        }
        return all_ok;
    }

    /**
     * @brief 入力モデルをスケルトン付きメッシュとして .skmesh と .anim に変換する。
     * @param in_path 読み込むモデルファイルのパス。
     * @param out_path 書き出し先 .skmesh ファイルのパス。
     * @param tex_folder テクスチャ用サブフォルダー名。
     * @return メッシュ変換に成功したら true。
     */
    bool ConvertSkMesh(const char* in_path, const char* out_path, const std::string& tex_folder, bool in_place)
    {
        Assimp::Importer importer;
        // スキンド用フラグの方針:
        //  - PreTransformVertices は使わない(階層がつぶれるため)。
        //  - ConvertToLeftHanded は使わない(offset/transform/anim/頂点を一括変換すると崩れやすいため)。
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
        {
            // マテリアルパスが指す場所に合わせて埋め込みテクスチャを書き出す。 //
            const std::filesystem::path tex_dir =
                BuildTextureDir(out_path, tex_folder);
            ExtractEmbeddedTextures(scene, tex_dir.string());
        }

        // 1. ノード階層を構築(topo順 + name マップ)。
        std::vector<FlatNode> nodes;
        std::unordered_map<std::string, int32_t> name_to_index;
        FlattenNodes(scene->mRootNode, -1, nodes, name_to_index);

        // 2. メッシュを読み込む(頂点・インデックス・スキンウェイト)。
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

        // 3. スキンウェイトを頂点へ確定(最大4本・正規化)。
        for (std::size_t i = 0; i < vertices.size(); ++i)
        {
            FinalizeSkinWeights(influences[i], vertices[i]);
        }

        // 4. マテリアル(最後にまとめて出力)。
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
            const std::filesystem::path tex_dir = BuildTextureRefDir(tex_folder);
            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &tex) == AI_SUCCESS)
            {
                data.diffuse_path = BuildTexturePathResolved(scene, tex, tex_dir);
            }
            if (material->GetTexture(aiTextureType_NORMALS, 0, &tex) == AI_SUCCESS)
            {
                data.normal_path = BuildTexturePathResolved(scene, tex, tex_dir);
            }
            if (material->GetTexture(aiTextureType_SPECULAR, 0, &tex) == AI_SUCCESS)
            {
                data.specular_path = BuildTexturePathResolved(scene, tex, tex_dir);
            }
            if (material->GetTexture(aiTextureType_HEIGHT, 0, &tex) == AI_SUCCESS)
            {
                data.height_path = BuildTexturePathResolved(scene, tex, tex_dir);
            }
            materials.push_back(data);
        }

        if (vertices.empty() || indices.empty())
        {
            std::printf("no geometry\n");
            return false;
        }

        if (!WriteSkMesh(out_path, vertices, indices, materials, sub_meshes, nodes, skin_counter))
        {
            return false;
        }

        // アニメ用ファイル(.anim)も出力する。メッシュ変換は成功扱いにするが、
        // アニメの書き出しに失敗したときは警告だけ出して処理を続ける。
        if (!WriteAnimations(out_path, *scene, in_place))
        {
            std::printf("warn: some animations failed to write\n");
        }
        return true;
    }

    bool ConvertAnim(const char* in_path, const char* out_path, bool in_place)
    {
        Assimp::Importer importer;
        const unsigned int flags = aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_LimitBoneWeights |
            aiProcess_PopulateArmatureData;

        const aiScene* scene = importer.ReadFile(in_path, flags);
        if (scene == nullptr || scene->mRootNode == nullptr)
        {
            std::printf("assimp: %s\n", importer.GetErrorString());
            return false;
        }
        return WriteAnimations(out_path, *scene, in_place);
    }

    /**
     * @brief 文字列の前後にあるダブルクォートを取り除く。
     * @param s クォートを取り除く文字列。
     * @return クォートを取り除いた文字列。
     */
    std::string StripQuotes(std::string s)
    {
        if (s.size() >= 2 && s.front() == '"' && s.back() == '"')
        {
            s = s.substr(1, s.size() - 2);
        }
        return s;
    }

    /**
     * @brief Enter 入力までコンソールを閉じずに待つ。
     */
    void WaitEnter()
    {
        std::printf("\nPushEnter Finish...");
        std::string dummy;
        std::getline(std::cin, dummy);
    }
}

/**
 * @brief コマンドライン指定または対話入力でメッシュ変換を実行するエントリーポイント。
 * @param argc コマンドライン引数の数。
 * @param argv コマンドライン引数の配列。
 * @return 正常終了なら 0、変換失敗なら 2。
 */
int main(int argc, char** argv)
{
    std::string in_path;
    std::string out_path;

    if (argc >= 3)
    {
        in_path = argv[1];
        out_path = argv[2];
        // 省略時: Assets/Texture/ 直下のサブフォルダーなし。
        const std::string tex_folder = (argc >= 4) ? argv[3] : "";

        // 出力拡張子が .skmesh ならスケルトン付きとして変換する。
        const std::filesystem::path ext =
            std::filesystem::path(out_path).extension();
        bool in_place = false;

        const bool ok =
            ext == ".anim"
                ? ConvertAnim(in_path.c_str(), out_path.c_str(), in_place)
                : ext == ".skmesh"
                ? ConvertSkMesh(in_path.c_str(), out_path.c_str(), tex_folder, in_place)
                : ConvertMesh(in_path.c_str(), out_path.c_str(),
                              tex_folder);
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

        std::printf("output (.mesh / .skmesh / .anim): ");
        std::getline(std::cin, out_path);
        out_path = StripQuotes(out_path);

        // Assets/Texture/ 配下のサブフォルダー。空Enterなら Assets/Texture/ 直下に置く。
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
            // 出力拡張子が .skmesh ならスケルトン付きとして変換する。
            const std::filesystem::path ext =
                std::filesystem::path(out_path).extension();
            bool in_place = false;
            // その場でずっとアニメーションをするか？
            if (ext == ".skmesh" || ext == ".anim")
            {
                std::printf("in-place (remove root motion)?(y/N): ");
                std::string s;
                std::getline(std::cin, s);
                in_place = (!s.empty() && s[0] == 'y' || s[0] == 'Y');
            }

            const bool ok =
                ext == ".anim"
                    ? ConvertAnim(in_path.c_str(), out_path.c_str(), in_place)
                    : ext == ".skmesh"
                    ? ConvertSkMesh(in_path.c_str(), out_path.c_str(), tex_folder, in_place)
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
