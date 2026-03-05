/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 12:32:31 by cvizcain          #+#    #+#             */
/*   Updated: 2026/02/26 11:48:03 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	g_signal = 0;

/**
 * @brief Tokenizes, parses, expands, and executes a single input line.
 *
 * @param line The raw input line to process.
 * @param shell The shell state structure.
 */
static void	run_line(char *line, t_shell *shell)
{
	t_token	*tokens;

	tokens = lexer(line);
	if (!tokens)
		return ;
	shell->ast = parser(tokens);
	free_tokens(tokens);
	if (!shell->ast)
		return ;
	expand_ast(shell->ast, shell);
	shell->last_exit = execute(shell->ast, shell);
	free_ast(shell->ast);
	shell->ast = NULL;
}

/**
 * @brief Runs the main read-eval-print loop of the shell.
 *
 * @param shell The shell state structure.
 */
static void	main_loop(t_shell *shell)
{
	while (1)
	{
		setup_signals();
		shell->input = readline("minishell> ");
		if (!shell->input)
		{
			ft_putstr_fd("exit\n", STDOUT_FILENO);
			break ;
		}
		if (*shell->input)
			add_history(shell->input);
		run_line(shell->input, shell);
		free(shell->input);
		shell->input = NULL;
	}
}

/**
 * @brief Entry point of the minishell program.
 *
 * @param argc The argument count.
 * @param argv The argument vector.
 * @param envp The environment variables.
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
	shell.ast = NULL;
	main_loop(&shell);
	env_free(shell.env);
	rl_clear_history();
	return (shell.last_exit);
}
