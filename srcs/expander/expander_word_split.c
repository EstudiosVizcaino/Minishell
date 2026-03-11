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
 * Used by word_split() to know how large to make the output array
 * before actually extracting the words.
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
		/* Skip any leading whitespace between words. */
		while (ft_isspace(s[i]))
			i++;

		/* If we are not at the end, we found the start of a new word. */
		if (s[i])
		{
			n++;
			/* Skip over the word itself (all non-whitespace chars). */
			while (s[i] && !ft_isspace(s[i]))
				i++;
		}
	}
	return (n);
}

/**
 * @brief Extracts the next whitespace-separated word from string s.
 *
 * Advances *i past any leading whitespace, then collects non-whitespace
 * characters until the next whitespace or end-of-string.
 *
 * @param s The source string.
 * @param i Pointer to the current index; updated to point past the word.
 * @return A newly-allocated string containing just the word.
 */
static char	*next_ws_word(char *s, int *i)
{
	int	start;

	/* Skip any whitespace before the word. */
	while (ft_isspace(s[*i]))
		(*i)++;

	/* Record the start of the word content. */
	start = *i;

	/* Walk forward until we hit whitespace or the end of the string. */
	while (s[*i] && !ft_isspace(s[*i]))
		(*i)++;

	/* Return a copy of just the word characters. */
	return (ft_substr(s, start, *i - start));
}

/**
 * @brief Splits a string on IFS (whitespace) into a NULL-terminated word array.
 *
 * This is the POSIX "word splitting" operation applied after variable
 * expansion.  For example, if $FILES expands to "a.txt b.txt c.txt",
 * word_split turns that into ["a.txt", "b.txt", "c.txt", NULL].
 *
 * The function makes two passes over the string:
 *   1. count_ws_words() to determine the array size.
 *   2. next_ws_word() in a loop to fill the array.
 *
 * Returns NULL (not an empty array) if the string contains no words,
 * so callers can easily check whether splitting produced anything.
 *
 * @param s The expanded string to split.
 * @return A malloc'd NULL-terminated array of word strings, or NULL if
 *         the string is empty/only whitespace.
 */
char	**word_split(char *s)
{
	char	**arr;
	int		n;    /* number of words */
	int		i;    /* current read position in s */
	int		j;    /* current write position in arr */

	/* Count words first so we know the array size. */
	n = count_ws_words(s);
	if (n == 0)
		return (NULL);  /* no words → nothing to split */

	/*
	** Allocate n+1 pointers: one per word plus the NULL sentinel.
	** Each element will be individually malloc'd by next_ws_word().
	*/
	arr = malloc((n + 1) * sizeof(char *));
	if (!arr)
		return (NULL);

	i = 0;
	j = 0;
	/*
	** Extract each word in order.  next_ws_word() advances 'i' past
	** the word it extracts, so each call picks up where the previous
	** one left off.
	*/
	while (j < n)
		arr[j++] = next_ws_word(s, &i);

	/* NULL-terminate the array. */
	arr[n] = NULL;
	return (arr);
}
