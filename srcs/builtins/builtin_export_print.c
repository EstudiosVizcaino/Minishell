/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export_print.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gisidro- <gisidro-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/12 16:12:06 by gisidro-          #+#    #+#             */
/*   Updated: 2026/02/23 11:42:27 by gisidro-         ###   ########.fr       */
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
 * @brief Builds a sorted array of env node pointers by key.
 *
 * @param env The head of the environment linked list.
 * @param n The number of nodes.
 * @return A malloc'd sorted array of t_env pointers, or NULL on failure.
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
void	print_export(t_env *env)
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
