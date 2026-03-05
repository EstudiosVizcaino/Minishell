/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals_wait.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gisidro- <gisidro-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 18:15:04 by gisidro-          #+#    #+#             */
/*   Updated: 2026/03/05 18:15:04 by gisidro-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Handles SIGINT while the shell waits for a child process.
 *        Only records the signal; does not touch readline state.
 *
 * @param sig The signal number received.
 */
static void	sig_handler_wait(int sig)
{
	if (sig == SIGINT)
		g_signal = sig;
}

/**
 * @brief Sets up signal handlers while waiting for a foreground child.
 *        SIGINT is recorded but readline is not called, preventing
 *        spurious prompt redraws in the parent process.
 */
void	setup_signals_wait(void)
{
	struct sigaction	sa;

	ft_memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sig_handler_wait;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
}
