/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gisidro- <gisidro-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 20:46:07 by gisidro-          #+#    #+#             */
/*   Updated: 2026/02/20 20:56:34 by gisidro-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Frees all resources held by the shell structure.
 *
 * @param shell Pointer to the shell structure to free.
 */
void	free_shell(t_shell *shell)
{
	if (!shell)
		return ;
	if (shell->env)
	{
		env_free(shell->env);
		shell->env = NULL;
	}
	if (shell->input)
	{
		free(shell->input);
		shell->input = NULL;
	}
}

/**
 * @brief Joins three strings into one newly allocated string.
 *
 * @param s1 The first string.
 * @param s2 The second string.
 * @param s3 The third string.
 * @return The newly allocated concatenated string, or NULL on failure.
 */
char	*ft_strjoin3(char *s1, char *s2, char *s3)
{
	char	*tmp;
	char	*result;

	tmp = ft_strjoin(s1, s2);
	if (!tmp)
		return (NULL);
	result = ft_strjoin(tmp, s3);
	free(tmp);
	return (result);
}
