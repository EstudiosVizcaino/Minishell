#include "minishell.h"

static int	cd_home(t_shell *shell)
{
	char	*home;

	home = env_get(shell->env, "HOME");
	if (!home)
	{
		ft_putstr_fd("cd: HOME not set\n", STDERR_FILENO);
		return (1);
	}
	if (chdir(home) != 0)
	{
		perror("cd");
		return (1);
	}
	return (0);
}

static void	update_pwd(t_shell *shell)
{
	char	buf[4096];
	char	*old;

	old = env_get(shell->env, "PWD");
	if (old)
		env_set(&shell->env, "OLDPWD", old);
	if (getcwd(buf, sizeof(buf)))
		env_set(&shell->env, "PWD", buf);
}

int	builtin_cd(t_cmd *cmd, t_shell *shell)
{
	char	*path;

	if (!cmd->args[1])
	{
		if (cd_home(shell) != 0)
			return (1);
		update_pwd(shell);
		return (0);
	}
	path = cmd->args[1];
	if (chdir(path) != 0)
	{
		ft_putstr_fd("cd: ", STDERR_FILENO);
		ft_putstr_fd(path, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putstr_fd(strerror(errno), STDERR_FILENO);
		ft_putchar_fd('\n', STDERR_FILENO);
		return (1);
	}
	update_pwd(shell);
	return (0);
}
