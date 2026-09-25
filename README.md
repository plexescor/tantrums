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
- Operations on variables / print / literals working
- Function returning working

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
| Function calls | ✅ Done |
| Function return values | ✅ Done |
| Function arguments | ✅ Done |
| Variable updation/reassignment | ✅ Done |
| `if` / `else` | ❌ Not started |
| `for` loop | ❌ Not started |
| `while` loop | ❌ Not started |
| Actual `test` cases | 🔧 WIP |

Once all of the above are working and tested, v0.1.0 is out.

> [!NOTE]
> Keep in mind that "✅ Done" doesn't mean working perfectly or tested perfectly

---

## Working example

The compiler currently compiles `.tnt` files with functions declarations and calls, variable declarations, and arithmetic expressions:

```tnt
int64 testFunc()
{
	int64 ffjfj = 9 + 18;
	return ffjfj;
}

void greet()
{
	print("Hello World!\n");
}

void testFakeConcat(string a)
{
	print("\nI received: ");
	print(a);
	print("\n");
}

int32 main()
{
	int32 x = -5 * 1;
	int64 y = 5 / 48 + (47488 + 38 * 37);

	float64 a = 59.4;
	int8 b = 99;
	greet(); 
	print(testFunc() + y);

	testFakeConcat("testString");
	return 0; 
}
```

Output (IR):
```tnt
C:\Users\Plexescor\Projects\tantrums\tests>..\build\Debug\tantrums.exe playground.tnt -o TEST.obj -l hello.exe --emit-llvm-ir
Lexing file: playground.tnt : Progress: 0%
Lexing took: 0 ms
Parsing file: playground.tnt : Progress: 20%
[FunctionDecl] name=testFunc returnType=int64 null=false mut=false heap=false io=false throws=false pure=false
[FunctionDecl] name=greet returnType=void null=false mut=false heap=false io=false throws=false pure=false
[FunctionDecl] name=testFakeConcat returnType=void null=false mut=false heap=false io=false throws=false pure=false
Token: return
[FunctionDecl] name=main returnType=int32 null=false mut=false heap=false io=false throws=false pure=false
Parsing took: 1 ms
Generating IR for: playground.tnt : Progress: 40%
; ModuleID = 'tantrums'
source_filename = "tantrums"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@0 = private unnamed_addr constant [14 x i8] c"Hello World!\0A\00", align 1
@print.format = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@1 = private unnamed_addr constant [14 x i8] c"\0AI received: \00", align 1
@print.format.1 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@print.format.2 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@2 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@print.format.3 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@print.format.4 = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@3 = private unnamed_addr constant [11 x i8] c"testString\00", align 1

define i64 @testFunc() {
entry:
  %ffjfj = alloca i64, align 8
  store i64 27, ptr %ffjfj, align 8
  %0 = load i64, ptr %ffjfj, align 8
  ret i64 %0
}

declare i32 @printf(ptr, ...)

define void @greet() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @print.format, ptr @0)
  ret void
}

define void @testFakeConcat(ptr %a) {
entry:
  %a1 = alloca ptr, align 8
  store ptr %a, ptr %a1, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @print.format.1, ptr @1)
  %1 = load ptr, ptr %a1, align 8
  %2 = call i32 (ptr, ...) @printf(ptr @print.format.2, ptr %1)
  %3 = call i32 (ptr, ...) @printf(ptr @print.format.3, ptr @2)
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
  %0 = call i64 @testFunc()
  %1 = load i64, ptr %y, align 8
  %2 = add i64 %0, %1
  %3 = call i32 (ptr, ...) @printf(ptr @print.format.4, i64 %2)
  call void @testFakeConcat(ptr @3)
  ret i32 0
}
IR generation took: 17 ms
Compilation time: 19 ms

C:\Users\Plexescor\Projects\tantrums\tests>hello.exe
Hello World!
48921
I received: testString

C:\Users\Plexescor\Projects\tantrums\tests>
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

