/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_str_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/18 18:06:03 by cvizcain          #+#    #+#             */
/*   Updated: 2026/02/26 20:34:53 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Replaces asterisk characters with a mask byte to prevent
 *        wildcard expansion.
 *
 * @param s The string in which to mask wildcards.
 */
static void	mask_wildcards(char *s)
{
	while (s && *s)
	{
		if (*s == '*')
			*s = '\x01';
		s++;
	}
}

/**
 * @brief Extracts text within single quotes, masking wildcards.
 *
 * @param str The input string containing the quoted segment.
 * @param i Pointer to the current index in the string.
 * @return The extracted and masked substring.
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
	mask_wildcards(result);
	return (result);
}

/**
 * @brief Expands variables within double-quoted text, masking wildcards.
 *
 * @param str The input string containing the quoted segment.
 * @param i Pointer to the current index in the string.
 * @param shell The shell state structure.
 * @return The expanded and masked substring.
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
	mask_wildcards(result);
	return (result);
}

/**
 * @brief Expands a single token element (quote, variable, or literal).
 *
 * @param str The input string to expand from.
 * @param i Pointer to the current index in the string.
 * @param shell The shell state structure.
 * @return The expanded substring.
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
 * @brief Expands all variables and removes quotes from a string (bonus version).
 *
 * @param str The input string to expand.
 * @param shell The shell state structure.
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
