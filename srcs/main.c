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
 * @brief Processes one line of user input.
 *
 * Takes the line through the full pipeline:
 * lexer, parser, expander and executor.
 * The exit code goes into shell->last_exit for $?.
 *
 * @param line  The raw input line.
 * @param shell The shell context.
 */
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

/**
 * @brief Main loop of the shell (read-eval-print).
 *
 * Sets up signals, reads with readline, and sends
 * lines to run_line. NULL from readline means EOF
 * so we print "exit" and stop.
 *
 * @param shell The shell context.
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
 * @brief Entry point of the minishell program.
 *
 * Initialises the shell struct and env, runs 
 * the main loop, then cleans up.
 *
 * @param argc The argument count (unused).
 * @param argv The argument vector (unused).
 * @param envp The environment variables.
 * @return The last exit status.
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
