/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_str.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 17:52:04 by cvizcain          #+#    #+#             */
/*   Updated: 2026/02/13 18:49:08 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Handles a single-quoted segment.
 *
 * Copies everything between the quotes literally,
 * no variable expansion happens here.
 *
 * @param str The input string.
 * @param i   Current index pointer.
 * @return The text inside the quotes.
 */
static char	*expand_single_quote(char *str, int *i)
{
	char	*result;
	int		start;

	(*i)++;
	start = *i;
	while (str[*i] && str[*i] != '\'')
		(*i)++;
	result = ft_substr(str, start, *i - start);
	if (str[*i] == '\'')
		(*i)++;
	return (result);
}

/**
 * @brief Handles a double-quoted segment.
 *
 * Expands $VAR inside double quotes but keeps
 * everything else literal.
 *
 * @param str   The input string.
 * @param i     Current index pointer.
 * @param shell The shell context (has env).
 * @return The expanded text.
 */
static char	*expand_double_quote(char *str, int *i, t_shell *shell)
{
	char	*result;
	char	*part;
	char	buf[2];

	result = ft_strdup("");
	(*i)++;
	while (str[*i] && str[*i] != '"')
	{
		if (str[*i] == '$')
			part = expand_var(str, i, shell);
		else
		{
			buf[0] = str[(*i)++];
			buf[1] = '\0';
			part = ft_strdup(buf);
		}
		result = join_free(result, part);
	}
	if (str[*i] == '"')
		(*i)++;
	return (result);
}

/**
 * @brief Expands one token element.
 *
 * Delegates to the right handler depending on
 * whether we see a quote, $ or a normal char.
 *
 * @param str   The input string.
 * @param i     Current index pointer.
 * @param shell The shell context (has env).
 * @return The expanded element as a new string.
 */
static char	*expand_one(char *str, int *i, t_shell *shell)
{
	char	buf[2];

	if (str[*i] == '\'')
		return (expand_single_quote(str, i));
	if (str[*i] == '"')
		return (expand_double_quote(str, i, shell));
	if (str[*i] == '$')
		return (expand_var(str, i, shell));
	buf[0] = str[(*i)++];
	buf[1] = '\0';
	return (ft_strdup(buf));
}

/**
 * @brief Fully expands a string (vars + quotes).
 *
 * Walks the string char by char, calling expand_one
 * and joining the pieces into one result.
 *
 * @param str   The raw input string.
 * @param shell The shell context (has env).
 * @return The fully expanded string.
 */
char	*expand_str(char *str, t_shell *shell)
{
	char	*result;
	char	*part;
	int		i;

	if (!str)
		return (ft_strdup(""));
	result = ft_strdup("");
	i = 0;
	while (str[i])
	{
		part = expand_one(str, &i, shell);
		result = join_free(result, part);
	}
	return (result);
}
