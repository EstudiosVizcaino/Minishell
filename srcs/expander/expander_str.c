#include "minishell.h"

static char	*expand_single_quote(char *str, int *i)
{
	char	*result;
	int		start;

	(*i)++;
	start = *i;
	while (str[*i] && str[*i] != '\'')
		(*i)++;
	result = ft_substr(str, start, *i - start);
	if (str[*i] == '\'')
		(*i)++;
	return (result);
}

static char	*expand_double_quote(char *str, int *i, t_shell *shell)
{
	char	*result;
	char	*part;
	char	buf[2];

	result = ft_strdup("");
	(*i)++;
	while (str[*i] && str[*i] != '"')
	{
		if (str[*i] == '$')
			part = expand_var(str, i, shell);
		else
		{
			buf[0] = str[(*i)++];
			buf[1] = '\0';
			part = ft_strdup(buf);
		}
		result = join_free(result, part);
	}
	if (str[*i] == '"')
		(*i)++;
	return (result);
}

static char	*expand_one(char *str, int *i, t_shell *shell)
{
	char	buf[2];

	if (str[*i] == '\'')
		return (expand_single_quote(str, i));
	if (str[*i] == '"')
		return (expand_double_quote(str, i, shell));
	if (str[*i] == '$')
		return (expand_var(str, i, shell));
	buf[0] = str[(*i)++];
	buf[1] = '\0';
	return (ft_strdup(buf));
}

char	*expand_str(char *str, t_shell *shell)
{
	char	*result;
	char	*part;
	int		i;

	if (!str)
		return (ft_strdup(""));
	result = ft_strdup("");
	i = 0;
	while (str[i])
	{
		part = expand_one(str, &i, shell);
		result = join_free(result, part);
	}
	return (result);
}
