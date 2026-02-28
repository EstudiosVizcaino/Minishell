#include "minishell.h"

/**
 * @brief Entry point for the parser (bonus version using AND/OR parsing).
 *
 * @param tokens The head of the token list to parse.
 * @return The root AST node of the parsed input.
 */
t_ast	*parser(t_token *tokens)
{
	t_token	*tok;

	tok = tokens;
	if (!tok)
		return (NULL);
	return (parse_and_or(&tok));
}
