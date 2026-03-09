/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipe.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 14:25:42 by adherrer          #+#    #+#             */
/*   Updated: 2026/03/01 22:15:32 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Waits for both pipe children.
 *
 * Returns the exit status of the second child
 * (the right side of the pipe).
 *
 * @param pid1 Left child PID.
 * @param pid2 Right child PID.
 * @return Exit status of pid2.
 */
static int	wait_children(pid_t pid1, pid_t pid2)
{
	int	status;
	int	exit_code;

	exit_code = 0;
	waitpid(pid1, &status, 0);
	waitpid(pid2, &status, 0);
	if (WIFEXITED(status))
		exit_code = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		exit_code = 128 + WTERMSIG(status);
	return (exit_code);
}

/**
 * @brief Runs the left side of a pipe (child).
 *
 * Redirects stdout to the pipe write end, then
 * executes and exits.
 *
 * @param ast    The pipe AST node.
 * @param pipefd Pipe file descriptors.
 * @param shell  The shell context.
 */
static void	child_left(t_ast *ast, int *pipefd, t_shell *shell)
{
	setup_signals_child();
	dup2(pipefd[1], STDOUT_FILENO);
	close(pipefd[0]);
	close(pipefd[1]);
	shell->last_exit = execute(ast->left, shell);
	free_ast(ast);
	env_free(shell->env);
	exit(shell->last_exit);
}

/**
 * @brief Runs the right side of a pipe (child).
 *
 * Redirects stdin from the pipe read end, then
 * executes and exits.
 *
 * @param ast    The pipe AST node.
 * @param pipefd Pipe file descriptors.
 * @param shell  The shell context.
 */
static void	child_right(t_ast *ast, int *pipefd, t_shell *shell)
{
	setup_signals_child();
	dup2(pipefd[0], STDIN_FILENO);
	close(pipefd[0]);
	close(pipefd[1]);
	shell->last_exit = execute(ast->right, shell);
	free_ast(ast);
	env_free(shell->env);
	exit(shell->last_exit);
}

/**
 * @brief Forks the right child and waits for both.
 *
 * Called after the left child is already forked.
 * Closes pipe fds in the parent after forking.
 *
 * @param ast    The pipe AST node.
 * @param pipefd Pipe file descriptors.
 * @param shell  The shell context.
 * @param p1     PID of the left child.
 * @return Exit status of the right child.
 */
static int	fork_right(t_ast *ast, int *pipefd, t_shell *shell, pid_t p1)
{
	pid_t	pid2;

	pid2 = fork();
	if (pid2 < 0)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		waitpid(p1, NULL, 0);
		return (1);
	}
	if (pid2 == 0)
		child_right(ast, pipefd, shell);
	close(pipefd[0]);
	close(pipefd[1]);
	return (wait_children(p1, pid2));
}

/**
 * @brief Executes a pipe: forks left and right.
 *
 * Creates a pipe, forks left child, then calls
 * fork_right for the right child.
 *
 * @param ast   The pipe AST node.
 * @param shell The shell context.
 * @return Exit status of the right side.
 */
int	exec_pipe(t_ast *ast, t_shell *shell)
{
	int		pipefd[2];
	pid_t	pid1;

	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		return (1);
	}
	pid1 = fork();
	if (pid1 < 0)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		return (1);
	}
	if (pid1 == 0)
		child_left(ast, pipefd, shell);
	return (fork_right(ast, pipefd, shell, pid1));
}
