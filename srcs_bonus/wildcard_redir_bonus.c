#include "minishell.h"

static void	redir_ambiguous(t_redir *redir)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd(redir->file, STDERR_FILENO);
	ft_putstr_fd(": ambiguous redirect\n", STDERR_FILENO);
	free(redir->file);
	redir->file = NULL;
}

static void	apply_redir_wildcard(t_redir *redir)
{
	char	**exp;

	exp = expand_wildcard(redir->file);
	if (!exp)
		return ;
	if (array_len(exp) == 1)
	{
		free(redir->file);
		redir->file = ft_strdup(exp[0]);
	}
	else
		redir_ambiguous(redir);
	free_array(exp);
}

void	expand_wildcard_redir(t_redir *redir)
{
	while (redir)
	{
		if (redir->type != TOKEN_HEREDOC && redir->file
			&& ft_strchr(redir->file, '*'))
			apply_redir_wildcard(redir);
		if (redir->file)
			unmask_wildcards(redir->file);
		redir = redir->next;
	}
}
