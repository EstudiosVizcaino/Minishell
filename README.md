*Este proyecto ha sido creado como parte del currículo de 42 por estudiosvizcaino*

# Minishell

## Description

Minishell is a simple UNIX shell implemented in C as part of the 42 School curriculum. The goal of the project is to recreate the core functionality of a POSIX-compliant shell — similar to Bash — from scratch.

The shell supports interactive command input with history, execution of external binaries via `PATH` resolution or explicit paths, a full set of built-in commands, I/O redirections, pipes, environment variable expansion, and correct signal handling. The bonus part extends the shell with logical operators (`&&`, `||`), sub-shell grouping with parentheses, and wildcard (`*`) expansion in the current directory.

### Key features

- **Prompt & history** – displays a prompt and keeps a command history (navigable with arrow keys)
- **Command execution** – resolves executables using `PATH`, relative paths, or absolute paths
- **Built-in commands** – `echo`, `cd`, `pwd`, `export`, `unset`, `env`, `exit`
- **Redirections** – `<` (input), `>` (output), `>>` (append), `<<` (here-doc)
- **Pipes** – chains commands with `|`
- **Variable expansion** – `$VAR` and `$?` (exit status of last command)
- **Quoting** – single quotes preserve literals; double quotes preserve literals except `$`
- **Signals** – `Ctrl-C` (new prompt), `Ctrl-D` (exit), `Ctrl-\` (ignored in interactive mode)
- **Bonus** – `&&` / `||` operators, `()` grouping, `*` glob expansion

## Instructions

### Requirements

Linux (Debian/Ubuntu):
- `cc` compiler
- GNU `make`
- readline development library:

```sh
sudo apt-get install libreadline-dev
```

### Compilation

Build the mandatory part:
```sh
make
```

Build with bonus features (`&&`, `||`, `()`, `*`):
```sh
make bonus
```

### Running

```sh
./minishell
```

The shell starts in interactive mode. Type commands just like in Bash.

### Usage examples

```sh
# Basic command
$ echo "Hello, World!"
Hello, World!

# Pipe
$ ls -la | grep ".c"

# Redirections
$ echo "log entry" >> log.txt
$ cat < log.txt

# Here-doc
$ cat << EOF
> line one
> EOF

# Variable expansion
$ export MY_VAR=42
$ echo $MY_VAR
42
$ echo $?
0

# Bonus – logical operators and grouping
$ (echo "a" && echo "b") || echo "c"
a
b

# Bonus – wildcard expansion
$ ls *.c
```

### Built-in commands

| Command | Description |
|---|---|
| `echo [-n] [string]` | Print text to stdout; `-n` suppresses the trailing newline |
| `cd [path]` | Change the current working directory |
| `pwd` | Print the current working directory |
| `export [KEY=VALUE]` | Set or display environment variables |
| `unset KEY` | Remove an environment variable |
| `env` | Print all environment variables |
| `exit [n]` | Exit the shell with optional exit code |

## Resources

### References

- [Bash Reference Manual](https://www.gnu.org/software/bash/manual/bash.html) – authoritative guide to Bash behaviour used as the reference implementation
- [GNU readline library](https://tiswww.case.edu/php/chet/readline/rltop.html) – documentation for the readline API used for line editing and history
- [`man 2 execve`](https://man7.org/linux/man-pages/man2/execve.2.html) – Linux manual page for `execve`
- [`man 2 fork`](https://man7.org/linux/man-pages/man2/fork.2.html) – Linux manual page for `fork`
- [`man 2 pipe`](https://man7.org/linux/man-pages/man2/pipe.2.html) – Linux manual page for `pipe`
- [`man 7 signal`](https://man7.org/linux/man-pages/man7/signal.7.html) – Linux manual page for signals
- [Writing a Unix Shell – Part I](https://indradhanush.github.io/blog/writing-a-unix-shell-part-1/) – tutorial series on building a shell step by step
- [The Architecture of Open Source Applications – The Bash Shell](http://www.aosabook.org/en/bash.html) – high-level design overview of Bash internals

### AI Usage

AI tools were used in this project for the following purposes:

- **Testing** – to help create and run test cases that verify correct shell behaviour
- **Documentation** – for drafting parts of the inline code documentation
- **Understanding** – to help understand specific aspects of the codebase and underlying system concepts

All AI-assisted content was reviewed, tested, and validated manually by the project authors.
