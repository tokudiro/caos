# caos
[English](README.md) | 日本語

C言語にオブジェクト指向の記述を追加するソースコード

## 概要
C言語向けのオブジェクト指向ソーストランスパイラです。
作者が最低限必要とする機能だけを実装しています。

## 設計方針
Cコンパイラしかない小さなシステムで使うことを目指しています。
以下の方針は、**生成されるCコード**に対するものです。変換器本体はPC上で動くので、この制約は受けません。

* 読めるコードを出す: 生成するCは、手で書いたCのように読めること。名前は素直な形（`Class_method`）にし、`#line` で元の `.caos` に戻れるようにし、見えない補助コードを足さない。
* 隠れたランタイムを持たない: 生成コードは、ランタイムライブラリも libc も必要としない。
* 動的確保をしない: 生成コードは `malloc` を呼ばない。オブジェクトは静的に確保できる。
* 暗黙の生成をしない: ソースに書いていないものは生成しない（暗黙のインスタンス化や、自動のコンストラクタ・デストラクタを作らない）。
* 機能は最小限: 機能は必要になったときだけ足す。上の方針を満たさないものは入れない。

---
## ビルド
ビルド環境 : gcc + flex

``` make ```

## セルフホスト
caos 自身のクラス（`Caos`、`SBuf`、`RBuf`、`ListBuf`、`Element`、`SLib`）は `src/*.caos` に書かれ、caos が生成します。
生成された `.c` / `.h` はコミットしてあるので、caos がなくても caos をビルドできます。
`.caos` を編集したら `make regen` を実行してください。`make test` で、コミット済みのファイルと caos の生成結果が一致するかを確認します。
字句解析部（`src/*.l`、`src/prolog.c`）と `main.c` は C / flex で書かれています。
[クラス図](doc/class_diagram.md)もあります。
文法、メモリ管理の約束事、エラーの扱いは[言語リファレンス](doc/language.ja.md)にあります。

## サンプルコード

* SampleClass.caos

```c
#include <stdio.h>
#class $SampleClass

#@ int publicMember;
int privateMember;

#+int publicMethod(void){
    @privateMethod();
    return @privateMember;
}

#-void privateMethod(){
    @privateMember = 2 + @publicMember;
    return;
}

#endclass

SampleClass obj_impl;

int main(){
    $SampleClass obj = &obj_impl;
    SampleClass_set_publicMember(obj, 1);
    printf("%d\n", SampleClass_get_publicMember(obj) );
    printf("%d\n", SampleClass_publicMethod(obj) );
}
```

## テスト
``` make test ```

`test/*.caos` を順に変換し、`test/expected/` のスナップショットと比較したうえで、gcc でコンパイルして実行します。
出力を意図して変更した場合は、`UPDATE=1 sh test/test.sh` でスナップショットを作り直し、`git diff` で内容を確認してください。

## ディレクティブ
| ディレクティブ | 意味 |
|---|---|
| `#class $Name` ... `#endclass` | クラスの定義（`#struct`、`#singleton`、`#utility` は派生形） |
| `#+` / `#public` | public メソッド |
| `#-` / `#private` | private メソッド |
| `##` | protected メソッド |
| `#$` / `#classmethod` | クラスメソッド（this ポインタなし） |
| `#@` | public メンバー（getter / setter を生成） |
| `#import` | インクルード |
| `@name` | 現在のオブジェクトのメンバーまたはメソッド（`pthis->name` / `Class_name(pthis)`） |

## オプション
| オプション | 意味 |
|---|---|
| `-h` | ヘルプ |
| `-V` | 詳細表示 |
| `-L` | 行番号を出力しない |
| `-C` | `//` コメントを `/* */` に置き換える |
| `-k <文字列>` | this ポインタの名前を変更（既定は `pthis`） |
| `-p` | private ヘッダを出力しない |
| `-d` | define ヘッダを出力しない |
| `-o <パス>` | 出力先フォルダ（既定は入力ファイルと同じフォルダ） |
| `-M <ファイル>` | make の依存関係ファイルを書く（後述） |

## ビルドとの連携
`-M <file>` は、`gcc -MF` のように、make 形式の依存関係を書きます。

```
A.c A.h A_private.h A_define.h : A.caos B.h
```

* 左辺は出力するファイルです。`-p`、`-d`、`-o` を反映します（`#struct` は define ヘッダを作らないので載りません）。
* 右辺は入力と、`#import $Name` または `#import "x.h"` で参照するヘッダです（生成される `#include` と同じく、出力先から探します）。`#import <x.h>` は載せません。

利用者のプロジェクトの Makefile のひな形です。

```make
CAOS    ?= caos
CFLAGS  += -MMD
CLASSES := $(basename $(wildcard *.caos))
OBJS    := main.o $(addsuffix .o,$(CLASSES))

app : $(OBJS)
	$(CC) -o $@ $^

# クラスは X.c、X.h、X_define.h、X_private.h にまとめて変換される。
# -M で X.cd を書く（生成物は X.caos と #import したヘッダに依存する）。
# （X.d は使わない。X.o の gcc -MMD ファイルと衝突する）
%.c : %.caos
	$(CAOS) -L -M $*.cd $<

# ヘッダが揃うよう、コンパイルの前に全クラスを変換する
$(OBJS) : | $(addsuffix .c,$(CLASSES))

-include $(addsuffix .d,$(basename $(OBJS))) $(addsuffix .cd,$(CLASSES))
```

`caos` 自身の `Makefile` は、クラスの一覧を `src/*.caos` から作ります（`make regen`）。

## ファイルの生成
```caos -L SampleClass.caos```

## 出力例
* SampleClass.c

```c
#include <stdio.h>

#include "SampleClass_define.h"
#include "SampleClass_private.h"

int SampleClass_get_publicMember(const SampleClass* pthis) { return pthis->publicMember; }
void SampleClass_set_publicMember(SampleClass* pthis, int publicMember) { pthis->publicMember = publicMember; }

int SampleClass_publicMethod(SampleClass* pthis){
    SampleClass_privateMethod(pthis);
    return pthis->privateMember;
}

void SampleClass_privateMethod(SampleClass* pthis){
    pthis->privateMember = 2 + pthis->publicMember;
    return;
}

SampleClass obj_impl;

int main(){
    SampleClass* obj = &obj_impl;
    SampleClass_set_publicMember(obj, 1);
    printf("%d\n", SampleClass_get_publicMember(obj) );
    printf("%d\n", SampleClass_publicMethod(obj) );
}
```

* SampleClass.h

```c
#ifndef __SAMPLECLASS_H__
#define __SAMPLECLASS_H__
typedef struct SampleClass_struct SampleClass;
int SampleClass_get_publicMember(const SampleClass* pthis);
void SampleClass_set_publicMember(SampleClass* pthis, int publicMember);
int SampleClass_publicMethod(SampleClass* pthis);
#endif /* __SAMPLECLASS_H__ */
```

* SampleClass_define.h

```c
#include "SampleClass.h"
struct SampleClass_struct{
int publicMember;
int privateMember;
};
```

* SampleClass_private.h

```c
static void SampleClass_privateMethod(SampleClass* pthis);
```
