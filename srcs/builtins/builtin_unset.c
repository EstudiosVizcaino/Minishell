#include "minishell.h"

/**
 * @brief Implements the unset builtin command.
 *
 * @param cmd Pointer to the command structure.
 * @param shell Pointer to the shell structure.
 * @return 0 on success.
 */
int	builtin_unset(t_cmd *cmd, t_shell *shell)
{
	int	i;

	i = 1;
	while (cmd->args[i])
	{
		env_unset(&shell->env, cmd->args[i]);
		i++;
	}
	return (0);
}
