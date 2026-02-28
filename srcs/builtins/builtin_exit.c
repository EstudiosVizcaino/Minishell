#include "minishell.h"

static int	all_digits(char *s)
{
	int	i;

	i = 0;
	if (s[i] == '-' || s[i] == '+')
		i++;
	if (!s[i])
		return (0);
	while (s[i])
	{
		if (!ft_isdigit(s[i]))
			return (0);
		i++;
	}
	return (1);
}

static void	do_exit(t_shell *shell, int code)
{
	env_free(shell->env);
	shell->env = NULL;
	rl_clear_history();
	exit(code);
}

int	builtin_exit(t_cmd *cmd, t_shell *shell)
{
	int	code;

	ft_putstr_fd("exit\n", STDOUT_FILENO);
	if (!cmd->args[1])
		do_exit(shell, shell->last_exit);
	if (cmd->args[2])
	{
		ft_putstr_fd("exit: too many arguments\n", STDERR_FILENO);
		return (1);
	}
	if (!all_digits(cmd->args[1]))
	{
		ft_putstr_fd("exit: numeric argument required\n", STDERR_FILENO);
		do_exit(shell, 2);
	}
	code = ft_atoi(cmd->args[1]) & 0xFF;
	do_exit(shell, code);
	return (0);
}
