/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_cmd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gisidro- <gisidro-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 18:48:46 by gisidro-          #+#    #+#             */
/*   Updated: 2026/02/17 21:12:45 by gisidro-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Opens all heredoc redirections in a redirection list.
 *
 * @param redir The head of the redirection list.
 * @param shell The shell state.
 */
void	open_heredocs(t_redir *redir, t_shell *shell)
{
	while (redir)
	{
		if (redir->type == TOKEN_HEREDOC)
			open_heredoc(redir, shell);
		redir = redir->next;
	}
}

/**
 * @brief Waits for a child process and returns its exit status.
 *
 * @param pid The process ID of the child to wait for.
 * @return The exit status of the child process.
 */
static int	wait_for_child(pid_t pid)
{
	int	status;
	int	exit_code;

	setup_signals_wait();
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		exit_code = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		exit_code = 128 + WTERMSIG(status);
	else
		exit_code = 1;
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
		write(STDOUT_FILENO, "\n", 1);
	g_signal = 0;
	return (exit_code);
}

/**
 * @brief Executes redirections for a command with no arguments.
 *
 * @param cmd The command structure containing redirections.
 * @param shell The shell state.
 * @return 0 on success, non-zero on failure.
 */
static int	exec_redir_only(t_cmd *cmd, t_shell *shell)
{
	open_heredocs(cmd->redirs, shell);
	return (apply_redirs(cmd->redirs));
}

/**
 * @brief Executes a single command AST node.
 *
 * @param ast The AST node representing the command.
 * @param shell The shell state.
 * @return The exit status of the executed command.
 */
int	exec_cmd(t_ast *ast, t_shell *shell)
{
	t_cmd	*cmd;
	pid_t	pid;
	int		ret;

	cmd = ast->cmd;
	if (!cmd)
		return (0);
	if ((!cmd->args || !cmd->args[0]) && cmd->redirs)
		return (exec_redir_only(cmd, shell));
	if (!cmd->args || !cmd->args[0])
		return (0);
	open_heredocs(cmd->redirs, shell);
	if (is_builtin(cmd->args[0]))
	{
		exec_builtin_redir(cmd, shell, &ret);
		return (ret);
	}
	pid = fork();
	if (pid < 0)
		return (1);
	if (pid == 0)
		exec_child(cmd, shell);
	return (wait_for_child(pid));
}
