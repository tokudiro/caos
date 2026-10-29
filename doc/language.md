# caos language reference

English | [日本語](language.ja.md)

caos adds a few directives to C. Everything that is not a directive is plain C and is copied to the output as it is.
This page lists where each directive may be written and what it generates, and describes the conventions for memory and errors.
The classes of caos itself (`src/*.caos`) are used as examples.

## Where things can be written

Three places exist: **outside a class**, **inside a class** (between `#class` and `#endclass`) and **inside a method body**.

| place | what can be written |
|---|---|
| outside a class | plain C (copied to the `.c`), `#import`, `#header` / `#endheader`, `#'` comment. `$Name` becomes `Name*`. `@name` is an error. |
| inside a class | member declarations, `#@` members, method directives (`#+` `#-` `#()+` `#$`). `$Name` becomes `Name*`. A nested class, or a method directive outside a class, is an error. |
| inside a method body | C code. `@name` and `@@` are available. |

| syntax | meaning |
|---|---|
| `#interface $Name` ... `#endinterface` | an interface (see "Interfaces") |
| `#import <x.h>` | `#include` written to the public header and to the define header (not to the define header under `-d`) |
| `#header` ... `#endheader` | the lines between them (each directive alone on its line) go to the **public header** instead of the `.c`. Use it for types that a public prototype needs. |
| `#' text` (`#comment`) | a caos-only comment; the line is not output |
| `$Name` | the type `Name*` |
| `@name` | in a method: a member of the current object (`pthis->name`), or a method of it (`Class_name(pthis, ...)`) |
| `@@` | in a method: the this pointer itself (`pthis`, or the `-k` string) |
| `#@ type name;` | public member: declared like a member, and `Class_get_name` / `Class_set_name` are generated |

A member is a C declaration written directly in the class. It is placed in the struct, not in the `.c`.
A read-only member is not supported: `#@` always makes a getter and a setter. A `const` member does not help, because it cannot be set after the object is made; write a method that returns the value, and keep the member private.
A member initializer (`int a = 1;`) is reported as an error, because the struct cannot hold one.
Comments (`/* */`, `//`) may be written anywhere; `-C` rewrites `//` into `/* */`.

## Methods

| directive | generated function | written to |
|---|---|---|
| `#+` / `#public` | `R Class_name(Class* pthis, ...)` | `.c`, public header |
| `#()+` / `#()public` | same, but `const Class* pthis` (does not change the object) | `.c`, public header |
| `#-` / `#private` | `static R Class_name(Class* pthis, ...)` | `.c`, private header (`-p`: not written) |
| `#$` / `#classmethod` | `R Class_name(...)`, no this pointer | `.c`, public header |
| `##` | not supported: there is no inheritance, so protected has no meaning. It is reported as an error. Use `#-` (private) or `#+` (public). | |

A method body starts at `{` and ends at the matching `}`.

## Kinds of class

| directive | struct | extra output |
|---|---|---|
| `#class $Name` ... `#endclass` | `struct Name_struct`, in `Name_define.h`. The public header only has an opaque `typedef`. | |
| `#struct $Name` ... `#endstruct` | `struct Name`, appended to the **public header** (the define header is not left behind). The users can see the members. | |
| `#singleton $Name` ... `#endsingleton` | like `#class` | a static instance and `Name* Name_getInstance()` |
| `#utility $Name` ... `#endutility` | like `#class` | none. For classes made of `#$` methods only. |

`#endclass` and the other `#end...` directives are interchangeable; they only close the class.
Use one class kind per file: the kind that was seen last decides how the files are assembled.

## Interfaces

An interface lets a caller use an object without knowing its class (swap a driver, or a mock in a test).
It is a table of function pointers; nothing is added to the classes, and a class that implements no interface is not changed.

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

| written | generated |
|---|---|
| `#interface $Reader` ... `#endinterface` | `Reader.h`: `struct Reader_vtable` (one function pointer per method), `Reader` (a pair of the table and the object pointer), and `Reader_read(Reader r, ...)` for each method. `Reader.c`: the bodies of these (`r.vt->read(r.self, ...)`). |
| `#class $File implements $Reader` | `File.c`: an adapter per method (the only cast), a `static const` table, and `Reader File_as_Reader(File* p)`, which is declared in `File.h`. |

The caller knows only `Reader`:

```c
int total(Reader r) { return Reader_read(r, buf, 4); }
...
total(File_as_Reader(&file));   /* in a test: total(Mock_as_Reader(&mock)); */
```

* An interface declares methods only (`type name(args);`), and `$Name` is allowed in the types. The parameter names `r` and `self` are reserved. Function pointer and array parameters are not supported.
* The interface is read from the same file (written before the class), or from `Name.caos` in the folder of the input file. The file must be named after the interface. Link `Name.c`, too.
* Several interfaces: `implements $A, $B` gives `File_as_A` and `File_as_B`.
* The methods must be public (`#+` or `#()+`) with the same name, return type and parameter types (compared as text, without the parameter names). A missing method or a different signature is an error: `File.caos:1: error: class File does not implement Reader::reset`.
* `-M` lists `Name.caos` and `Name.h` as dependencies of the class.
* The cost is one indirect call per call through `Reader`. A call of `File_read` itself is a plain function call, as before.

## Generated files

`caos Name.caos` writes the following (the names follow the input file, not the class).

| file | content |
|---|---|
| `Name.c` | the method bodies, plus the plain C of the source |
| `Name.h` | the public header: typedef, prototypes of public methods, `#header` text |
| `Name_define.h` | the struct body (`-d`: not written; `#struct`: merged into `Name.h`) |
| `Name_private.h` | prototypes of private methods (`-p`: not written) |

Keep the define header out of the users' include path if the members should stay hidden.

## Memory

The rule is in the design policy: **the generated code never calls `malloc`** and nothing is allocated or freed behind your back.
There are no constructors or destructors. If a class needs the heap, you write that yourself with class methods.

### Static allocation (the default)

Allocate the object where it is used and initialize it with an `init` method. `caos` itself does so in `main.c`:

```c
SBuf thispointer_buf_impl;
thispointer_buf = &thispointer_buf_impl;
SBuf_init(thispointer_buf);
```

`init` sets the members to a known state; it does not allocate. This also works on systems without a heap.

### Heap allocation (written by you)

Provide a class method for creation and a method for destruction. `src/Element.caos` is the example:

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

* `new` is a class method (`#$`) that returns the memory. It does not initialize. `delete` frees the object itself.
* `init` and `finish` are a pair. `init` sets the members up, including memory the object owns; `finish` releases what `init` took. They are separate from `new` / `delete` so that a statically allocated object can use them too.
* The names `new` and `delete` are only a convention; caos does not treat them specially.

### Ownership

* Whoever calls `new` owns the object and must call `finish` (if `init` was called) and then `delete`, exactly once.
* An object that owns memory (like `Element.string`) is the only one that frees it, in `finish`.
* Passing a pointer to a method lends it. A method does not keep or free a borrowed pointer unless its name says so (for example `ListBuf_append`, which takes the element over).
* `ListBuf` (`src/ListBuf.caos`) shows the pattern: it holds `Element` pointers made by `Element_new`, and releases each in its own `finish` with `Element_finish` followed by `Element_delete`.
* A singleton lives for the whole run; nobody frees it.

## Errors

caos does not define an error mechanism; it is C. Follow the usual C conventions and write them in the method:

* A method that can fail returns a status. Pick one scheme per project and keep to it: `0` for success and a negative value for an error, or a `boolean`-style `TRUE` / `FALSE`.
* A result goes out through a pointer argument, so that the return value stays free for the status.
* A method that creates something returns `NULL` when it cannot (`new` above returns what `malloc` returns).

```c
#+int setBuf(const char* top, int len)
{
    if (top == NULL) { return -1; }
    ...
    return 0;
}
```

The caller checks the return value; nothing is thrown and nothing is checked automatically.

Errors in the caos source itself (unclosed class, `@` outside a class, member initializer, ...) are printed as `file:line: error: ...` and caos exits non-zero.
