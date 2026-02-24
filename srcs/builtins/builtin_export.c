#include "minishell.h"

static void	print_export(t_env *env)
{
	while (env)
	{
		ft_putstr_fd("declare -x ", STDOUT_FILENO);
		ft_putstr_fd(env->key, STDOUT_FILENO);
		if (env->value)
		{
			ft_putchar_fd('=', STDOUT_FILENO);
			ft_putchar_fd('"', STDOUT_FILENO);
			ft_putstr_fd(env->value, STDOUT_FILENO);
			ft_putchar_fd('"', STDOUT_FILENO);
		}
		ft_putchar_fd('\n', STDOUT_FILENO);
		env = env->next;
	}
}

static int	is_valid_key(char *key)
{
	int	i;

	if (!key || !key[0])
		return (0);
	if (!ft_isalpha(key[0]) && key[0] != '_')
		return (0);
	i = 1;
	while (key[i])
	{
		if (!ft_isalnum(key[i]) && key[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

static int	export_one(char *arg, t_shell *shell)
{
	char	*eq;
	char	*key;
	char	*value;
	int		ret;

	eq = ft_strchr(arg, '=');
	if (!eq)
	{
		if (is_valid_key(arg))
			env_set(&shell->env, arg, NULL);
		return (0);
	}
	key = ft_substr(arg, 0, eq - arg);
	if (!is_valid_key(key))
	{
		free(key);
		ft_putstr_fd("export: invalid identifier\n", STDERR_FILENO);
		return (1);
	}
	value = ft_strdup(eq + 1);
	ret = env_set(&shell->env, key, value);
	free(key);
	free(value);
	return (ret);
}

int	builtin_export(t_cmd *cmd, t_shell *shell)
{
	int	i;
	int	ret;

	if (!cmd->args[1])
	{
		print_export(shell->env);
		return (0);
	}
	ret = 0;
	i = 1;
	while (cmd->args[i])
	{
		if (export_one(cmd->args[i], shell))
			ret = 1;
		i++;
	}
	return (ret);
}
