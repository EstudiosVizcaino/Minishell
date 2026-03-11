/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_entry_bonus.c                               :+:      :+:    :+:  */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 17:18:27 by cvizcain          #+#    #+#             */
/*   Updated: 2026/03/11 14:00:00 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Creates a new AST node.
 *
 * Mallocs a t_ast, sets its type, and NULLs
 * the cmd, left and right pointers.
 *
 * @param type The kind of node (CMD, PIPE, etc.).
 * @return The new node, or NULL on malloc fail.
 */
t_ast	*new_ast_node(t_node_type type)
{
	t_ast	*node;

	node = malloc(sizeof(t_ast));
	if (!node)
		return (NULL);
	node->type = type;
	node->cmd = NULL;
	node->left = NULL;
	node->right = NULL;
	return (node);
}

/**
 * @brief Creates a new empty command struct.
 *
 * Sets args and redirs to NULL so they can be
 * filled later by the parser.
 *
 * @return The new cmd, or NULL on malloc fail.
 */
t_cmd	*new_cmd(void)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->args = NULL;
	cmd->redirs = NULL;
	return (cmd);
}

/**
 * @brief Parses a pipeline (commands joined by |).
 *
 * Reads the first command, then if a pipe token
 * follows, it recursively parses the rest.
 *
 * @param tokens Current position in the token list.
 * @return AST subtree for the pipeline, or NULL.
 */
t_ast	*parse_pipeline(t_token **tokens)
{
	t_ast	*left;
	t_ast	*pipe_node;

	left = parse_command(tokens);
	if (!left)
		return (NULL);
	if (*tokens && (*tokens)->type == TOKEN_PIPE)
	{
		*tokens = (*tokens)->next;
		pipe_node = new_ast_node(NODE_PIPE);
		if (!pipe_node)
		{
			free_ast(left);
			return (NULL);
		}
		pipe_node->left = left;
		pipe_node->right = parse_pipeline(tokens);
		if (!pipe_node->right)
		{
			free_ast(pipe_node);
			return (NULL);
		}
		return (pipe_node);
	}
	return (left);
}

/**
 * @brief Entry point for the parser (bonus version using AND/OR parsing).
 *
 * @param tokens The head of the token list to parse.
 * @return The root AST node of the parsed input.
 */
t_ast	*parser(t_token *tokens)
{
	t_token	*tok;

	tok = tokens;
	if (!tok)
		return (NULL);
	return (parse_and_or(&tok));
}
