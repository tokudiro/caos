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
| `#require expr` / `#ensure expr` | メソッドの見出しと `{` の間に書く事前条件と事後条件（「契約」の節） |
| `#interface $Name` ... `#endinterface` | インタフェース（「インタフェース」の節） |
| `#class $Name<T>` ... / `#instance $X = $Name<int>` | テンプレートとその展開（「テンプレート」の節） |
| `#import <x.h>` | `#include` を public ヘッダと define ヘッダに書く（`-d` のときは define ヘッダには書かない） |
| `#header` ... `#endheader` | 間の行を `.c` ではなく **public ヘッダ**に出す（ディレクティブは行に単独で書く）。public のプロトタイプが使う型に使う。 |
| `#' text`（`#comment`） | caos だけのコメント。その行は出力されない |
| `$Name` | 型 `Name*` |
| `x->m()` / `v.m()` | メソッド内で、変数のクラスのメソッド呼び出し（「変数のメソッド呼び出し」の節） |
| `@name` | メソッド内で、現在のオブジェクトのメンバ（`pthis->name`）またはメソッド（`Class_name(pthis, ...)`） |
| `@@` | メソッド内で、this ポインタそのもの（`pthis`、または `-k` の文字列） |
| `#@ type name;` | public メンバ。メンバとして宣言され、`Class_get_name` / `Class_set_name` が生成される |

メンバは、クラスの中に直接書いた C の宣言です。`.c` ではなく struct に入ります。
読み取り専用のメンバは未対応です。`#@` は常に getter と setter を作ります。`const` のメンバは、オブジェクトを作った後で値を入れられないので代用になりません。値を返すメソッドを書き、メンバは private のままにしてください。
メンバの初期化子（`int a = 1;`）は struct に持てないのでエラーになります。
コメント（`/* */`、`//`）はどこにでも書けます。`-C` は `//` を `/* */` に置き換えます。

## メソッド

| ディレクティブ | 生成される関数 | 出力先 |
|---|---|---|
| `#+` / `#public` | `R Class_name(Class* pthis, ...)` | `.c`、public ヘッダ |
| `#()+` / `#()public` | 同上。ただし `const Class* pthis`（オブジェクトを変更しない） | `.c`、public ヘッダ |
| `#-` / `#private` | `static R Class_name(Class* pthis, ...)` | `.c`、private ヘッダ（`-p`: 出力しない） |
| `#$` / `#classmethod` | `R Class_name(...)`、this ポインタなし | `.c`、public ヘッダ |
| `##` | 未対応。継承がないので protected に意味がなく、エラーとして報告される。`#-`（private）か `#+`（public）を使う。 | |

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

## 変数のメソッド呼び出し

メソッドの中では、クラスの変数が C と同じ書き方でメソッドを呼べます。ポインタは `->`、値は `.` です。

| 宣言 | 書く | 生成 |
|---|---|---|
| `$Foo p` または `Foo* p` | `p->bar(a)` | `Foo_bar(p, a)` |
| `Foo v` | `v.bar(a)` | `Foo_bar(&v, a)` |
| `Reader r`（インタフェース） | `r.read(a)` | `Reader_read(r, a)` |
| メンバ `$Foo m` | `@m->bar(a)` | `Foo_bar(pthis->m, a)` |
| メンバ `Foo m` | `@m.bar(a)` | `Foo_bar(&pthis->m, a)` |

```c
#+int run($Counter other)
{
    Counter local;
    local.add(1);          /* Counter_add(&local, 1) */
    other->add(2);         /* Counter_add(other, 2)  */
    return local.get() + other->get();
}
```

* **対象の変数**: メソッドの中で宣言されたもの（引数と、文の最後にある `Class name`、`Class* name`、`$Class name` の形のローカル変数）と、クラスのメンバです。それ以外は追いません。`a->b->c()`、呼び出しの結果、配列の要素は対象外です。いったんローカル変数に取ってください（`T* e = @items[i]; e->finish();`）。
* **対象の型**: 同じファイルのクラス、`#import $Foo` の `Foo.caos` のクラス、インタフェースです。関数ポインタのメンバを持つ構造体など、それ以外の型の `s.fp(x)` は、そのままにします。
* **検査しないもの**: メソッドが存在するか（C コンパイラが報告します）と、ブロックのスコープです。別の型で宣言し直した名前は、後の宣言になります。
* 演算子の取り違え（ポインタに `.`、値に `->`）はエラーです。使えるのはクラスのメソッドの中だけで、ファイル内の普通の C 関数では使えません。別のファイルのクラスの値には、その struct が必要です（`#include "Foo_define.h"`）。

## インタフェース

インタフェースを使うと、呼び出し側は、オブジェクトのクラスを知らずに使えます（ドライバの差し替えや、テストでのモックの差し込み）。
実体は関数ポインタの表で、クラスには何も足しません。インタフェースを実装しないクラスは今までどおりです。

```
#interface $Reader              // Reader.caos
int read(char* buf, int len);
void reset();
#endinterface

#class $File implements $Reader // File.caos
int pos;
#+int read(char* buf, int len){ @pos += len; return len; }
#+void reset(){ @pos = 0; }
#endclass
```

| 書くもの | 生成されるもの |
|---|---|
| `#interface $Reader` ... `#endinterface` | `Reader.h`: `struct Reader_vtable`（メソッドごとの関数ポインタ）、`Reader`（表とオブジェクトのポインタの組）、メソッドごとの `Reader_read(Reader r, ...)`。`Reader.c`: その本体（`r.vt->read(r.self, ...)`）。 |
| `#class $File implements $Reader` | `File.c`: メソッドごとのアダプタ（唯一のキャスト）、`static const` の表、`Reader File_as_Reader(File* p)`（宣言は `File.h`）。 |

呼び出し側は `Reader` だけを知っています。

```c
int total(Reader r) { return Reader_read(r, buf, 4); }
...
total(File_as_Reader(&file));   /* テストでは total(Mock_as_Reader(&mock)); */
```

* インタフェースにはメソッドの宣言だけ（`type name(args);`）を書きます。型には `$Name` が使えます。引数名 `r` と `self` は予約されています。関数ポインタや配列の引数は未対応です。
* インタフェースは、同じファイル（クラスより前）か、入力ファイルと同じフォルダの `Name.caos` から読みます。ファイル名はインタフェース名にしてください。`Name.c` もリンクします。
* 複数のインタフェース: `implements $A, $B` で `File_as_A` と `File_as_B` ができます。
* メソッドは public（`#+` または `#()+`）で、名前、戻り値の型、引数の型が同じである必要があります（引数名は無視して、文字列で比較）。足りないメソッドや型の違いはエラーです: `File.caos:1: error: class File does not implement Reader::reset`。
* `-M` は、`Name.caos` と `Name.h` をクラスの依存として載せます。
* コストは、`Reader` 越しの呼び出しごとに間接呼び出し 1 回です。`File_read` を直接呼ぶ場合は、今までどおり普通の関数呼び出しです。

## テンプレート

クラスは型パラメータを取れます。テンプレート自体は何も生成せず、`#instance` を書いた場所で、与えた名前で展開されます。

```
#class $List<T>                       // List.caos
int length;
T* items;
#+int append(T item){ ... }
#()+T at(int index){ return @items[index]; }
#endclass

#instance $IntList = $List<int>       // Lists.caos
#instance $StrList = $List<const char*>
```

`IntList_append(IntList*, int)` や `StrList_at(...)` などが、クラスを 2 つ手で書いたのと同じように生成されます。
利用者には、普通のクラス名 `IntList` と `StrList` だけが見えます。

* **仕組み**: テンプレートのテキストを、型パラメータ（単語単位）を引数に、`$List` を `$IntList` に置き換えて、もう一度走査します。そのほかの規則（メンバ、`#line`、`implements`、オプション）は手書きのクラスと同じです。型推論や特殊化はありません。
* **展開される場所**: `#instance` を書いたファイルに生成されます（`Lists.c`、`Lists.h` など）。同じインスタンスを複数のファイルで使うときは、`#instance` を 1 か所に書き、そのヘッダを include します。
* **テンプレートの探し方**: 同じファイルの `#instance` より前か、入力ファイルと同じフォルダの `Name.caos`（テンプレート名のファイル）です。テンプレートのクラスの種類（`#class`、`#struct` など）は引き継がれます。`-M` は `Name.caos` を載せます。
* **`T_name`**: 型パラメータに `_` が続く単語（`T_finish`）は、型引数のメソッド（`Element_finish`）になります。型に依存する処理を呼ぶときに使います（`T*` のローカル変数に取って `e->finish()` と書くほうが簡単です）。型引数は単純な名前（`Element`。`const char*` は不可）でなければならず、その関数を持たない型は C コンパイラのエラーになります。`src/ListBuf.caos` は `List<T>` を `Element` で展開したもので、`T_finish` と `T_delete` で要素を解放します。
* **型引数**: `int`、`char*`、`Name`、`$Name`（= `Name*`）。複数は `,` で区切ります。`<...>` の入れ子は未対応です。ポインタはテンプレートの中で自分で書きます（`T*`）。`$T` は置き換えません。
* **引き継がれないもの**: `#class` から `#endclass` までの行だけがテンプレートです。必要な `#include` や `#header` は、`#instance` を書くファイルに置いてください。
* **エラー**: 引数の数の違いや、未定義のテンプレートなどは `file:line: error: ...` で報告されます。展開後のテキストのエラーは、テンプレートのファイルの、テンプレートの行を指します。単語の置き換えは、コメントや文字列の中でも行われます。
* `<T implements $Comparable>` のような制約は、まだありません。

## 生成されるファイル

`caos Name.caos` は次を書きます（名前は入力ファイル名で、クラス名ではありません）。

| ファイル | 内容 |
|---|---|
| `Name.c` | メソッド本体と、ソースの素の C |
| `Name.h` | public ヘッダ。typedef、public メソッドのプロトタイプ、`#header` の内容 |
| `Name_define.h` | struct 本体（`-d`: 出力しない、`#struct`: `Name.h` に統合） |
| `Name_private.h` | private メソッドのプロトタイプ（`-p`: 出力しない） |

メンバを隠したいときは、define ヘッダを利用者の include パスに置かないようにします。

## 契約

`#require` と `#ensure` は、メソッドに入るときと出るときの検査です。メソッドの見出しと `{` の間に、1 行に 1 つの式を C で書きます（`@member`、`x->method()`、引数が使えます）。

```
#+int pop()
#require @length > 0
#ensure result >= 0
{
    if (@length == 1) return @items[--@length];
    return @items[--@length] + 0;
}
```

は、次のようになります。

```c
int Stack_pop(Stack* pthis)
{ int result; CAOS_REQUIRE(pthis->length > 0);
    if (pthis->length == 1) { result = pthis->items[--pthis->length]; CAOS_ENSURE(result >= 0); return result; }
    { result = pthis->items[--pthis->length] + 0; CAOS_ENSURE(result >= 0); return result; }
}
```

* `#require expr` は最初に検査します。`#ensure expr` は、`return` の前ごとに検査します（値は `result` に取ります）。`void` のメソッドでは、終わりの前にも検査します。複数行は、順にすべて検査します。
* すべての種類のメソッド（`#+`、`#-`、`#()+`、`#$`）で使えます。`void` のメソッドに `result` はありません。
* **マクロ**: 生成される `.c` は `caos_contract.h`（caos の `include/` にあります。コピーするか、自分で書いてください）を include します。`CAOS_REQUIRE(cond)` と `CAOS_ENSURE(cond)` が先に定義されていなければ `assert(cond)` になるので、`NDEBUG` で検査が消えます。`assert.h` を使いたくないときは、ヘッダより前か自分の `caos_contract.h` で、2 つのマクロ（停止やログ出力など）を定義します。
* 式の誤りは C コンパイラのエラーになり、`{` の行で報告されます。
* `#invariant` は未対応です。ほかの場所の `#require` と `#ensure` はエラーです。

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
