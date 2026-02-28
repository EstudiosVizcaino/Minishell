#include "minishell.h"

static int	open_redir_fd(t_redir *redir)
{
	if (!redir->file)
		return (-1);
	if (redir->type == TOKEN_REDIR_IN)
		return (open(redir->file, O_RDONLY));
	if (redir->type == TOKEN_REDIR_OUT)
		return (open(redir->file, O_WRONLY | O_CREAT | O_TRUNC, 0644));
	return (-1);
}

static int	apply_one_redir(t_redir *redir)
{
	int	fd;
	int	target;

	if (!redir->file)
		return (1);
	fd = open_redir_fd(redir);
	if (fd < 0)
	{
		perror(redir->file);
		return (1);
	}
	if (redir->type == TOKEN_REDIR_IN)
		target = STDIN_FILENO;
	else
		target = STDOUT_FILENO;
	dup2(fd, target);
	close(fd);
	return (0);
}

static int	apply_append(t_redir *redir)
{
	int	fd;

	if (!redir->file)
		return (1);
	fd = open(redir->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd < 0)
	{
		perror(redir->file);
		return (1);
	}
	dup2(fd, STDOUT_FILENO);
	close(fd);
	return (0);
}

int	open_heredoc(t_redir *redir)
{
	int		pipefd[2];
	char	*line;

	if (pipe(pipefd) == -1)
		return (1);
	setup_signals_heredoc();
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, redir->file) == 0)
		{
			free(line);
			break ;
		}
		write(pipefd[1], line, ft_strlen(line));
		write(pipefd[1], "\n", 1);
		free(line);
	}
	setup_signals();
	close(pipefd[1]);
	redir->heredoc_fd = pipefd[0];
	return (0);
}

int	apply_redirs(t_redir *redirs)
{
	t_redir	*r;
	int		ret;

	r = redirs;
	ret = 0;
	while (r)
	{
		if (r->type == TOKEN_HEREDOC)
		{
			if (r->heredoc_fd >= 0)
			{
				dup2(r->heredoc_fd, STDIN_FILENO);
				close(r->heredoc_fd);
				r->heredoc_fd = -1;
			}
		}
		else if (r->type == TOKEN_APPEND)
			ret = apply_append(r);
		else
			ret = apply_one_redir(r);
		if (ret)
			return (ret);
		r = r->next;
	}
	return (0);
}
