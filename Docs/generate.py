#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Framework 説明書ジェネレータ
============================================================================
Src/ と Shaders/ を走査し、各ソースファイルの Doxygen コメント・クラス定義・
メソッド・メンバ変数を抽出して、単一の HTML ドキュメントビューア
(Docs/index.html) を生成する。

■ 使い方
    python Docs/generate.py

■ 新しいファイルを追加したとき
    ソースに Doxygen コメント(/** @brief ... */)を書いて、このスクリプトを
    もう一度実行するだけ。ツリーと解説の両方へ自動で反映される。

■ レイヤーの概要文だけは手書き(LAYER_OVERVIEW)。必要なら編集する。
============================================================================
"""
import os, re, json, html

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT  = os.path.join(ROOT, "Docs", "index.html")

# 走査対象。ImGui などの外部ライブラリと生成物は除外。
SCAN_DIRS = ["Src", "Shaders"]
EXCLUDE_DIRS = {"ImGui", "x64", "Debug", "Release", ".git", ".vs", ".idea"}
CODE_EXT = {".h", ".hpp", ".cpp", ".cc", ".hlsl"}

LAYER_BY_TOP = {
    "Core": "core", "Platform": "platform", "Graphics": "graphics",
    "Resource": "resource", "Renderer": "renderer", "Engine": "engine",
    "Debug": "debug", "Game": "game", "Play": "play",
    "Title": "play", "Select": "play",
}

LAYER_META = {
    "core":     ("Core",     "#8b6cef"),
    "platform": ("Platform", "#c2568a"),
    "graphics": ("Graphics", "#1f95c8"),
    "resource": ("Resource", "#d98324"),
    "renderer": ("Renderer", "#2fa36b"),
    "engine":   ("Engine",   "#1f74d0"),
    "debug":    ("Debug",    "#6b7684"),
    "game":     ("Game",     "#d64550"),
    "play":     ("Play",     "#b5892a"),
    "shader":   ("Shader",   "#5a6b7a"),
}

# 各レイヤーの設計上の位置づけ(手書き)。フォルダをクリックすると表示される。
LAYER_OVERVIEW = {
    "Core": "全レイヤーの土台。数学(ベクトル/行列/クォータニオン/交差判定)や共通の型・定数を提供する。他のどの層にも依存しない最下層。",
    "Platform": "OS(Win32)との境界。ウィンドウ生成とメッセージ処理、キーボード/マウス/パッド入力を扱う。ワイド文字列(wchar_t)を扱うのは主にこの層。",
    "Graphics": "DirectX 12 の薄いラッパ群。デバイス・コマンドキュー・スワップチェーン・各種バッファ・パイプラインステートなど、D3D12 のオブジェクトを RAII で包む。学習目的のためラッパは薄く明示的に保たれている。",
    "Resource": "メッシュ・テクスチャ・マテリアル・アニメーション・音声などのアセットを読み込み、管理する層。多くが *Manager として一元管理される。",
    "Renderer": "Graphics の部品を組み合わせて実際の絵を描く層。シーン/メッシュ/スキンメッシュ/空/影/スプライト/UI など、描画の種類ごとにレンダラーを持つ。",
    "Engine": "ゲームの骨格。Actor(物) と Component(部品) と World(場) と Level(画面) からなるコンポーネント指向のフレームワーク。RenderSystem がこの層から描画を駆動する。",
    "Debug": "開発補助。ImGui を用いたデバッグ UI とデバッグシーン。ImGui 本体は外部ライブラリのため解説対象からは除外している。",
    "Game": "アプリケーションの入口(main)と最上位の GameMain。ウィンドウ・入力・描画・ゲーム内容を束ねてメインループを回す。",
    "Play": "このフレームワークで作られた実際のゲーム内容。プレイヤー・ステージ・ギミック・UI など、ゲーム固有の実装が集まる。",
    "Title": "タイトル画面のレベルと UI。",
    "Select": "ステージ選択画面のレベルと UI。",
}

# 「XのデータとPXXをまとめる型。」のような定型 @brief を、より読みやすい表現へ。
GENERIC_BRIEF = re.compile(r"^(.+?)のデータと処理をまとめる型。?$")


def decode(path):
    with open(path, "rb") as f:
        b = f.read()
    try:
        s = b.decode("utf-8")
    except UnicodeDecodeError:
        try:
            s = b.decode("cp932")
        except UnicodeDecodeError:
            s = b.decode("utf-8", "replace")
    return s.lstrip("﻿")


def clean_ws(s):
    return re.sub(r"\s+", " ", s).strip()


def parse_doc_comment(block):
    """/** ... */ の中身から @brief/@param/@return を取り出す。"""
    body = block
    body = re.sub(r"^/\*\*", "", body)
    body = re.sub(r"\*/$", "", body)
    lines = []
    for ln in body.splitlines():
        ln = re.sub(r"^\s*\*\s?", "", ln)
        lines.append(ln)
    text = "\n".join(lines)
    brief, params, ret = "", [], ""
    # @brief
    m = re.search(r"@brief\s*(.*?)(?=(?:\n\s*@)|\Z)", text, re.S)
    if m:
        brief = clean_ws(m.group(1))
    else:
        # @tag が無いコメントは全体を brief とみなす
        t = clean_ws(text)
        if t and not t.startswith("@"):
            brief = t
    for pm in re.finditer(r"@param\s+(\S+)\s*(.*?)(?=(?:\n\s*@)|\Z)", text, re.S):
        params.append((pm.group(1), clean_ws(pm.group(2))))
    rm = re.search(r"@return\s*(.*?)(?=(?:\n\s*@)|\Z)", text, re.S)
    if rm:
        ret = clean_ws(rm.group(1))
    return brief, params, ret


def next_decl(src, pos):
    """コメント直後の宣言テキストを ; か { まで取り出す。"""
    i = pos
    n = len(src)
    while i < n and src[i] in " \t\r\n":
        i += 1
    j = i
    depth = 0
    while j < n:
        c = src[j]
        if c in ";{":
            break
        if c == "\n" and src[j-1] == "\n":
            break
        j += 1
    decl = clean_ws(src[i:j])
    return decl


CLASS_RE = re.compile(r"\b(class|struct)\s+([A-Za-z_]\w*)\s*(?::\s*(public|private|protected)\s+([A-Za-z_]\w*))?")
# private メンバ規約: 末尾が _ 。型 名前_ [= ...];  ( ) を含まない行。
MEMBER_RE = re.compile(r"^\s*([A-Za-z_][\w:<>,\s\*&]*?\s[\*&]?\s*)([A-Za-z_]\w*_)(\s*\[[^\]]*\])?\s*(=\s*[^;]+)?;\s*$")


def is_method(decl):
    # テンプレート宣言や戻り値付き、コンストラクタなど () を含めば関数扱い
    d = decl
    # `template<...>` は次の実体を見たいが、簡易的に () があれば関数
    return "(" in d and ")" in d


def is_type_decl(decl):
    return re.match(r"\s*(class|struct|enum)\b", decl) is not None


def norm(s):
    return re.sub(r"[^a-z0-9]", "", s.lower())


def valid_class(src, m):
    """CLASS_RE のマッチが本物の型定義かを判定。
    テンプレート引数(class T,)・前方宣言(class Foo;)・
    実体化された型指定子(class Foo*)・enum class を除外する。"""
    start, end = m.start(), m.end()
    # 直前の語が enum なら enum class 扱い(型一覧からは除外)
    pre = src[max(0, start-8):start]
    if re.search(r"\benum\s+$", pre):
        return False
    # 名前(+ 継承句)の直後に来る最初の意味のある文字を見る
    i = m.end(2)  # 名前の直後
    n = len(src)
    # 継承句 : public/... があれば定義とみなす
    j = i
    while j < n and src[j] in " \t\r\n":
        j += 1
    if j < n and src[j] == ":":
        return True
    if j < n and src[j] == "{":
        return True
    return False


def parse_doc_and_members(src, doc, documented_members, lead_candidates):
    # 1) /** */ コメント → 宣言
    for m in re.finditer(r"/\*\*.*?\*/", src, re.S):
        brief, params, ret = parse_doc_comment(m.group(0))
        decl = next_decl(src, m.end())
        if not decl:
            continue
        if is_type_decl(decl):
            cm = CLASS_RE.search(decl)
            if cm and brief:
                lead_candidates.append((cm.group(2), cm.group(4), brief))
            continue
        if is_method(decl):
            extra = []
            for pn, pd in params:
                if pd:
                    extra.append(f"<i>{html.escape(pn)}</i> — {html.escape(pd)}")
            if ret:
                extra.append(f"<i>→</i> {html.escape(ret)}")
            doc["funcs"].append({"sig": decl, "desc": brief, "extra": extra})
        else:
            mm = re.search(r"([A-Za-z_]\w*)\s*(\[[^\]]*\])?\s*(=|;|$)", decl)
            name = mm.group(1) if mm else decl
            documented_members.add(name)
            doc["vars"].append({"name": name, "type": decl, "desc": brief})

    # 2) // 行コメント → class/struct
    for m in re.finditer(r"(?:^|\n)[ \t]*//[ \t]*(.+?)[ \t]*\n[ \t]*(?:class|struct)\s+([A-Za-z_]\w*)", src):
        cname, ctext = m.group(2), clean_ws(m.group(1))
        if ctext and not any(c[0] == cname for c in lead_candidates):
            base = None
            cm = CLASS_RE.search(src[m.start():m.start()+240])
            if cm:
                base = cm.group(4)
            lead_candidates.append((cname, base, ctext))

    # 3) 型一覧(継承関係)
    seen = set()
    for cm in CLASS_RE.finditer(src):
        if not valid_class(src, cm):
            continue
        name, base = cm.group(2), cm.group(4)
        if name in seen:
            continue
        seen.add(name)
        doc["classes"].append({"name": name, "base": base})

    # 4) enum
    for em in re.finditer(r"enum\s+(?:class\s+)?([A-Za-z_]\w*)", src):
        if em.group(1) not in doc["enums"]:
            doc["enums"].append(em.group(1))

    # 5) 未ドキュメントの private メンバ(_ 終わり規約)
    for ln in src.splitlines():
        mm = MEMBER_RE.match(ln)
        if not mm:
            continue
        name = mm.group(2)
        if name in documented_members:
            continue
        typ = clean_ws(mm.group(1))
        arr = mm.group(3) or ""
        if any(k in typ for k in ("return", "using", "typedef", "#", "//")) or typ == "":
            continue
        documented_members.add(name)
        doc["vars"].append({"name": name + arr, "type": clean_ws(ln.strip().rstrip(";")), "desc": ""})


CPPDEF_RE = re.compile(
    r"(?:^|\n)[ \t]*([A-Za-z_][\w:<>,\*&\s]*?)\b([A-Za-z_]\w*)::(~?[A-Za-z_]\w*|operator\s*\S+)\s*\(([^;{]*?)\)",
    re.S)
CTORDTOR_RE = re.compile(r"(?:^|\n)[ \t]*([A-Za-z_]\w*)::(~?[A-Za-z_]\w*)\s*\(([^;{]*?)\)")
FREEFN_RE = re.compile(r"(?:^|\n)[ \t]*((?:[A-Za-z_][\w:<>\*&]*\s+)+WINAPI\s+\w+|int\s+main)\s*\(([^;{]*?)\)\s*\{")


def preceding_comment(src, pos):
    """定義の直前にある // か /** */ コメントの brief を返す。"""
    head = src[:pos]
    m = re.search(r"/\*\*(.*?)\*/\s*$", head, re.S)
    if m:
        b, _, _ = parse_doc_comment("/**" + m.group(1) + "*/")
        return b
    lines = head.rstrip("\n").splitlines()
    if lines and lines[-1].strip().startswith("//"):
        return clean_ws(lines[-1].strip().lstrip("/ "))
    return ""


def parse_cpp_defs(src, doc):
    seen = set()
    # コンストラクタ/デストラクタ(戻り値なし、行頭が Class::)
    for m in CTORDTOR_RE.finditer(src):
        cls, meth, params = m.group(1), m.group(2), clean_ws(m.group(3))
        # メンバ名と同名(ctor) か ~名(dtor) のみ対象
        if meth != cls and meth != "~" + cls:
            continue
        key = f"{cls}::{meth}({params})"
        if key in seen:
            continue
        seen.add(key)
        doc["funcs"].append({"sig": key, "desc": preceding_comment(src, m.start()), "extra": []})
    for m in CPPDEF_RE.finditer(src):
        ret, cls, meth, params = m.group(1), m.group(2), m.group(3), m.group(4)
        ret = clean_ws(ret)
        # コンストラクタ/デストラクタは戻り値なし
        sig = f"{cls}::{clean_ws(meth)}({clean_ws(params)})"
        if ret and ret not in ("", "return") and "=" not in ret:
            sig = f"{ret} {sig}"
        key = f"{cls}::{meth}({clean_ws(params)})"
        if key in seen:
            continue
        seen.add(key)
        doc["funcs"].append({"sig": sig, "desc": preceding_comment(src, m.start()), "extra": []})
    for m in FREEFN_RE.finditer(src):
        sig = f"{clean_ws(m.group(1))}({clean_ws(m.group(2))})"
        doc["funcs"].append({"sig": sig, "desc": preceding_comment(src, m.start()), "extra": []})


def parse_file(path):
    src = decode(path)
    ext = os.path.splitext(path)[1].lower()
    base_stem = os.path.splitext(os.path.basename(path))[0]
    doc = {"funcs": [], "vars": [], "classes": [], "lead": "", "enums": []}
    documented_members = set()
    lead_candidates = []

    parse_doc_and_members(src, doc, documented_members, lead_candidates)

    is_cpp = ext in (".cpp", ".cc")
    if is_cpp and not doc["funcs"]:
        parse_cpp_defs(src, doc)

    # lead(ファイル/クラスの説明)を決める ── ファイル名に一致するクラスを優先
    fnorm = norm(base_stem)
    chosen = None
    for cand in lead_candidates:
        if norm(cand[0]) == fnorm:
            chosen = cand
            break
    if chosen is None and lead_candidates:
        chosen = lead_candidates[-1] if is_cpp else lead_candidates[0]

    if chosen:
        _, base, brief = chosen
        b = brief.rstrip("/ ")
        gm = GENERIC_BRIEF.match(b)
        if gm:
            b = f"{gm.group(1)} に関するデータと処理をまとめたクラス。"
        if base:
            b += f"（<code>{html.escape(base)}</code> を継承）"
        doc["lead"] = b
    elif not is_cpp and doc["classes"]:
        # コメントは無いが型定義はある → クラス名から最低限の lead を作る
        prim = None
        for c in doc["classes"]:
            if norm(c["name"]) == fnorm:
                prim = c
                break
        prim = prim or doc["classes"][0]
        b = f"<code>{html.escape(prim['name'])}</code> を定義するファイル。"
        if prim["base"]:
            b += f"（<code>{html.escape(prim['base'])}</code> を継承）"
        doc["lead"] = b
    elif is_cpp:
        inc = re.search(r'#include\s+"([^"]+\.h)"', src)
        if inc and norm(os.path.splitext(os.path.basename(inc.group(1)))[0]) == fnorm:
            doc["lead"] = f"<code>{html.escape(os.path.basename(inc.group(1)))}</code> で宣言された型の実装ファイル。"
        else:
            doc["lead"] = "実装ファイル。対応するヘッダで宣言された関数の中身を定義する。"

    # 最終フォールバック(定数・型・マクロ定義ヘッダやシェーダー)
    if not doc["lead"]:
        low = base_stem.lower()
        if ext == ".hlsl":
            if ".vs" in low:
                doc["lead"] = "頂点シェーダー(HLSL)。頂点をクリップ空間へ変換し、後段へデータを渡す。"
            elif ".ps" in low:
                doc["lead"] = "ピクセルシェーダー(HLSL)。ピクセルごとの最終カラーを計算する。"
            else:
                doc["lead"] = "HLSL シェーダー。"
        elif ext in (".h", ".hpp"):
            if re.search(r"\b(constexpr|#define|using|typedef|enum)\b", src):
                doc["lead"] = "定数・型エイリアス・マクロ・共通インクルードなどをまとめたヘッダ。"

    return doc


def rel_layer(relpath):
    parts = relpath.replace("\\", "/").split("/")
    if parts[0] == "Shaders":
        return "shader"
    if parts[0] == "Src" and len(parts) > 1:
        return LAYER_BY_TOP.get(parts[1], "engine")
    return "engine"


def build():
    files = []
    for base in SCAN_DIRS:
        root = os.path.join(ROOT, base)
        for dirpath, dirnames, filenames in os.walk(root):
            dirnames[:] = [d for d in dirnames if d not in EXCLUDE_DIRS]
            for fn in filenames:
                ext = os.path.splitext(fn)[1].lower()
                if ext not in CODE_EXT:
                    continue
                full = os.path.join(dirpath, fn)
                rel = os.path.relpath(full, ROOT).replace("\\", "/")
                files.append(rel)
    files.sort()

    docs = {}
    tree_paths = []
    for rel in files:
        pid = rel  # パスIDはリポジトリ相対パスそのもの
        layer = rel_layer(rel)
        d = parse_file(os.path.join(ROOT, rel))
        d["title"] = os.path.basename(rel)
        d["layer"] = layer
        d["kicker"] = rel
        docs[pid] = d
        tree_paths.append((rel, pid, layer))

    # ツリー構築(ネスト辞書)
    tree = {}
    for rel, pid, layer in tree_paths:
        parts = rel.split("/")
        node = tree
        for i, part in enumerate(parts):
            leaf = (i == len(parts) - 1)
            if leaf:
                node.setdefault("__files__", []).append((part, pid, layer))
            else:
                node = node.setdefault(part, {})

    def to_nodes(d, dirpath):
        out = []
        for k in sorted([k for k in d.keys() if k != "__files__"]):
            child_dir = (dirpath + "/" + k) if dirpath else k
            node = {"f": k, "folder": True,
                    "layer": rel_layer(child_dir + "/x") if child_dir.startswith(("Src", "Shaders")) else None,
                    "overview": LAYER_OVERVIEW.get(k, ""),
                    "children": to_nodes(d[k], child_dir)}
            out.append(node)
        for (fname, pid, layer) in sorted(d.get("__files__", [])):
            out.append({"f": fname, "p": pid, "layer": layer})
        return out

    tree_nodes = to_nodes(tree, "")

    data = {
        "layers": {k: {"name": v[0], "color": v[1]} for k, v in LAYER_META.items()},
        "tree": tree_nodes,
        "docs": docs,
        "count": len(files),
    }

    tpl_path = os.path.join(ROOT, "Docs", "_template.html")
    with open(tpl_path, "r", encoding="utf-8") as f:
        template = f.read()
    htmlout = template.replace("/*__DATA__*/", json.dumps(data, ensure_ascii=False))
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    with open(OUT, "w", encoding="utf-8") as f:
        f.write(htmlout)
    print(f"generated {OUT}  ({len(files)} files)")


if __name__ == "__main__":
    build()
