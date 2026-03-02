/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_bonus.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adherrer <adherrer@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 18:42:45 by adherrer          #+#    #+#             */
/*   Updated: 2026/02/27 17:09:12 by adherrer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Executes AND/OR logical operator AST nodes.
 *
 * @param ast The AST node containing the AND/OR operator.
 * @param shell The shell state structure.
 * @return The exit status of the executed branch.
 */
int	exec_and_or(t_ast *ast, t_shell *shell)
{
	int	left_status;

	if (!ast || !ast->left || !ast->right)
		return (1);
	left_status = execute(ast->left, shell);
	shell->last_exit = left_status;
	if (ast->type == NODE_AND && left_status != 0)
		return (left_status);
	if (ast->type == NODE_OR && left_status == 0)
		return (left_status);
	return (execute(ast->right, shell));
}

/**
 * @brief Dispatches AST node execution based on node type (bonus version with AND/OR support).
 *
 * @param ast The AST node to execute.
 * @param shell The shell state structure.
 * @return The exit status of the executed command.
 */
int	execute(t_ast *ast, t_shell *shell)
{
	if (!ast)
		return (0);
	if (ast->type == NODE_PIPE)
		return (exec_pipe(ast, shell));
	if (ast->type == NODE_AND || ast->type == NODE_OR)
		return (exec_and_or(ast, shell));
	if (ast->type == NODE_CMD)
		return (exec_cmd(ast, shell));
	return (0);
}
