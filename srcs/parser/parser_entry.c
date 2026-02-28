#include "minishell.h"

/**
 * @brief Entry point for the parser, converts tokens into an AST.
 *
 * @param tokens The linked list of tokens to parse.
 * @return A pointer to the root of the AST, or NULL if tokens are empty.
 */
t_ast	*parser(t_token *tokens)
{
	t_token	*tok;

	tok = tokens;
	if (!tok)
		return (NULL);
	return (parse_pipeline(&tok));
}
