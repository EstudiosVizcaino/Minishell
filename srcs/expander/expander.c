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

static int	is_only_vars(char *str)
{
	int	i;

	i = 0;
	if (!str || !*str)
		return (0);
	while (str[i])
	{
		if (str[i] == '\'' || str[i] == '"')
			return (0);
		if (str[i] != '$')
			return (0);
		i++;
		if (str[i] == '?')
			i++;
		else if (ft_isalpha(str[i]) || str[i] == '_')
		{
			while (str[i] && (ft_isalnum(str[i]) || str[i] == '_'))
				i++;
		}
		else
			return (0);
	}
	return (1);
}

static void	expand_args(t_cmd *cmd, t_shell *shell)
{
	char	*expanded;
	char	*orig;
	int		i;
	int		j;

	i = 0;
	j = 0;
	while (cmd->args[i])
	{
		orig = cmd->args[i];
		expanded = expand_str(orig, shell);
		if (*expanded == '\0' && is_only_vars(orig))
			free(expanded);
		else
			cmd->args[j++] = expanded;
		free(orig);
		i++;
	}
	cmd->args[j] = NULL;
	if (j == 0)
	{
		free(cmd->args);
		cmd->args = NULL;
	}
}

static void	expand_cmd(t_cmd *cmd, t_shell *shell)
{
	if (!cmd)
		return ;
	if (cmd->args)
		expand_args(cmd, shell);
	expand_redirs(cmd->redirs, shell);
#ifdef BONUS
	expand_wildcards_cmd(cmd);
#endif
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
