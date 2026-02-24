#include "minishell.h"

static void	expand_redirs(t_redir *redir, t_shell *shell)
{
	char	*expanded;

	while (redir)
	{
		if (redir->type != TOKEN_HEREDOC && redir->file)
		{
			expanded = expand_str(redir->file, shell);
			free(redir->file);
			redir->file = expanded;
		}
		redir = redir->next;
	}
}

static void	expand_cmd(t_cmd *cmd, t_shell *shell)
{
	char	*expanded;
	int		i;

	if (!cmd)
		return ;
	if (cmd->args)
	{
		i = 0;
		while (cmd->args[i])
		{
			expanded = expand_str(cmd->args[i], shell);
			free(cmd->args[i]);
			cmd->args[i] = expanded;
			i++;
		}
	}
	expand_redirs(cmd->redirs, shell);
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
