/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 15:56:53 by cvizcain          #+#    #+#             */
/*   Updated: 2026/02/12 14:15:03 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Expands vars in a command's args and redirs.
 *
 * Calls expand_args for the arg list and
 * expand_redirs for the redir filenames.
 *
 * @param cmd   The command to expand.
 * @param shell The shell context (has env).
 */
static void	expand_cmd(t_cmd *cmd, t_shell *shell)
{
	if (!cmd)
		return ;
	if (cmd->args)
		expand_args(cmd, shell);
	expand_redirs(cmd->redirs, shell);
}

/**
 * @brief Recursively expands the whole AST.
 *
 * For CMD nodes we expand the command. For pipes
 * we recurse into left and right.
 *
 * @param ast   The AST node to process.
 * @param shell The shell context (has env).
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
