/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_bonus.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gisidro- <gisidro-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 18:42:45 by gisidro-          #+#    #+#             */
/*   Updated: 2026/03/05 19:00:00 by gisidro-         ###   ########.fr       */
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
 * @brief Runs the subshell inner AST in a child process and exits.
 *
 * @param ast The subshell AST node.
 * @param shell The shell state structure.
 */
static void	child_subshell(t_ast *ast, t_shell *shell)
{
	int	code;

	setup_signals_child();
	code = execute(ast->left, shell);
	free_ast(ast);
	env_free(shell->env);
	exit(code);
}

/**
 * @brief Forks a child process to execute a subshell AST node.
 *
 * @param ast The subshell AST node (left child is the inner expression).
 * @param shell The shell state structure.
 * @return The exit status of the child process.
 */
static int	exec_subshell(t_ast *ast, t_shell *shell)
{
	pid_t	pid;
	int		status;
	int		exit_code;

	pid = fork();
	if (pid < 0)
		return (1);
	if (pid == 0)
		child_subshell(ast, shell);
	setup_signals_wait();
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		exit_code = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		exit_code = 128 + WTERMSIG(status);
	else
		exit_code = 1;
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
		write(STDOUT_FILENO, "\n", 1);
	g_signal = 0;
	return (exit_code);
}

/**
 * @brief Dispatches AST node execution based on node type (bonus version
 *        with AND/OR and subshell support).
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
	if (ast->type == NODE_SUBSHELL)
		return (exec_subshell(ast, shell));
	if (ast->type == NODE_CMD)
		return (exec_cmd(ast, shell));
	return (0);
}
