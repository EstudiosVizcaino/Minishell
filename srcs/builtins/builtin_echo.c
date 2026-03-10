/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gisidro- <gisidro-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 11:32:15 by gisidro-          #+#    #+#             */
/*   Updated: 2026/02/21 14:42:31 by gisidro-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Checks if a string is a valid -n flag for echo (e.g. -n, -nn, -nnn).
 *
 * @param s The string to check.
 * @return 1 if the string is a valid -n flag, 0 otherwise.
 */
static int	is_n_flag(char *s)
{
	int	i;

	if (!s || s[0] != '-')
		return (0);
	i = 1;
	if (!s[i])
		return (0);
	while (s[i] == 'n')
		i++;
	return (s[i] == '\0');
}

/**
 * @brief The echo builtin.
 *
 * Prints its args separated by spaces. The -n
 * flag suppresses the trailing newline.
 *
 * @param cmd The command struct.
 * @return Always 0.
 */
int	builtin_echo(t_cmd *cmd)
{
	int	i;
	int	newline;

	newline = 1;
	i = 1;
	while (cmd->args[i] && is_n_flag(cmd->args[i]))
	{
		newline = 0;
		i++;
	}
	while (cmd->args[i])
	{
		ft_putstr_fd(cmd->args[i], STDOUT_FILENO);
		if (cmd->args[i + 1])
			ft_putchar_fd(' ', STDOUT_FILENO);
		i++;
	}
	if (newline)
		ft_putchar_fd('\n', STDOUT_FILENO);
	return (0);
}
