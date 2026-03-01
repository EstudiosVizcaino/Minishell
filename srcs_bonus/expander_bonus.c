/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_bonus.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/18 14:29:15 by cvizcain          #+#    #+#             */
/*   Updated: 2026/02/26 11:28:51 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Expands variables and wildcards in a command's arguments and redirections.
 *
 * @param cmd The command structure to expand.
 * @param shell The shell state structure.
 */
static void	expand_cmd(t_cmd *cmd, t_shell *shell)
{
	if (!cmd)
		return ;
	if (cmd->args)
		expand_args(cmd, shell);
	expand_redirs(cmd->redirs, shell);
	expand_wildcards_cmd(cmd);
	expand_wildcard_redir(cmd->redirs);
}

/**
 * @brief Recursively expands variables and wildcards in all commands of an AST.
 *
 * @param ast The AST node to expand.
 * @param shell The shell state structure.
 */
void	expand_ast(t_ast *ast, t_shell *shell)
{
	if (!ast)
		return ;
	if (ast->type == NODE_CMD)
		expand_cmd(ast->cmd, shell);
	else
	{
		expand_ast(ast->left, shell);
		expand_ast(ast->right, shell);
	}
}
