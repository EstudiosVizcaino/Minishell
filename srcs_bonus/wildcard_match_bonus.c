/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_match_bonus.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adherrer <adherrer@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 17:32:58 by adherrer          #+#    #+#             */
/*   Updated: 2026/02/27 18:10:03 by adherrer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Replaces mask bytes back to asterisk characters.
 *
 * @param s The string in which to unmask wildcards.
 */
void	unmask_wildcards(char *s)
{
	while (s && *s)
	{
		if (*s == '\x01')
			*s = '*';
		s++;
	}
}

/**
 * @brief Recursively matches a string against a wildcard pattern.
 *
 * @param pattern The wildcard pattern to match.
 * @param str The string to test against the pattern.
 * @return 1 if the string matches, 0 otherwise.
 */
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
