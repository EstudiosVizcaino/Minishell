#include "minishell.h"

static void	expand_cmd(t_cmd *cmd, t_shell *shell)
{
	if (!cmd)
		return ;
	if (cmd->args)
		expand_args(cmd, shell);
	expand_redirs(cmd->redirs, shell);
	expand_wildcards_cmd(cmd);
	expand_wildcard_redir(cmd->redirs);
}

void	expand_ast(t_ast *ast, t_shell *shell)
{
	if (!ast)
		return ;
	if (ast->type == NODE_CMD)
		expand_cmd(ast->cmd, shell);
	else
	{
		expand_ast(ast->left, shell);
		expand_ast(ast->right, shell);
	}
}
