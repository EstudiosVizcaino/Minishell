/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 16:10:29 by cvizcain          #+#    #+#             */
/*   Updated: 2026/01/28 10:46:56 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Checks if a character is a shell operator character.
 *
 * The characters '|', '<', and '>' are the only ones that start
 * a shell operator token.  Everything else is part of a word.
 *
 * @param c The character to check.
 * @return 1 if operator, 0 otherwise.
 */
int	is_operator(char c)
{
	if (c == '|' || c == '<' || c == '>')
		return (1);
	return (0);
}

/**
 * @brief Handles the two-character operators << and >>.
 *
 * Peeks at the next two characters.  If they form a known double
 * operator, advances 'i' by 2 and returns the appropriate token.
 * Returns NULL if the characters do not match any double operator,
 * allowing the caller to fall back to single-character handling.
 *
 * @param input The full input string.
 * @param i     Pointer to the current read index.
 * @return TOKEN_HEREDOC for "<<", TOKEN_APPEND for ">>", else NULL.
 */
static t_token	*handle_double_op(char *input, int *i)
{
	/*
	** "<<" is the heredoc operator: the shell reads lines from stdin
	** until it sees the specified delimiter word.
	*/
	if (input[*i] == '<' && input[*i + 1] == '<')
	{
		(*i) += 2;  /* consume both '<' characters */
		return (new_token(TOKEN_HEREDOC, "<<"));
	}

	/*
	** ">>" is the append-redirect operator: output is appended to a
	** file instead of truncating it.
	*/
	if (input[*i] == '>' && input[*i + 1] == '>')
	{
		(*i) += 2;  /* consume both '>' characters */
		return (new_token(TOKEN_APPEND, ">>"));
	}

	/* Not a double operator – caller will try a single-char match. */
	return (NULL);
}

/**
 * @brief Reads a single operator token from the input.
 *
 * First tries two-character operators (<< and >>).  If that fails,
 * falls back to single-character operators (|, <, >).
 *
 * @param input The full input string.
 * @param i     Pointer to the current read index (advanced past the op).
 * @return The operator token, or NULL on malloc failure.
 */
t_token	*handle_operator(char *input, int *i)
{
	t_token			*tok;
	t_token_type	type;
	char			two[3];  /* small buffer to build the value string */

	/*
	** Pre-build a two-character string so that if handle_double_op
	** does NOT match, we still have the first character ready to use
	** for the single-operator token's value string below.
	*/
	two[0] = input[*i];
	two[1] = input[*i + 1];
	two[2] = '\0';

	/*
	** Try the double operators first: if one matches, handle_double_op
	** already advanced *i and returned the finished token.
	*/
	tok = handle_double_op(input, i);
	if (tok)
		return (tok);

	/*
	** Single-character operator: figure out which type it is.
	** '|' → pipe between two commands.
	** '<' → redirect stdin from a file.
	** '>' → redirect stdout to a file (truncate).
	*/
	if (input[*i] == '|')
		type = TOKEN_PIPE;
	else if (input[*i] == '<')
		type = TOKEN_REDIR_IN;
	else
		type = TOKEN_REDIR_OUT;

	(*i)++;         /* consume the single operator character */
	two[1] = '\0';  /* shorten the buffer to just the one character */
	return (new_token(type, two));
}

/**
 * @brief Returns the total length (in chars) of a quoted segment.
 *
 * Counts from the opening quote character up to and including the
 * matching closing quote.  This is used by word_end() so that quoted
 * sections are treated as one indivisible chunk during tokenisation.
 *
 * Example: for s = "hello 'world' end" with i pointing at the first
 * quote, quote_len returns 7 ( 'world' including both quotes ).
 *
 * @param s The string containing the quoted segment.
 * @param i Index of the opening quote character in s.
 * @return Total length of the quoted segment (including both quotes).
 */
int	quote_len(char *s, int i)
{
	char	quote;  /* the opening quote character (' or ") */
	int		len;

	quote = s[i];  /* remember whether it's single or double */
	len = 1;       /* count the opening quote itself */
	i++;
	/*
	** Walk forward until we find the matching closing quote or the
	** end of the string (unclosed quote – handled gracefully by not
	** incrementing len for the missing closing char).
	*/
	while (s[i] && s[i] != quote)
	{
		len++;
		i++;
	}

	/* Count the closing quote if it is present. */
	if (s[i] == quote)
		len++;
	return (len);
}
