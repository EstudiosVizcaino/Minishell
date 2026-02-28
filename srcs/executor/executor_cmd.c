#include "minishell.h"

void	open_heredocs(t_redir *redir, t_shell *shell)
{
	(void)shell;
	while (redir)
	{
		if (redir->type == TOKEN_HEREDOC)
			open_heredoc(redir);
		redir = redir->next;
	}
}

static int	wait_for_child(pid_t pid)
{
	int	status;
	int	exit_code;

	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		exit_code = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		exit_code = 128 + WTERMSIG(status);
	else
		exit_code = 1;
	if (g_signal == SIGINT)
		write(STDOUT_FILENO, "\n", 1);
	g_signal = 0;
	return (exit_code);
}

static int	exec_redir_only(t_cmd *cmd, t_shell *shell)
{
	open_heredocs(cmd->redirs, shell);
	return (apply_redirs(cmd->redirs));
}

int	exec_cmd(t_ast *ast, t_shell *shell)
{
	t_cmd	*cmd;
	pid_t	pid;
	int		ret;

	cmd = ast->cmd;
	if (!cmd)
		return (0);
	if ((!cmd->args || !cmd->args[0]) && cmd->redirs)
		return (exec_redir_only(cmd, shell));
	if (!cmd->args || !cmd->args[0])
		return (0);
	open_heredocs(cmd->redirs, shell);
	if (is_builtin(cmd->args[0]))
	{
		exec_builtin_redir(cmd, shell, &ret);
		return (ret);
	}
	pid = fork();
	if (pid < 0)
		return (1);
	if (pid == 0)
		exec_child(cmd, shell);
	return (wait_for_child(pid));
}
