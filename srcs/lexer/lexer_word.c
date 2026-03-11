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
 * @brief Finds the index just past the end of a word token.
 *
 * A "word" in shell terms is any sequence of characters that is not
 * a whitespace character and not an unquoted operator.
 * Quoted sections (e.g. 'hello world' or "foo") are part of the same
 * word – we skip over them in one jump so their contents are never
 * mistaken for operators or delimiters.
 *
 * @param input The full input string.
 * @param i     Starting index (first character of the word).
 * @return Index of the first character that is NOT part of this word.
 */
static int	word_end(char *input, int i)
{
	/*
	** Keep advancing while the current character is:
	**   - not the end of the string ('\0')
	**   - not a space or tab (horizontal whitespace = word separator)
	**   - not an unquoted operator (|, <, >)
	*/
	while (input[i] && input[i] != ' ' && input[i] != '\t'
		&& !is_operator(input[i]))
	{
		if (input[i] == '\'' || input[i] == '"')
			/*
			** We are at the start of a quoted segment.
			** quote_len() returns the full length of that segment
			** (including both quote characters), so we can jump
			** over the entire thing in one step.
			** This is critical: without this, the quote character
			** would be misinterpreted as a word boundary or the
			** content could match an operator character.
			*/
			i += quote_len(input, i);
		else
			i++;
	}
	return (i);
}

/**
 * @brief Extracts the next word from the input and wraps it in a token.
 *
 * Uses word_end() to locate where the word finishes, then copies the
 * slice with ft_substr and builds a TOKEN_WORD.
 * The index pointer *i is advanced past the word so the lexer loop
 * can continue from the right position.
 *
 * @param input The full input string.
 * @param i     Pointer to the current read index (updated on return).
 * @return A TOKEN_WORD token, or NULL on malloc failure.
 */
t_token	*handle_word(char *input, int *i)
{
	t_token	*tok;
	char	*word;
	int		end;
	int		len;

	/* Find where this word ends (exclusive index). */
	end = word_end(input, *i);

	/* Calculate the number of characters in the word. */
	len = end - *i;

	/*
	** ft_substr(input, *i, len) allocates a new string containing
	** exactly the characters from index *i to *i+len-1.
	** We keep the raw word (with quotes still in it) because the
	** expander stage will strip the quotes and expand variables later.
	*/
	word = ft_substr(input, *i, len);
	if (!word)
		return (NULL);

	/* Wrap the extracted string in a token node. */
	tok = new_token(TOKEN_WORD, word);

	/*
	** new_token() copies 'word' internally with ft_strdup, so we own
	** 'word' and must free it here to avoid a memory leak.
	*/
	free(word);

	/* Advance the caller's index past the word we just consumed. */
	*i = end;
	return (tok);
}
