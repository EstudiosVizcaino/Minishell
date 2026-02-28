#include "minishell.h"

int	is_builtin(char *name)
{
	if (!name)
		return (0);
	if (ft_strcmp(name, "echo") == 0)
		return (1);
	if (ft_strcmp(name, "cd") == 0)
		return (1);
	if (ft_strcmp(name, "pwd") == 0)
		return (1);
	if (ft_strcmp(name, "export") == 0)
		return (1);
	if (ft_strcmp(name, "unset") == 0)
		return (1);
	if (ft_strcmp(name, "env") == 0)
		return (1);
	if (ft_strcmp(name, "exit") == 0)
		return (1);
	return (0);
}

int	exec_builtin(t_cmd *cmd, t_shell *shell)
{
	char	*name;

	name = cmd->args[0];
	if (ft_strcmp(name, "echo") == 0)
		return (builtin_echo(cmd));
	if (ft_strcmp(name, "cd") == 0)
		return (builtin_cd(cmd, shell));
	if (ft_strcmp(name, "pwd") == 0)
		return (builtin_pwd());
	if (ft_strcmp(name, "export") == 0)
		return (builtin_export(cmd, shell));
	if (ft_strcmp(name, "unset") == 0)
		return (builtin_unset(cmd, shell));
	if (ft_strcmp(name, "env") == 0)
		return (builtin_env(shell));
	if (ft_strcmp(name, "exit") == 0)
		return (builtin_exit(cmd, shell));
	return (1);
}
