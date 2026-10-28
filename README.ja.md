# caos
[English](README.md) | 日本語

C言語にオブジェクト指向の記述を追加するソースコード

## 概要
C言語向けのオブジェクト指向ソーストランスパイラです。
作者が最低限必要とする機能だけを実装しています。

---
## ビルド
ビルド環境 : gcc + flex

``` make ```

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
| `-o <パス>` | 出力先フォルダ（未実装、issue #2 を参照） |

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
