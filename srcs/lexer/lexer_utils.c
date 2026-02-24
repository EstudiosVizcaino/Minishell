#include "minishell.h"

int	is_operator(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

t_token	*handle_operator(char *input, int *i)
{
	t_token_type	type;
	char			two[3];

	two[0] = input[*i];
	two[1] = input[*i + 1];
	two[2] = '\0';
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

static int	word_end(char *input, int i)
{
	while (input[i] && input[i] != ' ' && input[i] != '\t'
		&& !is_operator(input[i]))
	{
		if (input[i] == '\'' || input[i] == '"')
			i += quote_len(input, i);
		else
			i++;
	}
	return (i);
}

t_token	*handle_word(char *input, int *i)
{
	t_token	*tok;
	char	*word;
	int		end;
	int		len;

	end = word_end(input, *i);
	len = end - *i;
	word = ft_substr(input, *i, len);
	if (!word)
		return (NULL);
	tok = new_token(TOKEN_WORD, word);
	free(word);
	*i = end;
	return (tok);
}
