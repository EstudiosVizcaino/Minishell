#include "minishell.h"

t_token	*new_token(t_token_type type, char *value)
{
	t_token	*tok;

	tok = malloc(sizeof(t_token));
	if (!tok)
		return (NULL);
	tok->type = type;
	tok->value = value ? ft_strdup(value) : NULL;
	tok->next = NULL;
	return (tok);
}

void	free_tokens(t_token *tokens)
{
	t_token	*next;

	while (tokens)
	{
		next = tokens->next;
		free(tokens->value);
		free(tokens);
		tokens = next;
	}
}

static t_token	*append_token(t_token **head, t_token **tail, t_token *tok)
{
	if (!tok)
		return (NULL);
	if (!*head)
		*head = tok;
	else
		(*tail)->next = tok;
	*tail = tok;
	return (tok);
}

t_token	*lexer(char *input)
{
	t_token	*head;
	t_token	*tail;
	t_token	*tok;
	int		i;

	head = NULL;
	tail = NULL;
	i = 0;
	while (input[i])
	{
		while (input[i] == ' ' || input[i] == '\t')
			i++;
		if (!input[i])
			break ;
		if (is_operator(input[i]))
			tok = handle_operator(input, &i);
		else
			tok = handle_word(input, &i);
		if (!append_token(&head, &tail, tok))
		{
			free_tokens(head);
			return (NULL);
		}
	}
	return (head);
}
