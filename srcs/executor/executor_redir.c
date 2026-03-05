/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_redir.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adherrer <adherrer@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 18:28:07 by adherrer          #+#    #+#             */
/*   Updated: 2026/02/18 13:14:04 by adherrer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Opens a file descriptor for a redirection based on its type.
 *
 * @param redir The redirection structure containing file and type info.
 * @return The opened file descriptor, or -1 on failure.
 */
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

/**
 * @brief Applies a single input or output redirection.
 *
 * @param redir The redirection structure to apply.
 * @return 0 on success, 1 on failure.
 */
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

/**
 * @brief Applies an append redirection.
 *
 * @param redir The redirection structure to apply.
 * @return 0 on success, 1 on failure.
 */
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

/**
 * @brief Reads heredoc input until the delimiter and stores it in a pipe.
 *        Variables in the body are expanded unless the delimiter was quoted.
 *
 * @param redir The redirection structure containing the heredoc delimiter.
 * @param shell The shell context used for variable expansion.
 * @return 0 on success, 1 on failure.
 */
int	open_heredoc(t_redir *redir, t_shell *shell)
{
	int		pipefd[2];
	char	*line;
	char	*expanded;

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
		if (!redir->quoted)
		{
			expanded = expand_str(line, shell);
			write(pipefd[1], expanded, ft_strlen(expanded));
			free(expanded);
		}
		else
			write(pipefd[1], line, ft_strlen(line));
		write(pipefd[1], "\n", 1);
		free(line);
	}
	setup_signals();
	close(pipefd[1]);
	redir->heredoc_fd = pipefd[0];
	return (0);
}

/**
 * @brief Applies all redirections in a redirection list.
 *
 * @param redirs The head of the redirection list.
 * @return 0 on success, non-zero on failure.
 */
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
