#include "minishell.h"

/**
 * @brief Dispatches AST node execution based on node type.
 *
 * @param ast The AST node to execute.
 * @param shell The shell state.
 * @return The exit status of the executed node.
 */
int	execute(t_ast *ast, t_shell *shell)
{
	if (!ast)
		return (0);
	if (ast->type == NODE_PIPE)
		return (exec_pipe(ast, shell));
	if (ast->type == NODE_CMD)
		return (exec_cmd(ast, shell));
	return (0);
}
