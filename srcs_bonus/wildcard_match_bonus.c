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
