#include "minishell.h"

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

int	exec_pipe(t_ast *ast, t_shell *shell)
{
	int		pipefd[2];
	pid_t	pid1;
	pid_t	pid2;

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
	pid2 = fork();
	if (pid2 < 0)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		waitpid(pid1, NULL, 0);
		return (1);
	}
	if (pid2 == 0)
		child_right(ast, pipefd, shell);
	close(pipefd[0]);
	close(pipefd[1]);
	return (wait_children(pid1, pid2));
}
