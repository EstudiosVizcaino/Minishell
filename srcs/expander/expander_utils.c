#include "minishell.h"

char	*get_var_value(char *name, t_shell *shell)
{
	char	*val;

	if (!name)
		return (ft_strdup(""));
	if (ft_strcmp(name, "?") == 0)
		return (ft_itoa(shell->last_exit));
	val = env_get(shell->env, name);
	if (!val)
		return (ft_strdup(""));
	return (ft_strdup(val));
}

char	*expand_var(char *str, int *i, t_shell *shell)
{
	int		start;
	char	*name;
	char	*val;

	(*i)++;
	if (str[*i] == '?')
	{
		(*i)++;
		return (ft_itoa(shell->last_exit));
	}
	start = *i;
	while (str[*i] && (ft_isalnum(str[*i]) || str[*i] == '_'))
		(*i)++;
	if (*i == start)
		return (ft_strdup("$"));
	name = ft_substr(str, start, *i - start);
	if (!name)
		return (ft_strdup(""));
	val = get_var_value(name, shell);
	free(name);
	return (val);
}

static char	*join_free(char *s1, char *s2)
{
	char	*tmp;

	tmp = ft_strjoin(s1, s2);
	free(s1);
	free(s2);
	if (!tmp)
		fatal_error("malloc");
	return (tmp);
}

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

char	*expand_str(char *str, t_shell *shell)
{
	char	*result;
	char	*part;
	char	buf[2];
	int		i;

	if (!str)
		return (ft_strdup(""));
	result = ft_strdup("");
	i = 0;
	while (str[i])
	{
		if (str[i] == '\'')
			part = expand_single_quote(str, &i);
		else if (str[i] == '"')
			part = expand_double_quote(str, &i, shell);
		else if (str[i] == '$')
			part = expand_var(str, &i, shell);
		else
		{
			buf[0] = str[i++];
			buf[1] = '\0';
			part = ft_strdup(buf);
		}
		result = join_free(result, part);
	}
	return (result);
}
