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

char	*join_free(char *s1, char *s2)
{
	char	*tmp;

	tmp = ft_strjoin(s1, s2);
	free(s1);
	free(s2);
	if (!tmp)
		fatal_error("malloc");
	return (tmp);
}
