/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipe.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adherrer <adherrer@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 14:25:42 by adherrer          #+#    #+#             */
/*   Updated: 2026/02/19 20:23:28 by adherrer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Waits for two child processes and returns the exit status of the second.
 *
 * @param pid1 The process ID of the first child.
 * @param pid2 The process ID of the second child.
 * @return The exit status of the second child process.
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
 * @brief Executes the left side of a pipe in a child process.
 *
 * @param ast The AST node representing the pipe.
 * @param pipefd The pipe file descriptors.
 * @param shell The shell state.
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
 * @brief Executes the right side of a pipe in a child process.
 *
 * @param ast The AST node representing the pipe.
 * @param pipefd The pipe file descriptors.
 * @param shell The shell state.
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
 * @brief Forks the right child of a pipe and waits for both children.
 *
 * @param ast The AST node representing the pipe.
 * @param pipefd The pipe file descriptors.
 * @param shell The shell state.
 * @param p1 The process ID of the left child.
 * @return The exit status of the right child process.
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
 * @brief Executes a pipe AST node by forking two child processes.
 *
 * @param ast The AST node representing the pipe.
 * @param shell The shell state.
 * @return The exit status of the right side of the pipe.
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
