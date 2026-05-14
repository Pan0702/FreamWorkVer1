# Framework アーキテクチャ方針

## 全体方針

このフレームワークは **Unreal Engine 5 の構造に近づける** ことを目指す。
DirectX 12 を1から学習しつつ、最終的に UE5 ライクな Actor / Component 系のフレームワークを構築する。

---

## 重要な設計原則

### 1. Object は Update と Render を「両方は」持たない

Unity の `GameObject` のように1クラスが `Update()` と `Render()` を両方持つ構造は**採用しない**。
UE5 に倣い、**Tick（更新）と描画は完全に分離する**。

- `Actor` / `Component` は `Tick(deltaTime)` のみ持つ
- 描画は `PrimitiveComponent` 系が **Renderer に「自分を描いて」と登録する** 形
- Renderer が登録された描画要求を集約して描く

### 2. レイヤー依存方向

```
Game → Engine → Renderer → Graphics → Platform → Core
```

一方通行。逆参照は禁止。

---

## 階層構造（UE5 対応）

| 層 | 役割 | UE5 対応 |
|---|---|---|
| `Object` | 基底クラス。名前・ID・ライフサイクル | `UObject` |
| `Actor` | World に配置可能。Transform、Tick を持つ | `AActor` |
| `Component` | Actor に着脱する機能単位。Tick を持つ | `UActorComponent` |
| `SceneComponent` | Transform を持つ Component | `USceneComponent` |
| `PrimitiveComponent` | Renderer に登録される描画コンポーネント | `UPrimitiveComponent` |
| `World` | Actor の集合 | `UWorld` |
| `Renderer` / `Scene` | 描画要求を処理 | `FScene` / RenderingThread |
| `GameInstance` | World を跨いだ状態 | `UGameInstance` |
| `Subsystem` | グローバルサービス | `UEngineSubsystem` 等 |

---

## 実装フェーズ

### フェーズ1（現在 / 最優先）: 描画パイプラインを動かす

**目的:** DX12 で「1フレーム描く」手順を実際に通し、Renderer の API を経験から決める。
ここを通さずに `Object`/`Actor` 階層を作ると、`Render()` の引数設計が空想になり、後で大規模リファクタになる。

タスク:
1. `DescriptorHeap` クラス（RTV/DSV/SRV/CBV/UAV 共通基底）
2. SwapChain のバックバッファに RTV を生成
3. `Application::Render` 直書きで画面クリア（単色塗りつぶし）
4. 三角形描画（RootSignature, PSO, Shader, VertexBuffer）
5. 立方体描画（IndexBuffer, ConstantBuffer, Camera）
6. テクスチャ描画

### フェーズ2: UE5 風オブジェクト階層

- `Object` 基底（名前、ID、ライフサイクル）
- `Actor` 基底（Transform、Tick）
- `Component` 基底（着脱可能、Tick）
- `World`（Actor の集合）
- Tick システム（毎フレーム全 Component の Tick を呼ぶ）
- この時点では描画は `Application::Render` のまま

### フェーズ3: 描画分離

- `Renderer` クラスに描画手順を移す
- `PrimitiveComponent` を作る
- `PrimitiveComponent` が `Renderer` に登録 → `Renderer::Render` がそれを描く
- 2D描画（Sprite）も `PrimitiveComponent` の派生として乗せる

### フェーズ4: 拡張

- マルチスレッド化（ゲームスレッド / レンダースレッド分離）
- `Subsystem`、`GameMode`、`PlayerController` 等
- リフレクション（UE5 の `UCLASS`/`UPROPERTY` 相当）

---

## シェーダーとライティングの導入タイミング

### シェーダーの導入

| 段階 | 登場ステップ | シェーダーの内容 | 必要な前提 |
|---|---|---|---|
| 初登場 | フェーズ1ステップ4「三角形描画」 | VS: 頂点座標をそのまま出力 / PS: 単色出力 | RootSignature, PSO, Shader コンパイル基盤 |
| 行列変換 | フェーズ1ステップ5「立方体」 | VS: World × View × Projection、PS: 単色 or 頂点カラー | ConstantBuffer、Camera |
| テクスチャサンプリング | フェーズ1ステップ6「テクスチャ」 | PS: SRV からサンプリング | SRV ヒープ、Sampler、UV 座標 |
| 深度テスト対応 | フェーズ1ステップ7「深度バッファ」 | シェーダ内容は変わらず、PSO の DepthStencil 設定 | DSV、深度テクスチャ |

**シェーダーの管理方針:**
- HLSL は実行時コンパイル（`D3DCompileFromFile`）
- `.cso` ビルド時生成は採用しない
- シェーダーファイルは `Src/Shaders/` 配下に配置
- 当面は VS / PS の2種のみ。Compute / Geometry / Hull / Domain はフェーズ4以降

### ライティングの導入

ライティング実装には以下が前提として揃っている必要がある:

1. **頂点に法線（Normal）が入っている** — フェーズ1ステップ5（立方体）で対応開始
2. **ConstantBuffer でライト情報を伝達できる** — フェーズ1ステップ5で土台ができる
3. **Material 概念がある** — フェーズ1終盤〜フェーズ2で導入
4. **複数オブジェクトをまとめて管理** — Mesh / Scene の概念、フェーズ2以降

**ライティングの段階分け:**

| 段階 | 内容 | 必要なもの | 導入予定フェーズ |
|---|---|---|---|
| A. Lambert（拡散反射のみ） | 法線とライト方向の内積で陰影 | 法線、Directional Light 1個 | フェーズ1終盤（Mesh/Material 導入直後） |
| B. Phong / Blinn-Phong | 鏡面反射追加 | 視線ベクトル、Material の specular | フェーズ1終盤〜フェーズ2 |
| C. Normal Mapping | 法線マップで表面の凹凸 | TBN 行列、法線テクスチャ | フェーズ3 |
| D. 複数ライト対応 | Point / Spot Light を複数 | ライト配列、`LightComponent` 抽象 | フェーズ3 |
| E. PBR（物理ベース） | 業界標準のシェーディング | Metallic/Roughness テクスチャ、IBL | フェーズ4 |
| F. Shadow Mapping | 影を落とす | 深度パスを別途実行 | フェーズ4 |

**UE5 風構造でのライト配置:**

```
LightComponent : SceneComponent     ← UE5 の ULightComponent 相当
  ├ DirectionalLightComponent
  ├ PointLightComponent
  └ SpotLightComponent
```

`PrimitiveComponent` と同様、`LightComponent` も **Renderer に登録**する。
Renderer がシーン内のライトを集約してシェーダーに渡す。
Actor が自分の中でライト計算するのではない（UE5 風の重要原則）。

### 注意点（忘れがちな事）

- シェーダー初登場は早い（**三角形のタイミング = フェーズ1ステップ4**）
- ただし**ライティングはずっと後**（Lambert ですら Mesh/Material が揃ってから）
- 法線を含まない頂点構造で進めると、ライティング導入時に頂点フォーマットの大変更が発生する
- → **立方体段階（フェーズ1ステップ5）で頂点に法線フィールドを入れておく**と後が楽
- Material は最初から「色だけ持つクラス」として作っておき、後で specular/roughness を足す方が良い
- ConstantBuffer は最初は1個でも、複数（オブジェクト用 / フレーム用 / マテリアル用）に分ける設計を見据える

---

## 外部ライブラリ方針

- **ImGui のみ採用**（Debug 層に隔離）
- 数学ライブラリ（Vec/Mat）も自前。DirectXMath は使わない
- HLSL は実行時コンパイル（`D3DCompileFromFile`）。`.cso` ビルド時生成は採用しない

---

## メモ

- プロジェクト名は `Framework`（sln/vcxproj とも `Framework.{sln,vcxproj}`）
- ローカルディレクトリ名のみ歴史的経緯で `FreamWork`
- 3D レンダリング学習が主軸。2D 描画にも対応する構成