/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pwd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gisidro- <gisidro-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 15:27:26 by gisidro-          #+#    #+#             */
/*   Updated: 2026/02/19 17:55:46 by gisidro-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Implements the pwd builtin command.
 *
 * @return 0 on success, 1 on failure.
 */
int	builtin_pwd(void)
{
	char	buf[4096];

	if (getcwd(buf, sizeof(buf)) == NULL)
	{
		perror("pwd");
		return (1);
	}
	ft_putstr_fd(buf, STDOUT_FILENO);
	ft_putchar_fd('\n', STDOUT_FILENO);
	return (0);
}
