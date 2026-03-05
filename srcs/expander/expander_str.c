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
 * @brief Extracts text within single quotes without expansion.
 *
 * @param str The input string containing the single-quoted text.
 * @param i Pointer to the current index in the string.
 * @return The extracted text inside the single quotes.
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
 * @brief Expands variables within double-quoted text.
 *
 * @param str The input string containing the double-quoted text.
 * @param i Pointer to the current index in the string.
 * @param shell The shell context containing environment variables.
 * @return The expanded text inside the double quotes.
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
 * @brief Expands a single token element (quote, variable, or literal).
 *
 * @param str The input string to expand from.
 * @param i Pointer to the current index in the string.
 * @param shell The shell context containing environment variables.
 * @return The expanded token element as a new string.
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
 * @brief Expands all variables and removes quotes from a string.
 *
 * @param str The input string to expand.
 * @param shell The shell context containing environment variables.
 * @return The fully expanded string with quotes removed.
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
