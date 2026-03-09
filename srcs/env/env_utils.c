/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 16:38:04 by cvizcain          #+#    #+#             */
/*   Updated: 2026/01/30 16:24:38 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Removes an env var by key.
 *
 * Finds the node, unlinks it from the list,
 * and frees its memory.
 *
 * @param env Pointer to the env list head.
 * @param key The key to remove.
 */
void	env_unset(t_env **env, char *key)
{
	t_env	*curr;
	t_env	*prev;

	curr = *env;
	prev = NULL;
	while (curr)
	{
		if (ft_strcmp(curr->key, key) == 0)
		{
			if (prev)
				prev->next = curr->next;
			else
				*env = curr->next;
			free(curr->key);
			free(curr->value);
			free(curr);
			return ;
		}
		prev = curr;
		curr = curr->next;
	}
}

/**
 * @brief Fills a string array with "KEY=VALUE" entries from the env list.
 *
 * @param env The head of the environment linked list.
 * @param arr The string array to fill.
 */
static void	env_arr_fill(t_env *env, char **arr)
{
	t_env	*curr;
	char	*tmp;
	int		i;

	i = 0;
	curr = env;
	while (curr)
	{
		tmp = ft_strjoin(curr->key, "=");
		if (curr->value)
			arr[i] = ft_strjoin(tmp, curr->value);
		else
			arr[i] = ft_strjoin(tmp, "");
		free(tmp);
		i++;
		curr = curr->next;
	}
}

/**
 * @brief Converts the environment linked list to a NULL-terminated string array.
 *
 * @param env The head of the environment linked list.
 * @return A NULL-terminated array of "KEY=VALUE" strings, or NULL on failure.
 */
char	**env_to_array(t_env *env)
{
	t_env	*curr;
	char	**arr;
	int		count;

	count = 0;
	curr = env;
	while (curr)
	{
		count++;
		curr = curr->next;
	}
	arr = malloc((count + 1) * sizeof(char *));
	if (!arr)
		return (NULL);
	env_arr_fill(env, arr);
	arr[count] = NULL;
	return (arr);
}
