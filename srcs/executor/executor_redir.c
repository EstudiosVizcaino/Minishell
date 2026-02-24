#include "minishell.h"

static int	apply_one_redir(t_redir *redir)
{
	int	fd;

	if (redir->type == TOKEN_REDIR_IN)
	{
		fd = open(redir->file, O_RDONLY);
		if (fd < 0)
		{
			perror(redir->file);
			return (1);
		}
		dup2(fd, STDIN_FILENO);
		close(fd);
	}
	else if (redir->type == TOKEN_REDIR_OUT)
	{
		fd = open(redir->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd < 0)
		{
			perror(redir->file);
			return (1);
		}
		dup2(fd, STDOUT_FILENO);
		close(fd);
	}
	return (0);
}

static int	apply_append(t_redir *redir)
{
	int	fd;

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
