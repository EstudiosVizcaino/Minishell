# Minishell — Complete Study Guide & Defense Manual

> **Purpose:** This guide gives you a fast, deep understanding of every module in the
> `srcs/` directory, their data structures, how they connect, and the answers to
> the questions you will face during an oral defence.
>
> Read it top-to-bottom once, then use the **Quick-Reference** section as a
> last-minute cheat-sheet before walking into the evaluation room.

---

## Table of Contents

1. [Project Architecture Overview](#1-project-architecture-overview)
2. [Key Data Structures](#2-key-data-structures)
3. [Module: main.c — The REPL Loop](#3-module-mainc--the-repl-loop)
4. [Module: lexer/ — Tokenisation](#4-module-lexer--tokenisation)
5. [Module: parser/ — Abstract Syntax Tree](#5-module-parser--abstract-syntax-tree)
6. [Module: expander/ — Variable & Quote Expansion](#6-module-expander--variable--quote-expansion)
7. [Module: executor/ — Command Execution](#7-module-executor--command-execution)
8. [Module: builtins/ — Built-in Commands](#8-module-builtins--built-in-commands)
9. [Module: env/ — Environment Management](#9-module-env--environment-management)
10. [Module: signals/ — Signal Handling](#10-module-signals--signal-handling)
11. [Module: utils/ — Helper Utilities](#11-module-utils--helper-utilities)
12. [Bonus Features (srcs_bonus/)](#12-bonus-features-srcs_bonus)
13. [End-to-End Execution Flow](#13-end-to-end-execution-flow)
14. [Inter-Module Dependency Map](#14-inter-module-dependency-map)
15. [Defense Q&A — Common Questions & Model Answers](#15-defense-qa--common-questions--model-answers)
16. [Quick-Reference Cheat Sheet](#16-quick-reference-cheat-sheet)

---

## 1. Project Architecture Overview

Minishell is a POSIX-compliant shell implemented in C. Its execution model
mirrors that of every real UNIX shell: **Read → Lex → Parse → Expand → Execute**.

```
┌──────────────────────────────────────────────────────────────────┐
│                          main loop (REPL)                        │
│                                                                  │
│  readline()  ──►  lexer()  ──►  parser()  ──►  expand_ast()     │
│                                               ──►  execute()     │
└──────────────────────────────────────────────────────────────────┘

 raw string → [t_token list] → [t_ast tree] → (expansion in-place) → result
```

### Top-level pipeline (one line of input)

```
"echo $HOME | cat -n > out.txt"
        │
        ▼  lexer()
  TOKEN_WORD("echo") → TOKEN_WORD("$HOME") → TOKEN_PIPE → TOKEN_WORD("cat")
  → TOKEN_WORD("-n") → TOKEN_REDIR_OUT → TOKEN_WORD("out.txt")
        │
        ▼  parser()
        NODE_PIPE
       /         \
  NODE_CMD        NODE_CMD
  args[echo,$HOME] args[cat,-n], redir→TOKEN_REDIR_OUT "out.txt"
        │
        ▼  expand_ast()
  $HOME replaced with "/home/user"
        │
        ▼  execute()
  fork two children, connect via pipe, apply redirections
```

### Directory map

| Directory | Role |
|-----------|------|
| `srcs/main.c` | Entry point + REPL loop |
| `srcs/lexer/` | Converts raw input string → token linked list |
| `srcs/parser/` | Converts token list → AST |
| `srcs/expander/` | Walks AST and expands variables/quotes |
| `srcs/executor/` | Walks AST and runs each node |
| `srcs/builtins/` | Implementations of the 7 built-in commands |
| `srcs/env/` | Linked list for environment variables |
| `srcs/signals/` | SIGINT / SIGQUIT handling |
| `srcs/utils/` | Memory helpers, free helpers, misc |
| `srcs_bonus/` | AND/OR logic, wildcards, parentheses, syntax check |
| `includes/` | Single shared header `minishell.h` |
| `libft/` | Re-usable C library (ft_* functions) |

---

## 2. Key Data Structures

Understanding these six structures is the single most important step for
the defence. Every piece of the shell uses them.

### 2.1 `t_token` — a single lexical token

```c
typedef struct s_token {
    t_token_type  type;   // what kind of token (see enum below)
    char         *value;  // raw string captured from input
    struct s_token *next; // singly-linked list
} t_token;
```

**Token types:**

| Enum value | Meaning | Example input |
|---|---|---|
| `TOKEN_WORD` | Any word, filename, flag | `echo`, `foo`, `-n` |
| `TOKEN_PIPE` | `\|` operator | `\|` |
| `TOKEN_REDIR_IN` | `<` redirect stdin | `< file` |
| `TOKEN_REDIR_OUT` | `>` redirect stdout | `> file` |
| `TOKEN_HEREDOC` | `<<` here-document | `<< EOF` |
| `TOKEN_APPEND` | `>>` append stdout | `>> file` |
| `TOKEN_AND` | `&&` logical AND *(bonus)* | `&&` |
| `TOKEN_OR` | `\|\|` logical OR *(bonus)* | `\|\|` |
| `TOKEN_LPAREN` | `(` grouping *(bonus)* | `(` |
| `TOKEN_RPAREN` | `)` grouping *(bonus)* | `)` |
| `TOKEN_EOF` | end of input | — |

### 2.2 `t_redir` — one redirection

```c
typedef struct s_redir {
    t_token_type  type;        // REDIR_IN / REDIR_OUT / HEREDOC / APPEND
    char         *file;        // target filename (or heredoc delimiter)
    int           heredoc_fd;  // read-end of pipe for heredoc; -1 otherwise
    struct s_redir *next;
} t_redir;
```

### 2.3 `t_cmd` — one command node (leaf of AST)

```c
typedef struct s_cmd {
    char    **args;    // NULL-terminated argv array  ["echo", "hello", NULL]
    t_redir  *redirs;  // linked list of redirections for this command
} t_cmd;
```

`args[0]` is always the command name (or `NULL` for a bare-redirection node).

### 2.4 `t_ast` — one node of the Abstract Syntax Tree

```c
typedef struct s_ast {
    t_node_type  type;   // NODE_CMD / NODE_PIPE / NODE_AND / NODE_OR
    t_cmd       *cmd;    // non-NULL only when type == NODE_CMD
    struct s_ast *left;
    struct s_ast *right;
} t_ast;
```

**Tree topology:**

- `NODE_CMD` → leaf node; `cmd` field holds command + redirections.
- `NODE_PIPE` → `left` is the writer side, `right` is the reader side.
- `NODE_AND` / `NODE_OR` → bonus; `left` runs first, `right` runs conditionally.

Example: `ls | grep foo | wc -l`

```
        NODE_PIPE
       /         \
  NODE_CMD     NODE_PIPE
  [ls]        /          \
         NODE_CMD     NODE_CMD
         [grep,foo]   [wc,-l]
```

### 2.5 `t_env` — one environment variable

```c
typedef struct s_env {
    char      *key;    // variable name e.g. "HOME"
    char      *value;  // variable value e.g. "/home/user"  (may be NULL)
    struct s_env *next;
} t_env;
```

**Why a linked list, not `char **envp`?**  
A linked list makes add / update / delete O(n) but conceptually simple, and
lets `export` add variables without re-allocating the whole array. `env_to_array`
converts it back to `char **` only when needed by `execve`.

### 2.6 `t_shell` — the global shell state

```c
typedef struct s_shell {
    t_env  *env;         // live environment list
    int     last_exit;   // last command's exit code ($?)
    char   *input;       // unused in current version (placeholder)
    int     in_heredoc;  // flag: currently reading heredoc
} t_shell;
```

### 2.7 `g_signal` — the only global variable

```c
extern int g_signal;
```

The subject mandates that **only one global variable** is permitted. It stores
the last received signal number (`SIGINT` = 2). Signal handlers write to it;
the main process reads it after `waitpid` to decide whether to print a newline.

---

## 3. Module: `main.c` — The REPL Loop

**Files:** `srcs/main.c`  
**Entry point:** `int main(int argc, char **argv, char **envp)`

### What it does

1. Initialises `t_shell` (calls `env_init(envp)` to copy the process environment).
2. Calls `main_loop` which loops forever reading one line at a time with
   `readline("minishell> ")`.
3. Passes each non-empty line to `run_line`, which wires together the entire
   pipeline: lex → parse → expand → execute.
4. On EOF (`readline` returns `NULL`), prints `"exit"` and breaks.
5. Cleans up with `env_free` and `rl_clear_history`.

### `run_line` step-by-step

```c
static void run_line(char *line, t_shell *shell)
{
    t_token *tokens = lexer(line);          // 1. tokenise
    if (!tokens) return;
    t_ast *ast = parser(tokens);            // 2. build AST
    free_tokens(tokens);                    // 3. tokens no longer needed
    if (!ast) return;
    expand_ast(ast, shell);                 // 4. expand $VAR and quotes
    shell->last_exit = execute(ast, shell); // 5. run it; capture exit code
    free_ast(ast);                          // 6. free the tree
}
```

### Why `add_history` only when `*line != '\0'`?

Empty strings should not pollute the history. bash behaves the same way.

### Exit code propagation

`shell->last_exit` is updated after every `execute` call. It is read when
`$?` is expanded and is also used as the default argument to `exit`.

---

## 4. Module: `lexer/` — Tokenisation

**Files:** `lexer.c`, `lexer_utils.c`, `lexer_word.c`  
**Public interface:** `t_token *lexer(char *input)`

### Responsibilities

Turn the raw input string into a singly-linked list of `t_token` nodes.
The lexer does **not** interpret meaning — it just identifies boundaries.

### `lexer()` algorithm

```
i = 0
while input[i]:
    skip spaces / tabs
    if is_operator(input[i]):
        tok = handle_operator(input, &i)
    else:
        tok = handle_word(input, &i)
    append tok to list
return head
```

### `handle_operator()` — `lexer_utils.c`

```
Check for two-character ops first (<<, >>)
    → TOKEN_HEREDOC or TOKEN_APPEND
Then single-character:
    | → TOKEN_PIPE
    < → TOKEN_REDIR_IN
    > → TOKEN_REDIR_OUT
```

*(Bonus version also handles `&&`, `||`, `(`, `)`.)*

### `handle_word()` — `lexer_word.c`

Advances `i` via `word_end()`:

```
while not space, not tab, not operator:
    if ' or " → skip the whole quoted segment (via quote_len)
    else       → advance one character
extract substring from start to end → new TOKEN_WORD
```

**Key insight:** Quotes are kept *intact* in the token value at this stage.
The expander removes them later. This separation of concerns is fundamental.

### `quote_len()` — `lexer_utils.c`

```c
int quote_len(char *s, int i)  // i points at opening ' or "
```

Counts characters until the matching close quote (inclusive). Used by
`word_end` to skip over quoted content without treating inner spaces or
operators as word boundaries.

**Example:**

```
input: echo "hello world"
tokens: TOKEN_WORD("echo")  TOKEN_WORD("\"hello world\"")
                                         ^ quotes kept ^
```

### Memory model

Every `new_token` duplicates the value string with `ft_strdup`. The list is
freed with `free_tokens` in `run_line` **after** the parser has finished —
the parser only reads token values, it does not take ownership.

---

## 5. Module: `parser/` — Abstract Syntax Tree

**Files:** `parser.c`, `parser_cmd.c`, `parser_entry.c`, `parser_redir.c`  
**Public interface:** `t_ast *parser(t_token *tokens)`

### Responsibilities

Convert the flat token list into a tree that represents operator precedence
and structure:

```
cmd1 | cmd2 | cmd3
→  NODE_PIPE(NODE_CMD(cmd1), NODE_PIPE(NODE_CMD(cmd2), NODE_CMD(cmd3)))
```

### Grammar (mandatory version)

```
pipeline  ::=  command  ( '|'  command )*
command   ::=  ( WORD | redir )*
redir     ::=  ( '<' | '>' | '<<' | '>>' )  WORD
```

### `parser()` — `parser_entry.c`

Simple entry point: takes a pointer to the first token and calls
`parse_pipeline`.

### `parse_pipeline()` — `parser.c`

Recursive descent, left-to-right:

```
left = parse_command(tokens)
if next token is '|':
    consume '|'
    pipe_node = NODE_PIPE
    pipe_node.left  = left
    pipe_node.right = parse_pipeline(tokens)   ← recursion handles n-way pipes
    return pipe_node
else:
    return left
```

### `parse_command()` — `parser_cmd.c`

```
count = count_word_tokens(current position)   // how many args?
cmd.args = malloc((count+1) * sizeof(char*))
call fill_cmd(cmd, tokens)
    → for each token until pipe / EOF:
        if it's a word  → copy to args[i++]
        if it's a redir → call make_redir → append to cmd.redirs
node = NODE_CMD; node.cmd = cmd
return node
```

### `make_redir()` — `parser_redir.c`

```
redir.type = current token type
advance token
redir.file = ft_strdup(next word token value)
advance token
return redir
```

### `free_ast()` — `parser_redir.c`

Recursive post-order traversal: free left subtree → free right subtree →
free the node itself (and its `t_cmd` if it's a `NODE_CMD`).

### Key concepts

- **Tokens are not consumed destructively.** The parser uses a `t_token **`
  pointer that it advances in place; the original list is freed separately.
- **Redirections can appear anywhere in a command.** `< file echo` is valid
  and equivalent to `echo < file`.

---

## 6. Module: `expander/` — Variable & Quote Expansion

**Files:** `expander.c`, `expander_args.c`, `expander_str.c`, `expander_utils.c`  
**Public interface:** `void expand_ast(t_ast *ast, t_shell *shell)`

### Responsibilities

Walk the AST and, for every `NODE_CMD` node:
1. Expand `$VAR` / `$?` in each argument string.
2. Remove surrounding quotes (`'...'`, `"..."`).
3. Discard arguments that were *only* variable references that expanded to empty.
4. Expand `$VAR` in redirection file targets (but **not** in heredoc delimiters).

### `expand_ast()` — `expander.c`

```
if node is NODE_CMD → expand_cmd(node.cmd, shell)
else                → recurse into left and right subtrees
```

### `expand_str()` — `expander_str.c`

The core engine. Processes a raw token value character-by-character:

```
result = ""
while str[i]:
    if str[i] == '\'' → expand_single_quote   (no expansion inside)
    if str[i] == '"'  → expand_double_quote   (expand $VAR inside)
    if str[i] == '$'  → expand_var
    else              → copy literal character
    result += part
return result
```

### `expand_single_quote()` — `expander_str.c`

- Skip the opening `'`.
- Copy everything verbatim until the closing `'`.
- No variable expansion, no escape sequences. `'$HOME'` → literal `$HOME`.

### `expand_double_quote()` — `expander_str.c`

- Skip the opening `"`.
- Inside: if `$` → call `expand_var`; otherwise copy character.
- `"$HOME"` → value of HOME.
- Wildcards (`*`) are **not** expanded inside double quotes in either version.
  In the bonus version the `\x01` masking technique enforces this: any `*` found
  inside a double-quoted segment is replaced with `\x01` so the wildcard expander
  (which runs later) never sees it as a glob character.
  In the mandatory version there is no wildcard expansion at all, so this is a
  non-issue.

### `expand_var()` — `expander_utils.c`

```
consume '$'
if next char is '?':  return ft_itoa(shell->last_exit)
else: read alphanum/_ identifier → name
      return get_var_value(name, shell)
if '$' is followed by non-identifier → return literal "$"
```

### `get_var_value()` — `expander_utils.c`

```
if name == "?" → ft_itoa(shell->last_exit)
else           → env_get(shell->env, name)   (returns "" if not found)
```

### `expand_args()` — `expander_args.c`

```
for each arg in cmd.args:
    orig  = arg
    expanded = expand_str(orig, shell)
    if expanded == "" AND orig was only variable references:
        discard expanded  ← e.g. "$UNSET_VAR" disappears
    else:
        cmd.args[j++] = expanded
    free(orig)
cmd.args[j] = NULL
```

**Why discard empty expansions?**  
In bash, `echo $UNSET` prints a blank line (`echo` with no arguments), but
`cmd $UNSET` would try to run a command with no name. Discarding the
*argument* (not the whole command) matches POSIX behaviour.

### `expand_redirs()` — `expander_args.c`

Same `expand_str` logic but applied to `redir->file`. Heredoc delimiters
(`TOKEN_HEREDOC`) are **skipped** — the shell must not expand the delimiter.

### `join_free()` — `expander_utils.c`

```c
char *join_free(char *s1, char *s2)
// ft_strjoin(s1, s2) then free both s1 and s2
```

Used everywhere in the expander to build strings incrementally without
memory leaks.

---

## 7. Module: `executor/` — Command Execution

**Files:** `executor.c`, `executor_cmd.c`, `executor_pipe.c`,
`executor_redir.c`, `executor_run.c`, `executor_utils.c`  
**Public interface:** `int execute(t_ast *ast, t_shell *shell)`

### Responsibilities

Walk the AST and carry out the semantics of each node.

### `execute()` — `executor_run.c`

```c
if ast->type == NODE_PIPE → exec_pipe(ast, shell)
if ast->type == NODE_CMD  → exec_cmd(ast, shell)
// bonus: NODE_AND / NODE_OR → exec_and_or(ast, shell)
```

### `exec_cmd()` — `executor_cmd.c`

Decision tree:

```
cmd = ast->cmd

Case 1: No args AND no redirs → no-op, return 0
Case 2: No args but has redirs → open heredocs, apply redirs only (exec_redir_only)
Case 3: args[0] is a builtin → exec_builtin_redir (runs builtin in parent process)
Case 4: external command → fork() → exec_child() in child; wait in parent
```

**Why builtins run in the parent process?**  
Builtins like `cd`, `export`, `unset`, and `exit` must modify the **parent**
shell's state (current directory, environment, process lifetime). Running them
in a fork would affect only the child, which would then exit and discard all
changes.

### `exec_child()` — `executor_utils.c`

```
setup_signals_child()     // reset SIGINT/SIGQUIT to defaults
apply_redirs(cmd->redirs) // set up stdin/stdout from redirection list
path = find_executable(cmd->args[0], shell->env)
if !path → print "command not found"; exit(127)
execve(path, cmd->args, env_to_array(shell->env))
// execve only returns on error:
if ENOENT → exit(127)
else      → exit(126)
```

### `exec_builtin_redir()` — `executor_utils.c`

Saves stdout/stdin with `dup`, applies redirections, runs the builtin,
then restores original fds. This is necessary when a builtin has redirections
(e.g., `echo hello > file`).

```
saved_in  = dup(STDIN_FILENO)
saved_out = dup(STDOUT_FILENO)
apply_redirs(cmd->redirs)
*ret = exec_builtin(cmd, shell)
dup2(saved_in,  STDIN_FILENO)    // restore
dup2(saved_out, STDOUT_FILENO)
close(saved_in); close(saved_out)
```

### `exec_pipe()` — `executor_pipe.c`

```
pipe(pipefd)
fork() → child_left:
    dup2(pipefd[1], STDOUT_FILENO)  // left side writes to pipe
    close both ends
    execute(ast->left, shell)
    exit(status)
fork() → child_right:
    dup2(pipefd[0], STDIN_FILENO)   // right side reads from pipe
    close both ends
    execute(ast->right, shell)
    exit(status)
parent: close both ends; waitpid x2; return exit code of right child
```

**Why two forks?**  
Both sides of a pipe can block (the writer blocks if the buffer fills, the
reader blocks waiting for data). Running them sequentially would deadlock.

### `apply_redirs()` — `executor_redir.c`

Iterates the `t_redir` list and applies each redirection in order:

| Type | Action |
|---|---|
| `TOKEN_REDIR_IN` | `open(file, O_RDONLY)` then `dup2(fd, STDIN)` |
| `TOKEN_REDIR_OUT` | `open(file, O_WRONLY\|O_CREAT\|O_TRUNC)` then `dup2(fd, STDOUT)` |
| `TOKEN_APPEND` | `open(file, O_WRONLY\|O_CREAT\|O_APPEND)` then `dup2(fd, STDOUT)` |
| `TOKEN_HEREDOC` | `dup2(redir->heredoc_fd, STDIN)` then `close(heredoc_fd)` |

Redirections are applied **in order** — last one wins for the same fd.

### `open_heredoc()` — `executor_redir.c`

```
pipe(pipefd)
setup_signals_heredoc()
loop:
    line = readline("> ")
    if line == NULL OR line == delimiter → break
    write(line + "\n" to pipefd[1])
close(pipefd[1])
redir->heredoc_fd = pipefd[0]  // consumer reads from this
```

**Why a pipe instead of a temp file?**  
Pipes are simpler, avoid filesystem side effects, and are automatically
freed when both ends are closed.

### `find_executable()` — `executor_utils.c`

```
if name contains '/' → return ft_strdup(name)  // absolute or relative path
get PATH from env
split PATH on ':'
for each dir:
    candidate = dir + "/" + name
    if access(candidate, X_OK) == 0 → return candidate
return NULL
```

### Exit code conventions

| Code | Meaning |
|---|---|
| 0 | Success |
| 1 | General error |
| 2 | Syntax error (bash convention) |
| 126 | Command found but not executable |
| 127 | Command not found |
| 128 + N | Killed by signal N (e.g., 130 = killed by SIGINT) |

---

## 8. Module: `builtins/` — Built-in Commands

**Files:** `builtin_cd.c`, `builtin_echo.c`, `builtin_env.c`,
`builtin_exit.c`, `builtin_export.c`, `builtin_pwd.c`, `builtin_unset.c`  
**Dispatcher:** `exec_builtin()` in `executor.c`

All builtins run in the **parent** shell process (except when they appear on
the non-last position of a pipeline, in which case they run in a child — but
the mandatory subject does not require this edge case).

### `builtin_echo` — `builtin_echo.c`

```
if args[1] == "-n" → newline = 0; start printing from args[2]
else               → newline = 1; start from args[1]
print all remaining args separated by ' '
if newline → print '\n'
return 0
```

**Edge cases:** Only checks the first `-n`; multiple `-n` flags (e.g., `-n -n`)
are not required by the subject to suppress the newline repeatedly.

### `builtin_cd` — `builtin_cd.c`

```
if no argument → cd_home(): chdir(env_get("HOME"))
if > 1 argument → error "too many arguments"
else → chdir(args[1])
on success → update_pwd():
    OLDPWD = current PWD
    PWD    = getcwd()
```

### `builtin_pwd` — `builtin_pwd.c`

```
getcwd(buf, 4096)
print buf + '\n'
```

### `builtin_export` — `builtin_export.c`

```
if no arguments → print_export(): print "declare -x KEY="VALUE"" for each env var
else:
    for each argument:
        if contains '=':
            key  = substring before '='
            value = substring after '='
            validate key (must start with letter/_; only alnum/_)
            env_set(&shell->env, key, value)
        else:
            validate key
            env_set(&shell->env, key, NULL)  // export without value
```

**`declare -x` format:** this is what `bash` uses when you run `export` with no
arguments. The value is printed in double quotes.

### `builtin_unset` — `builtin_unset.c`

```
for each argument i:
    env_unset(&shell->env, args[i])
```

No error for non-existent variables.

### `builtin_env` — `builtin_env.c`

```
for each node in shell->env:
    if node->value != NULL:   // only print variables that have been assigned a value
        print "KEY=VALUE\n"
```

Variables added with `export VAR` (no `=`) are not shown.

### `builtin_exit` — `builtin_exit.c`

```
print "exit\n"
if no argument     → exit with shell->last_exit
if > 1 argument    → print error; return 1  (stay in shell)
if not all digits  → print error; exit(2)
else               → code = atoi(args[1]) & 0xFF; exit(code)
```

`& 0xFF` clamps the exit code to 0–255, mirroring bash behaviour.

---

## 9. Module: `env/` — Environment Management

**Files:** `env.c`, `env_utils.c`  
**Public interface:** `env_init`, `env_get`, `env_set`, `env_unset`,
`env_to_array`, `env_free`, `env_find`, `env_new`, `env_parse_entry`

### Internal representation

The environment is a singly-linked list of `t_env` nodes.

```
shell.env → [PATH="/usr/bin:/bin"] → [HOME="/home/user"] → [USER="user"] → NULL
```

### `env_init(char **envp)` — `env.c`

Iterates `envp` (the `char **` passed to `main`), parses each `"KEY=VALUE"`
string with `env_parse_entry`, and builds the linked list.

### `env_parse_entry(char *entry)` — `env_utils.c`

```
eq = strchr(entry, '=')
key   = substr(entry, 0, eq - entry)
value = strdup(eq + 1)
return env_new(key, value)
```

Entries without `=` (rare in practice when loading from the process environment)
are silently ignored (`return NULL`). This only applies to `env_init`'s initial
population from `envp`. At runtime, `export VAR` (no `=`) uses `env_set` directly
with a `NULL` value, which is a different code path that *does* create a list node.

### `env_set(t_env **env, char *key, char *value)` — `env_utils.c`

```
node = env_find(*env, key)
if found:
    free(node->value)
    node->value = strdup(value)   // NULL if value is NULL
else:
    new = env_new(key, value)
    new->next = *env              // prepend — O(1)
    *env = new
```

**Important:** New variables are prepended to the list, not appended. This
does not affect correctness but means the order differs from bash.

### `env_unset(t_env **env, char *key)` — `env_utils.c`

Standard singly-linked-list removal: walks with `curr`/`prev` pair.

### `env_to_array(t_env *env)` — `env_utils.c`

```
count all nodes
arr = malloc((count+1) * sizeof(char*))
for each node: arr[i] = join(key, "=", value)
arr[count] = NULL
return arr
```

This array is passed to `execve`. It is a **fresh allocation** every time and
must be freed by the caller.

### `env_get(t_env *env, char *key)` — `env.c`

Returns `node->value` directly — **no copy**. Callers must not free the result.

---

## 10. Module: `signals/` — Signal Handling

**Files:** `signals.c`  
**Public interface:** `setup_signals`, `setup_signals_child`, `setup_signals_heredoc`

### Why signals are tricky in a shell

A shell has at least three signal contexts:

| Context | SIGINT should… | SIGQUIT should… |
|---|---|---|
| Interactive (waiting for input) | cancel the line, redisplay prompt | be ignored |
| Running a child process | propagate to child | propagate to child |
| Reading heredoc | cancel the heredoc | be ignored |

### `sig_handler(int sig)` — interactive mode

```c
void sig_handler(int sig) {
    g_signal = sig;
    write(STDOUT_FILENO, "\n", 1);
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}
```

1. Records signal in `g_signal`.
2. Moves to new line.
3. Clears the current readline buffer.
4. Redisplays the prompt — so the user sees `minishell> ` again.

**Why `write` instead of `printf` in a signal handler?**  
`printf` is not async-signal-safe. `write` is.

### `setup_signals()` — interactive mode

```c
sigaction(SIGINT,  &sa_handler, NULL);  // → sig_handler
signal(SIGQUIT, SIG_IGN);               // ignore Ctrl-\
```

Uses `sigaction` with `SA_RESTART` so that `readline` restarts automatically
after receiving SIGINT rather than returning `EINTR`.

### `setup_signals_child()` — child processes

```c
signal(SIGINT,  SIG_DFL);   // default: terminate
signal(SIGQUIT, SIG_DFL);   // default: core dump (Ctrl-\)
```

Children should behave like normal Unix processes.

### `setup_signals_heredoc()` — heredoc reading

```c
void sig_heredoc(int sig) {
    g_signal = sig;
    write(STDOUT_FILENO, "\n", 1);
    close(STDIN_FILENO);   // makes readline return NULL → exits the loop
}
```

Closing stdin causes `readline("> ")` to return `NULL`, which the
`open_heredoc` loop treats as EOF, terminating the heredoc gracefully.

---

## 11. Module: `utils/` — Helper Utilities

**Files:** `free.c`, `utils.c`

### `utils.c`

| Function | Purpose |
|---|---|
| `ft_malloc(size)` | `malloc` that calls `fatal_error` on NULL |
| `ft_isspace(c)` | Returns true for space, tab, newline, carriage-return, form-feed, vertical-tab |
| `free_array(char **arr)` | Frees each string then the array itself |
| `array_len(char **arr)` | Counts NULL-terminated string array |
| `fatal_error(char *msg)` | Prints to stderr and `exit(1)` |

### `free.c`

| Function | Purpose |
|---|---|
| `free_shell(t_shell *)` | Frees `env` and `input` fields |
| `ft_strjoin3(s1, s2, s3)` | Joins three strings: `ft_strjoin(s1,s2)` then `ft_strjoin(tmp,s3)` |

### Memory ownership rules (important for defence)

| Allocation | Owner / When freed |
|---|---|
| `readline` return value | Caller (`run_line`) after use |
| Token list | `free_tokens` in `run_line` after parsing |
| AST | `free_ast` in `run_line` after execution |
| `env_to_array` result | The `exec_child_run` call site (implicitly freed by `execve` or process exit) |
| `expand_str` result | Replaces the original arg string; original is freed |
| Heredoc pipe fd | Closed in `apply_redirs` or `free_redir` |

---

## 12. Bonus Features (`srcs_bonus/`)

The bonus compilation replaces or adds to several mandatory modules:

| Bonus file | Replaces or extends |
|---|---|
| `main_bonus.c` | `srcs/main.c` — adds `check_syntax` call |
| `lexer_utils_bonus.c` | `srcs/lexer/lexer_utils.c` — adds `&&`, `\|\|`, `(`, `)`, bare `&` |
| `parser_entry_bonus.c` | `srcs/parser/parser_entry.c` — calls `parse_and_or` instead of `parse_pipeline` |
| `parser_bonus.c` | **new** — `check_syntax` and the `parse_andor` support functions |
| `parse_andor_bonus.c` | **new** — `parse_and_or`, `parse_parens`, `make_and_or` |
| `executor_bonus.c` | `srcs/executor/executor_run.c` — `execute` dispatches `NODE_AND`/`NODE_OR` |
| `expander_bonus.c` | `srcs/expander/expander.c` — calls wildcard expansion after variable expansion |
| `expander_str_bonus.c` | `srcs/expander/expander_str.c` — masks `*` inside quotes |
| `wildcard_bonus.c` | **new** — `expand_wildcard`: opens `.`, filters with `match_wildcard` |
| `wildcard_expand_bonus.c` | **new** — `expand_wildcards_cmd`: replaces wildcard args |
| `wildcard_match_bonus.c` | **new** — `match_wildcard`, `unmask_wildcards` |
| `wildcard_redir_bonus.c` | **new** — `expand_wildcard_redir`: wildcards in redirect targets |

### Bonus grammar

```
and_or   ::=  pipeline  ( ('&&' | '||')  pipeline )*
pipeline ::=  command   ( '|'  command )*
           |  '(' and_or ')'
```

### Wildcard expansion flow

```
expand_ast()
  └─ expand_cmd()
       ├─ expand_args()           ← variable expansion
       ├─ expand_redirs()         ← variable expansion on file targets
       ├─ expand_wildcards_cmd()  ← wildcard expansion on args
       └─ expand_wildcard_redir() ← wildcard expansion on file targets
```

### Wildcard masking trick

Inside single-quoted or double-quoted strings, `*` must **not** expand.
The bonus expander replaces `*` with the byte `\x01` (a non-printable
character) during quote processing. After variable expansion, `*` bytes
remaining in the string are real wildcards. After wildcard expansion,
`unmask_wildcards` replaces `\x01` back to `*` in any string that was not
expanded (e.g., if no files matched, the literal pattern is kept).

### `match_wildcard(pattern, str)` — `wildcard_match_bonus.c`

Recursive matching:

```
if *pattern == '\0' → return *str == '\0'
if *pattern == '*':
    try matching pattern+1 against str, str+1, str+2, ...
    return true if any matches
else:
    if *str != *pattern → false
    recurse on pattern+1, str+1
```

This is an O(n·m) recursive algorithm (n = pattern length, m = string length)
suitable for the typical small filenames encountered in practice.

### `check_syntax(t_token *tokens)` — `parser_bonus.c`

Validates the token list before parsing:

| Rule | Example violation |
|---|---|
| Must not start with an operator | `| cmd` |
| Redirection must be followed by a word | `> >`, `>` at end |
| Two consecutive operators | `\|\|&&`, `&&\|\|` |
| Operator at end of input | `cmd \|` |
| Bare `&` (not `&&`) | `cmd & cmd` |

Returns 1 on error (sets `shell->last_exit = 2`), 0 on clean syntax.

---

## 13. End-to-End Execution Flow

### Example: `export GREETING="hello world" && echo $GREETING | cat`

**Step 1 — Lex:**

```
TOKEN_WORD("export")  TOKEN_WORD("GREETING=\"hello world\"")
TOKEN_AND("&&")
TOKEN_WORD("echo")    TOKEN_WORD("$GREETING")
TOKEN_PIPE("|")
TOKEN_WORD("cat")
```

**Step 2 — Parse (bonus):**

```
NODE_AND
├─ left:  NODE_CMD  [export, GREETING="hello world"]
└─ right: NODE_PIPE
          ├─ left:  NODE_CMD  [echo, $GREETING]
          └─ right: NODE_CMD  [cat]
```

**Step 3 — Expand:**

```
expand_ast() is called on the ENTIRE AST before execute() is called at all.
At expansion time, export has not run yet, so GREETING is not in the environment.

NODE_AND — no direct args; recurse into children
├─ NODE_CMD: args = ["export", "GREETING=hello world"]
│            (quotes removed by expand_str)
└─ NODE_PIPE
   ├─ NODE_CMD: expand "$GREETING" → "" (GREETING not yet in env at expand time)
   │            empty expansion → args discarded → args = ["echo"] only
   └─ NODE_CMD: args = ["cat"]
```

*(Because `expand_ast` walks the **whole tree** before `execute` is invoked,
`$GREETING` is resolved before the `export` command has had a chance to run.
This is a consequence of the implementation's ordering: lex → parse → expand →
execute as one sequential pass over each input line. It also matches standard
POSIX shell behaviour: variables assigned in the same command line are not
visible to other words that are expanded on the same line.)*

**Step 4 — Execute:**

```
exec_and_or(NODE_AND):
    left_status = execute(NODE_CMD [export …])
        → builtin_export: env_set("GREETING", "hello world")
        → returns 0
    left_status == 0 AND NODE_AND → execute right side
    execute(NODE_PIPE)
        fork child_left:  exec_cmd([echo]) → prints "" + newline
        fork child_right: exec_cmd([cat])  → reads from pipe, prints ""
        wait both; return 0
```

---

## 14. Inter-Module Dependency Map

```
main.c
  ├── lexer()           [lexer/]
  ├── parser()          [parser/]
  ├── expand_ast()      [expander/]
  │     ├── expand_str()
  │     │     └── expand_var()
  │     │           └── env_get()   [env/]
  │     └── expand_redirs()
  ├── execute()         [executor/]
  │     ├── exec_pipe()
  │     │     └── execute() (recursive)
  │     ├── exec_cmd()
  │     │     ├── open_heredocs()
  │     │     │     └── open_heredoc()
  │     │     ├── is_builtin() + exec_builtin()  → [builtins/]
  │     │     │     └── env_get/set/unset()      → [env/]
  │     │     └── exec_child()
  │     │           ├── apply_redirs()
  │     │           ├── find_executable()
  │     │           │     └── env_get("PATH")    → [env/]
  │     │           └── execve(path, args, env_to_array())
  │     │                                         → [env/]
  │     └── exec_and_or() [bonus]
  ├── setup_signals()   [signals/]
  └── env_init/free()   [env/]
```

Every module uses `libft` functions (`ft_strdup`, `ft_strcmp`, `ft_split`,
etc.) and the utility functions from `utils/`.

---

## 15. Defense Q&A — Common Questions & Model Answers

### General architecture

**Q: Why is the shell split into lexer / parser / expander / executor stages?**  
A: Separation of concerns. Each stage has a single responsibility and a clear
interface. The lexer only identifies tokens; the parser only builds structure;
the expander only resolves variables and quotes; the executor only runs
processes. This makes each stage testable independently and follows how
production shells like bash/zsh work.

**Q: What is an AST and why use one?**  
A: An Abstract Syntax Tree is a tree where each node represents a grammatical
construct. Pipes and AND/OR operators become inner nodes; commands become
leaves. The tree naturally encodes operator precedence and associativity,
and it can be evaluated by a simple recursive traversal.

**Q: Why does the lexer keep quote characters in the token value?**  
A: Because the meaning of quotes is not a lexical concern — it's an expansion
concern. The lexer's only job is identifying word boundaries. The expander then
processes the quotes and applies the correct rules (no expansion inside single
quotes, variable expansion inside double quotes).

---

### Tokenisation

**Q: How does the lexer handle `echo "hello world"` — does it produce one or two tokens?**  
A: One `TOKEN_WORD` with value `"hello world"` (quotes included). The `word_end`
function skips over the quoted segment as a single unit via `quote_len`.

**Q: What is `quote_len` for and how does it work?**  
A: It calculates the total character count of a quoted segment including the
delimiters. Starting at an opening `'` or `"`, it scans forward until the
matching close quote and returns the count. `word_end` uses this to advance
the index past the entire quoted fragment without treating inner spaces or
operators as word separators.

---

### Parsing

**Q: How are multi-stage pipelines handled (e.g., `a | b | c`)?**  
A: `parse_pipeline` is recursive. After parsing `a`, it sees `|`, then
recursively calls itself to parse `b | c`. This builds a right-associative
tree: `PIPE(a, PIPE(b, c))`. During execution, the recursion naturally
forks pairs of processes.

**Q: Can redirections appear before the command name? e.g., `< input.txt cat`**  
A: Yes. `fill_cmd` processes tokens in order; any `TOKEN_REDIR_*` is
immediately consumed into the redirection list regardless of its position
relative to the command name. So `< input.txt cat` and `cat < input.txt`
produce identical AST nodes.

---

### Expansion

**Q: What happens when an unset variable like `$UNSET` appears as a standalone argument?**  
A: `expand_str` returns an empty string. `expand_args` then checks `is_only_vars`:
if the original token consisted *entirely* of variable references that all
resolved to empty, the argument is dropped. So `echo $UNSET` becomes `echo`
(with no extra argument), which prints a blank line.

**Q: What is the difference between `'$HOME'` and `"$HOME"` in expansion?**  
A: Single quotes prevent all expansion; `'$HOME'` → literal string `$HOME`.
Double quotes allow variable expansion; `"$HOME"` → value of HOME, e.g.
`/home/user`.

**Q: How is `$?` expanded?**  
A: When `expand_var` sees `?` after `$`, it returns `ft_itoa(shell->last_exit)`
instead of looking up the environment. `shell->last_exit` is updated after
every command execution.

---

### Execution

**Q: Why do builtins (`cd`, `export`, etc.) run in the parent process?**  
A: Because they must change the parent shell's state. `cd` changes the working
directory; `export`/`unset` modify the environment list; `exit` terminates the
shell. If run in a child fork, those changes would be lost when the child exits.

**Q: How does a pipe between two processes work at the OS level?**  
A: `pipe(pipefd)` creates two file descriptors: `pipefd[1]` (write end) and
`pipefd[0]` (read end). The left child `dup2`s its stdout to `pipefd[1]`; the
right child `dup2`s its stdin to `pipefd[0]`. Both then close the original
pipe fds. The kernel buffers data between them.

**Q: Why fork twice for a pipe instead of once?**  
A: To avoid deadlocks. If the left command produces more data than the kernel
pipe buffer (~64 KB), its `write` calls will block until the right side reads
data. Running both sides concurrently (two forks) prevents this deadlock.

**Q: What exit code does a process killed by SIGINT get?**  
A: `128 + SIGINT = 128 + 2 = 130`. The executor code `WIFSIGNALED(status)` /
`WTERMSIG(status)` extracts the signal number and adds 128.

**Q: How are heredocs implemented?**  
A: A `pipe()` is created. `open_heredoc` reads lines from `readline("> ")` and
writes them to `pipefd[1]`, stopping when the delimiter is matched or EOF occurs.
`pipefd[1]` is then closed and `pipefd[0]` is stored in `redir->heredoc_fd`.
When `apply_redirs` processes this heredoc, it `dup2`s `heredoc_fd` to stdin.

**Q: What does `exec_builtin_redir` do and why is it needed?**  
A: It lets builtins have redirections (e.g., `echo hello > file`). Since
builtins run in the parent, their stdout/stdin must be temporarily redirected.
The function saves the original file descriptors with `dup`, applies
redirections, runs the builtin, then restores the original fds.

---

### Environment

**Q: How is the environment stored and why not use the raw `char **envp`?**  
A: As a linked list of `t_env` nodes. This allows O(1) prepend for `export` and
clean O(n) deletion for `unset`, without needing to shift array elements or
reallocate the whole array. `env_to_array` converts back to `char **` only
when `execve` requires it.

**Q: What is the difference between `env_set` with `NULL` value and `env_unset`?**  
A: `env_set(env, "VAR", NULL)` marks the variable as exported but without a
value — `export VAR` without `=`. The variable exists in the list but
`env->value` is `NULL`. `builtin_env` skips nodes with `NULL` value.
`env_unset` removes the node entirely from the list.

---

### Signals

**Q: Why is `g_signal` the only global variable?**  
A: The 42 subject rules mandate it. Signal handlers can only reliably
communicate with the main program through a global (or `volatile sig_atomic_t`)
variable. All other state is encapsulated in `t_shell`.

**Q: What does `SA_RESTART` do in `setup_signals`?**  
A: It tells the kernel to automatically restart interrupted system calls after
the signal handler returns, instead of returning `EINTR`. This prevents
`readline` from returning prematurely when Ctrl-C is pressed.

**Q: How does the shell know to print a newline after a child is killed by SIGINT?**  
A: In `wait_for_child`, after `waitpid` returns, the code checks `g_signal`:
if it equals `SIGINT`, it writes `"\n"` to stdout and then resets `g_signal`
to 0. The child's terminal output stopped mid-line when it was killed, so
the newline corrects the display.

---

### Bonus

**Q: How does `&&` work in execution?**  
A: `exec_and_or(NODE_AND)` runs the left side; if it returns 0 (success), it
runs the right side. If left returns non-zero, the right side is skipped.
This short-circuit evaluation matches bash semantics.

**Q: How does wildcard `*` expansion avoid expanding `*` inside quotes?**  
A: The bonus expander replaces `*` characters with a non-printable byte
`\x01` (the masking step) inside any quoted segment during `expand_str`. After
wildcard expansion, `unmask_wildcards` replaces `\x01` back to `*` in
unmatched patterns. This way only unquoted `*` characters trigger globbing.

**Q: What happens if a wildcard matches multiple files in a redirection (e.g., `> *.txt`)?**  
A: `expand_wildcard_redir` calls `expand_wildcard`. If exactly one file
matches, `redir->file` is replaced with that filename. If more than one
file matches, it prints an "ambiguous redirect" error and sets `redir->file`
to `NULL`, which causes `apply_redirs` to fail with an error.

---

## 16. Quick-Reference Cheat Sheet

### Execution pipeline in one sentence per step

| Step | Function | Input → Output |
|---|---|---|
| 1. Lex | `lexer(line)` | `char *` → `t_token *` list |
| 2. Parse | `parser(tokens)` | `t_token *` → `t_ast *` tree |
| 3. Expand | `expand_ast(ast, shell)` | modifies AST in-place |
| 4. Execute | `execute(ast, shell)` | walks AST; returns exit code |

### Token type cheat sheet

```
TOKEN_WORD      → any word, flag, filename
TOKEN_PIPE      → |
TOKEN_REDIR_IN  → <
TOKEN_REDIR_OUT → >
TOKEN_HEREDOC   → <<
TOKEN_APPEND    → >>
TOKEN_AND       → &&  (bonus)
TOKEN_OR        → ||  (bonus)
TOKEN_LPAREN    → (   (bonus)
TOKEN_RPAREN    → )   (bonus)
```

### AST node types

```
NODE_CMD  → leaf, has t_cmd* (args + redirs)
NODE_PIPE → binary, left | right
NODE_AND  → binary, left && right (bonus)
NODE_OR   → binary, left || right (bonus)
```

### Builtin list and special behaviours

| Builtin | Runs in parent? | Modifies shell state? |
|---|---|---|
| `echo [-n]` | Yes | No |
| `cd [path]` | Yes | Yes — changes `PWD`, `OLDPWD` |
| `pwd` | Yes | No |
| `export [K=V]` | Yes | Yes — modifies env list |
| `unset K` | Yes | Yes — modifies env list |
| `env` | Yes | No |
| `exit [n]` | Yes | Yes — terminates shell |

### Signal contexts

| Context | SIGINT | SIGQUIT |
|---|---|---|
| Waiting for input | `sig_handler` (new prompt) | Ignored |
| Running child | `SIG_DFL` (kill child) | `SIG_DFL` |
| Reading heredoc | `sig_heredoc` (close stdin) | Ignored |

### Exit codes

```
0      → success
1      → general failure
2      → syntax error / misuse (bonus)
126    → command found, not executable
127    → command not found
128+N  → killed by signal N  (e.g., 130 = Ctrl-C)
```

### Memory: who frees what

```
readline()       → free(line) in main_loop after run_line
lexer()          → free_tokens(tokens) in run_line after parser
parser()         → free_ast(ast) in run_line after execute
env_to_array()   → freed on process exit (child) / implicit
expand_str()     → returned string replaces original arg; original freed
heredoc_fd       → closed in apply_redirs or free_redir
```

### Key system calls used

```
fork()     → creates a child process
execve()   → replaces child image with new program
pipe()     → creates a unidirectional data channel between processes
dup2()     → redirects a file descriptor to another
waitpid()  → parent waits for a specific child
open()     → opens a file for redirection
close()    → closes a file descriptor
chdir()    → changes working directory  (cd builtin)
getcwd()   → gets current working directory (pwd builtin)
access()   → checks if file is executable (find_executable)
readline() → reads a line with editing + history
sigaction()→ installs a signal handler
```

---

*End of Study Guide*  
*Good luck in your defence — you've got this!*
