/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redir.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 19:20:31 by cvizcain          #+#    #+#             */
/*   Updated: 2026/02/08 10:07:59 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Frees a linked list of redir nodes.
 *
 * Also closes any open heredoc fds.
 *
 * @param redir Head of the redir list.
 */
void	free_redir(t_redir *redir)
{
	t_redir	*next;

	while (redir)
	{
		next = redir->next;
		if (redir->heredoc_fd >= 0)
			close(redir->heredoc_fd);
		free(redir->file);
		free(redir);
		redir = next;
	}
}

/**
 * @brief Frees a command struct and everything inside.
 *
 * Frees args array, redir list, and the cmd itself.
 *
 * @param cmd The command struct to free.
 */
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

/**
 * @brief Recursively frees the whole AST.
 *
 * Goes left, right, then frees the node's cmd.
 *
 * @param node Root of the subtree to free.
 */
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

/**
 * @brief Builds a redir node from the current token.
 *
 * Reads the redir type, advances past the token,
 * then grabs the filename if the next token is
 * a word.
 *
 * @param tokens Current token position.
 * @return The new redir node, or NULL.
 */
t_redir	*make_redir(t_token **tokens)
{
	t_redir	*redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->type = (*tokens)->type;
	redir->file = NULL;
	redir->heredoc_fd = -1;
	redir->quoted = 0;
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

/**
 * @brief Parses consecutive redir tokens into a list.
 *
 * Keeps reading redir tokens until it hits a
 * word, pipe, EOF or another stop condition.
 *
 * @param tokens Current token position.
 * @return Head of the redir list, or NULL.
 */
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
