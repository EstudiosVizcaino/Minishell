/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 11:09:40 by cvizcain          #+#    #+#             */
/*   Updated: 2026/02/06 12:50:43 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Allocates a new environment variable node.
 *
 * The shell stores the environment as a singly-linked list of t_env
 * nodes instead of the raw "KEY=VALUE" string array because a linked
 * list makes lookup, insertion and deletion easier and cheaper.
 *
 * Both key and value are ft_strdup'd so the node owns its own memory
 * independently of whatever string the caller passed in.
 *
 * @param key   The variable name (e.g. "PATH").
 * @param value The variable value (e.g. "/usr/bin").  Can be NULL for
 *              variables that were exported without a value.
 * @return The new node, or NULL on malloc failure.
 */
t_env	*env_new(char *key, char *value)
{
	t_env	*node;

	node = malloc(sizeof(t_env));
	if (!node)
		return (NULL);

	/* ft_strdup allocates a fresh copy of the key string. */
	node->key = ft_strdup(key);

	/*
	** Value may legally be NULL (e.g. after `export FOO` without =VALUE).
	** In that case we store NULL directly instead of duplicating a NULL.
	*/
	if (value)
		node->value = ft_strdup(value);
	else
		node->value = NULL;

	node->next = NULL;

	/*
	** If either strdup failed, clean up both allocations we already made
	** and return NULL so the caller can detect the failure.
	*/
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
 * @brief Finds an environment variable node by its key name.
 *
 * Performs a linear scan of the linked list, comparing each node's
 * key with the requested key using ft_strcmp (exact match).
 *
 * @param env The head of the environment linked list.
 * @param key The key to search for.
 * @return A pointer to the matching t_env node, or NULL if not found.
 */
t_env	*env_find(t_env *env, char *key)
{
	while (env)
	{
		/*
		** ft_strcmp returns 0 when the two strings are identical.
		** Once we find a match we return the node immediately rather
		** than continuing the scan (keys are unique in the env list).
		*/
		if (ft_strcmp(env->key, key) == 0)
			return (env);
		env = env->next;
	}

	/* Reached the end of the list without a match. */
	return (NULL);
}

/**
 * @brief Gets the string value of an environment variable.
 *
 * This is the typical way other modules (e.g. the expander) look up
 * a variable.  It wraps env_find() and returns just the value string.
 *
 * @param env The head of the environment linked list.
 * @param key The key to search for.
 * @return The value string, or NULL if the key is not found.
 *         Callers must NOT free this pointer; it belongs to the node.
 */
char	*env_get(t_env *env, char *key)
{
	t_env	*node;

	/* Use env_find to locate the node. */
	node = env_find(env, key);
	if (!node)
		return (NULL);

	/* Return the stored value pointer (may be NULL for valueless vars). */
	return (node->value);
}

/**
 * @brief Converts the raw envp[] array into a linked list of t_env nodes.
 *
 * The operating system passes the environment to main() as a NULL-
 * terminated array of "KEY=VALUE" strings.  We convert that into our
 * linked list representation at startup so every other part of the
 * shell can use the cleaner env_get/env_set/env_unset API.
 *
 * @param envp The array of environment strings from main().
 * @return Head of the constructed t_env linked list.
 */
t_env	*env_init(char **envp)
{
	t_env	*head;  /* first node (returned to caller) */
	t_env	*tail;  /* last node (for O(1) appending) */
	t_env	*node;
	int		i;

	head = NULL;
	tail = NULL;
	i = 0;
	while (envp[i])
	{
		/*
		** env_parse_entry() splits one "KEY=VALUE" string into a t_env
		** node with separate key and value fields.
		** Strings without '=' are silently skipped (parse_entry returns
		** NULL for them).
		*/
		node = env_parse_entry(envp[i]);
		if (node)
		{
			if (!head)
				head = node;  /* first element */
			else
				tail->next = node;  /* append to existing list */
			tail = node;
		}
		i++;
	}
	return (head);
}

/**
 * @brief Frees the entire environment linked list.
 *
 * Called during shell cleanup to release all env memory so that
 * memory-leak checkers (e.g. valgrind) report no leaks.
 *
 * @param env Head of the environment linked list.
 */
void	env_free(t_env *env)
{
	t_env	*next;

	while (env)
	{
		next = env->next;   /* save next before freeing the node */
		free(env->key);     /* free the key string */
		free(env->value);   /* free the value string (safe if NULL) */
		free(env);          /* free the node itself */
		env = next;
	}
}
