/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 14:49:41 by cvizcain          #+#    #+#             */
/*   Updated: 2026/02/04 15:07:18 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Creates a new token node.
 *
 * Mallocs a t_token, sets its type and dups the
 * value string. Returns NULL if malloc fails.
 *
 * @param type  Token type (WORD, PIPE, etc.).
 * @param value The string value to store.
 * @return New token or NULL on failure.
 */
t_token	*new_token(t_token_type type, char *value)
{
	t_token	*tok;

	tok = malloc(sizeof(t_token));
	if (!tok)
		return (NULL);
	tok->type = type;
	if (value)
	{
		tok->value = ft_strdup(value);
		if (!tok->value)
		{
			free(tok);
			return (NULL);
		}
	}
	else
		tok->value = NULL;
	tok->next = NULL;
	return (tok);
}

/**
 * @brief Frees a whole token linked list.
 *
 * Walks through each node, frees value and node.
 *
 * @param tokens Head of the list to free.
 */
void	free_tokens(t_token *tokens)
{
	t_token	*next;

	while (tokens)
	{
		next = tokens->next;
		free(tokens->value);
		free(tokens);
		tokens = next;
	}
}

/**
 * @brief Adds a token at the end of the list.
 *
 * Updates head if the list was empty, otherwise
 * links it after the current tail.
 *
 * @param head Pointer to the head pointer.
 * @param tail Pointer to the tail pointer.
 * @param tok  The token to add.
 * @return The added token, or NULL if tok is NULL.
 */
static t_token	*append_token(t_token **head, t_token **tail, t_token *tok)
{
	if (!tok)
		return (NULL);
	if (!*head)
		*head = tok;
	else
		(*tail)->next = tok;
	*tail = tok;
	return (tok);
}

/**
 * @brief Main lexer function, turns input into tokens.
 *
 * Skips whitespace, then checks if the next char is
 * an operator or a word and creates the right token.
 *
 * @param input The raw input string.
 * @return Head of the token list, or NULL on error.
 */
t_token	*lexer(char *input)
{
	t_token	*head;
	t_token	*tail;
	t_token	*tok;
	int		i;

	head = NULL;
	tail = NULL;
	i = 0;
	while (input[i])
	{
		while (input[i] == ' ' || input[i] == '\t')
			i++;
		if (!input[i])
			break ;
		if (is_operator(input[i]))
			tok = handle_operator(input, &i);
		else
			tok = handle_word(input, &i);
		if (!append_token(&head, &tail, tok))
		{
			free_tokens(head);
			return (NULL);
		}
	}
	return (head);
}

/**
 * @brief Checks if an input string has an unclosed quote.
 *
 * Scans through tracking whether the current char
 * is inside a single or double quote context.
 *
 * @param input The input string to scan.
 * @return 1 if there is an unclosed quote, 0 otherwise.
 */
int	has_unclosed_quote(char *input)
{
	char	quote;
	int		i;

	quote = 0;
	i = 0;
	while (input[i])
	{
		if (quote)
		{
			if (input[i] == quote)
				quote = 0;
		}
		else if (input[i] == '\'' || input[i] == '"')
			quote = input[i];
		i++;
	}
	return (quote != 0);
}
