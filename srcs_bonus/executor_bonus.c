#include "minishell.h"

int	exec_and_or(t_ast *ast, t_shell *shell)
{
	int	left_status;

	if (!ast || !ast->left || !ast->right)
		return (1);
	left_status = execute(ast->left, shell);
	shell->last_exit = left_status;
	if (ast->type == NODE_AND && left_status != 0)
		return (left_status);
	if (ast->type == NODE_OR && left_status == 0)
		return (left_status);
	return (execute(ast->right, shell));
}
