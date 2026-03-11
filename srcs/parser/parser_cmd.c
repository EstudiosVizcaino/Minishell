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
 * @brief Returns non-zero if the token type is any redirection type.
 *
 * Used throughout this file to distinguish "real" arguments (TOKEN_WORD)
 * from redirection operators (<, >, <<, >>).
 *
 * @param type The token type to check.
 * @return Non-zero if it is a redirection, 0 otherwise.
 */
static int	is_redir_token(t_token_type type)
{
	return (type == TOKEN_REDIR_IN || type == TOKEN_REDIR_OUT
		|| type == TOKEN_HEREDOC || type == TOKEN_APPEND);
}

/**
 * @brief Counts the number of actual argument words in the upcoming tokens.
 *
 * We need this BEFORE building the args array so that we can malloc it
 * with exactly the right size (count + 1 for the terminating NULL).
 *
 * The function skips redir tokens in pairs (operator + filename) so that
 * only real argument tokens are counted.  It stops when it reaches a
 * TOKEN_PIPE or the end of the token list.
 *
 * Example: for tokens [echo] [>] [out.txt] [hello]
 *   - echo  → count++ (it's a word)
 *   - >     → skip the operator and the next token (out.txt)
 *   - hello → count++
 *   result: 2
 *
 * @param tok The token to start counting from.
 * @return Number of argument words found.
 */
static int	count_word_tokens(t_token *tok)
{
	int	count;

	count = 0;
	/*
	** We continue only while the current token is either a word or a
	** redirection.  A TOKEN_PIPE (or end-of-list) marks the boundary
	** of this command.
	*/
	while (tok && (tok->type == TOKEN_WORD || is_redir_token(tok->type)))
	{
		if (is_redir_token(tok->type))
		{
			/*
			** Redirection operator: skip this token and the one after
			** it (the filename/delimiter).  We do not count either of
			** them as arguments.
			*/
			tok = tok->next;       /* skip the operator itself */
			if (tok)
				tok = tok->next;   /* skip the filename/delimiter */
		}
		else
		{
			/* Regular word → counts as one argument. */
			count++;
			tok = tok->next;
		}
	}
	return (count);
}

/**
 * @brief Appends one redirection node to the command's redir list.
 *
 * Calls make_redir() to parse and build the node, then links it at
 * the end of the existing redir list.  The 'tail' pointer is updated
 * so the next call can append in O(1) without traversing the list.
 *
 * @param cmd    The command we are building.
 * @param tail   Pointer to the tail pointer of the redir list.
 * @param tokens Current token position (updated by make_redir).
 */
static void	add_redir(t_cmd *cmd, t_redir **tail, t_token **tokens)
{
	t_redir	*r;

	/*
	** make_redir() consumes the operator token and the filename/delimiter
	** token from the stream and returns a populated t_redir node.
	*/
	r = make_redir(tokens);

	/*
	** Link the new redir node into the command's redir list.
	** If it's the first one, set cmd->redirs to point at it.
	** Otherwise, append it after the current tail.
	*/
	if (!cmd->redirs)
		cmd->redirs = r;
	else
		(*tail)->next = r;

	/* Always update *tail so the next append is still O(1). */
	*tail = r;
}

/**
 * @brief Fills the cmd struct with arguments and redirections.
 *
 * Walks the token stream as long as tokens belong to this command
 * (i.e. are TOKEN_WORD or a redirection type).  Words are stored in
 * cmd->args; redirections are handled via add_redir().
 *
 * After the loop, cmd->args[i] is set to NULL to terminate the array
 * (execve expects a NULL-terminated argv).
 *
 * @param cmd    The command struct to fill (args already allocated).
 * @param tokens Pointer to the current token (advanced as we consume).
 */
static void	fill_cmd(t_cmd *cmd, t_token **tokens)
{
	t_redir	*redir_tail;  /* tracks the last redir so we can append cheaply */
	int		i;            /* index into cmd->args for the next word */

	redir_tail = NULL;
	i = 0;
	while (*tokens && ((*tokens)->type == TOKEN_WORD
			|| is_redir_token((*tokens)->type)))
	{
		if (is_redir_token((*tokens)->type))
		{
			/*
			** This token is a redirection operator.
			** add_redir() will consume the operator AND the following
			** filename token from the stream.
			*/
			add_redir(cmd, &redir_tail, tokens);
		}
		else
		{
			/*
			** This token is an argument word.
			** ft_strdup copies the value so cmd->args owns its strings
			** independently of the token list (which will be freed soon).
			*/
			if (cmd->args)
				cmd->args[i++] = ft_strdup((*tokens)->value);
			*tokens = (*tokens)->next;  /* consume the word token */
		}
	}

	/* Terminate the args array with NULL (like a real argv). */
	if (cmd->args)
		cmd->args[i] = NULL;
}

/**
 * @brief Parses one simple command (arguments + redirections) from tokens.
 *
 * A "simple command" in shell terms is a word followed by zero or more
 * arguments and zero or more redirections, in any order.
 * This function:
 *   1. Creates the AST node and cmd struct.
 *   2. Counts how many word-arguments there are (to malloc the array).
 *   3. Allocates the args array.
 *   4. Calls fill_cmd() to populate args and redirs.
 *
 * @param tokens Pointer to the current token position (advanced on return).
 * @return The NODE_CMD AST node with a fully populated cmd, or NULL.
 */
t_ast	*parse_command(t_token **tokens)
{
	t_ast	*node;
	t_cmd	*cmd;
	int		count;  /* number of argument words */

	/* Allocate the AST wrapper node and the command data struct. */
	node = new_ast_node(NODE_CMD);
	cmd = new_cmd();
	if (!node || !cmd)
	{
		free(node);
		free_cmd(cmd);
		return (NULL);
	}

	/*
	** Count argument words first so we can allocate cmd->args with the
	** right size in one malloc instead of realloc-ing repeatedly.
	** count_word_tokens() does a peek-ahead without consuming tokens.
	*/
	count = count_word_tokens(*tokens);
	if (count > 0)
	{
		/*
		** +1 for the terminating NULL pointer (required by execve).
		** We allocate an array of (char *) pointers; each element will
		** be set to a ft_strdup'd string inside fill_cmd().
		*/
		cmd->args = malloc((count + 1) * sizeof(char *));
		if (!cmd->args)
		{
			free_ast(node);
			return (NULL);
		}
	}

	/*
	** Now actually parse the tokens into the cmd struct.
	** After this call, tokens points to the next unconsumed token
	** (a pipe, EOF, or nothing).
	*/
	fill_cmd(cmd, tokens);

	/* Attach the filled cmd to the AST node. */
	node->cmd = cmd;
	return (node);
}
