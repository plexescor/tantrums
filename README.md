# Tantrums
> This is my beloved child, treat it with care... 

A compiled programming language with an LLVM backend, written from scratch in C++23.

> **This language is not trying to be the next C, C++, or Rust.**
> It exists so I can learn how compilers actually work — and to prove I can build one without relying on AI-generated code.
> Every line in this repository was written and understood by me personally.

---

## Why

My previous attempt at a language ([Tantrums-DEPRECATED](https://github.com/plexescor/Tantrums-DEPRECATED)) was 100% AI generated. It worked, sort of, but I didn't understand most of what it was doing. I couldn't debug it confidently. I couldn't extend it without breaking things I didn't understand.

This is the rewrite. Minimal AI. Just me, the LLVM docs, an LLM or Two for understanding, and time.

The long-term goal is to make Tantrums capable enough to port a real application I'm actively developing to it — proving the language works on a non-trivial real-world codebase.

---

## What Tantrums is not

- Not a production language
- Not competing with Rust, Zig, Go, or anything else
- Not trying to have a large ecosystem or community
- Not AI Written

---

## Design goals

- **Compiled to native code** via LLVM — not interpreted, not transpiled
- **Statically typed** as the primary mode — the compiler knows every type at compile time
- **Native LLVM types** in static mode — no NaN-boxing, no unified value wrapper, no overhead

---

## Current status

- Lexer lexing
- Parser parsing
- Type checker type checking
- Code generater code gen-ing
- Function calls working though with no args for now or no returns for now
- Expressions working
- Variable declaration and usage and funciton declarations working

---

## Aim for v0.1.0

### What I want working

| Feature | Status |
|---|---|
| Hardcoded `print`  | ✅ Done |
| Variable declaration | ✅ Done |
| Variable usage in expressions | ✅ Done |
| Basic arithmetic (`+`, `-`, `*`, `/`, unary `-`, grouping) | ✅ Done |
| Function declaration | ✅ Done |
| Function calls (no args, no return) | ✅ Done |
| Function arguments | 🔧 WIP |
| Function return values | 🔧 WIP |
| `if` / `else` | ❌ Not started |
| `for` loop | ❌ Not started |
| `while` loop | ❌ Not started |

Once all of the above are working and tested, v0.1.0 is out.

---

## Working example

The compiler currently compiles `.tnt` files with functions declarations and calls, variable declarations, and arithmetic expressions:

```tnt
void testFunc()
{
	int64 ffjfj = 9 + 18;
}

void greet()
{
  print("Hello World!");
}

int32 main()
{
	int32 x = -5 * 1;
	int64 y = 5 / 48 + (47488 + 38 * 37);

	float64 a = 59.4;
	int8 b = 99;
  greet();  
}
```

Output (IR):
```tnt
C:\Users\Plexescor\Projects\tantrums\build>Debug\tantrums.exe ..\tests\helloWorld.tnt -o TEST.obj -l hello.exe --emit-llvm-ir
Lexing file: ..\tests\helloWorld.tnt : Progress: 0%
Lexing took: 1 ms
Parsing file: ..\tests\helloWorld.tnt : Progress: 20%
[FunctionDecl] name=testFunc returnType=void null=false mut=false heap=false io=false throws=false pure=false
[FunctionDecl] name=greet returnType=void null=false mut=false heap=false io=false throws=false pure=false
[FunctionDecl] name=main returnType=int32 null=false mut=false heap=false io=false throws=false pure=false
Parsing took: 0 ms
Generating IR for: ..\tests\helloWorld.tnt : Progress: 40%
; ModuleID = 'tantrums'
source_filename = "tantrums"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@0 = private unnamed_addr constant [13 x i8] c"Hello World!\00", align 1
@print.format = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1

define void @testFunc() {
entry:
  %ffjfj = alloca i64, align 8
  store i64 27, ptr %ffjfj, align 8
  ret void
}

declare i32 @printf(ptr, ...)

define void @greet() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @print.format, ptr @0)
  ret void
}

define i32 @main() {
entry:
  %b = alloca i8, align 1
  %a = alloca double, align 8
  %y = alloca i64, align 8
  %x = alloca i32, align 4
  store i32 -5, ptr %x, align 4
  store i64 48894, ptr %y, align 8
  store double 0x404DB33333333333, ptr %a, align 8
  store i8 99, ptr %b, align 1
  call void @greet()
  ret i32 0
}
IR generation took: 7 ms
Compilation time: 8 ms

C:\Users\Plexescor\Projects\tantrums\build>hello.exe
Hello world!

C:\Users\Plexescor\Projects\tantrums\build>
```

---

## Implementation status

- `[x]` **Lexer** — Tokenize source code, locations, keywords, literals, operators, comments
- `[x]` **Parser** — Recursive descent AST for functions, variables, expressions with precedence
- `[x]` **Arithmetic Expression** — Binary operations (`+`, `-`, `*`, `/`), unary `-`, and parenthesized grouping
- `[x]` **LLVM IR Codegen** — Native types, entry block `alloca`, arithmetic IR emission, loads/stores
- `[x]` **Object Emission & Linking** — Emit `.o` and link native executables (`link.exe` / `ld.lld`)
- `[-]` **Type Checker & Symbol Table** — Scoped symbol table, literal bounds validation, and type deduction (WIP)

---

## Building and running

**Requirements:**
- CMake 3.15+
- A C++23 compiler (GCC 13+, Clang 16+, or MSVC 2022+)
- LLVM development libraries (components: `core`, `support`, `irreader`, `native`)

### Build

```bash
git clone https://github.com/plexescor/tantrums
cd tantrums
cmake -B build
cmake --build build
```

### Run

```bash
# Dump generated LLVM IR to stdout:
./build/Debug/tantrums tests/helloWorld.tnt --emit-llvm-ir

# Compile to object file and link into native executable:
./build/Debug/tantrums tests/helloWorld.tnt
```

---

## License

GPL-3.0

