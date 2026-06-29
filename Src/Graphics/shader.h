#pragma once
#include <d3d12.h>
#include "../Core/common.h"

/**
 * @brief Shaderのデータと処理をまとめる型。
 */
class Shader
{
public:
    /**
     * @brief ファイルや外部データを読み込んで内部表現へ変換する。
     * @param filename 読み書きするファイルパス。
     * @param entry_point シェーダーのエントリーポイント名。
     * @param target コンパイル対象のシェーダーモデル。
     * @return 指定データの読み込みが完了した場合は true。
     */
    bool LoadFromFile(const wchar_t* filename, const char* entry_point, const char* target);
    /**
     * @brief 現在の状態が条件を満たしているか調べる。
     * @return 内部リソースが使用可能な状態なら true。
     */
    bool IsValid() const;
    /**
     * @brief Bytecode を取得する。
     * @return 現在保持している Bytecode。
     */
    D3D12_SHADER_BYTECODE GetBytecode() const;

private:
    ComPtr<ID3DBlob> bytecode_;
};
