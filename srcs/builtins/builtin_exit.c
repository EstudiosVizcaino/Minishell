/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adherrer <adherrer@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/12 19:36:30 by adherrer          #+#    #+#             */
/*   Updated: 2026/02/24 13:50:30 by adherrer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Checks if a string consists entirely of digits (with optional sign).
 *
 * @param s The string to check.
 * @return 1 if valid numeric string, 0 otherwise.
 */
static int	all_digits(char *s)
{
	int	i;

	i = 0;
	if (s[i] == '-' || s[i] == '+')
		i++;
	if (!s[i])
		return (0);
	while (s[i])
	{
		if (!ft_isdigit(s[i]))
			return (0);
		i++;
	}
	return (1);
}

/**
 * @brief Cleans up shell resources and exits with the given code.
 *
 * @param shell Pointer to the shell structure.
 * @param code The exit code to terminate with.
 */
static void	do_exit(t_shell *shell, int code)
{
	env_free(shell->env);
	shell->env = NULL;
	rl_clear_history();
	exit(code);
}

/**
 * @brief Implements the exit builtin command.
 *
 * @param cmd Pointer to the command structure.
 * @param shell Pointer to the shell structure.
 * @return 0 on success, 1 on failure.
 */
int	builtin_exit(t_cmd *cmd, t_shell *shell)
{
	int	code;

	ft_putstr_fd("exit\n", STDOUT_FILENO);
	if (!cmd->args[1])
		do_exit(shell, shell->last_exit);
	if (cmd->args[2])
	{
		ft_putstr_fd("exit: too many arguments\n", STDERR_FILENO);
		return (1);
	}
	if (!all_digits(cmd->args[1]))
	{
		ft_putstr_fd("exit: numeric argument required\n", STDERR_FILENO);
		do_exit(shell, 2);
	}
	code = ft_atoi(cmd->args[1]) & 0xFF;
	do_exit(shell, code);
	return (0);
}
