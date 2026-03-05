/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_word_split.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gisidro- <gisidro-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 19:00:00 by gisidro-          #+#    #+#             */
/*   Updated: 2026/03/05 19:00:00 by gisidro-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Counts the number of whitespace-separated words in a string.
 *
 * @param s The string to scan.
 * @return The number of whitespace-delimited words found.
 */
static int	count_ws_words(char *s)
{
	int	i;
	int	n;

	i = 0;
	n = 0;
	while (s[i])
	{
		while (ft_isspace(s[i]))
			i++;
		if (s[i])
		{
			n++;
			while (s[i] && !ft_isspace(s[i]))
				i++;
		}
	}
	return (n);
}

/**
 * @brief Extracts the next whitespace-separated word starting at index *i.
 *
 * @param s The source string.
 * @param i Pointer to the current index; advanced past the word.
 * @return A new string containing the extracted word.
 */
static char	*next_ws_word(char *s, int *i)
{
	int	start;

	while (ft_isspace(s[*i]))
		(*i)++;
	start = *i;
	while (s[*i] && !ft_isspace(s[*i]))
		(*i)++;
	return (ft_substr(s, start, *i - start));
}

/**
 * @brief Splits a string by whitespace into a NULL-terminated word array.
 *
 * @param s The string to split.
 * @return A malloc'd array of word strings, or NULL if no words found.
 */
char	**word_split(char *s)
{
	char	**arr;
	int		n;
	int		i;
	int		j;

	n = count_ws_words(s);
	if (n == 0)
		return (NULL);
	arr = malloc((n + 1) * sizeof(char *));
	if (!arr)
		return (NULL);
	i = 0;
	j = 0;
	while (j < n)
		arr[j++] = next_ws_word(s, &i);
	arr[n] = NULL;
	return (arr);
}
