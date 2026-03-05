/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gisidro- <gisidro-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 13:34:28 by gisidro-          #+#    #+#             */
/*   Updated: 2026/02/27 12:27:11 by gisidro-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	g_signal = 0;

/**
 * @brief Tokenizes, syntax-checks, parses, expands, and executes an
 *        input line.
 *
 * @param line The input line to process.
 * @param shell The shell state structure.
 */
static void	run_line(char *line, t_shell *shell)
{
	t_token	*tokens;
	t_ast	*ast;

	tokens = lexer(line);
	if (!tokens)
		return ;
	if (check_syntax(tokens))
	{
		shell->last_exit = 2;
		free_tokens(tokens);
		return ;
	}
	ast = parser(tokens);
	free_tokens(tokens);
	if (!ast)
		return ;
	expand_ast(ast, shell);
	shell->last_exit = execute(ast, shell);
	free_ast(ast);
}

/**
 * @brief Runs the main read-eval-print loop of the shell.
 *
 * @param shell The shell state structure.
 */
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

/**
 * @brief Entry point of the minishell program (bonus version).
 *
 * @param argc The argument count.
 * @param argv The argument vector.
 * @param envp The environment variables array.
 * @return The last exit status of the shell.
 */
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
