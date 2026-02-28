#include "minishell.h"

void	free_redir(t_redir *redir)
{
	t_redir	*next;

	while (redir)
	{
		next = redir->next;
		free(redir->file);
		free(redir);
		redir = next;
	}
}

void	free_cmd(t_cmd *cmd)
{
	int	i;

	if (!cmd)
		return ;
	if (cmd->args)
	{
		i = 0;
		while (cmd->args[i])
		{
			free(cmd->args[i]);
			i++;
		}
		free(cmd->args);
	}
	free_redir(cmd->redirs);
	free(cmd);
}

void	free_ast(t_ast *node)
{
	if (!node)
		return ;
	free_ast(node->left);
	free_ast(node->right);
	if (node->cmd)
		free_cmd(node->cmd);
	free(node);
}

t_redir	*make_redir(t_token **tokens)
{
	t_redir	*redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->type = (*tokens)->type;
	redir->file = NULL;
	redir->heredoc_fd = -1;
	redir->next = NULL;
	*tokens = (*tokens)->next;
	if (*tokens && (*tokens)->type == TOKEN_WORD)
	{
		redir->file = ft_strdup((*tokens)->value);
		if (!redir->file)
		{
			free(redir);
			return (NULL);
		}
		*tokens = (*tokens)->next;
	}
	return (redir);
}

t_redir	*parse_redir(t_token **tokens)
{
	t_redir	*redir;
	t_redir	*head;
	t_redir	*tail;

	head = NULL;
	tail = NULL;
	while (*tokens && (*tokens)->type != TOKEN_PIPE
		&& (*tokens)->type != TOKEN_EOF
		&& (*tokens)->type != TOKEN_AND
		&& (*tokens)->type != TOKEN_OR
		&& (*tokens)->type != TOKEN_RPAREN)
	{
		if ((*tokens)->type == TOKEN_WORD)
			break ;
		redir = make_redir(tokens);
		if (!redir)
			return (head);
		if (!head)
			head = redir;
		else
			tail->next = redir;
		tail = redir;
	}
	return (head);
}
