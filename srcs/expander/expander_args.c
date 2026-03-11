/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_args.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gisidro- <gisidro-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 13:56:36 by gisidro-          #+#    #+#             */
/*   Updated: 2026/03/05 19:00:00 by gisidro-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Checks if a string is only $VAR refs.
 *
 * Used to decide whether an empty expansion means
 * the arg should be removed entirely.
 *
 * @param str The string to check.
 * @return 1 if only vars, 0 otherwise.
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
 * @brief Expands one arg, inserting words into dst[] via word splitting
 *        when the original token was a bare variable reference.
 *
 * @param dst  The destination argument array.
 * @param j    Pointer to the current write index in dst.
 * @param orig The original (unexpanded) argument string.
 * @param shell The shell context for variable expansion.
 */
static void	fill_one_arg(char **dst, int *j, char *orig, t_shell *shell)
{
	char	*exp;
	char	**words;
	int		k;

	exp = expand_str(orig, shell);
	if (!exp)
		return ;
	if (is_only_vars(orig) && *exp)
	{
		words = word_split(exp);
		free(exp);
		if (!words)
			return ;
		k = 0;
		while (words[k])
			dst[(*j)++] = words[k++];
		free(words);
		return ;
	}
	if (*exp || !is_only_vars(orig))
		dst[(*j)++] = exp;
	else
		free(exp);
}

/**
 * @brief Counts total arguments after expansion and word splitting.
 *
 * @param args  The original NULL-terminated argument array.
 * @param shell The shell context for variable expansion.
 * @return The number of arguments after expansion and splitting.
 */
static int	count_new_args(char **args, t_shell *shell)
{
	char	*exp;
	char	**words;
	int		i;
	int		total;

	i = 0;
	total = 0;
	while (args[i])
	{
		exp = expand_str(args[i], shell);
		if (!exp)
		{
			i++;
			continue ;
		}
		if (is_only_vars(args[i]) && *exp)
		{
			words = word_split(exp);
			total += array_len(words);
			free_array(words);
		}
		else if (*exp || !is_only_vars(args[i]))
			total++;
		free(exp);
		i++;
	}
	return (total);
}

/**
 * @brief Expands all args and drops empty ones.
 *
 * If a var like $EMPTY expands to nothing and the
 * original was only vars, we skip that arg.
 *
 * @param cmd   The command with args to expand.
 * @param shell The shell context (has env).
 */
void	expand_args(t_cmd *cmd, t_shell *shell)
{
	char	**new;
	int		n;
	int		i;
	int		j;

	n = count_new_args(cmd->args, shell);
	new = malloc((n + 1) * sizeof(char *));
	if (!new)
		return ;
	i = 0;
	j = 0;
	while (cmd->args[i])
	{
		fill_one_arg(new, &j, cmd->args[i], shell);
		free(cmd->args[i]);
		i++;
	}
	new[j] = NULL;
	free(cmd->args);
	cmd->args = new;
	if (j == 0)
	{
		free(new);
		cmd->args = NULL;
	}
}

/**
 * @brief Expands vars in redir filenames.
 *
 * Skips heredoc redirs (delimiter stays literal).
 *
 * @param redir Head of the redir list.
 * @param shell The shell context (has env).
 */
void	expand_redirs(t_redir *redir, t_shell *shell)
{
	char	*exp;
	int		i;

	while (redir)
	{
		i = 0;
		redir->quoted = 0;
		if (redir->type == TOKEN_HEREDOC && redir->file)
		{
			while (redir->file[i])
			{
				if (redir->file[i] == '\'' || redir->file[i] == '"'
					|| redir->file[i] == '\\')
					redir->quoted = 1;
				i++;
			}
		}
		if (redir->file)
		{
			exp = expand_str(redir->file, shell);
			free(redir->file);
			redir->file = exp;
		}
		redir = redir->next;
	}
}
