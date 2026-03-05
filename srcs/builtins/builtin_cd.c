/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adherrer <adherrer@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 13:04:57 by adherrer          #+#    #+#             */
/*   Updated: 2026/02/22 10:55:21 by adherrer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Changes directory to the HOME env variable.
 *
 * @param shell Pointer to the shell structure.
 * @return 0 on success, 1 on failure.
 */
static int	cd_home(t_shell *shell)
{
	char	*home;

	home = env_get(shell->env, "HOME");
	if (!home)
	{
		ft_putstr_fd("cd: HOME not set\n", STDERR_FILENO);
		return (1);
	}
	if (chdir(home) != 0)
	{
		perror("cd");
		return (1);
	}
	return (0);
}

/**
 * @brief Updates PWD and OLDPWD environment variables after a directory change.
 *        OLDPWD is only updated if it already exists in the environment,
 *        so that an explicit 'unset OLDPWD' is respected.
 *
 * @param shell Pointer to the shell structure.
 */
static void	update_pwd(t_shell *shell)
{
	char	buf[4096];
	char	*old;

	old = env_get(shell->env, "PWD");
	if (old && env_find(shell->env, "OLDPWD"))
		env_set(&shell->env, "OLDPWD", old);
	if (getcwd(buf, sizeof(buf)))
		env_set(&shell->env, "PWD", buf);
}

/**
 * @brief Prints a cd error message with the path and errno string.
 *
 * @param path The path that caused the error.
 * @return 1 to indicate failure.
 */
static int	cd_error(char *path)
{
	ft_putstr_fd("cd: ", STDERR_FILENO);
	ft_putstr_fd(path, STDERR_FILENO);
	ft_putstr_fd(": ", STDERR_FILENO);
	ft_putstr_fd(strerror(errno), STDERR_FILENO);
	ft_putchar_fd('\n', STDERR_FILENO);
	return (1);
}

/**
 * @brief Implements the cd builtin command.
 *
 * @param cmd Pointer to the command structure.
 * @param shell Pointer to the shell structure.
 * @return 0 on success, 1 on failure.
 */
int	builtin_cd(t_cmd *cmd, t_shell *shell)
{
	char	*path;

	if (!cmd->args[1])
	{
		if (cd_home(shell) != 0)
			return (1);
		update_pwd(shell);
		return (0);
	}
	path = cmd->args[1];
	if (cmd->args[2])
	{
		ft_putstr_fd("cd: too many arguments\n", STDERR_FILENO);
		return (1);
	}
	if (chdir(path) != 0)
		return (cd_error(path));
	update_pwd(shell);
	return (0);
}
