/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 13:34:28 by cvizcain          #+#    #+#             */
/*   Updated: 2026/02/27 12:27:11 by cvizcain         ###   ########.fr       */
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
		line = read_full_input();
		if (!line)
		{
			ft_putstr_fd("exit\n", STDOUT_FILENO);
			break ;
		}
		if (is_blank_line(line))
		{
			free(line);
			continue ;
		}
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
	shell.ast = NULL;
	main_loop(&shell);
	env_free(shell.env);
	rl_clear_history();
	return (shell.last_exit);
}
