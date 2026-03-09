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
 * @brief Finds where a word token ends.
 *
 * Walks forward until it hits a space, tab, or
 * operator. Quoted segments are skipped over.
 *
 * @param input The input string.
 * @param i     Start index of the word.
 * @return Index right after the last char of the word.
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
 * @brief Extracts a word and creates its token.
 *
 * Uses word_end to know how far the word goes, then
 * does a substr and wraps it in a TOKEN_WORD.
 *
 * @param input The input string.
 * @param i     Current index pointer (updated).
 * @return The word token, or NULL on malloc fail.
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
