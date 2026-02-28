#include "minishell.h"

static int	count_args(t_token *tok)
{
	int	count;

	count = 0;
	while (tok && tok->type == TOKEN_WORD)
	{
		count++;
		tok = tok->next;
	}
	return (count);
}

static char	**build_args(t_token **tokens)
{
	char	**args;
	int		count;
	int		i;

	count = count_args(*tokens);
	if (count == 0)
		return (NULL);
	args = malloc((count + 1) * sizeof(char *));
	if (!args)
		return (NULL);
	i = 0;
	while (*tokens && (*tokens)->type == TOKEN_WORD)
	{
		args[i] = ft_strdup((*tokens)->value);
		if (!args[i])
		{
			while (i-- > 0)
				free(args[i]);
			free(args);
			return (NULL);
		}
		i++;
		*tokens = (*tokens)->next;
	}
	args[i] = NULL;
	return (args);
}

t_ast	*parse_command(t_token **tokens)
{
	t_ast	*node;
	t_cmd	*cmd;
	t_redir	*pre_redir;
	t_redir	*post_redir;
	t_redir	*tail;

	node = new_ast_node(NODE_CMD);
	if (!node)
		return (NULL);
	cmd = new_cmd();
	if (!cmd)
	{
		free(node);
		return (NULL);
	}
	pre_redir = parse_redir(tokens);
	cmd->args = build_args(tokens);
	post_redir = parse_redir(tokens);
	cmd->redirs = pre_redir;
	if (pre_redir)
	{
		tail = pre_redir;
		while (tail->next)
			tail = tail->next;
		tail->next = post_redir;
	}
	else
		cmd->redirs = post_redir;
	node->cmd = cmd;
	return (node);
}
