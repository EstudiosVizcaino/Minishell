/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gisidro- <gisidro-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/07 16:46:21 by gisidro-          #+#    #+#             */
/*   Updated: 2026/02/21 11:15:12 by gisidro-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Handles SIGINT in interactive mode by redisplaying the prompt.
 *
 * @param sig The signal number received.
 */
void	sig_handler(int sig)
{
	if (sig == SIGINT)
	{
		g_signal = sig;
		write(STDOUT_FILENO, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

/**
 * @brief Handles SIGINT during heredoc input by closing stdin.
 *
 * @param sig The signal number received.
 */
void	sig_heredoc(int sig)
{
	if (sig == SIGINT)
	{
		g_signal = sig;
		write(STDOUT_FILENO, "\n", 1);
		close(STDIN_FILENO);
	}
}

/**
 * @brief Sets up signal handlers for interactive shell mode.
 */
void	setup_signals(void)
{
	struct sigaction	sa;

	ft_memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sig_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
}

/**
 * @brief Resets signal handlers to defaults for child processes.
 */
void	setup_signals_child(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

/**
 * @brief Sets up signal handlers for heredoc input mode.
 */
void	setup_signals_heredoc(void)
{
	struct sigaction	sa;

	ft_memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sig_heredoc;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
}
