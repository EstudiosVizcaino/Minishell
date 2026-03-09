*Este proyecto ha sido creado como parte del currículo de 42 por estudiosvizcaino*

# Minishell

## Description

Minishell is a simple UNIX shell implemented in C, following the 42 School curriculum requirements. It supports command execution, pipes, redirections, environment variable expansion, signal handling, and a set of built-in commands, all conforming to POSIX standards.

## Instructions

### Requirements

#### Linux (Debian/Ubuntu)
- cc compiler
- GNU make
- readline library: `sudo apt-get install libreadline-dev`

#### macOS
- Xcode Command Line Tools: `xcode-select --install`
- [Homebrew](https://brew.sh)
- GNU readline via Homebrew: `brew install readline`

> The Makefile auto-detects macOS and resolves the Homebrew readline path (works on
> both Intel `/usr/local/opt/readline` and Apple Silicon `/opt/homebrew/opt/readline`).

### Compilation
```sh
make
```

### Running
```sh
./minishell
```

### Extremely detailed step-by-step explanation (HTML)
- Open `docs/index.html` in your browser.
- This document explains, in detail and step by step, how Minishell works from input reading to execution.
- Additional focused walkthrough file: `docs/study-step-by-step.html`.

### Built-in commands
- `echo [-n]` – Print text to stdout
- `cd <path>` – Change directory
- `pwd` – Print working directory
- `export [KEY=VALUE]` – Set environment variable
- `unset KEY` – Remove environment variable
- `env` – Print environment variables
- `exit [n]` – Exit the shell

### Bonus features
- `&&` and `||` logical operators
- `()` parenthesis for priority
- `*` wildcard expansion in current directory

Build with bonus:
```sh
make bonus
```

## Resources

### AI Usage
This project was developed with AI assistance (GitHub Copilot / ChatGPT) for architectural design, code generation, and debugging. All AI-generated code was reviewed and validated manually by the project authors to ensure correctness and Norminette compliance.
