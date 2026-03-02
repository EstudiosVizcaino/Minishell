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
 * @brief Entry point for the parser, converts tokens into an AST.
 *
 * @param tokens The linked list of tokens to parse.
 * @return A pointer to the root of the AST, or NULL if tokens are empty.
 */
t_ast	*parser(t_token *tokens)
{
	t_token	*tok;

	tok = tokens;
	if (!tok)
		return (NULL);
	return (parse_pipeline(&tok));
}
