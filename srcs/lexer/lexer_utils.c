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
 * @brief Checks if a character is a shell operator.
 *
 * @param c The character to check.
 * @return 1 if the character is an operator, 0 otherwise.
 */
int	is_operator(char c)
{
	if (c == '|' || c == '<' || c == '>')
		return (1);
	return (0);
}

/**
 * @brief Handles double-character operators (heredoc and append).
 *
 * @param input The input string being tokenized.
 * @param i A pointer to the current index in the input string.
 * @return A token for the double operator, or NULL if none matched.
 */
static t_token	*handle_double_op(char *input, int *i)
{
	if (input[*i] == '<' && input[*i + 1] == '<')
	{
		(*i) += 2;
		return (new_token(TOKEN_HEREDOC, "<<"));
	}
	if (input[*i] == '>' && input[*i + 1] == '>')
	{
		(*i) += 2;
		return (new_token(TOKEN_APPEND, ">>"));
	}
	return (NULL);
}

/**
 * @brief Tokenizes a shell operator from the input string.
 *
 * @param input The input string being tokenized.
 * @param i A pointer to the current index in the input string.
 * @return A pointer to the newly created operator token.
 */
t_token	*handle_operator(char *input, int *i)
{
	t_token			*tok;
	t_token_type	type;
	char			two[3];

	two[0] = input[*i];
	two[1] = input[*i + 1];
	two[2] = '\0';
	tok = handle_double_op(input, i);
	if (tok)
		return (tok);
	if (input[*i] == '|')
		type = TOKEN_PIPE;
	else if (input[*i] == '<')
		type = TOKEN_REDIR_IN;
	else
		type = TOKEN_REDIR_OUT;
	(*i)++;
	two[1] = '\0';
	return (new_token(type, two));
}

/**
 * @brief Calculates the length of a quoted segment including quotes.
 *
 * @param s The string containing the quoted segment.
 * @param i The starting index of the opening quote.
 * @return The total length of the quoted segment.
 */
int	quote_len(char *s, int i)
{
	char	quote;
	int		len;

	quote = s[i];
	len = 1;
	i++;
	while (s[i] && s[i] != quote)
	{
		len++;
		i++;
	}
	if (s[i] == quote)
		len++;
	return (len);
}
