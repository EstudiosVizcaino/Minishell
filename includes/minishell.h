/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 21:35:34 by cvizcain          #+#    #+#             */
/*   Updated: 2026/02/25 14:47:37 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <errno.h>
# include <signal.h>
# include <dirent.h>
# include <termios.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "../libft/libft.h"

/**
 * @brief Token kind produced by the lexer.
 *
 * Classifies every syntactic unit in the raw input
 * so the parser knows how to handle each token.
 */
typedef enum e_token_type
{
	/* Regular word or argument. */
	TOKEN_WORD,
	/* Pipe operator (|). */
	TOKEN_PIPE,
	/* Input redirection (<). */
	TOKEN_REDIR_IN,
	/* Output redirection (>). */
	TOKEN_REDIR_OUT,
	/* Here-document operator (<<). */
	TOKEN_HEREDOC,
	/* Append output redirection (>>). */
	TOKEN_APPEND,
	/* Logical AND operator (&&). */
	TOKEN_AND,
	/* Logical OR operator (||). */
	TOKEN_OR,
	/* Left parenthesis for subshell. */
	TOKEN_LPAREN,
	/* Right parenthesis for subshell. */
	TOKEN_RPAREN,
	/* End-of-input sentinel. */
	TOKEN_EOF
}	t_token_type;

/**
 * @brief Lexer token node in a singly-linked list.
 *
 * Stores the type and raw string value of one
 * syntactic unit from the input line.
 */
typedef struct s_token
{
	/* Classification of this token (WORD, PIPE, etc.). */
	t_token_type		type;
	/* Raw string content of the token. */
	char				*value;
	/* Next token in the list. */
	struct s_token		*next;
}	t_token;

/**
 * @brief Redirection node in a singly-linked list.
 *
 * Holds type, target file, and heredoc state for
 * one redirection attached to a command.
 */
typedef struct s_redir
{
	/* Redirection kind: TOKEN_REDIR_IN/OUT/HEREDOC/APPEND. */
	t_token_type		type;
	/* Target filename or heredoc delimiter string. */
	char				*file;
	/* Read-end fd of the heredoc pipe, or -1 if unused. */
	int					heredoc_fd;
	/* 1 if the heredoc delimiter was quoted (no expansion). */
	int					quoted;
	/* Next redirection in the list. */
	struct s_redir		*next;
}	t_redir;

/**
 * @brief Simple command with arguments and redirs.
 *
 * args[0] is the command name. redirs points to
 * the head of the redirection list.
 */
typedef struct s_cmd
{
	/* NULL-terminated array of arguments; args[0] is the command name. */
	char				**args;
	/* Head of the redirection list for this command. */
	t_redir				*redirs;
}	t_cmd;

/**
 * @brief AST node type enumeration.
 *
 * Identifies what each node in the Abstract Syntax
 * Tree represents during parsing and execution.
 */
typedef enum e_node_type
{
	/* Leaf node: a simple command. */
	NODE_CMD,
	/* Branch node: pipe between two commands. */
	NODE_PIPE,
	/* Branch node: logical AND (&&) operator. */
	NODE_AND,
	/* Branch node: logical OR (||) operator. */
	NODE_OR,
	/* Branch node: subshell group ((...)) . */
	NODE_SUBSHELL
}	t_node_type;

/**
 * @brief Abstract Syntax Tree node.
 *
 * Represents one node in the command tree built by
 * the parser. Leaves are commands; branches are
 * pipes or logical operators.
 */
typedef struct s_ast
{
	/* Kind of node: CMD, PIPE, AND, OR, or SUBSHELL. */
	t_node_type			type;
	/* Pointer to the command data (set for NODE_CMD only). */
	t_cmd				*cmd;
	/* Left child: first operand or the pipe's left side. */
	struct s_ast		*left;
	/* Right child: second operand or the pipe's right side. */
	struct s_ast		*right;
}	t_ast;

/**
 * @brief Environment variable node in a linked list.
 *
 * Stores one key=value pair from the shell's
 * environment, used to look up and export variables.
 */
typedef struct s_env
{
	/* Environment variable name (key part of key=value). */
	char				*key;
	/* Environment variable value; may be NULL if unset. */
	char				*value;
	/* Next variable node in the linked list. */
	struct s_env		*next;
}	t_env;

/**
 * @brief Top-level shell state structure.
 *
 * Passed through the whole pipeline so every module
 * can access the environment, last exit code, and
 * the current AST.
 */
typedef struct s_shell
{
	/* Head of the environment variable linked list. */
	t_env				*env;
	/* Exit code of the most recently executed command. */
	int					last_exit;
	/* Raw input string returned by readline each cycle. */
	char				*input;
	/* Non-zero while the shell is reading a heredoc body. */
	int					in_heredoc;
	/* Root of the AST for the current command line. */
	t_ast				*ast;
}	t_shell;

extern int	g_signal;

/* Lexer */
t_token		*lexer(char *input);
t_token		*new_token(t_token_type type, char *value);
void		free_tokens(t_token *tokens);
int			is_operator(char c);
t_token		*handle_operator(char *input, int *i);
t_token		*handle_word(char *input, int *i);
int			quote_len(char *s, int i);
int			has_unclosed_quote(char *input);

/* Parser */
t_ast		*parser(t_token *tokens);
t_ast		*new_ast_node(t_node_type type);
t_cmd		*new_cmd(void);
void		free_ast(t_ast *node);
void		free_cmd(t_cmd *cmd);
void		free_redir(t_redir *redir);
t_ast		*parse_pipeline(t_token **tokens);
t_ast		*parse_command(t_token **tokens);
t_redir		*parse_redir(t_token **tokens);
t_redir		*make_redir(t_token **tokens);

/* Expander */
void		expand_ast(t_ast *ast, t_shell *shell);
char		*expand_str(char *str, t_shell *shell);
char		*expand_heredoc_str(char *str, t_shell *shell);
char		*expand_var(char *str, int *i, t_shell *shell);
char		*get_var_value(char *name, t_shell *shell);
char		*join_free(char *s1, char *s2);
void		expand_args(t_cmd *cmd, t_shell *shell);
void		expand_redirs(t_redir *redir, t_shell *shell);
char		**word_split(char *s);

/* Executor */
int			execute(t_ast *ast, t_shell *shell);
int			exec_cmd(t_ast *ast, t_shell *shell);
int			exec_pipe(t_ast *ast, t_shell *shell);
int			exec_builtin(t_cmd *cmd, t_shell *shell);
int			is_builtin(char *name);
char		*find_executable(char *name, t_env *env);
int			apply_redirs(t_redir *redirs);
int			open_heredoc(t_redir *redir, t_shell *shell);
void		open_heredocs(t_redir *redir, t_shell *shell);
void		exec_child(t_cmd *cmd, t_shell *shell);
void		exec_builtin_redir(t_cmd *cmd, t_shell *shell, int *ret);

/* Builtins */
int			builtin_echo(t_cmd *cmd);
int			builtin_cd(t_cmd *cmd, t_shell *shell);
int			builtin_pwd(void);
int			builtin_export(t_cmd *cmd, t_shell *shell);
void		print_export(t_env *env);
int			builtin_unset(t_cmd *cmd, t_shell *shell);
int			builtin_env(t_shell *shell);
int			builtin_exit(t_cmd *cmd, t_shell *shell);

/* Env */
t_env		*env_init(char **envp);
t_env		*env_new(char *key, char *value);
t_env		*env_parse_entry(char *entry);
char		*env_get(t_env *env, char *key);
int			env_set(t_env **env, char *key, char *value);
void		env_unset(t_env **env, char *key);
char		**env_to_array(t_env *env);
void		env_free(t_env *env);
t_env		*env_find(t_env *env, char *key);

/* Signals */
void		setup_signals(void);
void		setup_signals_child(void);
void		setup_signals_heredoc(void);
void		setup_signals_wait(void);
void		sig_handler(int sig);
void		sig_heredoc(int sig);

/* Utils */
void		*ft_malloc(size_t size);
char		*ft_strjoin3(char *s1, char *s2, char *s3);
int			ft_isspace(int c);
void		free_array(char **arr);
int			array_len(char **arr);
void		free_shell(t_shell *shell);
void		fatal_error(char *msg);

/* Bonus */
t_ast		*parse_and_or(t_token **tokens);
int			exec_and_or(t_ast *ast, t_shell *shell);
void		expand_wildcards_cmd(t_cmd *cmd);
char		**expand_wildcard(char *pattern);
int			match_wildcard(char *pattern, char *str);
void		unmask_wildcards(char *s);
int			check_syntax(t_token *tokens);
void		expand_wildcard_redir(t_redir *redir);

#endif
