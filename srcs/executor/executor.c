/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gisidro- <gisidro-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/03 18:55:38 by gisidro-          #+#    #+#             */
/*   Updated: 2026/02/18 16:13:59 by gisidro-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Checks if a command name is a builtin.
 *
 * Compares against all 7 builtins we support.
 *
 * @param name The command name.
 * @return 1 if builtin, 0 otherwise.
 */
int	is_builtin(char *name)
{
	if (!name)
		return (0);
	if (ft_strcmp(name, "echo") == 0)
		return (1);
	if (ft_strcmp(name, "cd") == 0)
		return (1);
	if (ft_strcmp(name, "pwd") == 0)
		return (1);
	if (ft_strcmp(name, "export") == 0)
		return (1);
	if (ft_strcmp(name, "unset") == 0)
		return (1);
	if (ft_strcmp(name, "env") == 0)
		return (1);
	if (ft_strcmp(name, "exit") == 0)
		return (1);
	return (0);
}

/**
 * @brief Runs the matching builtin function.
 *
 * Compares cmd name and calls the right builtin
 * handler (echo, cd, pwd, export, unset, env, exit).
 *
 * @param cmd  The command struct with name and args.
 * @param shell The shell context.
 * @return Exit status of the builtin.
 */
int	exec_builtin(t_cmd *cmd, t_shell *shell)
{
	char	*name;

	name = cmd->args[0];
	if (ft_strcmp(name, "echo") == 0)
		return (builtin_echo(cmd));
	if (ft_strcmp(name, "cd") == 0)
		return (builtin_cd(cmd, shell));
	if (ft_strcmp(name, "pwd") == 0)
		return (builtin_pwd());
	if (ft_strcmp(name, "export") == 0)
		return (builtin_export(cmd, shell));
	if (ft_strcmp(name, "unset") == 0)
		return (builtin_unset(cmd, shell));
	if (ft_strcmp(name, "env") == 0)
		return (builtin_env(shell));
	if (ft_strcmp(name, "exit") == 0)
		return (builtin_exit(cmd, shell));
	return (1);
}
