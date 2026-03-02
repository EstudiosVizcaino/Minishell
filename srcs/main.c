/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 12:32:31 by cvizcain          #+#    #+#             */
/*   Updated: 2026/03/02 19:10:31 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	g_signal = 0;

static void	print_conchita(void)
{
	ft_putstr_fd("\033[31m \n", STDOUT_FILENO);
	ft_putstr_fd("                      _     _ _    \n", STDOUT_FILENO);
	ft_putstr_fd("  ___ ___  _ __   ___| |__ (_) |_ __ _  \n", STDOUT_FILENO);
	ft_putstr_fd(" / __/ _ \\| '_ \\ / __| '_ \\| | __/ _` | \n",
		STDOUT_FILENO);
	ft_putstr_fd("| (_| (_) | | | | (__| | | | | || (_| | \n", STDOUT_FILENO);
	ft_putstr_fd(" \\___\\___/|_| |_|\\___|_| |_|_|\\__\\__,_| \n\n",
		STDOUT_FILENO);
	ft_putstr_fd("             `)\n", STDOUT_FILENO);
	ft_putstr_fd("            _ \\\n", STDOUT_FILENO);
	ft_putstr_fd("          (( }/  ,)\n", STDOUT_FILENO);
	ft_putstr_fd("          )))__ /\n", STDOUT_FILENO);
	ft_putstr_fd("         (((---'\n", STDOUT_FILENO);
	ft_putstr_fd("           \\ .'\n", STDOUT_FILENO);
	ft_putstr_fd("            )|____.---- )\n", STDOUT_FILENO);
	ft_putstr_fd("           / \\ `       (\n", STDOUT_FILENO);
	ft_putstr_fd("          / ' \\ `      )\n", STDOUT_FILENO);
	ft_putstr_fd("         /  '  \\  `   /\n", STDOUT_FILENO);
	ft_putstr_fd("        /   '       _/\n", STDOUT_FILENO);
	ft_putstr_fd("       /   _!____.-'\n", STDOUT_FILENO);
	ft_putstr_fd("      /_.-'\\/  /\n", STDOUT_FILENO);
	ft_putstr_fd("           '| |`_\n", STDOUT_FILENO);
	ft_putstr_fd("           \\/\n\033[0m\n", STDOUT_FILENO);
}

/**
 * @brief Tokenizes, parses, expands, and executes a single input line.
 *
 * @param line The raw input line to process.
 * @param shell The shell state structure.
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
		line = readline("Conchita> ");
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
	if (0)
		print_conchita();
	main_loop(&shell);
	env_free(shell.env);
	rl_clear_history();
	return (shell.last_exit);
}
