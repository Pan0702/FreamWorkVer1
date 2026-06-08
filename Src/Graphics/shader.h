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
     * @brief LoadFromFileを行う関数。
     * @param filename 読み込むファイル名。
     * @param entry_point 引数。
     * @param target 引数。
     * @return 条件を満たす場合は true。
     */
    bool LoadFromFile(const wchar_t* filename, const char* entry_point, const char* target);
    /**
     * @brief Validかどうかを確認する関数。
     * @return 条件を満たす場合は true。
     */
    bool IsValid() const;
    /**
     * @brief Bytecodeを取得する関数。
     * @return 戻り値。
     */
    D3D12_SHADER_BYTECODE GetBytecode() const;

private:
    ComPtr<ID3DBlob> bytecode_;
};
