/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 12:42:30 by cvizcain          #+#    #+#             */
/*   Updated: 2026/02/10 18:10:16 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Gets a var's value from the env.
 *
 * Handles $? as a special case (last exit code).
 * Returns "" if the var is not set.
 *
 * @param name  The variable name.
 * @param shell The shell context.
 * @return The value string (caller must free).
 */
char	*get_var_value(char *name, t_shell *shell)
{
	char	*val;

	if (!name)
		return (ft_strdup(""));
	if (ft_strcmp(name, "?") == 0)
		return (ft_itoa(shell->last_exit));
	val = env_get(shell->env, name);
	if (!val)
		return (ft_strdup(""));
	return (ft_strdup(val));
}

/**
 * @brief Parses and expands a $VAR from a string.
 *
 * Reads the var name after the $, looks it up,
 * and returns the value. Advances the index past
 * the variable name.
 *
 * @param str   The input string.
 * @param i     Current index pointer.
 * @param shell The shell context.
 * @return The expanded value.
 */
char	*expand_var(char *str, int *i, t_shell *shell)
{
	int		start;
	char	*name;
	char	*val;

	(*i)++;
	if (str[*i] == '?')
	{
		(*i)++;
		return (ft_itoa(shell->last_exit));
	}
	start = *i;
	while (str[*i] && (ft_isalnum(str[*i]) || str[*i] == '_'))
		(*i)++;
	if (*i == start)
		return (ft_strdup("$"));
	name = ft_substr(str, start, *i - start);
	if (!name)
		return (ft_strdup(""));
	val = get_var_value(name, shell);
	free(name);
	return (val);
}

/**
 * @brief Joins two strings and frees both.
 *
 * Calls ft_strjoin, then frees s1 and s2.
 * Calls fatal_error if malloc fails.
 *
 * @param s1 First string (freed).
 * @param s2 Second string (freed).
 * @return The joined result.
 */
char	*join_free(char *s1, char *s2)
{
	char	*tmp;

	tmp = ft_strjoin(s1, s2);
	free(s1);
	free(s2);
	if (!tmp)
		fatal_error("malloc");
	return (tmp);
}

/**
 * @brief Expands variables in a heredoc body without quote interpretation.
 *
 * Expands $var references while treating single and double quote
 * characters as literal characters, not as quoting constructs.
 *
 * @param str The heredoc line to expand.
 * @param shell The shell context for variable lookup.
 * @return The expanded string with quote characters preserved literally.
 */
char	*expand_heredoc_str(char *str, t_shell *shell)
{
	char	*result;
	char	*part;
	char	buf[2];
	int		i;

	if (!str)
		return (ft_strdup(""));
	result = ft_strdup("");
	i = 0;
	while (str[i])
	{
		if (str[i] == '$')
			part = expand_var(str, &i, shell);
		else
		{
			buf[0] = str[i++];
			buf[1] = '\0';
			part = ft_strdup(buf);
		}
		result = join_free(result, part);
	}
	return (result);
}
