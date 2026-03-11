/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_cmd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/21 18:38:12 by cvizcain          #+#    #+#             */
/*   Updated: 2026/03/01 22:14:56 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Checks if a token is a redirection type.
 *
 * Returns non-zero for <, >, << and >>.
 *
 * @param type The token type to check.
 * @return Non-zero if redir, 0 otherwise.
 */
static int	is_redir_token(t_token_type type)
{
	return (type == TOKEN_REDIR_IN || type == TOKEN_REDIR_OUT
		|| type == TOKEN_HEREDOC || type == TOKEN_APPEND);
}

/**
 * @brief Counts how many word tokens there are.
 *
 * Skips over redir tokens (and their filename) so
 * we only count actual command arguments.
 *
 * @param tok Token to start counting from.
 * @return Number of word tokens found.
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
 * @brief Appends a redir node to the command.
 *
 * Calls make_redir and links it at the end of
 * the redir list.
 *
 * @param cmd  The command we are building.
 * @param tail Pointer to the tail of the redir list.
 * @param tokens Current token position.
 */
static void	add_redir(t_cmd *cmd, t_redir **tail, t_token **tokens)
{
	t_redir	*r;

	r = make_redir(tokens);
	if (!r)
		return ;
	if (!cmd->redirs)
		cmd->redirs = r;
	else
		(*tail)->next = r;
	*tail = r;
}

/**
 * @brief Fills a cmd struct with args and redirs.
 *
 * Walks the token list, separating words (args)
 * from redirection operators.
 *
 * @param cmd    The command struct to fill.
 * @param tokens Current token position.
 */
static void	fill_cmd(t_cmd *cmd, t_token **tokens)
{
	t_redir	*redir_tail;
	char	*dup;
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
			{
				dup = ft_strdup((*tokens)->value);
				if (!dup)
					fatal_error("malloc");
				cmd->args[i++] = dup;
			}
			*tokens = (*tokens)->next;
		}
	}
	if (cmd->args)
		cmd->args[i] = NULL;
}

/**
 * @brief Parses one simple command from the tokens.
 *
 * Creates the AST node and cmd struct, counts
 * the args, mallocs the args array, and calls
 * fill_cmd to do the actual parsing.
 *
 * @param tokens Current token position.
 * @return AST node for the command, or NULL.
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
