/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_run.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 15:01:37 by cvizcain          #+#    #+#             */
/*   Updated: 2026/02/20 18:14:37 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Dispatches AST node execution based on node type.
 *
 * @param ast The AST node to execute.
 * @param shell The shell state.
 * @return The exit status of the executed node.
 */
int	execute(t_ast *ast, t_shell *shell)
{
	if (!ast)
		return (0);
	if (ast->type == NODE_PIPE)
		return (exec_pipe(ast, shell));
	if (ast->type == NODE_CMD)
		return (exec_cmd(ast, shell));
	return (0);
}
