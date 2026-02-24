#include "minishell.h"

void	free_shell(t_shell *shell)
{
	if (!shell)
		return ;
	if (shell->env)
	{
		env_free(shell->env);
		shell->env = NULL;
	}
	if (shell->input)
	{
		free(shell->input);
		shell->input = NULL;
	}
}
