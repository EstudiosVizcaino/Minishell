#include "minishell.h"

char	*find_executable(char *name, t_env *env)
{
	char	*path_env;
	char	**paths;
	char	*candidate;
	char	*tmp;
	int		i;

	if (ft_strchr(name, '/'))
		return (ft_strdup(name));
	path_env = env_get(env, "PATH");
	if (!path_env)
		return (NULL);
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		tmp = ft_strjoin(paths[i], "/");
		candidate = ft_strjoin(tmp, name);
		free(tmp);
		if (access(candidate, X_OK) == 0)
		{
			free_array(paths);
			return (candidate);
		}
		free(candidate);
		i++;
	}
	free_array(paths);
	return (NULL);
}

static void	exec_child(t_cmd *cmd, t_shell *shell)
{
	char	*path;
	char	**envp;

	setup_signals_child();
	if (apply_redirs(cmd->redirs))
		exit(1);
	path = find_executable(cmd->args[0], shell->env);
	if (!path)
	{
		ft_putstr_fd(cmd->args[0], STDERR_FILENO);
		ft_putstr_fd(": command not found\n", STDERR_FILENO);
		exit(127);
	}
	envp = env_to_array(shell->env);
	execve(path, cmd->args, envp);
	perror(path);
	free(path);
	free_array(envp);
	exit(126);
}

static void	exec_builtin_redir(t_cmd *cmd, t_shell *shell, int *ret)
{
	int	saved_in;
	int	saved_out;

	saved_in = dup(STDIN_FILENO);
	saved_out = dup(STDOUT_FILENO);
	if (apply_redirs(cmd->redirs))
	{
		dup2(saved_in, STDIN_FILENO);
		dup2(saved_out, STDOUT_FILENO);
		close(saved_in);
		close(saved_out);
		*ret = 1;
		return ;
	}
	*ret = exec_builtin(cmd, shell);
	dup2(saved_in, STDIN_FILENO);
	dup2(saved_out, STDOUT_FILENO);
	close(saved_in);
	close(saved_out);
}

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

int	exec_cmd(t_ast *ast, t_shell *shell)
{
	t_cmd	*cmd;
	pid_t	pid;
	int		status;
	int		exit_code;
	int		ret;

	cmd = ast->cmd;
	if (!cmd || !cmd->args || !cmd->args[0])
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
