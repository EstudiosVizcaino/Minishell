/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_redir_bonus.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 18:05:54 by cvizcain          #+#    #+#             */
/*   Updated: 2026/02/28 12:04:38 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Prints an ambiguous redirect error and nullifies the file target.
 *
 * @param redir The redirection that is ambiguous.
 */
static void	redir_ambiguous(t_redir *redir)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd(redir->file, STDERR_FILENO);
	ft_putstr_fd(": ambiguous redirect\n", STDERR_FILENO);
	free(redir->file);
	redir->file = NULL;
}

/**
 * @brief Applies wildcard expansion to a redirection file target.
 *
 * @param redir The redirection to expand.
 */
static void	apply_redir_wildcard(t_redir *redir)
{
	char	**exp;

	exp = expand_wildcard(redir->file);
	if (!exp)
		return ;
	if (array_len(exp) == 1)
	{
		free(redir->file);
		redir->file = ft_strdup(exp[0]);
	}
	else
		redir_ambiguous(redir);
	free_array(exp);
}

/**
 * @brief Expands wildcards in all non-heredoc redirections.
 *
 * @param redir The head of the redirection list.
 */
void	expand_wildcard_redir(t_redir *redir)
{
	while (redir)
	{
		if (redir->type != TOKEN_HEREDOC && redir->file
			&& ft_strchr(redir->file, '*'))
			apply_redir_wildcard(redir);
		if (redir->file)
			unmask_wildcards(redir->file);
		redir = redir->next;
	}
}
