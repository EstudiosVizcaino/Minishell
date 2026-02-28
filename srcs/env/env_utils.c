#include "minishell.h"

/**
 * @brief Parses a single "KEY=VALUE" string into an env node.
 *
 * @param entry The environment string to parse.
 * @return A pointer to the new env node, or NULL on failure.
 */
t_env	*env_parse_entry(char *entry)
{
	t_env	*node;
	char	*eq;
	char	*key;
	char	*value;

	eq = ft_strchr(entry, '=');
	if (!eq)
		return (NULL);
	key = ft_substr(entry, 0, eq - entry);
	if (!key)
		return (NULL);
	value = ft_strdup(eq + 1);
	if (!value)
	{
		free(key);
		return (NULL);
	}
	node = env_new(key, value);
	free(key);
	free(value);
	return (node);
}

/**
 * @brief Sets or updates an environment variable.
 *
 * @param env A pointer to the head pointer of the environment linked list.
 * @param key The key of the environment variable.
 * @param value The value to set.
 * @return 0 on success, or 1 on failure.
 */
int	env_set(t_env **env, char *key, char *value)
{
	t_env	*node;
	t_env	*new;
	char	*val;

	node = env_find(*env, key);
	if (node)
	{
		if (value)
			val = ft_strdup(value);
		else
			val = NULL;
		if (value && !val)
			return (1);
		free(node->value);
		node->value = val;
		return (0);
	}
	new = env_new(key, value);
	if (!new)
		return (1);
	new->next = *env;
	*env = new;
	return (0);
}

/**
 * @brief Removes an environment variable by key.
 *
 * @param env A pointer to the head pointer of the environment linked list.
 * @param key The key of the environment variable to remove.
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
