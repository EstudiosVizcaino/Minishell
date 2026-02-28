#include "minishell.h"

static char	**merge_args(char **old, int idx, char **exp, int old_len)
{
	char	**new_args;
	int		exp_len;
	int		i;
	int		j;

	exp_len = array_len(exp);
	new_args = malloc((old_len - 1 + exp_len + 1) * sizeof(char *));
	if (!new_args)
		return (NULL);
	i = 0;
	j = 0;
	while (j < idx)
		new_args[i++] = old[j++];
	j = 0;
	while (exp[j])
		new_args[i++] = ft_strdup(exp[j++]);
	j = idx + 1;
	while (j < old_len)
		new_args[i++] = old[j++];
	new_args[i] = NULL;
	return (new_args);
}

static int	replace_wildcard(t_cmd *cmd, int i)
{
	char	**exp;
	char	**new_args;
	int		exp_len;
	int		len;

	exp = expand_wildcard(cmd->args[i]);
	if (!exp)
		return (i + 1);
	exp_len = array_len(exp);
	len = array_len(cmd->args);
	new_args = merge_args(cmd->args, i, exp, len);
	free_array(exp);
	if (!new_args)
		return (-1);
	free(cmd->args[i]);
	free(cmd->args);
	cmd->args = new_args;
	return (i + exp_len);
}

static void	unmask_all_args(t_cmd *cmd)
{
	int	i;

	if (!cmd->args)
		return ;
	i = 0;
	while (cmd->args[i])
	{
		unmask_wildcards(cmd->args[i]);
		i++;
	}
}

void	expand_wildcards_cmd(t_cmd *cmd)
{
	int	i;

	if (!cmd || !cmd->args)
		return ;
	i = 0;
	while (cmd->args[i])
	{
		if (!ft_strchr(cmd->args[i], '*'))
		{
			i++;
			continue ;
		}
		i = replace_wildcard(cmd, i);
		if (i < 0)
			return ;
	}
	unmask_all_args(cmd);
}
