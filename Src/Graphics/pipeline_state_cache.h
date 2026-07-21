#pragma once
#include "graphics_device.h"
#include "root_param.h"
#include "../Core/Math/my_math.h"
class RootSignature;
class PipelineState;

enum class ShaderId : uint32
{
    kPbr,
    kCount,
};

enum class VertexFactoryId : uint32
{
    kStatic,
    kInstanced,
    kSkinned,
    kCount,
};

class PipelineStateCache
{
public:
    /**
     * @brief Rootsignatureを2本作った後、ShaderIdとVertexFactoryIdのすべての組み合わせのPSOを
     * 事前生成して配列に格納
     * @param device 使用するD3D12でばいす
     * @return RootSignatureとPSOの生成に成功した場合はＴｒｕｅ
     */
    bool Initialize(ID3D12Device* device);
    /**
     * @brief VertexFactoryに対応するRootSignatureを返す
     * @param factory 取得したいRootSignatureのVertexFactory
     * @return 対応するRootSignature。なければNullptr
     */
    ID3D12RootSignature* GetRootSignature(VertexFactoryId factory);
    /**
     * @brief 生成済みの配列から指定した組み合わせのPSOを返す。
     * @param shader 使用するShader
     * @param factory 使用するVertexFactory
     * @return 対応するPSO
     */
    PipelineState* Get(ShaderId shader,VertexFactoryId factory);
    
private:
    /**
     * @brief Static系MeshとSkinned用のRootSignatureを1本ずつ生成
     * @param device 使用するdevice
     * @return 両方のRootSignatureを返す
     */
    bool BuidRootSignature(ID3D12Device* device);
    /**
     * @brief 一組のPSOを生成してPSOを管理する配列に格納する
     * @param device 使用するdevice
     * @param shader PSを決めるシェーダー
     * @param factory VSと入力レイアウトを決めるVertexFactory
     * @return 生成に成功したらＴｒｕｅ
     */
    bool BuildPipelineState(ID3D12Device* device,ShaderId shader,VertexFactoryId factory);
    
    std::unique_ptr<RootSignature> static_rs_;
    std::unique_ptr<RootSignature> skinned_rs_;
    std::unique_ptr<PipelineState> pso_[ToIndex(ShaderId::kCount)][ToIndex(VertexFactoryId::kCount)];
};
