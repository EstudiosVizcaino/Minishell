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

typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_HEREDOC,
	TOKEN_APPEND,
	TOKEN_AND,
	TOKEN_OR,
	TOKEN_LPAREN,
	TOKEN_RPAREN,
	TOKEN_EOF
}	t_token_type;

typedef struct s_token
{
	t_token_type		type;
	char				*value;
	struct s_token		*next;
}	t_token;

typedef struct s_redir
{
	t_token_type		type;
	char				*file;
	int					heredoc_fd;
	struct s_redir		*next;
}	t_redir;

typedef struct s_cmd
{
	char				**args;
	t_redir				*redirs;
}	t_cmd;

typedef enum e_node_type
{
	NODE_CMD,
	NODE_PIPE,
	NODE_AND,
	NODE_OR
}	t_node_type;

typedef struct s_ast
{
	t_node_type			type;
	t_cmd				*cmd;
	struct s_ast		*left;
	struct s_ast		*right;
}	t_ast;

typedef struct s_env
{
	char				*key;
	char				*value;
	struct s_env		*next;
}	t_env;

typedef struct s_shell
{
	t_env				*env;
	int					last_exit;
	char				*input;
	int					in_heredoc;
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
char		*expand_var(char *str, int *i, t_shell *shell);
char		*get_var_value(char *name, t_shell *shell);
char		*join_free(char *s1, char *s2);
void		expand_args(t_cmd *cmd, t_shell *shell);
void		expand_redirs(t_redir *redir, t_shell *shell);

/* Executor */
int			execute(t_ast *ast, t_shell *shell);
int			exec_cmd(t_ast *ast, t_shell *shell);
int			exec_pipe(t_ast *ast, t_shell *shell);
int			exec_builtin(t_cmd *cmd, t_shell *shell);
int			is_builtin(char *name);
char		*find_executable(char *name, t_env *env);
int			apply_redirs(t_redir *redirs);
int			open_heredoc(t_redir *redir);
void		open_heredocs(t_redir *redir, t_shell *shell);
void		exec_child(t_cmd *cmd, t_shell *shell);
void		exec_builtin_redir(t_cmd *cmd, t_shell *shell, int *ret);

/* Builtins */
int			builtin_echo(t_cmd *cmd);
int			builtin_cd(t_cmd *cmd, t_shell *shell);
int			builtin_pwd(void);
int			builtin_export(t_cmd *cmd, t_shell *shell);
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
