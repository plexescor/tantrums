# Tantrums

A compiled programming language with an LLVM backend, written from scratch in C++23.

> **This language is not trying to be the next C, C++, or Rust.**
> It exists so I can learn how compilers actually work — and to prove I can build one without relying on AI-generated code.
> Every line in this repository was written and understood by me personally.

---

## Why

My previous attempt at a language ([Tantrums-DEPRECATED](https://github.com/plexescor/Tantrums-DEPRECATED)) was 100% AI generated. It worked, sort of, but I didn't understand most of what it was doing. I couldn't debug it confidently. I couldn't extend it without breaking things I didn't understand.

This is the rewrite. No AI. Just me, the LLVM docs, and time.

The long-term goal is to make Tantrums capable enough to port a real application I'm actively developing to it — proving the language works on a non-trivial real-world codebase.

---

## What Tantrums is not

- Not a production language
- Not competing with Rust, Zig, Go, or anything else
- Not trying to have a large ecosystem or community
- Not AI-assisted

---

## Design goals

- **Compiled to native code** via LLVM — not interpreted, not transpiled
- **Statically typed** as the primary mode — the compiler knows every type at compile time
- **Native LLVM types** in static mode — no NaN-boxing, no unified value wrapper, no overhead
- **Small implementation** — the compiler source should be readable in one sitting

---

## Current status

Nothing

---

## Roadmap

- `[-]` **Lexer** — tokenize source text into a token stream
- `[ ]` **Parser** — recursive descent, build an AST
- `[ ]` **Type checker** — resolve and validate types in static mode
- `[ ]` **LLVM IR codegen** — walk the AST, emit LLVM IR, run the optimizer
- `[ ]` **Module System** — Make a module / impl system
- `[ ]` **FFI** — call C libs from tantrums
- `[ ]` **Basic standard library** — I/O, strings, basic collections
- `[ ]` **Port first real feature** from my target application

---

## Building

**Requirements:** CMake 3.15+, a C++23 compiler (GCC 13+, Clang 16+, or MSVC 2022+)

```bash
git clone https://github.com/plexescor/tantrums
cd tantrums
cmake -B build
cmake --build build
```

---

## License

GPL-3.0
