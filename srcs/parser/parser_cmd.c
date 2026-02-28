#include "minishell.h"

/**
 * @brief Checks if a token type is a redirection.
 *
 * @param type The token type to check.
 * @return Non-zero if the token type is a redirection, 0 otherwise.
 */
static int	is_redir_token(t_token_type type)
{
	return (type == TOKEN_REDIR_IN || type == TOKEN_REDIR_OUT
		|| type == TOKEN_HEREDOC || type == TOKEN_APPEND);
}

/**
 * @brief Counts the number of word tokens for argument allocation.
 *
 * @param tok The starting token to count from.
 * @return The number of word tokens found.
 */
static int	count_word_tokens(t_token *tok)
{
	int	count;

	count = 0;
	while (tok && (tok->type == TOKEN_WORD || is_redir_token(tok->type)))
	{
		if (is_redir_token(tok->type))
		{
			tok = tok->next;
			if (tok)
				tok = tok->next;
		}
		else
		{
			count++;
			tok = tok->next;
		}
	}
	return (count);
}

/**
 * @brief Adds a redirection to a command's redirection list.
 *
 * @param cmd The command to add the redirection to.
 * @param tail A pointer to the tail of the redirection list.
 * @param tokens A pointer to the current position in the token list.
 */
static void	add_redir(t_cmd *cmd, t_redir **tail, t_token **tokens)
{
	t_redir	*r;

	r = make_redir(tokens);
	if (!cmd->redirs)
		cmd->redirs = r;
	else
		(*tail)->next = r;
	*tail = r;
}

/**
 * @brief Fills a command structure with arguments and redirections from tokens.
 *
 * @param cmd The command structure to fill.
 * @param tokens A pointer to the current position in the token list.
 */
static void	fill_cmd(t_cmd *cmd, t_token **tokens)
{
	t_redir	*redir_tail;
	int		i;

	redir_tail = NULL;
	i = 0;
	while (*tokens && ((*tokens)->type == TOKEN_WORD
			|| is_redir_token((*tokens)->type)))
	{
		if (is_redir_token((*tokens)->type))
			add_redir(cmd, &redir_tail, tokens);
		else
		{
			if (cmd->args)
				cmd->args[i++] = ft_strdup((*tokens)->value);
			*tokens = (*tokens)->next;
		}
	}
	if (cmd->args)
		cmd->args[i] = NULL;
}

/**
 * @brief Parses a single command from the token stream.
 *
 * @param tokens A pointer to the current position in the token list.
 * @return A pointer to the AST node representing the command, or NULL on failure.
 */
t_ast	*parse_command(t_token **tokens)
{
	t_ast	*node;
	t_cmd	*cmd;
	int		count;

	node = new_ast_node(NODE_CMD);
	cmd = new_cmd();
	if (!node || !cmd)
	{
		free(node);
		free_cmd(cmd);
		return (NULL);
	}
	count = count_word_tokens(*tokens);
	if (count > 0)
	{
		cmd->args = malloc((count + 1) * sizeof(char *));
		if (!cmd->args)
		{
			free_ast(node);
			return (NULL);
		}
	}
	fill_cmd(cmd, tokens);
	node->cmd = cmd;
	return (node);
}
