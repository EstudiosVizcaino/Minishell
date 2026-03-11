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
 * @brief Frees a linked list of t_redir nodes.
 *
 * Also closes any file descriptor that was opened for a heredoc,
 * because heredoc content is kept in a pipe fd until the executor
 * reads it; if we never execute (e.g. parse error), we must close it.
 *
 * @param redir Head of the redir list to free.
 */
void	free_redir(t_redir *redir)
{
	t_redir	*next;

	while (redir)
	{
		next = redir->next;

		/*
		** heredoc_fd is initialised to -1 in make_redir().
		** A value >= 0 means the executor stored an open pipe fd here;
		** we close it to prevent a fd leak.
		*/
		if (redir->heredoc_fd >= 0)
			close(redir->heredoc_fd);

		/* free() the filename or heredoc delimiter string. */
		free(redir->file);

		/* free() the node struct itself. */
		free(redir);
		redir = next;
	}
}

/**
 * @brief Frees a t_cmd struct and all heap memory it owns.
 *
 * Iterates through the NULL-terminated args array and frees each
 * string, then frees the array pointer, then frees the redir list,
 * and finally frees the cmd struct itself.
 *
 * @param cmd The command struct to free.  Safe to call with NULL.
 */
void	free_cmd(t_cmd *cmd)
{
	int	i;

	if (!cmd)
		return ;

	/* Free each argument string (they were ft_strdup'd in fill_cmd). */
	if (cmd->args)
	{
		i = 0;
		while (cmd->args[i])
		{
			free(cmd->args[i]);
			i++;
		}
		/* Free the array of pointers itself. */
		free(cmd->args);
	}

	/* Free the redirection list attached to this command. */
	free_redir(cmd->redirs);

	/* Free the t_cmd struct itself. */
	free(cmd);
}

/**
 * @brief Recursively frees an entire AST subtree.
 *
 * Post-order traversal: we go all the way to the leaves first and
 * free on the way back up.  This guarantees that we never dereference
 * a pointer that has already been freed.
 *
 * @param node Root of the subtree to free.  Safe to call with NULL.
 */
void	free_ast(t_ast *node)
{
	if (!node)
		return ;

	/* Recurse into children before freeing the parent. */
	free_ast(node->left);
	free_ast(node->right);

	/* Free the command data if this is a leaf command node. */
	if (node->cmd)
		free_cmd(node->cmd);

	/* Free the node struct itself. */
	free(node);
}

/**
 * @brief Parses one redirection token (and its target) into a t_redir node.
 *
 * Reads the operator token to get the type, then consumes the next
 * token as the filename or heredoc delimiter.
 *
 * Fields set by this function:
 *   redir->type       – which operator was used (<, >, <<, >>)
 *   redir->file       – the filename / delimiter that follows
 *   redir->heredoc_fd – starts at -1; set to an open fd later by the executor
 *   redir->quoted     – 0 here; set later by expand_redirs for heredocs
 *
 * @param tokens Pointer to the current position in the token stream.
 *               Advances past the operator AND the filename/delimiter.
 * @return The new t_redir node, or NULL on malloc failure.
 */
t_redir	*make_redir(t_token **tokens)
{
	t_redir	*redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);

	/*
	** Store the operator type (TOKEN_REDIR_IN, TOKEN_REDIR_OUT,
	** TOKEN_HEREDOC, or TOKEN_APPEND) from the current token.
	*/
	redir->type = (*tokens)->type;
	redir->file = NULL;
	redir->heredoc_fd = -1;  /* -1 means "no open fd yet" */
	redir->quoted = 0;       /* may be set to 1 by expand_redirs */
	redir->next = NULL;

	/* Consume the operator token and move to the filename/delimiter. */
	*tokens = (*tokens)->next;

	/*
	** The next token should be TOKEN_WORD: the filename to redirect
	** to/from, or the delimiter word for a heredoc.
	** ft_strdup copies it so the redir owns its own string.
	*/
	if (*tokens && (*tokens)->type == TOKEN_WORD)
	{
		redir->file = ft_strdup((*tokens)->value);
		if (!redir->file)
		{
			free(redir);
			return (NULL);
		}
		/* Consume the filename/delimiter token. */
		*tokens = (*tokens)->next;
	}
	return (redir);
}

/**
 * @brief Parses consecutive redirection tokens into a linked list.
 *
 * Keeps reading redir tokens until it hits a word, a pipe, or the end
 * of the relevant token types.  This function is currently unused in
 * the main parsing path (parse_command calls make_redir directly via
 * add_redir), but it is kept as a utility for potential future use.
 *
 * @param tokens Pointer to the current position in the token stream.
 * @return Head of the redir list, or NULL if no redirs were found.
 */
t_redir	*parse_redir(t_token **tokens)
{
	t_redir	*redir;
	t_redir	*head;
	t_redir	*tail;

	head = NULL;
	tail = NULL;

	/*
	** Continue as long as the current token belongs to this command
	** context.  Stop on: TOKEN_PIPE (next command), TOKEN_EOF,
	** TOKEN_AND / TOKEN_OR / TOKEN_RPAREN (bonus operators), or
	** TOKEN_WORD (which starts a new argument, not a redir).
	*/
	while (*tokens && (*tokens)->type != TOKEN_PIPE
		&& (*tokens)->type != TOKEN_EOF
		&& (*tokens)->type != TOKEN_AND
		&& (*tokens)->type != TOKEN_OR
		&& (*tokens)->type != TOKEN_RPAREN)
	{
		/* A word token ends the redir-only section. */
		if ((*tokens)->type == TOKEN_WORD)
			break ;

		/* Build and link the next redir node. */
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
