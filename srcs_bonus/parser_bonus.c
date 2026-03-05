/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gisidro- <gisidro-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 12:24:00 by gisidro-          #+#    #+#             */
/*   Updated: 2026/02/28 16:16:59 by gisidro-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Checks if a token type is a pipe, AND, or OR operator.
 *
 * @param type The token type to check.
 * @return 1 if the type is an operator, 0 otherwise.
 */
static int	is_op_token(t_token_type type)
{
	return (type == TOKEN_PIPE || type == TOKEN_AND || type == TOKEN_OR);
}

/**
 * @brief Checks if a token type is a redirection.
 *
 * @param type The token type to check.
 * @return 1 if the type is a redirection, 0 otherwise.
 */
static int	is_redir_type(t_token_type type)
{
	return (type == TOKEN_REDIR_IN || type == TOKEN_REDIR_OUT
		|| type == TOKEN_HEREDOC || type == TOKEN_APPEND);
}

/**
 * @brief Prints a syntax error message for an unexpected token.
 *
 * @param tok_val The token value that caused the error.
 * @return Always returns 1 to indicate an error.
 */
static int	syntax_error(char *tok_val)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `",
		STDERR_FILENO);
	ft_putstr_fd(tok_val, STDERR_FILENO);
	ft_putstr_fd("'\n", STDERR_FILENO);
	return (1);
}

/**
 * @brief Validates a single token for syntax errors.
 *
 * @param tok The token to validate.
 * @return 1 if a syntax error is found, 0 otherwise.
 */
static int	check_token(t_token *tok)
{
	if (tok->type == TOKEN_WORD && ft_strcmp(tok->value, "&") == 0)
		return (syntax_error(tok->value));
	if (is_redir_type(tok->type) && (!tok->next
			|| tok->next->type != TOKEN_WORD))
	{
		if (tok->next)
			return (syntax_error(tok->next->value));
		return (syntax_error("newline"));
	}
	if (is_redir_type(tok->type) && tok->next
		&& tok->next->type == TOKEN_WORD
		&& ft_strcmp(tok->next->value, "&") == 0)
		return (syntax_error(tok->next->value));
	if (is_op_token(tok->type) && tok->next
		&& is_op_token(tok->next->type))
		return (syntax_error(tok->next->value));
	if (is_op_token(tok->type) && !tok->next)
		return (syntax_error(tok->value));
	return (0);
}

/**
 * @brief Validates the syntax of a complete token list.
 *
 * @param tokens The head of the token list to validate.
 * @return 1 if a syntax error is found, 0 otherwise.
 */
int	check_syntax(t_token *tokens)
{
	t_token	*tok;

	tok = tokens;
	if (!tok)
		return (0);
	if (is_op_token(tok->type))
		return (syntax_error(tok->value));
	while (tok)
	{
		if (check_token(tok))
			return (1);
		tok = tok->next;
	}
	return (0);
}
