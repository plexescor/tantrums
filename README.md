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
- compiler looks for a `module.tnt` recursively in `src/modules/` (default) which contains the declaration of the module
- folder name can be anything, compiler matches against the one in module.tnt
- all `.tnt` files inside that folder (in which the requested module.tnt resides) that declare `impl module Maths;` belong to that module

#### Declaring a user module

every `.tnt` file that belongs to a module declares it at the top:

```tnt
impl module Maths;

expose void add(int32* x, int32* y, int32* result)
{
	*result = *x + *y;
}

expose void sub(int32* x, int32* y, int32* result)
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
				module.tnt		// module Maths;
				userAdd.tnt		// impl module Maths;
				userSub.tnt		// impl module Maths;
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

int32 main(auto argc, auto argv)
{
	heap int32* x = 5;
	heap int32* y = 5;
	heap int32* result = 0;
	Maths!->add(x, y, result);
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
break;		// exit loop
continue;	// skip to next iteration
```

---

## 5. Memory Model

### 5.1 Storage Classes

```tnt
heap int* x = ...;		// heap allocated — you own it, you free it
stack int y = 5;		// explicitly stack (usually implicit)
static int z = 0;		// static storage duration
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
unique heap int* x = ...;	// compiler guaranteed: no other pointer aliases this
shared heap int* y = ...;	// explicitly shared, treated conservatively
```

- `unique` unlocks aggressive compiler optimizations (vectorization, loop unrolling, register allocation)
- compiler enforces `unique` — creating an alias of a `unique` pointer is a compile error
- Tantrums code with `unique` pointers can outperform equivalent C code

---

## 6. Type System

### 6.1 Primitive Types

```tnt
int8	uint8
int16	uint16
int32	uint32
int64	uint64
int128	uint128

float32
float64

bool
byte		// alias for uint8, raw memory
char		// unicode codepoint, 4 bytes
```

- `int` is a convenience alias — `int32` on 32-bit targets, `int64` on 64-bit — fixed in the spec, not platform-ambiguous
- no implicit numeric conversion ever:

```tnt
int32 x = 5;
int64 y = int64(x);	// explicit cast required
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
const int32 maxSize = 1024;
const string appName = "MyApp";
```

- evaluated at compile time
- cannot be reassigned ever
- `const` implies `pure` — no runtime cost

### 6.4 Auto Inference

```tnt
auto x = 5;			// inferred as int32, locked in at declaration
auto y = 5.0;		// inferred as float64
x = "hello";		// COMPILE ERROR — x is int32
```

- `auto` infers once at declaration, type never changes
- `auto` is valid in function parameters
- no cross-type silent coercion ever

### 6.5 Nullable Types

```tnt
int x = 5;		// cannot be null, compiler enforced
int? y = null;	// explicitly nullable
int? z = 10;	// nullable but has a value
```

- non-nullable by default
- compiler performs null flow analysis:

```tnt
int? val = getResult();
io!->print(val->toString());	// COMPILE ERROR — might be null

if (val != null) {
	io!->print(val->toString());	// fine — compiler knows non-null here
}

io!->print(val ?? "default");	// null coalesce
```

### 6.6 Mutation

```tnt
mut int x = 5;	// mutable
int y = 10;		// immutable by default
```

- immutable by default, `mut` opts in
- `mut` on a class method = modifies the instance, compiler enforced
- non-`mut` method = read-only on the instance, compiler enforced

### 6.7 Enums

```tnt
enum initType {
	everything = 0,
	video = 1,
	audio = 2
}
```

- enum values are constants — cannot be reassigned
- accessed via `!->`: `sdl!->initType!->everything`
- underlying type defaults to `int32` unless specified:

```tnt
enum errorCode : uint8 {
	none = 0,
	generic = 1,
	timeout = 2
}
```

### 6.8 Structs

Structs are pure data containers. No methods, no behavior, no inheritance.

```tnt
struct vec2 {
	float32 x;
	float32 y;
}

struct color {
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
class window {
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
- `you` = self-reference inside class methods, replaces `this`

```tnt
class counter {
	mut int32 count = 0;

	mut void increment() {
		you->count++;
	}

	int32 get() {
		return you->count;
	}
}
```

### 6.10 Generics

```tnt
class list<T> {
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
T max<T: comparable<T>>(T a, T b) {
	return a->compareTo(b) > 0 ? a : b;
}
```

- no higher-kinded types, no associated types — kept intentionally simple

### 6.11 Interfaces

```tnt
interface drawable {
	mut void draw(renderer* r);
	int32 getWidth();
	int32 getHeight();
}

class sprite {
	mut void draw(renderer* r) { ... }
	int32 getWidth() { ... }
	int32 getHeight() { ... }
	// implicitly implements drawable — no declaration needed
}
```

- structural/implicit implementation — if the class has the methods, it satisfies the interface
- no boilerplate `implements` declarations

---

## 7. Call Cost Annotations

### 7.1 Philosophy

The compiler has zero knowledge of what any annotation means semantically. The compiler only knows three things — what the annotation source functions are, what the propagation rules are, and what the conflict rules are. Nothing more.

Every annotation — whether it's `heap` from stdlib or `network` you defined yourself — is defined using identical syntax. There is no distinction between stdlib annotations and user annotations at the language level. Stdlib authors and user code authors have equal power. The compiler is purely the enforcement engine. Stdlib is purely the definition layer.

This means annotations do not work on baremetal without stdlib. This is a conscious tradeoff. The compiler infrastructure to define, parse, propagate and enforce annotations is fully present — but the actual definitions of `heap`, `io`, `pure`, `throws` and every other annotation live in stdlib as real `.tnt` code.

### 7.2 Defining an Annotation

```tnt
annotation heap {
	propagate: up;
	message: "this function performs heap allocation";
}
```

Fields:

- `propagate` — direction the annotation spreads through the call graph. values: `up`, `down`, `none`
- `conflicts` — list of annotations that cannot coexist with this one on the same function
- `message` — human readable string shown in compiler errors when the annotation is violated

```tnt
annotation realtime {
	propagate: down;
	conflicts: heap;
	conflicts: io;
	conflicts: throws;
	message: "this function runs in a realtime context";
}
```

### 7.3 Propagation Directions

`propagate: up` — annotation climbs the call stack. if function C is annotated `heap`, any function that calls C must also declare `heap`. the annotation spreads upward to every caller through the entire call graph.

```tnt
// stdlib/memory.tnt
annotation heap {
	propagate: up;
	message: "this function performs heap allocation";
}

@source(heap)
extern void* __tntMalloc(uint64 size);

// anything calling __tntMalloc is now heap
// anything calling that is also heap
// all the way up the call graph
```

`propagate: down` — annotation pushes into callees. if a function is marked `realtime`, everything it calls must also be realtime-safe. the constraint flows downward into the implementation.

`propagate: none` — annotation is local only. labels the function for documentation and tooling but forces nothing on callers or callees.

### 7.4 Source Markers

`@source(annotationName)` marks a function as the root of an annotation. any function reachable from a source function inherits the annotation requirement according to the propagation rules.

```tnt
// stdlib/memory.tnt
@source(heap)
extern void* __tntMalloc(uint64 size);

@source(heap)
extern void* __tntRealloc(void* ptr, uint64 size);

// stdlib/io.tnt
@source(io)
extern int32 __tntWrite(int32 fd, void* buf, uint64 count);

@source(io)
extern int32 __tntRead(int32 fd, void* buf, uint64 count);
```

### 7.5 Conflict Detection

if a function has two conflicting annotations simultaneously, the compiler errors:

```tnt
// realtime conflicts with heap
// so this is a compile error:
realtime heap void badFunction() {
	// ERROR: annotation conflict — `realtime` forbids `heap`
}

// also a compile error if the conflict is implicit via propagation:
realtime void audioCallback(float32* buffer, int32 frames) {
	heap int32* temp = 5;
	// ERROR: heap allocation in realtime context
	// annotation conflict: realtime conflicts with heap
}
```

### 7.6 User Defined Annotations

defining a custom annotation is identical to stdlib. no special permissions needed. no compiler changes needed.

```tnt
annotation network {
	propagate: up;
	conflicts: pure;
	conflicts: realtime;
	message: "this function performs network I/O";
}

@source(network)
extern int32 __mySocketSend(int32 fd, void* buf, uint64 len);

// now network propagates up through anything that calls __mySocketSend
network throws void sendPacket(string data) {
	// ...
}

// caller must declare network too
network throws void uploadFile(string path) {
	sendPacket(readFile(path));
}

// compile error — missing network annotation
void badUpload(string path) {
	sendPacket(readFile(path));
	// ERROR: calling `network` function from non-`network` function
	// hint: mark `badUpload` as `network`
}
```

### 7.7 Stdlib Core Annotations

these are defined in stdlib, not the compiler. shown here for reference:

```tnt
// stdlib/annotations.tnt

annotation heap {
	propagate: up;
	message: "this function performs heap allocation";
}

annotation io {
	propagate: up;
	conflicts: pure;
	message: "this function performs I/O";
}

annotation pure {
	propagate: down;
	conflicts: io;
	conflicts: heap;
	message: "this function has no side effects";
}

annotation throws {
	propagate: up;
	message: "this function can propagate an error";
}
```

### 7.8 Composing Annotations

multiple annotations compose on a single function:

```tnt
heap throws window** makeWindow() { ... }
// heap = allocates
// throws = can fail

heap io throws string readFileToString(string path) { ... }
// heap = allocates the string
// io = reads from filesystem
// throws = can fail
```

### 7.9 Baremetal Note

without stdlib, annotation definitions do not exist. the compiler infrastructure is present — parsing, propagation engine, conflict checking — but there are no annotations to enforce because none are defined. annotations are valid syntax on baremetal but produce a compiler warning:

```
Warning: annotation `heap` is used but never defined
  hint: import stdlib or define `heap` manually
```

---

## 8. Relationships

### 8.1 Philosophy

annotations describe what a function DOES. relationships describe what a function NEEDS from its calling context or what it FORBIDS about its calling context.

annotations flow automatically through the call graph based on what you call. relationships are explicit contracts written at the function signature level that the compiler verifies at every call site.

### 8.2 `@callerRequires`

the callee demands its caller has a specific annotation. if a non-annotated function tries to call it, compile error.

```tnt
// this function can ONLY be called from realtime-annotated code
@callerRequires(realtime)
void writeSample(float32* buffer, float32 value) {
	buffer[0] = value;
}

// fine — caller is realtime
realtime void audioCallback(float32* buffer, int32 frames) {
	writeSample(buffer, 0.5);	// ✅
}

// compile error — caller is not realtime
void normalFunction() {
	float32 buf[512];
	writeSample(buf, 0.5);
	// ERROR: `writeSample` requires caller to be `realtime`
	// hint: mark `normalFunction` as `realtime` or don't call this here
}
```

### 8.3 `@callerForbids`

the callee demands its caller does NOT have a specific annotation.

```tnt
// this function ENTERS the gpu context
// if you're already in gpu context, calling this is wrong
@callerForbids(gpu)
gpu void beginGpuFrame() {
	// set up GPU state
}

// fine — caller is not gpu
void gameLoop() {
	beginGpuFrame();	// ✅
}

// compile error — caller is already gpu
gpu void renderPass() {
	beginGpuFrame();
	// ERROR: `beginGpuFrame` forbids caller to be `gpu`
	// hint: `beginGpuFrame` is an entry point, not for use inside gpu context
}
```

### 8.4 `@noRecurse`

prevents a function from being called recursively — directly or through any indirect call chain.

```tnt
@noRecurse
heap throws window** makeWindow() {
	// calling makeWindow() from anywhere reachable from here = compile error
}
```

indirect recursion is also caught:

```
Error: `@noRecurse` function `makeWindow` is reachable from itself
  Call chain:
    makeWindow() → helperA() at window.tnt:23
    helperA() → helperB() at helpers.tnt:47
    helperB() → makeWindow() at helpers.tnt:89
  hint: break the cycle or remove `@noRecurse`
```

### 8.5 `@noRecurseWith`

asymmetric mutual recursion restriction. prevents THIS function from reaching a specific other function through any call path.

```tnt
// parsePrimary cannot reach parseExpression through any path
// but parseExpression can still call parsePrimary
@noRecurseWith(parseExpression)
throws astNode* parsePrimary() {
	// ...
}
```

### 8.6 `@maxRecurse`

limits recursion depth. compiler verifies statically where possible. emits a runtime depth counter check where the depth is dynamic.

```tnt
@maxRecurse(8)
throws int32 searchTree(node* n, int32 depth) {
	// can recurse up to 8 levels deep
	// 9th call = compile error if statically determinable
	//           OR runtime panic if dynamic
}
```

### 8.7 `@callerForbids(you)` — Self Reference

`you` in a relationship context means the function itself. used to prevent self-recursion explicitly.

```tnt
@callerForbids(you)
heap throws database** Database::init() {
	// cannot call itself directly or indirectly
}
```

### 8.8 Named Relationships

when multiple functions share the same caller contract, define a named relationship once and apply it everywhere.

```tnt
relationship realtimeAudio {
	callerRequires: realtime;
	callerForbids: heap;
	callerForbids: io;
	callerForbids: throws;
}

@relationship(realtimeAudio)
void writeSample(float32* buffer, float32 value) { ... }

@relationship(realtimeAudio)
void mixBuffers(float32* out, float32* in, int32 frames) { ... }

@relationship(realtimeAudio)
void applyGain(float32* buffer, float32 gain, int32 frames) { ... }
```

changing the contract means changing the relationship definition in one place. the compiler re-enforces everywhere automatically.

### 8.9 Relationship Inheritance

relationships can extend other relationships.

```tnt
relationship strictRealtimeAudio extends realtimeAudio {
	callerRequires: validated;
}
```

`extends` on relationships is the only other place inheritance exists in Tantrums alongside `error` types.

---

## 9. Grants

### 9.1 Philosophy

a grant is a named token that a function declares it produces when called successfully. the caller explicitly receives the token at the call site. the token then unlocks other functions in the calling scope that require it.

grants maintain Tantrums' core rule — nothing is implicit. the callee declares it produces a token. the caller explicitly receives it. functions that need the token declare they need it. all three sides participate visibly.

### 9.2 Declaring a Grant on the Callee

the callee declares what token it produces on successful return using `@grants`:

```tnt
@grants(authenticated)
throws void login(string username, string password) {
	// verify credentials
	// if wrong — throw authError
	// if right — returns normally, produces authenticated token
}
```

the grant is only produced on SUCCESSFUL return. if the function throws, no token is produced.

### 9.3 Receiving a Grant on the Caller

the caller explicitly receives the token using `@grant(token) <-` syntax:

```tnt
throws void handleRequest(string userId) {
	@grant(authenticated) <- try login("admin", "password");
	// authenticated token is now in scope

	string data = try getUserData(userId);	// ✅ authenticated is in scope
}
```

the `@grant(...) <-` syntax is mandatory. if you call a `@grants` function without receiving, the token is discarded and functions requiring it remain locked:

```tnt
throws void badHandler(string userId) {
	try login("admin", "password");
	// token discarded — not received

	string data = try getUserData(userId);
	// ERROR: `getUserData` requires grant `authenticated`
	// hint: receive the grant — @grant(authenticated) <- try login(...)
}
```

### 9.4 Requiring a Grant

functions declare they require a grant using `@requiresGrant`:

```tnt
@requiresGrant(authenticated)
throws string getUserData(string userId) {
	// safely query database
	// compiler or runtime verifies authenticated is in scope
}
```

### 9.5 Bulk Grant Receiving

receiving multiple grants from multiple calls:

```tnt
throws void setup() {
	@grant(authenticated) <- try login("admin", "password");
	@grant(dbConnected)   <- try connectDatabase("prod");
	@grant(cacheReady)    <- try initCache();

	// all three tokens now in scope
	try processRequest();	// requires all three
}
```

chained grants using `<-->`:

```tnt
throws void setup() {
	@grant(authenticated, dbConnected) <- try login("admin", "password")
		<-->connectDatabase("prod");
	// receives both tokens from a chained call
}
```

### 9.6 Hybrid Compile-Time / Runtime Verification

the compiler uses flow-sensitive analysis to verify grants statically where it can. where it cannot — such as grants behind runtime conditionals — it falls back to a runtime check and emits a warning.

**compile-time verified — grant is unconditional in the scope:**

```tnt
throws void handleRequest(string userId) {
	@grant(authenticated) <- try login("admin", "password");
	// compiler sees grant unconditionally before getUserData
	// fully verified at compile time — zero runtime overhead
	string data = try getUserData(userId);	// ✅ compile-time verified
}
```

**runtime fallback — grant is behind a condition:**

```tnt
throws void handleRequest(string userId, bool isAdmin) {
	if (isAdmin) {
		@grant(authenticated) <- try login("admin", "password");
	}
	string data = try getUserData(userId);
	// Warning: grant `authenticated` cannot be verified statically
	//   → runtime check inserted before `getUserData`
	//   hint: move @grant outside conditional branches for compile-time verification
}
```

the runtime check panics with a clear message if the grant was not received:

```
TANTRUMS RUNTIME PANIC
Location: main.tnt:47:5
Function: getUserData
Message: required grant `authenticated` was not received in calling scope
  hint: call `login` and receive with @grant(authenticated) <- login(...)
```

**compile-time error — grant is provably missing:**

```tnt
throws void badHandler(string userId) {
	// no login call anywhere
	string data = try getUserData(userId);
	// ERROR: `getUserData` requires grant `authenticated`
	// grant `authenticated` is never received in this scope
	// hint: @grant(authenticated) <- try login(...)
}
```

### 9.7 Grant Scope

grants are scoped to the function they are received in. they do not propagate to callers automatically. if a function needs to pass a grant upward it must re-declare it:

```tnt
@grants(authenticated)
throws void loginAndSetup(string user, string pass) {
	@grant(authenticated) <- try login(user, pass);
	// authenticated in scope here
	// also re-granted to whoever calls loginAndSetup
}
```

---

## 10. The `fuck` and `you` Keywords

### 10.1 `you` — Self Reference

`you` is the self-reference keyword inside class methods. replaces `this`. reads as natural language — "your field", "your method".

```tnt
class counter {
	mut int32 count = 0;

	mut void increment() {
		you->count++;
	}

	mut void reset() {
		you->count = 0;
	}

	int32 get() {
		return you->count;
	}
}
```

`you` is also valid in relationship definitions as self-reference — `@callerForbids(you)` means the function forbids calling itself.

### 10.2 `fuck` — Compile Time Forcing

`fuck` forces an expression or block to be evaluated at compile time. if the expression has any runtime dependencies, it is a compile error.

```tnt
fuck int32 bufferSize = 1024 * 1024 * 4;
// evaluated at compile time, baked into binary as constant

fuck string appTitle = "MyApp " + versionString;
// versionString must also be a compile-time constant

fuck int32 result = computeHash("staticKey");
// computeHash must be pure and have no runtime dependencies
```

`fuck` block — entire block evaluated at compile time:

```tnt
fuck {
	int32 a = computeA();
	int32 b = computeB(a);
	int32 finalResult = a * b;
	// finalResult is now a compile-time constant in the enclosing scope
}
```

compile error when expression has runtime dependencies:

```
Error: `fuck` requires compile-time evaluable expression
  → call to `getCurrentTime()` is runtime-only
  hint: remove `fuck` or use a compile-time constant
```

### 10.3 `fuck you` — Unreachable Assertion

`fuck you` as a combined statement asserts that a code path should never be reached. the compiler attempts to verify this statically. if it cannot prove unreachability it emits a warning and inserts a runtime panic.

```tnt
int32 getDayCount(int32 month) {
	if (month == 1)  return 31;
	if (month == 2)  return 28;
	if (month == 3)  return 31;
	if (month == 4)  return 30;
	if (month == 5)  return 31;
	if (month == 6)  return 30;
	if (month == 7)  return 31;
	if (month == 8)  return 31;
	if (month == 9)  return 30;
	if (month == 10) return 31;
	if (month == 11) return 30;
	if (month == 12) return 31;
	fuck you;
	// compiler verifies this is unreachable given the above coverage
	// compiles to LLVM `unreachable` — optimization hint
}
```

when the compiler CAN prove unreachability — compiles to LLVM `unreachable`. zero runtime cost. the optimizer uses this to eliminate dead code and improve surrounding codegen.

when the compiler CANNOT prove unreachability:

```
Warning: cannot verify `fuck you` is unreachable — runtime panic inserted
  hint: ensure all paths above are exhaustive
```

runtime panic message if somehow reached:

```
TANTRUMS RUNTIME PANIC
Location: main.tnt:47:5
Message: fuck you
Stack trace:
  → getDayCount() at main.tnt:47
  → processDate() at main.tnt:103
  → main() at main.tnt:201
```

---

## 11. Error Handling

### 11.1 Error Types

```tnt
error sdlInitFailed {
	string message;
	int sdlErrorCode;
}
```

- `error` keyword — not `struct` or `class`
- all errors guaranteed to have at least a `message` field, compiler enforced
- errors auto pretty-print without manual `toString`
- errors support inheritance — one of only two places in Tantrums where inheritance exists

### 11.2 Error Hierarchies

```tnt
error appError {
	string message;
}

error sdlError extends appError {
	int sdlCode;
}

error sdlInitError extends sdlError {
	string subsystem;
}
```

- `extends` is exclusive to `error` types and `relationship` types
- child errors inherit all fields from parent

### 11.3 Throwing and Catching

```tnt
throws void initialize() {
	if (!hardware->available()) {
		throw sdlInitError {
			message: "init failed",
			sdlCode: -1,
			subsystem: "video"
		};
	}
}
```

```tnt
try sdl->initialize()
catch (sdlInitError e) {
	io!->print("subsystem: " + e->subsystem);
}
catch (sdlError e) {
	io!->print("sdl code: " + e->sdlCode->toString());
}
catch (appError e) {
	io!->print("error: " + e->message);
}
```

### 11.4 `try` Propagation

```tnt
heap window** w_ = try sdl!->makeWindow();
```

`try` inside a non-`throws` function is a compile error:

```
Error: `try` used inside non-throwing function `main`
  hint: mark `main` as `throws`, or use `try ... catch` to handle here
```

inline catch:

```tnt
heap window** w_ = try sdl!->makeWindow() catch (e) {
	io!->print("failed: " + e->message);
	return exitCodes!->code!->failure;
};
```

default value on failure:

```tnt
int port = try config->getPort() catch { 8080 };
```

### 11.5 Panic

```tnt
panic("invariant violated: context was null");
```

- unrecoverable — cannot be caught
- prints full stack trace to stderr
- exits with non-zero code
- **panic = bug. error = expected failure mode.**

---

## 12. The `<~~` Lazy Return Operator

### 12.1 Motivation

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

### 12.2 Semantics

1. evaluate left side — capture the return value
2. execute right side — the side effect
3. return the captured value

### 12.3 More Examples

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

### 12.4 Rules

- side effect runs strictly after capture, strictly before return
- only one `<~~` per return statement
- `<~~` is only valid in a `return` statement
- if side effect throws and function is `throws`, error propagates and captured value is discarded

---

## 13. The `<-->` Chain Operator

### 13.1 What it is

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

### 13.2 Rules

- valid on ANY method regardless of return type
- root expression is evaluated exactly once — compiler generates a hidden temporary
- evaluation strictly left to right, guaranteed in the spec
- chains are void — cannot assign result to a variable
- `try` can prefix an entire chain

### 13.3 Syntax

multi-line:

```tnt
**w_->setSize(1280, 720)
	<-->setPos(0, 0)
	<-->setTitle("My Window")
	<-->setFPS(sdl!->sync!->verticalSync!->default);
```

`try` over full chain:

```tnt
try **w_->setSize(1280, 720)<-->setPos(0, 0)<-->setFPS(60);
```

---

## 14. Entry Point

Two valid `main` signatures:

### Beginner

```tnt
int32 main(auto argc, auto argv)
{
	// compiler implicitly returns 0
}
```

### Explicit exit code

```tnt
exitCodes!->code main(auto argc, auto argv)
{
	return exitCodes!->code!->success;
}
```

- valid values: `exitCodes!->code!->success`, `exitCodes!->code!->failure`
- no other return type valid for `main`
- `auto` is valid for `argc` and `argv`

---

## 15. Stdlib Architecture

Stdlib is written as real `.tnt` files that call libc at the bottom via `extern`. one thin FFI layer at the very bottom, everything above it is pure Tantrums.

```
main.tnt
  → io!->print()		// pure Tantrums
	→ extern puts()		// one FFI call to libc
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

## 16. Complete Example

```tnt
use !string;
use !io;
use !exitCodes;
use !sdl!->sdlcore;
alias sdlcore sdl;

exitCodes!->code main(auto argc, auto argv)
{
	try sdl!->initialize(sdl!->initType!->everything);

	heap context** ctx  = try sdl!->makeContext();
	heap window**  w_   = try sdl!->makeWindow();
	heap renderer** r_  = try sdl!->makeRenderer();

	try **ctx->setAPI(sdl!->gApi!->vulkan);

	try **w_->setContext(*ctx);
	try **w_->setSize(1280, 720)
		<-->setPos(0, 0)
		<-->setFPS(sdl!->sync!->verticalSync!->default);

	try **r_->setContext(*ctx);
	try **r_->setWindow(*w_);
	try **r_->debugMakeBlack();
	try **r_->paint();

	io!->pauseByKey(io!->key!->anyKey, "Press any key to quit...");

	**r_->clear();
	**r_->detach();
	**w_->hide();

	sdl!->endAll();
	sdl!->freeResources();

	return exitCodes!->code!->success;
}
```

---

## 17. Identity Summary

> Tantrums is a systems programming language where the source code tells you exactly what it costs. Every allocation is visible. Every failure point is marked. Every mutation is declared. Every side effect is labeled. It doesn't hide complexity — it labels it. Because labeled complexity is manageable. Hidden complexity is a bug waiting to happen.