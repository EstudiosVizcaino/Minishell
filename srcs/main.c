#include "minishell.h"

int	g_signal = 0;

static void	run_line(char *line, t_shell *shell)
{
	t_token	*tokens;
	t_ast	*ast;

	tokens = lexer(line);
	if (!tokens)
		return ;
	ast = parser(tokens);
	free_tokens(tokens);
	if (!ast)
		return ;
	expand_ast(ast, shell);
	shell->last_exit = execute(ast, shell);
	free_ast(ast);
}

static void	main_loop(t_shell *shell)
{
	char	*line;

	while (1)
	{
		setup_signals();
		line = readline("minishell> ");
		if (!line)
		{
			ft_putstr_fd("exit\n", STDOUT_FILENO);
			break ;
		}
		if (*line)
			add_history(line);
		run_line(line, shell);
		free(line);
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;

	(void)argc;
	(void)argv;
	shell.env = env_init(envp);
	shell.last_exit = 0;
	shell.input = NULL;
	shell.in_heredoc = 0;
	main_loop(&shell);
	env_free(shell.env);
	rl_clear_history();
	return (shell.last_exit);
}
