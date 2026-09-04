# Tantrums Language Specification (Draft)
> This is just a draft, documentation on what the lang may look like, syntax, its functioning etc. Things may change randomly so keep in mind that.

## 1. Philosophy

Tantrums is a systems programming language for programmers who want to see exactly what their code is doing and what it costs to do it. Every heap allocation is declared. Every failure point is marked. Every mutation is annotated. Every side effect is labeled.

A Tantrums function signature tells you whether it allocates, whether it can fail, whether it mutates global state, and whether it does I/O — before you read a single line of its body.

It compiles to native code with no runtime overhead, no garbage collector, and no hidden magic. It has the performance ceiling of C, the error handling ergonomics of Zig, and a syntax that any systems programmer can read within an hour.

**Core belief:** labeled complexity is manageable. Hidden complexity is a bug waiting to happen.

---

## 2. File Extension

`.tnt`

---

## 3. Syntax Overview

### 3.1 Comments

```tnt
// single line comment

/* 
   multi line comment
*/
```

### 3.2 Module Imports

#### Stdlib modules

```tnt
use !io;
use !string;
use !exitCodes;
use !sdl!->sdlcore;
alias sdlcore sdl;
```

- `!` prefix = stdlib/builtin module only
- `!->` = module/namespace access operator, visually distinct from `->` (method call — does NOT dereference)
- `alias` = rename a module for convenience in current file

#### User defined modules

```tnt
use "Maths";
```

- no `!` prefix — user modules never use `!`
- compiler looks for a `module.tnt` recursively in `src/modules/`(default) which contains the declaration of the module
- folder name can be anything, compiler matches against the one in module.tnt
- all `.tnt` files inside that folder (in which the requested module.tnt resides) that declare `impl module Maths;` belong to that module

#### Declaring a user module

every `.tnt` file that belongs to a module declares it at the top:

```tnt
impl module Maths;

expose void Add(int32* x, int32* y, int32* result)
{
    *result = *x + *y;
}

expose void Sub(int32* x, int32* y, int32* result)
{
    *result = *x - *y;
}
```

- `expose` = visible to whoever imports this module
- no `expose` = private to the module
- compiler auto-crawls the folder and finds all files with matching `impl module`

#### Project structure

```
myproject/
    tantrum.proj
    src/
        main.tnt
        modules/
            MyMod/
                module.tnt       // module Maths;
                userAdd.tnt      // impl module Maths;
                userSub.tnt      // impl module Maths;
```

#### tantrum.proj

```
name: "myapp"
version: "0.1.0"
entry: "src/main.tnt"
modulesDir: "src/modules"
```

- `modulesDir` defaults to `src/modules` — overridable

#### Remote imports

```tnt
use "github.com/user/lib@1.2.0";
```

- version pinning is mandatory
- circular imports are a compile error with a full trace shown

#### Calling a user module

```tnt
use "Maths";

void main(auto argc, auto argv)
{
    heap int32* x = 5;
    heap int32* y = 5;
    heap int32* result = 0;
    Maths!->Add(x, y, result);
}
```

#### Compilation passes

1. lex and parse every file in the project
2. resolve all `impl module` declarations, build module map
3. resolve all `use` statements against module map
4. type checking and annotation enforcement
5. codegen

### 3.3 Operators

| Operator | Meaning |
|----------|---------|
| `->` | method call |
| `!->` | module/namespace access |
| `<-->` | method chain operator |
| `<~~` | lazy return operator |
| `??` | null coalesce |
| `*` | dereference prefix |

### 3.4 FFI

Tantrums can call external C functions via `extern`:

```tnt
extern int32 puts(string s);
extern void* malloc(uint64 size);
extern void free(void* ptr);
```

- `extern` tells the compiler this function exists in an external C library
- resolved by the linker at link time
- used internally by stdlib — Tantrums programmers rarely need this directly

---

## 4. Control Flow

### 4.1 Conditionals

```tnt
if (x > 0) {
    // ...
} else if (x == 0) {
    // ...
} else {
    // ...
}
```

### 4.2 Loops


```tnt
for i in range(0, 10) {
    // ...
}

for i in range(10) {
    // ...
}
```

Iterating over a collection:

```tnt
for item in myList {
    // ...
}
```

While loop:

```tnt
while (condition) {
    // ...
}
```

Loop control:

```tnt
break;      // exit loop
continue;   // skip to next iteration
```

---

## 5. Memory Model

### 5.1 Storage Classes

```tnt
heap int* x = ...;       // heap allocated — you own it, you free it
stack int y = 5;         // explicitly stack (usually implicit)
static int z = 0;        // static storage duration
```

- stack is the default when no storage class is specified
- `heap` always adds one layer of indirection — `makeContext()` returns `Context*`, so `heap Context** ctx = sdl!->makeContext();`
- the double pointer is intentional and visible — Tantrums is honest about memory layout
- manual memory management — you free what you allocate
- no GC, no borrow checker, trust the programmer

### 5.2 Memory Pools

```tnt
use !memory;

memory!->pool<1024> myPool;
pool heap int* x = myPool->alloc(int);
```

### 5.3 Aliasing

> Unique and shared, and the entire aliasing/optimisation concept is not finalized.

```tnt
unique heap int* x = ...;   // compiler guaranteed: no other pointer aliases this
shared heap int* y = ...;   // explicitly shared, treated conservatively
```

- `unique` unlocks aggressive compiler optimizations (vectorization, loop unrolling, register allocation)
- compiler enforces `unique` — creating an alias of a `unique` pointer is a compile error
- Tantrums code with `unique` pointers can outperform equivalent C code

---

## 6. Type System

### 6.1 Primitive Types

```tnt
int8    uint8
int16   uint16
int32   uint32
int64   uint64
int128  uint128

float32
float64

bool
byte        // alias for uint8, raw memory
char        // unicode codepoint, 4 bytes
```

- `int` is a convenience alias — `int32` on 32-bit targets, `int64` on 64-bit — fixed in the spec, not platform-ambiguous
- no implicit numeric conversion ever:

```tnt
int32 x = 5;
int64 y = int64(x);    // explicit cast required
```

### 6.2 Strings

`string` is a builtin type:
- UTF-8 encoded
- length-prefixed (not null terminated)
- immutable by default — use `mut string` for a mutable string
- heap allocated internally, managed automatically

```tnt
string s = "hello";
mut string b = "world";
b = b + "!";
int32 len = s->length();
char c = s->at(0);
```

### 6.3 Constants

```tnt
const int32 MAX_SIZE = 1024;
const string APP_NAME = "MyApp";
```

- evaluated at compile time
- cannot be reassigned ever
- `const` implies `pure` — no runtime cost

### 6.4 Auto Inference

```tnt
auto x = 5;         // inferred as int32, locked in at declaration
auto y = 5.0;       // inferred as float64
x = "hello";        // COMPILE ERROR — x is int32
```

- `auto` infers once at declaration, type never changes
- `auto` is valid in function parameters
- no cross-type silent coercion ever

### 6.5 Nullable Types

```tnt
int x = 5;          // cannot be null, compiler enforced
int? y = null;      // explicitly nullable
int? z = 10;        // nullable but has a value
```

- non-nullable by default
- compiler performs null flow analysis:

```tnt
int? val = getResult();
io!->print(val->toString());     // COMPILE ERROR — might be null

if (val != null) {
    io!->print(val->toString()); // fine — compiler knows non-null here
}

io!->print(val ?? "default");    // null coalesce
```

### 6.6 Mutation

```tnt
mut int x = 5;      // mutable
int y = 10;         // immutable by default
```

- immutable by default, `mut` opts in
- `mut` on a class method = modifies the instance, compiler enforced
- non-`mut` method = read-only on the instance, compiler enforced

### 6.7 Enums

```tnt
enum INIT_TYPE {
    EVERYTHING = 0,
    VIDEO = 1,
    AUDIO = 2
}
```

- enum values are constants — cannot be reassigned
- accessed via `!->`: `sdl!->INIT_TYPE!->EVERYTHING`
- underlying type defaults to `int32` unless specified:

```tnt
enum ErrorCode : uint8 {
    NONE = 0,
    GENERIC = 1,
    TIMEOUT = 2
}
```

### 6.8 Structs

Structs are pure data containers. No methods, no behavior, no inheritance.

```tnt
struct Vec2 {
    float32 x;
    float32 y;
}

struct Color {
    uint8 r;
    uint8 g;
    uint8 b;
    uint8 a;
}
```

- structs are value types — copied on assignment
- no methods allowed inside a struct
- can be used as fields inside classes

### 6.9 Classes

Classes are data + behavior. No inheritance (for now — may be revisited).

```tnt
class Window {
    int32 width;
    int32 height;
    string title;

    mut void setSize(int32 w, int32 h) {
        width = w;
        height = h;
    }

    mut void setTitle(string t) {
        title = t;
    }

    int32 getWidth() { return width; }
    int32 getHeight() { return height; }
}
```

- `mut` on a method = modifies the class instance, compiler enforced
- non-`mut` method = read-only on the instance, compiler enforced
- no `impl` blocks — methods defined directly inside class body
- no inheritance between classes — composition over inheritance
- classes are reference types when heap allocated

### 6.10 Generics

```tnt
class List<T> {
    heap T* data;
    int32 length;
    int32 capacity;

    mut void push(T item) { ... }
    T pop() { ... }
    T get(int32 index) { ... }
}
```

Constrained generics via interfaces:

```tnt
T max<T: Comparable<T>>(T a, T b) {
    return a->compareTo(b) > 0 ? a : b;
}
```

- no higher-kinded types, no associated types — kept intentionally simple

### 6.11 Interfaces

```tnt
interface Drawable {
    mut void draw(Renderer* r);
    int32 getWidth();
    int32 getHeight();
}

class Sprite {
    mut void draw(Renderer* r) { ... }
    int32 getWidth() { ... }
    int32 getHeight() { ... }
    // implicitly implements Drawable — no declaration needed
}
```

- structural/implicit implementation — if the class has the methods, it satisfies the interface
- no boilerplate `implements` declarations

---

## 7. Call Cost Annotations

This is Tantrums' most original feature. Function signatures declare their full cost profile — the compiler enforces every annotation.

```tnt
pure int add(int a, int b) { return a + b; }
// pure = no side effects, no allocation, no I/O, no global state
// compiler verified — violation is a compile error

heap Window** makeWindow() { ... }
// heap = this function performs heap allocation

throws void riskyOperation() { ... }
// throws = this function can fail and propagate an error

io void readFile(string path) { ... }
// io = does filesystem/network I/O, can block
```

Annotations compose:

```tnt
heap throws Window** makeWindow() { ... }
```

Compiler enforcement rules:
- `pure` function calling an `io` function → compile error
- `pure` function accessing a global → compile error
- function without `heap` that allocates → compile error
- function without `throws` that uses `try` without catching → compile error

---

## 8. Error Handling

### 8.1 Error Types

```tnt
error SDLInitFailed {
    string message;
    int sdlErrorCode;
}
```

- `error` keyword — not `struct` or `class`
- all errors guaranteed to have at least a `message` field, compiler enforced
- errors auto pretty-print without manual `toString`
- errors support inheritance — the only place in Tantrums where inheritance exists

### 8.2 Error Hierarchies

```tnt
error AppError {
    string message;
}

error SDLError extends AppError {
    int sdlCode;
}

error SDLInitError extends SDLError {
    string subsystem;
}
```

- `extends` is exclusive to `error` types
- child errors inherit all fields from parent

### 8.3 Throwing and Catching

```tnt
throws void initialize() {
    if (!hardware->available()) {
        throw SDLInitError {
            message: "init failed",
            sdlCode: -1,
            subsystem: "video"
        };
    }
}
```

```tnt
try sdl->initialize()
catch (SDLInitError e) {
    io!->print("subsystem: " + e->subsystem);
}
catch (SDLError e) {
    io!->print("sdl code: " + e->sdlCode->toString());
}
catch (AppError e) {
    io!->print("error: " + e->message);
}
```

### 8.4 `try` Propagation

```tnt
heap Window** w_ = try sdl!->makeWindow();
```

`try` inside a non-`throws` function is a compile error:

```
Error: `try` used inside non-throwing function `main`
  hint: mark `main` as `throws`, or use `try ... catch` to handle here
```

Inline catch:

```tnt
heap Window** w_ = try sdl!->makeWindow() catch (e) {
    io!->print("failed: " + e->message);
    return exitCodes!->Code!->FAILURE;
};
```

Default value on failure:

```tnt
int port = try config->getPort() catch { 8080 };
```

### 8.5 Panic

```tnt
panic("invariant violated: context was null");
```

- unrecoverable — cannot be caught
- prints full stack trace to stderr
- exits with non-zero code
- **panic = bug. error = expected failure mode.**

---

## 9. The `<~~` Lazy Return Operator

### 9.1 Motivation

```tnt
// verbose version
uint32 getId()
{
    static uint32 id = 0;
    uint32 temp = id;
    id++;
    return temp;
}

// with <~~
uint32 getId()
{
    static uint32 id = 0;
    return id <~~ id++;
}
```

### 9.2 Semantics

1. Evaluate left side — capture the return value
2. Execute right side — the side effect
3. Return the captured value

### 9.3 More Examples

```tnt
T pop()
{
    return data[length - 1] <~~ length--;
}

bool consumeFlag()
{
    return flag <~~ flag = false;
}

int32 advance()
{
    return cursor <~~ cursor += stride;
}
```

### 9.4 Rules

- side effect runs strictly after capture, strictly before return
- only one `<~~` per return statement
- `<~~` is only valid in a `return` statement
- if side effect throws and function is `throws`, error propagates and captured value is discarded

---

## 10. The `<-->` Chain Operator

### 10.1 What it is

`<-->` calls multiple methods on the same object without repeating the receiver. methods do NOT need to return anything special — the operator itself goes back to the original object.

without `<-->`:
```tnt
**w_->setSize(1280, 720);
**w_->setPos(0, 0);
**w_->setFPS(60);
```

with `<-->`:
```tnt
**w_->setSize(1280, 720)<-->setPos(0, 0)<-->setFPS(60);
```

**vs traditional chaining (C++/Java):** traditional chaining requires methods to return `this` — the method goes back to the object. in Tantrums the operator goes back to the object — methods don't know or care about chaining. any void method can be chained. caller-side decision, not API design decision.

### 10.2 Rules

- valid on ANY method regardless of return type
- root expression is evaluated exactly once — compiler generates a hidden temporary
- evaluation strictly left to right, guaranteed in the spec
- chains are void — cannot assign result to a variable
- `try` can prefix an entire chain

### 10.3 Syntax

Multi-line:

```tnt
**w_->setSize(1280, 720)
    <-->setPos(0, 0)
    <-->setTitle("My Window")
    <-->setFPS(sdl!->SYNC!->VERTICAL_SYNC!->default);
```

`try` over full chain:

```tnt
try **w_->setSize(1280, 720)<-->setPos(0, 0)<-->setFPS(60);
```

---

## 11. Entry Point

Two valid `main` signatures:

### Beginner

```tnt
void main(auto argc, auto argv)
{
    // compiler implicitly returns exitCodes!->Code!->SUCCESS
}
```

### Explicit exit code

```tnt
exitCodes!->Code main(auto argc, auto argv)
{
    return exitCodes!->Code!->SUCCESS;
}
```

- valid values: `exitCodes!->Code!->SUCCESS`, `exitCodes!->Code!->FAILURE`
- no other return type valid for `main`
- `auto` is valid for `argc` and `argv`

---

## 12. Stdlib Architecture

Stdlib is written as real `.tnt` files that call libc at the bottom via `extern`. one thin FFI layer at the very bottom, everything above it is pure Tantrums.

```
main.tnt
  → io!->print()        // pure Tantrums
    → extern puts()     // one FFI call to libc
      → libc
        → syscall
```

example `io.tnt`:

```tnt
extern int32 puts(string s);
extern int32 printf(string fmt);

expose io void print(string s) {
    puts(s);
}

expose io void printLine(string s) {
    printf(s + "\n");
}
```

no DLL tax. no overhead beyond one function call which the compiler can inline.

---

## 13. Complete Example

```tnt
use !string;
use !io;
use !exitCodes;
use sdl!->sdlcore;
alias sdlcore sdl;

exitCodes!->Code main(auto argc, auto argv)
{
    try sdl!->initialize(sdl!->INIT_TYPE!->EVERYTHING);

    heap Context** context = try sdl!->makeContext();
    heap Window** w_ = try sdl!->makeWindow();
    heap Renderer** r_ = try sdl!->makeRenderer();

    try **context->setAPI(sdl!->G_API!->VULKAN);

    try **w_->setContext(*context);
    try **w_->setSize(1280, 720)
        <-->setPos(0, 0)
        <-->setFPS(sdl!->SYNC!->VERTICAL_SYNC!->default);

    try **r_->setContext(*context);
    try **r_->setWindow(*w_);
    try **r_->debug_MakeBlack();
    try **r_->paint();

    io!->pauseByKey(io!->KEY!->ANY_KEY, "Press any key to quit...");

    **r_->clear();
    **r_->detatch();
    **w_->hide();

    sdl!->endAll();
    sdl!->freeResources();

    return exitCodes!->Code!->SUCCESS;
}
```

---

## 14. Identity Summary

> Tantrums is a systems programming language where the source code tells you exactly what it costs. Every allocation is visible. Every failure point is marked. Every mutation is declared. Every side effect is labeled. It doesn't hide complexity — it labels it. Because labeled complexity is manageable. Hidden complexity is a bug waiting to happen.