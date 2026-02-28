#include "minishell.h"

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
