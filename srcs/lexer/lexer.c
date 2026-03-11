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

	/* Allocate the token node itself. */
	tok = malloc(sizeof(t_token));
	if (!tok)
		return (NULL);

	/* Store the semantic type so the parser knows what this token means
	** (e.g. TOKEN_PIPE means '|', TOKEN_WORD means a command/argument). */
	tok->type = type;

	/*
	** ft_strdup copies the value string into its own heap allocation so
	** that the token owns its data independently of wherever 'value'
	** was pointing.  This lets callers safely free the original buffer.
	** If value is NULL (e.g. for an EOF marker) we store NULL directly.
	*/
	if (value)
		tok->value = ft_strdup(value);
	else
		tok->value = NULL;

	/* next is NULL because newly-created nodes are not yet in any list. */
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
		/* Save the next pointer before freeing, otherwise we lose it. */
		next = tokens->next;

		/* Free the heap-allocated string inside the token. */
		free(tokens->value);

		/* Free the token struct itself. */
		free(tokens);

		tokens = next;
	}
}

/**
 * @brief Adds a token at the end of the list.
 *
 * Maintains both a head (for return) and tail (for O(1) append).
 * Updates head if the list was empty, otherwise links after the tail.
 *
 * @param head Pointer to the head pointer.
 * @param tail Pointer to the tail pointer.
 * @param tok  The token to add.
 * @return The added token, or NULL if tok is NULL.
 */
static t_token	*append_token(t_token **head, t_token **tail, t_token *tok)
{
	/* If tok is NULL a malloc failed upstream; propagate the NULL. */
	if (!tok)
		return (NULL);

	/*
	** If the list is empty, this token becomes the head.
	** Otherwise we link it after the current last element (*tail).
	** Either way, *tail is updated to point at the new last element.
	*/
	if (!*head)
		*head = tok;
	else
		(*tail)->next = tok;
	*tail = tok;
	return (tok);
}

/**
 * @brief Main lexer function – converts a raw input string into tokens.
 *
 * The lexer is the first stage of the shell pipeline.
 * It walks the input left-to-right, skips whitespace, and decides
 * whether the next character starts an operator or a word.
 *
 * @param input The raw input string typed by the user.
 * @return Head of the token linked list, or NULL on error/empty input.
 */
t_token	*lexer(char *input)
{
	t_token	*head;   /* first token in the list (returned to caller) */
	t_token	*tail;   /* last token so far (for O(1) appending) */
	t_token	*tok;    /* the token just produced */
	int		i;       /* current read position in 'input' */

	head = NULL;
	tail = NULL;
	i = 0;
	while (input[i])
	{
		/*
		** Skip over spaces and horizontal tabs between tokens.
		** Newlines inside the string are part of multi-line input and
		** are handled separately (they appear inside quoted strings).
		*/
		while (input[i] == ' ' || input[i] == '\t')
			i++;

		/* After skipping whitespace we might be at the end of the string. */
		if (!input[i])
			break ;

		/*
		** Decide what kind of token to create next:
		**   - is_operator() returns true for '|', '<', and '>'.
		**     handle_operator() then determines the exact type
		**     (|, <, >, <<, >>) and advances 'i'.
		**   - Otherwise, handle_word() collects all characters up to
		**     the next whitespace or operator and creates a TOKEN_WORD.
		*/
		if (is_operator(input[i]))
			tok = handle_operator(input, &i);
		else
			tok = handle_word(input, &i);

		/*
		** append_token returns NULL only if 'tok' itself is NULL, which
		** means a malloc failed.  In that case we free what we built so
		** far and return NULL to signal the failure to the caller.
		*/
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
 * Scans through tracking whether we are currently inside a single
 * or double quote context.  Used by read_full_input() in main.c to
 * decide whether to keep prompting the user for more input.
 *
 * @param input The input string to scan.
 * @return 1 if there is an unclosed quote, 0 otherwise.
 */
int	has_unclosed_quote(char *input)
{
	char	quote; /* '\'' or '"' while inside a quoted region, else 0 */
	int		i;

	quote = 0;
	i = 0;
	while (input[i])
	{
		if (quote)
		{
			/*
			** We are currently inside a quoted region.
			** The only character that matters is the matching closing
			** quote – everything else (including the other quote char)
			** is treated as literal text.
			*/
			if (input[i] == quote)
				quote = 0;  /* found the closing quote – region ends */
		}
		else if (input[i] == '\'' || input[i] == '"')
			quote = input[i];  /* entering a quoted region */
		i++;
	}

	/*
	** If 'quote' is still non-zero at the end we never found the
	** closing quote, so the line is incomplete.
	*/
	return (quote != 0);
}
