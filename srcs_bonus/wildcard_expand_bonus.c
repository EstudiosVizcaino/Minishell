/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_expand_bonus.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 10:47:20 by cvizcain          #+#    #+#             */
/*   Updated: 2026/02/28 10:03:37 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Merges expanded wildcard results into the command arguments array.
 *
 * @param old The original arguments array.
 * @param idx The index of the argument being replaced.
 * @param exp The expanded wildcard results.
 * @param old_len The length of the original arguments array.
 * @return A new merged arguments array, or NULL on failure.
 */
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

/**
 * @brief Replaces a single wildcard argument with its expanded matches.
 *
 * @param cmd The command whose argument is being expanded.
 * @param i The index of the wildcard argument to replace.
 * @return The next index to process, or -1 on failure.
 */
static int	replace_wildcard(t_cmd *cmd, int i)
{
	char	**exp;
	char	**new_args;
	int		exp_len;
	int		len;

	exp = expand_wildcard(cmd->args[i]);
	if (!exp)
		return (i + 1);
	exp_len = array_len(exp);
	len = array_len(cmd->args);
	new_args = merge_args(cmd->args, i, exp, len);
	free_array(exp);
	if (!new_args)
		return (-1);
	free(cmd->args[i]);
	free(cmd->args);
	cmd->args = new_args;
	return (i + exp_len);
}

/**
 * @brief Unmasks all wildcard mask bytes in command arguments.
 *
 * @param cmd The command whose arguments are unmasked.
 */
static void	unmask_all_args(t_cmd *cmd)
{
	int	i;

	if (!cmd->args)
		return ;
	i = 0;
	while (cmd->args[i])
	{
		unmask_wildcards(cmd->args[i]);
		i++;
	}
}

/**
 * @brief Expands all wildcard arguments in a command.
 *
 * @param cmd The command whose arguments are expanded.
 */
void	expand_wildcards_cmd(t_cmd *cmd)
{
	int	i;

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
		i = replace_wildcard(cmd, i);
		if (i < 0)
			return ;
	}
	unmask_all_args(cmd);
}
