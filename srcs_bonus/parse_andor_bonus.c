#include "minishell.h"

/**
 * @brief Parses a parenthesized subexpression or falls through to pipeline parsing.
 *
 * @param tokens Pointer to the current token list position.
 * @return The parsed AST node.
 */
static t_ast	*parse_parens(t_token **tokens)
{
	t_ast	*node;

	if (!*tokens || (*tokens)->type != TOKEN_LPAREN)
		return (parse_pipeline(tokens));
	*tokens = (*tokens)->next;
	node = parse_and_or(tokens);
	if (!node)
		return (NULL);
	if (*tokens && (*tokens)->type == TOKEN_RPAREN)
		*tokens = (*tokens)->next;
	return (node);
}

/**
 * @brief Checks if a token is an AND or OR operator.
 *
 * @param tok The token to check.
 * @return 1 if the token is AND or OR, 0 otherwise.
 */
static int	is_and_or_token(t_token *tok)
{
	if (!tok)
		return (0);
	return (tok->type == TOKEN_AND || tok->type == TOKEN_OR);
}

/**
 * @brief Creates an AND/OR AST node with left and right children.
 *
 * @param tokens Pointer to the current token list position.
 * @param left The left child AST node.
 * @return The new AND/OR AST node, or NULL on failure.
 */
static t_ast	*make_and_or(t_token **tokens, t_ast *left)
{
	t_ast		*node;
	t_node_type	type;

	if ((*tokens)->type == TOKEN_AND)
		type = NODE_AND;
	else
		type = NODE_OR;
	*tokens = (*tokens)->next;
	node = new_ast_node(type);
	if (!node)
	{
		free_ast(left);
		return (NULL);
	}
	node->left = left;
	node->right = parse_parens(tokens);
	if (!node->right)
	{
		free_ast(node);
		return (NULL);
	}
	return (node);
}

/**
 * @brief Parses a sequence of AND/OR operators with their operands.
 *
 * @param tokens Pointer to the current token list position.
 * @return The root AST node of the parsed expression.
 */
t_ast	*parse_and_or(t_token **tokens)
{
	t_ast	*left;
	t_ast	*node;

	left = parse_parens(tokens);
	if (!left)
		return (NULL);
	while (is_and_or_token(*tokens))
	{
		node = make_and_or(tokens, left);
		if (!node)
			return (NULL);
		left = node;
	}
	return (left);
}
