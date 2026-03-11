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
 * @brief Opens all heredoc-type redirs in the list.
 *
 * Walks the redir list and calls open_heredoc
 * for each TOKEN_HEREDOC entry.
 *
 * @param redir Head of the redir list.
 * @param shell The shell context for expansion.
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
 * @brief Waits for a child and gets its exit code.
 *
 * Uses waitpid then checks WIFEXITED /
 * WIFSIGNALED to figure out the real exit code.
 *
 * @param pid PID of the child.
 * @return The exit status.
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
 * @brief Runs redirs when the command has no args.
 *
 * Saves stdin/stdout, opens heredocs, applies the
 * redirs, then restores the saved fds.
 *
 * @param cmd  The command (only redirs matter).
 * @param shell The shell context.
 * @return 0 on success, non-zero on failure.
 */
static int	exec_redir_only(t_cmd *cmd, t_shell *shell)
{
	int	saved_in;
	int	saved_out;
	int	ret;

	saved_in = dup(STDIN_FILENO);
	saved_out = dup(STDOUT_FILENO);
	if (saved_in < 0 || saved_out < 0)
	{
		if (saved_in >= 0)
			close(saved_in);
		if (saved_out >= 0)
			close(saved_out);
		return (1);
	}
	open_heredocs(cmd->redirs, shell);
	ret = apply_redirs(cmd->redirs);
	dup2(saved_in, STDIN_FILENO);
	dup2(saved_out, STDOUT_FILENO);
	close(saved_in);
	close(saved_out);
	return (ret);
}

/**
 * @brief Executes a single command AST node.
 *
 * Handles redir-only commands, builtins, and
 * external commands (which get forked).
 *
 * @param ast  The command AST node.
 * @param shell The shell context.
 * @return Exit status of the command.
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
