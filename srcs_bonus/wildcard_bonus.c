#include "minishell.h"

/**
 * @brief Sorts wildcard matches alphabetically using bubble sort.
 *
 * @param matches The array of matched filenames to sort.
 * @param count The number of elements in the array.
 */
static void	sort_matches(char **matches, int count)
{
	char	*tmp;
	int		i;
	int		j;

	i = 0;
	while (i < count - 1)
	{
		j = 0;
		while (j < count - 1 - i)
		{
			if (ft_strcmp(matches[j], matches[j + 1]) > 0)
			{
				tmp = matches[j];
				matches[j] = matches[j + 1];
				matches[j + 1] = tmp;
			}
			j++;
		}
		i++;
	}
}

/**
 * @brief Counts directory entries matching a wildcard pattern.
 *
 * @param dir The directory stream to read from.
 * @param pattern The wildcard pattern to match against.
 * @return The number of matching entries.
 */
static int	count_matches(DIR *dir, char *pattern)
{
	struct dirent	*entry;
	int				count;

	count = 0;
	entry = readdir(dir);
	while (entry != NULL)
	{
		if (entry->d_name[0] != '.')
			if (match_wildcard(pattern, entry->d_name))
				count++;
		entry = readdir(dir);
	}
	return (count);
}

/**
 * @brief Creates an array of directory entries matching a wildcard pattern.
 *
 * @param dir The directory stream to read from.
 * @param pattern The wildcard pattern to match against.
 * @param count The number of expected matches.
 * @return A sorted NULL-terminated array of matching filenames.
 */
static char	**fill_matches(DIR *dir, char *pattern, int count)
{
	struct dirent	*entry;
	char			**matches;
	int				i;

	matches = malloc((count + 1) * sizeof(char *));
	if (!matches)
		return (NULL);
	i = 0;
	entry = readdir(dir);
	while (entry != NULL && i < count)
	{
		if (entry->d_name[0] != '.')
			if (match_wildcard(pattern, entry->d_name))
				matches[i++] = ft_strdup(entry->d_name);
		entry = readdir(dir);
	}
	matches[i] = NULL;
	sort_matches(matches, i);
	return (matches);
}

/**
 * @brief Expands a wildcard pattern into matching filenames.
 *
 * @param pattern The wildcard pattern to expand.
 * @return A NULL-terminated array of matching filenames, or NULL if none match.
 */
char	**expand_wildcard(char *pattern)
{
	DIR		*dir;
	char	**matches;
	int		count;

	dir = opendir(".");
	if (!dir)
		return (NULL);
	count = count_matches(dir, pattern);
	rewinddir(dir);
	if (count == 0)
	{
		closedir(dir);
		return (NULL);
	}
	matches = fill_matches(dir, pattern, count);
	closedir(dir);
	return (matches);
}
