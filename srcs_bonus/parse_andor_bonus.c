#include "minishell.h"

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

static int	is_and_or_token(t_token *tok)
{
	if (!tok)
		return (0);
	return (tok->type == TOKEN_AND || tok->type == TOKEN_OR);
}

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
