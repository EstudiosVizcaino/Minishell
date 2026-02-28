#include "minishell.h"

/**
 * @brief Prints all environment variables in export format.
 *
 * @param env Pointer to the environment variable list.
 */
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

/**
 * @brief Validates an environment variable key name.
 *
 * @param key The key string to validate.
 * @return 1 if valid, 0 otherwise.
 */
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

/**
 * @brief Handles exporting a variable with an assigned value.
 *
 * @param arg The full argument string.
 * @param eq Pointer to the '=' character in arg.
 * @param shell Pointer to the shell structure.
 * @return 0 on success, 1 on failure.
 */
static int	export_with_value(char *arg, char *eq, t_shell *shell)
{
	char	*key;
	char	*value;
	int		ret;

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

/**
 * @brief Exports a single variable argument.
 *
 * @param arg The argument string to export.
 * @param shell Pointer to the shell structure.
 * @return 0 on success, 1 on failure.
 */
static int	export_one(char *arg, t_shell *shell)
{
	char	*eq;

	eq = ft_strchr(arg, '=');
	if (!eq)
	{
		if (!is_valid_key(arg))
		{
			ft_putstr_fd("export: invalid identifier\n", STDERR_FILENO);
			return (1);
		}
		env_set(&shell->env, arg, NULL);
		return (0);
	}
	return (export_with_value(arg, eq, shell));
}

/**
 * @brief Implements the export builtin command.
 *
 * @param cmd Pointer to the command structure.
 * @param shell Pointer to the shell structure.
 * @return 0 on success, 1 if any export failed.
 */
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
