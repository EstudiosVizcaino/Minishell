/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gisidro- <gisidro-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 10:05:59 by gisidro-          #+#    #+#             */
/*   Updated: 2026/02/25 13:10:26 by gisidro-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Checks if a character is a shell operator (bonus version with
 *        &&, ||, parentheses).
 *
 * @param c The character to check.
 * @return 1 if the character is an operator, 0 otherwise.
 */
int	is_operator(char c)
{
	if (c == '|' || c == '<' || c == '>')
		return (1);
	if (c == '&' || c == '(' || c == ')')
		return (1);
	return (0);
}

/**
 * @brief Handles double-character operators (heredoc and append).
 *
 * @param input The input string being tokenized.
 * @param i Pointer to the current index in the input string.
 * @return A new token for the operator, or NULL if not matched.
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
 * @brief Handles bonus operators (&&, ||, parentheses, bare &).
 *
 * @param input The input string being tokenized.
 * @param i Pointer to the current index in the input string.
 * @param two Buffer holding the current two-character sequence.
 * @return A new token for the operator, or NULL if not matched.
 */
static t_token	*handle_bonus_op(char *input, int *i, char *two)
{
	if (input[*i] == '&' && input[*i + 1] == '&')
	{
		(*i) += 2;
		return (new_token(TOKEN_AND, "&&"));
	}
	if (input[*i] == '|' && input[*i + 1] == '|')
	{
		(*i) += 2;
		return (new_token(TOKEN_OR, "||"));
	}
	if (input[*i] == '(' || input[*i] == ')')
	{
		two[1] = '\0';
		(*i)++;
		if (two[0] == '(')
			return (new_token(TOKEN_LPAREN, two));
		return (new_token(TOKEN_RPAREN, two));
	}
	if (input[*i] == '&')
	{
		(*i)++;
		two[1] = '\0';
		return (new_token(TOKEN_WORD, two));
	}
	return (NULL);
}

/**
 * @brief Tokenizes a shell operator from the input string (bonus version).
 *
 * @param input The input string being tokenized.
 * @param i Pointer to the current index in the input string.
 * @return A new token representing the operator.
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
	tok = handle_bonus_op(input, i, two);
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
 * @param s The input string containing the quoted segment.
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
