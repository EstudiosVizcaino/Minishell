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

static int	syntax_error(char *tok_val)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `",
		STDERR_FILENO);
	ft_putstr_fd(tok_val, STDERR_FILENO);
	ft_putstr_fd("'\n", STDERR_FILENO);
	return (1);
}

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
