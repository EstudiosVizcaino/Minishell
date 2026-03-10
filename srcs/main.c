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
 * @brief Checks if a string contains only whitespace characters.
 *
 * @param line The string to check.
 * @return 1 if the string is blank (all whitespace), 0 otherwise.
 */
static int	is_blank_line(char *line)
{
	int	i;

	i = 0;
	while (line[i])
	{
		if (!ft_isspace(line[i]))
			return (0);
		i++;
	}
	return (1);
}

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
 * @brief Reads a full (possibly multi-line) input from the user.
 *
 * If the initial line has unclosed quotes, additional lines are read
 * with a continuation prompt until all quotes are balanced.
 *
 * @return The assembled input string, or NULL on EOF.
 */
static char	*read_full_input(void)
{
	char	*line;
	char	*cont;
	char	*tmp;

	line = readline("minishell> ");
	if (!line)
		return (NULL);
	while (has_unclosed_quote(line))
	{
		cont = readline("> ");
		if (!cont)
			break ;
		tmp = ft_strjoin3(line, "\n", cont);
		free(line);
		free(cont);
		if (!tmp)
			return (NULL);
		line = tmp;
	}
	return (line);
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
	while (1)
	{
		setup_signals();
		shell->input = read_full_input();
		if (!shell->input)
		{
			ft_putstr_fd("exit\n", STDOUT_FILENO);
			break ;
		}
		if (is_blank_line(shell->input))
		{
			free(shell->input);
			shell->input = NULL;
			continue ;
		}
		add_history(shell->input);
		run_line(shell->input, shell);
		free(shell->input);
		shell->input = NULL;
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
	shell.ast = NULL;
	main_loop(&shell);
	env_free(shell.env);
	rl_clear_history();
	return (shell.last_exit);
}
