/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gisidro- <gisidro-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 11:09:40 by gisidro-          #+#    #+#             */
/*   Updated: 2026/02/06 12:50:43 by gisidro-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Creates a new environment variable node.
 *
 * @param key The key of the environment variable.
 * @param value The value of the environment variable.
 * @return A pointer to the new env node, or NULL on failure.
 */
t_env	*env_new(char *key, char *value)
{
	t_env	*node;

	node = malloc(sizeof(t_env));
	if (!node)
		return (NULL);
	node->key = ft_strdup(key);
	if (value)
		node->value = ft_strdup(value);
	else
		node->value = NULL;
	node->next = NULL;
	if (!node->key || (value && !node->value))
	{
		free(node->key);
		free(node->value);
		free(node);
		return (NULL);
	}
	return (node);
}

/**
 * @brief Finds an environment variable node by key.
 *
 * @param env The head of the environment linked list.
 * @param key The key to search for.
 * @return A pointer to the matching env node, or NULL if not found.
 */
t_env	*env_find(t_env *env, char *key)
{
	while (env)
	{
		if (ft_strcmp(env->key, key) == 0)
			return (env);
		env = env->next;
	}
	return (NULL);
}

/**
 * @brief Gets the value of an environment variable by key.
 *
 * @param env The head of the environment linked list.
 * @param key The key to search for.
 * @return The value string, or NULL if the key is not found.
 */
char	*env_get(t_env *env, char *key)
{
	t_env	*node;

	node = env_find(env, key);
	if (!node)
		return (NULL);
	return (node->value);
}

/**
 * @brief Initializes the environment linked list from the envp array.
 *
 * @param envp The array of environment strings.
 * @return A pointer to the head of the environment linked list.
 */
t_env	*env_init(char **envp)
{
	t_env	*head;
	t_env	*tail;
	t_env	*node;
	int		i;

	head = NULL;
	tail = NULL;
	i = 0;
	while (envp[i])
	{
		node = env_parse_entry(envp[i]);
		if (node)
		{
			if (!head)
				head = node;
			else
				tail->next = node;
			tail = node;
		}
		i++;
	}
	return (head);
}

/**
 * @brief Frees the entire environment linked list.
 *
 * @param env The head of the environment linked list.
 */
void	env_free(t_env *env)
{
	t_env	*next;

	while (env)
	{
		next = env->next;
		free(env->key);
		free(env->value);
		free(env);
		env = next;
	}
}
