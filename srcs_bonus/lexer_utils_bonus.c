#include "minishell.h"

int	is_operator(char c)
{
	if (c == '|' || c == '<' || c == '>')
		return (1);
	if (c == '&' || c == '(' || c == ')')
		return (1);
	return (0);
}

static t_token	*handle_double_op(char *input, int *i)
{
	if (input[*i] == '<' && input[*i + 1] == '<')
	{
		(*i) += 2;
		return (new_token(TOKEN_HEREDOC, "<<"));
	}
	if (input[*i] == '>' && input[*i + 1] == '>')
	{
		(*i) += 2;
		return (new_token(TOKEN_APPEND, ">>"));
	}
	return (NULL);
}

static t_token	*handle_bonus_op(char *input, int *i, char *two)
{
	if (input[*i] == '&' && input[*i + 1] == '&')
	{
		(*i) += 2;
		return (new_token(TOKEN_AND, "&&"));
	}
	if (input[*i] == '|' && input[*i + 1] == '|')
	{
		(*i) += 2;
		return (new_token(TOKEN_OR, "||"));
	}
	if (input[*i] == '(' || input[*i] == ')')
	{
		two[1] = '\0';
		(*i)++;
		if (two[0] == '(')
			return (new_token(TOKEN_LPAREN, two));
		return (new_token(TOKEN_RPAREN, two));
	}
	if (input[*i] == '&')
	{
		(*i)++;
		two[1] = '\0';
		return (new_token(TOKEN_WORD, two));
	}
	return (NULL);
}

t_token	*handle_operator(char *input, int *i)
{
	t_token			*tok;
	t_token_type	type;
	char			two[3];

	two[0] = input[*i];
	two[1] = input[*i + 1];
	two[2] = '\0';
	tok = handle_double_op(input, i);
	if (tok)
		return (tok);
	tok = handle_bonus_op(input, i, two);
	if (tok)
		return (tok);
	if (input[*i] == '|')
		type = TOKEN_PIPE;
	else if (input[*i] == '<')
		type = TOKEN_REDIR_IN;
	else
		type = TOKEN_REDIR_OUT;
	(*i)++;
	two[1] = '\0';
	return (new_token(type, two));
}

int	quote_len(char *s, int i)
{
	char	quote;
	int		len;

	quote = s[i];
	len = 1;
	i++;
	while (s[i] && s[i] != quote)
	{
		len++;
		i++;
	}
	if (s[i] == quote)
		len++;
	return (len);
}
