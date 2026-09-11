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

The compiler currently supports a complete end-to-end pipeline from source text to native machine code:

- **Lexer**: Tokenizes source text, tracks line and column coordinates, handles keywords, primitive types (`int8`–`int64`, `uint8`–`uint64`, `float32`, `float64`, `bool`, `string`, `void`), operators, literals, and comments (`//`).
- **Parser**: Recursive descent parser producing an AST for:
  - Function declarations (return types, nullability `?`, and annotations).
  - Variable declarations (`mut`, explicit types, `auto`, nullability).
  - Expressions: binary arithmetic (`+`, `-`, `*`, `/`) with operator precedence, unary negation (`-`), parenthesized sub-expressions `(...)`, typed literals, and variable identifiers.
  - Error recovery and synchronization (`synchronize()`).
- **LLVM IR Codegen**:
  - Target machine setup with host triple and data layout.
  - Emits native LLVM types (`i8`–`i64`, `float`, `double`, `i1`, `ptr`, `void`).
  - Stack allocations in function entry basic blocks (`alloca`).
  - Integer and floating-point arithmetic emission (`add`/`fadd`, `sub`/`fsub`, `mul`/`fmul`, `sdiv`/`fdiv`, `neg`/`fneg`).
  - Variable load and store operations.
- **Compiler & Native Linker**:
  - Emits native machine code object files (`output.o`) via LLVM `PassManager`.
  - Links directly to a native executable via MSVC `link.exe` (on Windows) or `ld.lld` (on Linux).
- **Driver**:
  - Multi-threaded file compilation (parallel worker threads per input file, <- just cosmetic).
  - Per-phase timing reports for lexing, parsing, IR generation, compiling, and linking.

---

## Working example

The compiler currently compiles `.tnt` files with functions, variable declarations, and arithmetic expressions:

```tnt
void testFunc()
{
	int64 ffjfj = 9 + 18;
}

int32 main()
{
	int32 x = -5 * 1;
	int64 y = 5 / 48 + (47488 + 38 * 37);

	float64 a = 59.4;
	int8 b = 99;
}
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

