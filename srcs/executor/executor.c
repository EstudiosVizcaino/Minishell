/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/03 18:55:38 by cvizcain          #+#    #+#             */
/*   Updated: 2026/02/18 16:13:59 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Checks if a command name corresponds to a shell builtin.
 *
 * @param name The command name to check.
 * @return 1 if the command is a builtin, 0 otherwise.
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
 * @brief Dispatches execution to the appropriate builtin function.
 *
 * @param cmd The command structure containing the builtin name and arguments.
 * @param shell The shell state.
 * @return The exit status of the executed builtin.
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
