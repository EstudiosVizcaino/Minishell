/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_entry.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 12:34:49 by cvizcain          #+#    #+#             */
/*   Updated: 2026/02/06 18:58:00 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Entry point of the parser.
 *
 * Takes the token list and kicks off pipeline
 * parsing. Returns the root of the AST.
 *
 * @param tokens The linked list of tokens.
 * @return Root of the AST, or NULL if empty.
 */
t_ast	*parser(t_token *tokens)
{
	t_token	*tok;

	tok = tokens;
	if (!tok)
		return (NULL);
	return (parse_pipeline(&tok));
}
