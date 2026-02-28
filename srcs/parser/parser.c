#include "minishell.h"

/**
 * @brief Allocates and initializes a new AST node.
 *
 * @param type The type of the AST node to create.
 * @return A pointer to the new AST node, or NULL on allocation failure.
 */
t_ast	*new_ast_node(t_node_type type)
{
	t_ast	*node;

	node = malloc(sizeof(t_ast));
	if (!node)
		return (NULL);
	node->type = type;
	node->cmd = NULL;
	node->left = NULL;
	node->right = NULL;
	return (node);
}

/**
 * @brief Allocates and initializes a new command structure.
 *
 * @return A pointer to the new command structure, or NULL on allocation failure.
 */
t_cmd	*new_cmd(void)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->args = NULL;
	cmd->redirs = NULL;
	return (cmd);
}

/**
 * @brief Parses a pipeline of commands connected by pipes.
 *
 * @param tokens A pointer to the current position in the token list.
 * @return A pointer to the AST representing the pipeline, or NULL on failure.
 */
t_ast	*parse_pipeline(t_token **tokens)
{
	t_ast	*left;
	t_ast	*pipe_node;

	left = parse_command(tokens);
	if (!left)
		return (NULL);
	if (*tokens && (*tokens)->type == TOKEN_PIPE)
	{
		*tokens = (*tokens)->next;
		pipe_node = new_ast_node(NODE_PIPE);
		if (!pipe_node)
		{
			free_ast(left);
			return (NULL);
		}
		pipe_node->left = left;
		pipe_node->right = parse_pipeline(tokens);
		if (!pipe_node->right)
		{
			free_ast(pipe_node);
			return (NULL);
		}
		return (pipe_node);
	}
	return (left);
}
