/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adherrer <adherrer@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/12 16:12:06 by adherrer          #+#    #+#             */
/*   Updated: 2026/02/23 11:42:27 by adherrer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Counts the number of nodes in the environment linked list.
 *
 * @param env The head of the environment linked list.
 * @return The number of nodes.
 */
static int	count_env(t_env *env)
{
	int	n;

	n = 0;
	while (env)
	{
		n++;
		env = env->next;
	}
	return (n);
}

/**
 * @brief qsort comparator for t_env pointers by key (alphabetical).
 *
 * @param a Pointer to first t_env pointer.
 * @param b Pointer to second t_env pointer.
 * @return Negative, zero, or positive integer as per ft_strcmp.
 */
static int	cmp_env_key(const void *a, const void *b)
{
	return (ft_strcmp((*(t_env **)a)->key, (*(t_env **)b)->key));
}

/**
 * @brief Builds a sorted array of env node pointers (by key, alphabetically).
 *
 * @param env The head of the environment linked list.
 * @param n The number of nodes.
 * @return A malloc'd array of t_env pointers sorted by key, or NULL on failure.
 */
static t_env	**build_sorted_env(t_env *env, int n)
{
	t_env	**arr;
	int		i;

	arr = malloc((n + 1) * sizeof(t_env *));
	if (!arr)
		return (NULL);
	i = 0;
	while (env)
	{
		arr[i++] = env;
		env = env->next;
	}
	arr[n] = NULL;
	qsort(arr, n, sizeof(t_env *), cmp_env_key);
	return (arr);
}

/**
 * @brief Prints a single env node in export (declare -x) format.
 *
 * @param node The env node to print.
 */
static void	print_env_node(t_env *node)
{
	ft_putstr_fd("declare -x ", STDOUT_FILENO);
	ft_putstr_fd(node->key, STDOUT_FILENO);
	if (node->value)
	{
		ft_putchar_fd('=', STDOUT_FILENO);
		ft_putchar_fd('"', STDOUT_FILENO);
		ft_putstr_fd(node->value, STDOUT_FILENO);
		ft_putchar_fd('"', STDOUT_FILENO);
	}
	ft_putchar_fd('\n', STDOUT_FILENO);
}

/**
 * @brief Prints all environment variables in sorted export format.
 *
 * @param env Pointer to the environment variable list.
 */
static void	print_export(t_env *env)
{
	t_env	**sorted;
	int		n;
	int		i;

	n = count_env(env);
	sorted = build_sorted_env(env, n);
	if (!sorted)
	{
		ft_putstr_fd("export: malloc failed\n", STDERR_FILENO);
		return ;
	}
	i = 0;
	while (i < n)
	{
		print_env_node(sorted[i]);
		i++;
	}
	free(sorted);
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
