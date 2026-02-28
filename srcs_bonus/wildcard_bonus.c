#include "minishell.h"

void	unmask_wildcards(char *s)
{
	while (s && *s)
	{
		if (*s == '\x01')
			*s = '*';
		s++;
	}
}

int	match_wildcard(char *pattern, char *str)
{
	if (*pattern == '\0')
		return (*str == '\0');
	if (*pattern == '*')
	{
		while (*str)
		{
			if (match_wildcard(pattern + 1, str))
				return (1);
			str++;
		}
		return (match_wildcard(pattern + 1, str));
	}
	if (*str == '\0' || (*pattern != *str))
		return (0);
	return (match_wildcard(pattern + 1, str + 1));
}

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

char	**expand_wildcard(char *pattern)
{
	DIR				*dir;
	struct dirent	*entry;
	char			**matches;
	int				count;
	int				i;

	dir = opendir(".");
	if (!dir)
		return (NULL);
	count = 0;
	while ((entry = readdir(dir)) != NULL)
		if (entry->d_name[0] != '.' && match_wildcard(pattern, entry->d_name))
			count++;
	rewinddir(dir);
	if (count == 0)
	{
		closedir(dir);
		return (NULL);
	}
	matches = malloc((count + 1) * sizeof(char *));
	if (!matches)
	{
		closedir(dir);
		return (NULL);
	}
	i = 0;
	while ((entry = readdir(dir)) != NULL && i < count)
		if (entry->d_name[0] != '.' && match_wildcard(pattern, entry->d_name))
			matches[i++] = ft_strdup(entry->d_name);
	matches[i] = NULL;
	closedir(dir);
	sort_matches(matches, i);
	return (matches);
}

static char	**merge_args(char **old, int idx, char **exp, int old_len)
{
	char	**new_args;
	int		exp_len;
	int		i;
	int		j;

	exp_len = array_len(exp);
	new_args = malloc((old_len - 1 + exp_len + 1) * sizeof(char *));
	if (!new_args)
		return (NULL);
	i = 0;
	j = 0;
	while (j < idx)
		new_args[i++] = old[j++];
	j = 0;
	while (exp[j])
		new_args[i++] = ft_strdup(exp[j++]);
	j = idx + 1;
	while (j < old_len)
		new_args[i++] = old[j++];
	new_args[i] = NULL;
	return (new_args);
}

void	expand_wildcards_cmd(t_cmd *cmd)
{
	int		i;
	int		len;
	int		exp_len;
	char	**exp;
	char	**new_args;

	if (!cmd || !cmd->args)
		return ;
	i = 0;
	while (cmd->args[i])
	{
		if (!ft_strchr(cmd->args[i], '*'))
		{
			i++;
			continue ;
		}
		exp = expand_wildcard(cmd->args[i]);
		if (!exp)
		{
			i++;
			continue ;
		}
		exp_len = array_len(exp);
		len = array_len(cmd->args);
		new_args = merge_args(cmd->args, i, exp, len);
		free_array(exp);
		if (!new_args)
			return ;
		free(cmd->args[i]);
		free(cmd->args);
		cmd->args = new_args;
		i += exp_len;
	}
	if (cmd->args)
	{
		i = 0;
		while (cmd->args[i])
		{
			unmask_wildcards(cmd->args[i]);
			i++;
		}
	}
}

void	expand_wildcard_redir(t_redir *redir)
{
	char	**exp;
	int		count;

	while (redir)
	{
		if (redir->type != TOKEN_HEREDOC && redir->file
			&& ft_strchr(redir->file, '*'))
		{
			exp = expand_wildcard(redir->file);
			if (exp)
			{
				count = array_len(exp);
				if (count == 1)
				{
					free(redir->file);
					redir->file = ft_strdup(exp[0]);
				}
				else
				{
					ft_putstr_fd("minishell: ", STDERR_FILENO);
					ft_putstr_fd(redir->file, STDERR_FILENO);
					ft_putstr_fd(": ambiguous redirect\n", STDERR_FILENO);
					free(redir->file);
					redir->file = NULL;
				}
				free_array(exp);
			}
		}
		if (redir->file)
			unmask_wildcards(redir->file);
		redir = redir->next;
	}
}
