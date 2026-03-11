/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_redir.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gisidro- <gisidro-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 18:28:07 by gisidro-          #+#    #+#             */
/*   Updated: 2026/02/18 13:14:04 by gisidro-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Applies a single input or output redir.
 *
 * Opens the file, dups the fd onto stdin or
 * stdout, then closes the original fd.
 *
 * @param redir The redir to apply.
 * @return 0 on success, 1 on failure.
 */
static int	apply_one_redir(t_redir *redir)
{
	int	fd;
	int	target;

	if (!redir->file)
		return (1);
	if (redir->type == TOKEN_REDIR_IN)
		fd = open(redir->file, O_RDONLY);
	else
		fd = open(redir->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
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
 * @brief Writes one heredoc line to the pipe, expanding vars if unquoted.
 *
 * @param fd The write end of the heredoc pipe.
 * @param line The input line to write.
 * @param shell The shell context for variable expansion.
 * @param quoted Whether the heredoc delimiter was quoted.
 */
static void	write_heredoc_line(int fd, char *line, t_shell *shell, int quoted)
{
	char	*expanded;

	if (!quoted)
	{
		expanded = expand_heredoc_str(line, shell);
		write(fd, expanded, ft_strlen(expanded));
		free(expanded);
	}
	else
		write(fd, line, ft_strlen(line));
	write(fd, "\n", 1);
}

/**
 * @brief Applies an append (>>) redirection.
 *
 * Opens the file with O_APPEND and dups onto
 * stdout.
 *
 * @param redir The redir to apply.
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
 * @brief Reads heredoc input until the delimiter.
 *
 * Uses readline in a loop, writing each line into
 * a pipe. Stores the read-end fd in redir.
 * Saves and restores STDIN_FILENO so that the
 * sig_heredoc handler's close(STDIN_FILENO) does not
 * permanently close the calling process's stdin.
 *
 * @param redir The heredoc redir with the delimiter.
 * @param shell The shell context for expansion.
 * @return 0 on success, 1 on failure.
 */
int	open_heredoc(t_redir *redir, t_shell *shell)
{
	int		pipefd[2];
	char	*line;
	int		saved_stdin;

	if (pipe(pipefd) == -1)
		return (1);
	saved_stdin = dup(STDIN_FILENO);
	setup_signals_heredoc();
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, redir->file) == 0)
		{
			free(line);
			break ;
		}
		write_heredoc_line(pipefd[1], line, shell, redir->quoted);
		free(line);
	}
	if (saved_stdin >= 0)
	{
		dup2(saved_stdin, STDIN_FILENO);
		close(saved_stdin);
	}
	setup_signals();
	close(pipefd[1]);
	redir->heredoc_fd = pipefd[0];
	return (0);
}

/**
 * @brief Applies all redirs in a list.
 *
 * Goes through each redir node and applies it
 * (heredoc, append, or normal in/out).
 *
 * @param redirs Head of the redir list.
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
