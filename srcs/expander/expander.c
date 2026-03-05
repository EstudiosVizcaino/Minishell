/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gisidro- <gisidro-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 15:56:53 by gisidro-          #+#    #+#             */
/*   Updated: 2026/02/12 14:15:03 by gisidro-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Expands variables in a command's arguments and redirections.
 *
 * @param cmd The command whose args and redirections to expand.
 * @param shell The shell context containing environment variables.
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
 * @brief Recursively expands variables in all commands of an AST.
 *
 * @param ast The abstract syntax tree node to process.
 * @param shell The shell context containing environment variables.
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
