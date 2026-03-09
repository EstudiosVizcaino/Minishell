/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_run.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adherrer <adherrer@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 15:01:37 by adherrer          #+#    #+#             */
/*   Updated: 2026/02/20 18:14:37 by adherrer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Main execute dispatcher.
 *
 * Checks the node type (pipe or cmd) and calls
 * the right handler.
 *
 * @param ast   The AST node to execute.
 * @param shell The shell context.
 * @return Exit status of the node.
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
