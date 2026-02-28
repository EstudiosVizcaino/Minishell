#include "minishell.h"

static int	is_op_token(t_token_type type)
{
	return (type == TOKEN_PIPE || type == TOKEN_AND || type == TOKEN_OR);
}

static int	is_redir_type(t_token_type type)
{
	return (type == TOKEN_REDIR_IN || type == TOKEN_REDIR_OUT
		|| type == TOKEN_HEREDOC || type == TOKEN_APPEND);
}

static void	syntax_error(char *tok_val)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `",
		STDERR_FILENO);
	ft_putstr_fd(tok_val, STDERR_FILENO);
	ft_putstr_fd("'\n", STDERR_FILENO);
}

static int	is_invalid_word(t_token *tok)
{
	if (tok->type != TOKEN_WORD)
		return (0);
	if (ft_strcmp(tok->value, "&") == 0)
		return (1);
	return (0);
}

int	check_syntax(t_token *tokens)
{
	t_token	*tok;

	tok = tokens;
	if (!tok)
		return (0);
	if (is_op_token(tok->type))
	{
		syntax_error(tok->value);
		return (1);
	}
	while (tok)
	{
		if (is_invalid_word(tok))
		{
			syntax_error(tok->value);
			return (1);
		}
		if (is_redir_type(tok->type))
		{
			if (!tok->next || tok->next->type != TOKEN_WORD
				|| is_invalid_word(tok->next))
			{
				if (tok->next)
					syntax_error(tok->next->value);
				else
					syntax_error("newline");
				return (1);
			}
		}
		if (is_op_token(tok->type) && tok->next && is_op_token(tok->next->type))
		{
			syntax_error(tok->next->value);
			return (1);
		}
		if (is_op_token(tok->type) && !tok->next)
		{
			syntax_error(tok->value);
			return (1);
		}
		tok = tok->next;
	}
	return (0);
}

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

t_ast	*parse_and_or(t_token **tokens)
{
	t_ast			*left;
	t_ast			*node;
	t_node_type		type;

	left = parse_parens(tokens);
	if (!left)
		return (NULL);
	while (is_and_or_token(*tokens))
	{
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
		left = node;
	}
	return (left);
}
