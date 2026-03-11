/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 11:55:40 by cvizcain          #+#    #+#             */
/*   Updated: 2026/02/07 14:53:40 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Allocates and initialises a new AST node.
 *
 * The Abstract Syntax Tree (AST) is the data structure that the parser
 * builds to represent the logical structure of the command line.
 * Every node has a type (e.g. NODE_CMD for a simple command, NODE_PIPE
 * for a pipeline) and pointers to its left/right children and optional
 * command data.
 *
 * @param type The kind of node (CMD, PIPE, etc.).
 * @return The new node, or NULL on malloc failure.
 */
t_ast	*new_ast_node(t_node_type type)
{
	t_ast	*node;

	node = malloc(sizeof(t_ast));
	if (!node)
		return (NULL);

	/* Record what kind of node this is so the executor can dispatch
	** to the correct handler (exec_cmd, exec_pipe, etc.). */
	node->type = type;

	/* cmd is only used by NODE_CMD nodes; NULL for everything else. */
	node->cmd = NULL;

	/* left/right are the children in the binary tree.
	** For a pipe "a | b": left = node for 'a', right = node for 'b'. */
	node->left = NULL;
	node->right = NULL;
	return (node);
}

/**
 * @brief Allocates and initialises a new, empty command struct.
 *
 * t_cmd holds all the data for one simple command: the argument array
 * (args[0] = command name, args[1..] = arguments) and the list of
 * redirections attached to that command.
 *
 * @return The new cmd struct, or NULL on malloc failure.
 */
t_cmd	*new_cmd(void)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);

	/* Start with empty args and no redirections.
	** parse_command() will fill these in after calling this function. */
	cmd->args = NULL;
	cmd->redirs = NULL;
	return (cmd);
}

/**
 * @brief Parses a pipeline: one or more commands separated by '|'.
 *
 * This is a recursive-descent parser for pipelines.
 * The grammar it implements is:
 *
 *   pipeline := command ( '|' pipeline )?
 *
 * i.e. a pipeline is a command optionally followed by a pipe and
 * the rest of the pipeline.  The recursion naturally produces a
 * right-leaning binary tree, which the executor walks left-to-right.
 *
 * @param tokens Pointer to the current position in the token list.
 * @return The AST subtree for this pipeline, or NULL on error.
 */
t_ast	*parse_pipeline(t_token **tokens)
{
	t_ast	*left;
	t_ast	*pipe_node;

	/*
	** Parse the left-hand command first.
	** parse_command() consumes TOKEN_WORD and redirection tokens until
	** it hits a TOKEN_PIPE or runs out of tokens.
	*/
	left = parse_command(tokens);
	if (!left)
		return (NULL);

	/*
	** Peek at the next token.  If it is TOKEN_PIPE we have more of the
	** pipeline to process, otherwise this command is the whole pipeline.
	*/
	if (*tokens && (*tokens)->type == TOKEN_PIPE)
	{
		/*
		** Consume the '|' token so the recursive call starts on
		** the next command, not on the pipe itself.
		*/
		*tokens = (*tokens)->next;

		/* Create the pipe node that will connect left and right. */
		pipe_node = new_ast_node(NODE_PIPE);
		if (!pipe_node)
		{
			free_ast(left);  /* don't leak the already-built left side */
			return (NULL);
		}

		/* The command we just parsed is the left child of the pipe. */
		pipe_node->left = left;

		/*
		** Recursively parse the rest of the pipeline as the right child.
		** If the user typed "a | b | c", this recursion yields:
		**   pipe(a, pipe(b, c))
		*/
		pipe_node->right = parse_pipeline(tokens);
		if (!pipe_node->right)
		{
			/* Syntax error or malloc fail – clean up everything. */
			free_ast(pipe_node);
			return (NULL);
		}
		return (pipe_node);
	}

	/* No pipe followed – the single command IS the full pipeline. */
	return (left);
}

/**
 * @brief Entry point of the parser.
 *
 * Receives the flat token list from the lexer and kicks off the
 * recursive-descent parsing.  Returns the root of the AST which the
 * expander and executor will then process.
 *
 * @param tokens The linked list of tokens produced by the lexer.
 * @return Root node of the AST, or NULL for empty input.
 */
t_ast	*parser(t_token *tokens)
{
	t_token	*tok;

	/*
	** We work through a local pointer so the caller's list head is
	** not modified (they need it later to free the list).
	** parse_pipeline will advance 'tok' as it consumes tokens.
	*/
	tok = tokens;
	if (!tok)
		return (NULL);
	return (parse_pipeline(&tok));
}
