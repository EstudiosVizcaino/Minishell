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

int	execute(t_ast *ast, t_shell *shell)
{
	if (!ast)
		return (0);
	if (ast->type == NODE_PIPE)
		return (exec_pipe(ast, shell));
	if (ast->type == NODE_AND || ast->type == NODE_OR)
		return (exec_and_or(ast, shell));
	if (ast->type == NODE_CMD)
		return (exec_cmd(ast, shell));
	return (0);
}
