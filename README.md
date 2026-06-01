# Custom Zero-Dependency XML Config & Tokenization Engine

A high-performance, block-buffered lexical analyzer and recursive descent parser built in native C++ to process structured token grammars under strict memory footprint constraints.

---

## Architectural Architecture & Implementation Details

- **Constant-Memory Tokenization:** Implements low-level file descriptor streaming into a fixed 32KB processing buffer. Memory utilization remains completely $O(1)$, independent of file sizes.

- **Context-Aware Macro Dereferencing:** Natively processes variable evaluations (`${VAR}`) by intersecting structural tokens with internal state mappings (`_envMap`) and system environment scopes.

- **Predictive Grammatical Parsing:** Leverages a custom lookahead and pushback pipeline to resolve structural ambiguities in highly nested tree formats without incurring performance penalties from deep recursive backtracking.

- **Policy-Based Threading Infrastructure** — Locking behavior is a compile-time template parameter (`SingleThreaded`, `ClassLevelLockable<T>`, `ObjectLevelLockable<T>`). The `Log` singleton, `SynchronizedQueue<T,N>`, and `Runnable<>` thread primitives compose cleanly for producer-consumer pipelines without runtime dispatch overhead.

---

## Grammar

```
document   : EOSTREAM | elements EOSTREAM

elements   : element | elements element

element    : shortformat | longformat

shortformat: '<' IDENTIFIER            '/>'
           | '<' IDENTIFIER attributes '/>'

longformat : '<' IDENTIFIER            '>'          '</' IDENTIFIER '>'
           | '<' IDENTIFIER attributes '>'          '</' IDENTIFIER '>'
           | '<' IDENTIFIER            '>' content  '</' IDENTIFIER '>'
           | '<' IDENTIFIER attributes '>' content  '</' IDENTIFIER '>'

content    : verbatim | elements

verbatim   : (TEXT | dereference)+

attributes : attribute+

attribute  : IDENTIFIER '=' value

value      : INTEGRAL | REAL | compound

compound   : (STRING | dereference)+

dereference: '${' IDENTIFIER '}'
```

**Terminals**

| Token        | Pattern                          |
|--------------|----------------------------------|
| `IDENTIFIER` | `[A-Za-z][A-Za-z0-9_]*`         |
| `STRING`     | `'` … `'` (single-quoted)        |
| `INTEGRAL`   | `[0-9]+`                         |
| `REAL`       | `[0-9]+(\.[0-9]*)?`              |
| `TEXT`       | any bytes not containing `<`/`$` |

---

## Example Input

```xml
<documentary>
  <personal>
    <address number=1 street='Alpha Loop Rd' city='Kansas City'/>
    <name first='Abraham' last='Schlotky'></name>
  </personal>
  <quote symbol='ibm' bsize=10 bid=123.45 ask=123.95/>
  <referendum date=${TARGETDATE} vote=${PARTY}/>
  <path url=${HOME}'/foo' abs=${SKU}'-gluemeto-'${PARTY}/>
</documentary>
```

Pass runtime bindings on the command line:

```sh
./appl { TARGETDATE=2026-11-03 PARTY=independent }
```

---

## Building

Requires a C++17-capable compiler and POSIX (`read`, `open`, `pthread`).

```sh
# Linux (original target)
make

# macOS — avoids Time.H / <time.h> case-collision on HFS+
g++ -std=c++17 -Wall -g -pthread -DMULTITHREADED -DCONSOLE_OFF \
    -iquote . \
    Document.C Lex.C Parser.C System.C Lockable.C Log.C Time.C Main.C \
    -o appl
```

---

## Source Map

| File | Role |
|------|------|
| `Lex.H` / `Lex.C` | Block-buffered lexer; `string_view`-based token emission |
| `Parser.H` / `Parser.C` | LL(1) recursive descent parser; attribute/content/dereference handling |
| `Document.H` / `Document.C` | DOM-style parse tree |
| `Log.H` / `Log.C` | Singleton logger; stderr / filesystem / syslog sinks |
| `SingletonHolder.H` | Policy-threaded singleton template |
| `ClassLevelLockable.H` / `ObjectLevelLockable.H` | Static vs. instance locking policies |
| `MultiThreaded.H` / `SingleThreaded.H` | Threading model selector |
| `SynchronizedQueue.H` | Lock-guarded bounded FIFO |
| `Runnable.H` / `AttachedThread.H` / `DetachedThread.H` | POSIX thread wrappers |
| `Lockable.H` / `Lockable.C` | `pthread_mutex` RAII wrapper |

---

## C++17 Modernization (2026)

Applied to the original 2008 codebase:

1. **Buffer overflow elimination** — Replaced all `sprintf` / fixed-size `char[]` formatting with `std::to_string`. Affects `Lex::ParseError` constructors and `TypedValue<int>::asString` / `TypedValue<double>::asString`.

2. **Zero-copy lexeme passing** — Introduced a reusable `std::string _tokenBuf` inside `Lex`; all token methods write into it and surface a `std::string_view` to callers. The parser propagates views through its lookahead pipeline, copying to `std::string` only where ownership is required.

3. **Exception specification cleanup** — Removed all dynamic exception specifications (`throw(ParseError)`, `throw()`). Destructors and `what()` are now marked `noexcept`; throwing functions carry no specification, matching C++11–17 semantics.
