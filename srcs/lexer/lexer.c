/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gisidro- <gisidro-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 14:49:41 by gisidro-          #+#    #+#             */
/*   Updated: 2026/02/04 15:07:18 by gisidro-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Allocates and initializes a new token node.
 *
 * @param type The type of the token.
 * @param value The string value of the token.
 * @return A pointer to the newly created token, or NULL on failure.
 */
t_token	*new_token(t_token_type type, char *value)
{
	t_token	*tok;

	tok = malloc(sizeof(t_token));
	if (!tok)
		return (NULL);
	tok->type = type;
	if (value)
		tok->value = ft_strdup(value);
	else
		tok->value = NULL;
	tok->next = NULL;
	return (tok);
}

/**
 * @brief Frees an entire linked list of tokens.
 *
 * @param tokens The head of the token linked list to free.
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
 * @brief Appends a token to the end of a token linked list.
 *
 * @param head A pointer to the head pointer of the token list.
 * @param tail A pointer to the tail pointer of the token list.
 * @param tok The token to append.
 * @return The appended token, or NULL if tok is NULL.
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
 * @brief Tokenizes an input string into a linked list of tokens.
 *
 * @param input The input string to tokenize.
 * @return A pointer to the head of the token linked list, or NULL on failure.
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
