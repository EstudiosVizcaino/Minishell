/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_word.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 21:16:32 by cvizcain          #+#    #+#             */
/*   Updated: 2026/01/29 12:32:58 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Finds the end index of a word token in the input.
 *
 * @param input The input string being tokenized.
 * @param i The starting index of the word.
 * @return The index past the end of the word.
 */
static int	word_end(char *input, int i)
{
	while (input[i] && input[i] != ' ' && input[i] != '\t'
		&& !is_operator(input[i]))
	{
		if (input[i] == '\'' || input[i] == '"')
			i += quote_len(input, i);
		else
			i++;
	}
	return (i);
}

/**
 * @brief Extracts and tokenizes a word from the input string.
 *
 * @param input The input string being tokenized.
 * @param i A pointer to the current index in the input string.
 * @return A pointer to the newly created word token, or NULL on failure.
 */
t_token	*handle_word(char *input, int *i)
{
	t_token	*tok;
	char	*word;
	int		end;
	int		len;

	end = word_end(input, *i);
	len = end - *i;
	word = ft_substr(input, *i, len);
	if (!word)
		return (NULL);
	tok = new_token(TOKEN_WORD, word);
	free(word);
	*i = end;
	return (tok);
}
