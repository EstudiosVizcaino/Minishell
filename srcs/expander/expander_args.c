/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_args.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 13:56:36 by cvizcain          #+#    #+#             */
/*   Updated: 2026/02/11 11:05:46 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Expands variables in redirection file targets.
 *
 * @param redir The linked list of redirections to expand.
 * @param shell The shell context containing environment variables.
 */
void	expand_redirs(t_redir *redir, t_shell *shell)
{
	char	*expanded;

	while (redir)
	{
		if (redir->type != TOKEN_HEREDOC && redir->file)
		{
			expanded = expand_str(redir->file, shell);
			free(redir->file);
			redir->file = expanded;
		}
		redir = redir->next;
	}
}

/**
 * @brief Checks if a string consists entirely of variable references.
 *
 * @param str The string to check.
 * @return 1 if the string contains only variable references, 0 otherwise.
 */
static int	is_only_vars(char *str)
{
	int	i;

	i = 0;
	if (!str || !*str)
		return (0);
	while (str[i])
	{
		if (str[i] == '\'' || str[i] == '"')
			return (0);
		if (str[i] != '$')
			return (0);
		i++;
		if (str[i] == '?')
			i++;
		else if (ft_isalpha(str[i]) || str[i] == '_')
		{
			while (str[i] && (ft_isalnum(str[i]) || str[i] == '_'))
				i++;
		}
		else
			return (0);
	}
	return (1);
}

/**
 * @brief Expands variables in all command arguments, removing empty expansions.
 *
 * @param cmd The command whose arguments to expand.
 * @param shell The shell context containing environment variables.
 */
void	expand_args(t_cmd *cmd, t_shell *shell)
{
	char	*expanded;
	char	*orig;
	int		i;
	int		j;

	i = 0;
	j = 0;
	while (cmd->args[i])
	{
		orig = cmd->args[i];
		expanded = expand_str(orig, shell);
		if (*expanded == '\0' && is_only_vars(orig))
			free(expanded);
		else
			cmd->args[j++] = expanded;
		free(orig);
		i++;
	}
	cmd->args[j] = NULL;
	if (j == 0)
	{
		free(cmd->args);
		cmd->args = NULL;
	}
}
