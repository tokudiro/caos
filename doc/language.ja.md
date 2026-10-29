# caos 言語リファレンス

[English](language.md) | 日本語

caos は C にいくつかのディレクティブを足します。ディレクティブ以外は素の C で、そのまま出力にコピーされます。
このページでは、ディレクティブをどこに書けるか、何が生成されるかを表にし、メモリとエラーの扱いの約束事を示します。
例には caos 自身のクラス（`src/*.caos`）を使います。

## 書ける場所

場所は 3 つです。**クラスの外**、**クラスの中**（`#class` から `#endclass` まで）、**メソッド本体の中**。

| 場所 | 書けるもの |
|---|---|
| クラスの外 | 素の C（`.c` にコピー）、`#import`、`#header` / `#endheader`、`#'` コメント。`$Name` は `Name*` になる。`@name` はエラー。 |
| クラスの中 | メンバ宣言、`#@` メンバ、メソッドのディレクティブ（`#+` `#-` `#()+` `#$`）。`$Name` は `Name*` になる。クラスの入れ子や、クラス外のメソッドディレクティブはエラー。 |
| メソッド本体の中 | C のコード。`@name` と `@@` が使える。 |

| 記法 | 意味 |
|---|---|
| `#import <x.h>` | `#include` を public ヘッダと define ヘッダに書く（`-d` のときは define ヘッダには書かない） |
| `#header` ... `#endheader` | 間の行を `.c` ではなく **public ヘッダ**に出す（ディレクティブは行に単独で書く）。public のプロトタイプが使う型に使う。 |
| `#' text`（`#comment`） | caos だけのコメント。その行は出力されない |
| `$Name` | 型 `Name*` |
| `@name` | メソッド内で、現在のオブジェクトのメンバ（`pthis->name`）またはメソッド（`Class_name(pthis, ...)`） |
| `@@` | メソッド内で、this ポインタそのもの（`pthis`、または `-k` の文字列） |
| `#@ type name;` | public メンバ。メンバとして宣言され、`Class_get_name` / `Class_set_name` が生成される |

メンバは、クラスの中に直接書いた C の宣言です。`.c` ではなく struct に入ります。
メンバの初期化子（`int a = 1;`）は struct に持てないのでエラーになります。
コメント（`/* */`、`//`）はどこにでも書けます。`-C` は `//` を `/* */` に置き換えます。

## メソッド

| ディレクティブ | 生成される関数 | 出力先 |
|---|---|---|
| `#+` / `#public` | `R Class_name(Class* pthis, ...)` | `.c`、public ヘッダ |
| `#()+` / `#()public` | 同上。ただし `const Class* pthis`（オブジェクトを変更しない） | `.c`、public ヘッダ |
| `#-` / `#private` | `static R Class_name(Class* pthis, ...)` | `.c`、private ヘッダ（`-p`: 出力しない） |
| `#$` / `#classmethod` | `R Class_name(...)`、this ポインタなし | `.c`、public ヘッダ |
| `##` | protected メソッド用に予約（#8）。未実装で、this のない素の C 関数としてそのまま出力される。 | |

メソッド本体は `{` から対応する `}` までです。

## クラスの種類

| ディレクティブ | struct | 追加の出力 |
|---|---|---|
| `#class $Name` ... `#endclass` | `struct Name_struct`。`Name_define.h` に出る。public ヘッダには不透明な `typedef` だけ。 | |
| `#struct $Name` ... `#endstruct` | `struct Name`。**public ヘッダ**の末尾に出る（define ヘッダは残らない）。利用者からメンバが見える。 | |
| `#singleton $Name` ... `#endsingleton` | `#class` と同じ | static なインスタンスと `Name* Name_getInstance()` |
| `#utility $Name` ... `#endutility` | `#class` と同じ | なし。`#$` メソッドだけのクラス向け。 |

`#endclass` などの `#end...` は互換で、クラスを閉じる働きしかありません。
1 つのファイルには 1 種類のクラスだけを書いてください。最後に見つけた種類で、ファイルの組み立て方が決まります。

## 生成されるファイル

`caos Name.caos` は次を書きます（名前は入力ファイル名で、クラス名ではありません）。

| ファイル | 内容 |
|---|---|
| `Name.c` | メソッド本体と、ソースの素の C |
| `Name.h` | public ヘッダ。typedef、public メソッドのプロトタイプ、`#header` の内容 |
| `Name_define.h` | struct 本体（`-d`: 出力しない、`#struct`: `Name.h` に統合） |
| `Name_private.h` | private メソッドのプロトタイプ（`-p`: 出力しない） |

メンバを隠したいときは、define ヘッダを利用者の include パスに置かないようにします。

## メモリ

方針は設計方針のとおりです。**生成コードは `malloc` を呼びません**し、見えないところで確保や解放をしません。
コンストラクタ・デストラクタはありません。クラスがヒープを必要とするなら、クラスメソッドで自分で書きます。

### 静的確保（基本）

使う場所でオブジェクトを確保し、`init` メソッドで初期化します。caos 自身も `main.c` でそうしています。

```c
SBuf thispointer_buf_impl;
thispointer_buf = &thispointer_buf_impl;
SBuf_init(thispointer_buf);
```

`init` はメンバを既知の状態にするだけで、確保はしません。ヒープのないシステムでも使えます。

### ヒープ確保（自分で書く）

生成用のクラスメソッドと、破棄用のメソッドを用意します。`src/Element.caos` が例です。

```c
#struct $Element
char* string;
int length;
int type;

#$Element* new()
{
    return (Element*)malloc( sizeof(Element) );
}

#+void delete()
{
    if(@@) { free(pthis); }
}

#+void init(const char* string, int length, int type) { ... }
#+void finish() { if(@string) { free(@string); } }
#endstruct
```

* `new` はメモリを返すクラスメソッド（`#$`）です。初期化はしません。`delete` はオブジェクト自身を解放します。
* `init` と `finish` は対です。`init` は、オブジェクトが所有するメモリを含めてメンバを準備し、`finish` は `init` が確保したものを解放します。`new` / `delete` と分けてあるので、静的に確保したオブジェクトでも使えます。
* `new` と `delete` は慣習の名前で、caos が特別扱いするわけではありません。

### 所有権

* `new` を呼んだ側がオブジェクトを所有し、（`init` を呼んだなら）`finish`、続けて `delete` をちょうど 1 回呼びます。
* メモリを持つオブジェクト（`Element.string` など）は、自分の `finish` でだけ解放します。
* メソッドにポインタを渡すのは貸すことです。名前で明示しない限り（例: 要素を引き取る `ListBuf_append`）、借りたポインタを保持も解放もしません。
* `ListBuf`（`src/ListBuf.caos`）がその例です。`Element_new` で作った `Element` のポインタを持ち、自分の `finish` で `Element_finish` に続けて `Element_delete` で解放します。
* シングルトンは実行中ずっと生きていて、誰も解放しません。

## エラー

caos にエラーの仕組みはありません。C のままです。普通の C の作法で、メソッドに書きます。

* 失敗しうるメソッドは状態を返す。プロジェクトで 1 つに決めて守る。成功を `0`、エラーを負の値にするか、`boolean` 風に `TRUE` / `FALSE` にする。
* 結果はポインタ引数で返し、戻り値は状態のために空けておく。
* 何かを作るメソッドは、作れなければ `NULL` を返す（上の `new` は `malloc` の戻り値をそのまま返す）。

```c
#+int setBuf(const char* top, int len)
{
    if (top == NULL) { return -1; }
    ...
    return 0;
}
```

戻り値の確認は呼び出し側の仕事です。例外は投げられず、自動の検査もありません。

caos のソース自体の誤り（クラスの閉じ忘れ、クラス外の `@`、メンバの初期化子など）は `file:line: error: ...` と表示され、caos は 0 以外で終了します。
