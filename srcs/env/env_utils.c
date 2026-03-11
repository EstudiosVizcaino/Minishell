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
 * @brief Splits a "KEY=VALUE" string into a t_env node.
 *
 * The OS gives us strings like "PATH=/usr/bin:/bin".  We need to split
 * on the first '=' to separate the key from the value so both parts
 * can be managed independently (e.g. `export` can update just the
 * value without touching the key).
 *
 * @param entry A single "KEY=VALUE" string from envp[].
 * @return A new t_env node, or NULL on malloc failure or missing '='.
 */
t_env	*env_parse_entry(char *entry)
{
	t_env	*node;
	char	*eq;    /* pointer to the '=' character inside entry */
	char	*key;
	char	*value;

	/*
	** ft_strchr scans 'entry' for the first '=' character and returns
	** a pointer to it.  If there is no '=', this entry has no value
	** (e.g. a shell function) – we skip it by returning NULL.
	*/
	eq = ft_strchr(entry, '=');
	if (!eq)
		return (NULL);

	/*
	** Extract the key: it starts at index 0 and is (eq - entry) chars long.
	** ft_substr allocates a fresh copy so we can free it independently.
	*/
	key = ft_substr(entry, 0, eq - entry);
	if (!key)
		return (NULL);

	/*
	** The value is everything after the '='.  eq+1 skips past the '='
	** itself; ft_strdup copies it into a new allocation.
	*/
	value = ft_strdup(eq + 1);
	if (!value)
	{
		free(key);
		return (NULL);
	}

	/*
	** env_new() copies key and value again internally, so we own the
	** local copies and must free them after the call.
	*/
	node = env_new(key, value);
	free(key);
	free(value);
	return (node);
}

/**
 * @brief Sets or updates an environment variable.
 *
 * If the key already exists in the list, its value is replaced in
 * place (the node is reused).  If the key is new, a fresh node is
 * prepended at the head of the list so that lookups for recently-set
 * variables are fast.
 *
 * @param env   Double pointer to the head of the env list (may change
 *              if a new node is prepended).
 * @param key   The variable name.
 * @param value The new value (can be NULL for valueless export).
 * @return 0 on success, 1 on malloc failure.
 */
int	env_set(t_env **env, char *key, char *value)
{
	t_env	*node;
	t_env	*new;
	char	*val;

	/*
	** First check whether the variable is already in the list.
	** If it is, update its value in place (no need to allocate a node).
	*/
	node = env_find(*env, key);
	if (node)
	{
		/*
		** Duplicate the new value before freeing the old one, so that
		** if the duplication fails the old value is still intact.
		*/
		if (value)
			val = ft_strdup(value);
		else
			val = NULL;
		if (value && !val)
			return (1);  /* malloc failed; original value unchanged */
		free(node->value);  /* release the old value string */
		node->value = val;  /* store the new value (may be NULL) */
		return (0);
	}

	/*
	** Key not found → create a new node and prepend it to the list.
	** Prepending is O(1) and avoids a full scan to find the tail.
	*/
	new = env_new(key, value);
	if (!new)
		return (1);
	new->next = *env;   /* the new node points to the old head */
	*env = new;         /* the caller's head pointer now points at the new node */
	return (0);
}

/**
 * @brief Removes a variable from the environment list by key.
 *
 * Performs a standard linked-list unlink operation: find the node,
 * relink the previous node (or the head pointer) to skip over it,
 * then free the node's memory.
 *
 * @param env Pointer to the head pointer of the env list.
 * @param key The key of the variable to remove.
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
			/*
			** Found the node to remove.
			** If there is a previous node, relink it to skip 'curr'.
			** If 'curr' is the head (prev == NULL), update the head pointer.
			*/
			if (prev)
				prev->next = curr->next;
			else
				*env = curr->next;

			/* Release the memory owned by this node. */
			free(curr->key);
			free(curr->value);
			free(curr);
			return ;
		}
		prev = curr;
		curr = curr->next;
	}
	/* If the key was not found we simply return without doing anything. */
}

/**
 * @brief Fills a pre-allocated string array with "KEY=VALUE" strings.
 *
 * This is a helper for env_to_array().  It walks the linked list once,
 * building one "KEY=VALUE" string per node by joining the key, "=", and
 * the value.  The intermediate string (key + "=") is freed immediately
 * after being consumed by the second join.
 *
 * @param env The head of the environment linked list.
 * @param arr The array to fill (must have at least count+1 slots).
 */
static void	env_arr_fill(t_env *env, char **arr)
{
	t_env	*curr;
	char	*tmp;   /* temporary "KEY=" string */
	int		i;

	i = 0;
	curr = env;
	while (curr)
	{
		/*
		** First join: "KEY" + "=" → "KEY="
		** We need the intermediate string because ft_strjoin takes
		** two arguments; there is no three-argument variant for this path.
		*/
		tmp = ft_strjoin(curr->key, "=");

		/*
		** Second join: "KEY=" + value → "KEY=VALUE"
		** If the value is NULL (valueless export) we append an empty
		** string so the result is still "KEY=" (valid for execve).
		*/
		if (curr->value)
			arr[i] = ft_strjoin(tmp, curr->value);
		else
			arr[i] = ft_strjoin(tmp, "");

		free(tmp);   /* the intermediate "KEY=" string is no longer needed */
		i++;
		curr = curr->next;
	}
}

/**
 * @brief Converts the env linked list to a NULL-terminated string array.
 *
 * execve() requires the environment as a char** array of "KEY=VALUE"
 * strings terminated by a NULL pointer.  This function builds that
 * array from our linked list representation so we can pass it to execve.
 *
 * @param env The head of the environment linked list.
 * @return A newly-allocated NULL-terminated array, or NULL on failure.
 *         The caller is responsible for freeing the array and its strings.
 */
char	**env_to_array(t_env *env)
{
	t_env	*curr;
	char	**arr;
	int		count;

	/* Count the nodes so we know how large to make the array. */
	count = 0;
	curr = env;
	while (curr)
	{
		count++;
		curr = curr->next;
	}

	/*
	** Allocate count+1 pointers: one per variable plus the NULL sentinel
	** that execve uses to know where the array ends.
	*/
	arr = malloc((count + 1) * sizeof(char *));
	if (!arr)
		return (NULL);

	/* Fill the array with "KEY=VALUE" strings. */
	env_arr_fill(env, arr);

	/* Terminate with NULL as required by execve. */
	arr[count] = NULL;
	return (arr);
}
