/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals_wait.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 18:15:04 by cvizcain          #+#    #+#             */
/*   Updated: 2026/03/05 18:15:04 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief SIGINT handler used while the shell waits for a foreground child.
 *
 * When the user presses Ctrl-C while a command is running:
 *   - The kernel sends SIGINT to both the parent (shell) and the child
 *     (the running program) because they are in the same process group.
 *   - The child has SIG_DFL for SIGINT (set by setup_signals_child),
 *     so it is killed immediately.
 *   - The parent (shell) uses this handler: it records the signal in
 *     g_signal so that after waitpid() returns, the main loop can set
 *     last_exit = 130 (128 + SIGINT), matching bash behaviour.
 *
 * We intentionally do NOT call any readline functions here because the
 * parent is not at the readline prompt; calling rl_* from a signal
 * handler while not reading input would corrupt readline's internal state
 * and produce spurious prompt redraws.
 *
 * @param sig The signal number (always SIGINT here).
 */
static void	sig_handler_wait(int sig)
{
	if (sig == SIGINT)
		g_signal = sig;  /* record it; the wait loop checks this after waitpid */
}

/**
 * @brief Installs signal handlers for the period when the shell waits
 *        for a foreground child process to finish.
 *
 * Called in exec_cmd/exec_pipe right after fork() and before waitpid().
 * After waitpid() returns we call setup_signals() again to restore
 * the interactive-mode handlers.
 *
 * Key difference from setup_signals():
 *   - The handler is sig_handler_wait (not sig_handler), so rl_* is
 *     never called from inside this handler.
 *   - SA_RESTART is kept so that waitpid() is not interrupted by
 *     SIGINT; we let waitpid() finish collecting the child status
 *     before checking g_signal.
 *
 * SIGQUIT is still ignored in the parent (children reset it to SIG_DFL
 * via setup_signals_child).
 */
void	setup_signals_wait(void)
{
	struct sigaction	sa;

	/*
	** Zero out the struct to avoid undefined behaviour from uninitialised
	** padding bytes in the sa_mask / sa_flags fields.
	*/
	ft_memset(&sa, 0, sizeof(sa));

	/* Use the lightweight handler that only records the signal. */
	sa.sa_handler = sig_handler_wait;

	/* No additional signals are blocked while the handler runs. */
	sigemptyset(&sa.sa_mask);

	/*
	** SA_RESTART ensures that if SIGINT arrives while waitpid() is
	** sleeping, waitpid() is restarted rather than returning EINTR.
	** This way we always get the child's exit status before inspecting
	** g_signal.
	*/
	sa.sa_flags = SA_RESTART;

	sigaction(SIGINT, &sa, NULL);

	/* SIGQUIT is ignored in the parent while a child runs. */
	signal(SIGQUIT, SIG_IGN);
}
