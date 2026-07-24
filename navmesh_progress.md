# NavMesh実装 引き継ぎメモ

## 目標

C++／DirectX 12製の自作エンジンに、Recast方式を参考にしたNavMeshを実装する。

- ワールド単位は1 unit = 1 cm。
- GeometryはComponentによって`NavigationSystem`へ登録する。
- 最終的にNavMeshを生成・デバッグ描画し、A*による経路探索と経路の直線化まで行う。
- 所有者本人がコードを書くことが学習目的。Codexは一度に大量のコードを出さず、現在作っている処理の目的、必要な入力と出力、処理手順を言葉で説明してからコードを確認する。

## 会話の進め方

- 一度に1つの関数または小さな処理だけ進める。
- 毎回、最初に「今はNavMeshのどの部分を実装しているか」を説明する。
- ユーザーの「ど」「どう？」は、直近に書いたコードを確認してほしいという意味。
- 原則としてCodexは実装せず、ユーザーが書いたコードをレビューする。
- 守りすぎるコードより、現在のエンジンで成立する前提を優先する。ただし範囲外アクセスなど重大な問題は指摘する。
- 命名はリポジトリのGoogle C++形式に従う。

## 現在の設定

`NavigationConfig`はcm単位で、現在の初期値は次のとおり。

- Cell幅：20 cm
- Cell高さ：10 cm
- Agent高さ：180 cm
- Agent半径：40 cm
- 最大昇降高：40 cm
- 最大傾斜：45度

## 完成済みのデータと登録処理

### NavigationGeometry

次の情報を保持する。

- ローカル頂点
- インデックス
- ワールド行列
- ワールドAABB

ワールドAABBは、頂点をワールド変換して全頂点の最小値・最大値から求める。

### NavigationSourceComponent

- `MeshColliderComponent`からGeometryを取得する。
- Attach時に`NavigationSystem`へ登録し、Detach時に解除する。
- Geometryは毎フレーム取得せず、NavMesh生成時に収集する。

### NavigationSystem

- Source Componentを単調増加IDで登録する。
- `CollectGeometries()`で有効なGeometryを配列として収集する。

## 完成済みのHeightfield

### NavigationSpan

1つのCell内に存在する縦方向の固体区間。

- `min_height`：Heightfield最小Yを基準とした下端のボクセル番号
- `max_height`：上端のボクセル番号
- `is_walk`：結合後の最上面が歩行可能候補か

高さ番号には`uint32_t`を使用している。

### NavigationHeightfieldCell

1つのXZ Cellに含まれる`NavigationSpan`の配列を持つ。

### NavigationHeightfield

次を保持・実行する。

- ワールドAABB
- Cell幅、Cell高さ
- X方向のCell数、Z方向のCell数
- 全Cell
- `Initialize`
- `GetCell`
- `AddSpan`
- `MergeSpans`

重なる、または接しているSpanは結合する。結合後の`is_walk`は、結合後に最も高い上面を持つSpanの状態を採用する。同じ高さなら、どちらかが歩行可能なら歩行可能とする。

## 完成済みのラスタライズ処理

`NavigationMeshBuilder`に以下を実装済み。

1. `GetWorldTriangle`
   - Geometryのインデックス3個から三角形を1枚取り出し、頂点をワールド変換する。
2. `IsWalkableTriangle`
   - 三角形の傾斜と`agent_max_slope_deg`から歩行可能候補か判定する。
3. `CalcTriangleBounds`
   - 三角形のワールドAABBを求める。
4. `CalcCellRange`
   - 三角形AABBと重なるHeightfieldのXZ Cell範囲を求める。
5. `CalcCellBounds`
   - 指定Cellのワールド空間AABBを求める。
6. `ClipPolygonAgainstMinX／MaxX／MinZ／MaxZ`
   - Sutherland–Hodgman方式でポリゴンをCellの各境界へ順番にクリップする。
7. `ClipTriangleToCell`
   - 三角形をCellのXZ範囲へ切り抜く。結果は三角形とは限らず、4頂点以上の一時ポリゴンになる場合がある。
8. `CalcPolygonHeightRange`
   - クリップ後ポリゴンの最小Yと最大Yを求める。
9. `CreateSpanFromHeightRange`
   - ワールドY範囲をHeightfield基準の高さ番号へ変換し、Spanを作る。
10. `RasterizeTriangle`
    - 三角形と重なる各CellへSpanを追加する。
11. `RasterizeGeometry`
    - Geometryのインデックスを3個ずつ処理し、全三角形をラスタライズする。
12. `Build`
    - 全Geometryをラスタライズし、最後に`heightfield.MergeSpans()`を一度だけ呼ぶ。

ここまでで「入力GeometryをHeightfieldへボクセル化する処理」が完成している。

## 重要な理解

- HeightfieldはXZ平面のグリッドで、各Cellに縦方向のSpanを持つ。
- 三角形AABBは処理対象Cellを絞るための大まかな判定。
- 実際のCell内の高さは、三角形をそのCellへクリップした結果から求める。
- 斜面はHeightfield上では階段状になる。後の輪郭生成とポリゴン化で簡略化される。
- `is_walk`は元の三角形の傾斜判定から取得し、その三角形から作るSpanへ渡す。
- `AddSpan(x, z, span)`の`x`と`z`は、`CellRange`を走査するループ変数。
- HeightfieldはNavMeshそのものではなく、NavMeshを作るための中間データ。

## 今後の実装順

この順番で固定して進める。

1. 低い天井を除外
2. 高すぎる段差・崖を除外
3. エージェント半径ぶん壁際を削る
4. 歩行可能なSpan同士を接続
5. 接続領域（Region）に分割
6. Regionの輪郭を作る
7. 輪郭をNavMeshポリゴンへ変換
8. ポリゴン同士の隣接情報を作る
9. NavMeshポリゴンからデバッグ描画用の頂点・インデックスを作る
10. 半透明の面と境界線を描画する
11. NavMesh生成結果や経路を色分けする
12. A*で経路探索
13. 経路を直線的に整える

## 次に行う作業

「低い天井を除外」を`NavigationMeshBuilder`へ実装する。

各CellのSpanを下から順番に確認し、現在のSpan上面から次のSpan下面までの空間を求める。その空間が`agent_height`未満なら、エージェントが立てないため現在のSpanを歩行不可にする。最上段Spanの上には天井がないものとして扱う。

最初に決めるべきものは、この処理を行う関数名、引数、戻り値である。処理内容を理解してからループを書く。

## 描画について

`Src/Debug/navigation_debug_renderer.h/.cpp`は存在するが、現時点では中身がほぼ空。

NavMeshポリゴン完成後に、既存の`DebugLineRenderer`を利用して面・境界線を描画する予定。必要なら開発途中の確認用として、HeightfieldのSpanをワイヤーフレームの箱で表示できる。
